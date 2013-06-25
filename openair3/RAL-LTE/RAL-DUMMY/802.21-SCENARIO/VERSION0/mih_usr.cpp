//==============================================================================
// Brief   : MIH-User
// Authors : Bruno Santos <bsantos@av.it.pt>
//------------------------------------------------------------------------------
// ODTONE - Open Dot Twenty One
//
// Copyright (C) 2009-2012 Universidade Aveiro
// Copyright (C) 2009-2012 Instituto de Telecomunicações - Pólo Aveiro
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//==============================================================================

#include <odtone/base.hpp>
#include <odtone/debug.hpp>
#include <odtone/logger.hpp>
#include <odtone/mih/request.hpp>
#include <odtone/mih/response.hpp>
#include <odtone/mih/indication.hpp>
#include <odtone/mih/confirm.hpp>
#include <odtone/mih/tlv_types.hpp>
#include <odtone/sap/user.hpp>

#include <boost/utility.hpp>
#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

#include <iostream>

#define THRESHOLD_HIGH_VAL 90
#define THRESHOLD_LOW_VAL  15

///////////////////////////////////////////////////////////////////////////////
// The scenario coded in this MIH-USER is the following (with mRALlteDummy and NASUEDummy executables)
//  +--------+                                             +-----+
//  |MIH_USER|                                             |MIH-F|
//  +---+----+                                             +--+--+
//      |---------- User_Register.indication ---------------->| (supported_commands) Handler next msg=user_reg_handler
//      |                                                     |
//      |---------- Capability_Discover.request ------------->| Handler next msg=receive_MIH_Capability_Discover_confirm
//      |<--------- Capability_Discover.confirm --------------| (success)
//      |                                                     |
//      |---------- Event_Subscribe.request ----------------->| Handler next msg=receive_MIH_Event_Subscribe_response
//      |<--------- Event_Subscribe.response -----------------| (success)
//      |                                                     |
//      |---------- Configure_Thresholds.request ------------>| (High signal strength value) Handler next msg=event_handler
//      |<--------- Configure_Thresholds.confirm -------------| link_get_parameters_request = 1
//      |                                                     |
//      |---------- Link_Get_Parameters.request ------------->| Handler next msg=event_handler (link_get_parameters_request)
//      |<--------- Link_Get_Parameters.confirm --------------| (success)
//      |                         .                           |
//      |                         .                           |
//      |                         .                           |
//      |<--------- Link_Detected.indication -----------------| link_action_request = 0
//      |---------- Link_Actions.request -------------------->| (power-up+scan) Handler next msg=event_handler
//      |<--------- Link_Actions.confirm ---------------------| (success)
//      |---------- Link_Actions.request -------------------->| (power-up) Handler next msg=event_handler
//      |<--------- Link_Actions.confirm ---------------------| (success)
//      |                         .                           |
//      |                         .                           |
//      |<--------- Link_Parameters_Report.indication --------| (Threshold crossed) Now (signal strength will decrease)
//      |                                                     |
//      |---------- Configure_Thresholds.request ------------>| (Low signal strength value) Handler next msg=event_handler
//      |<--------- Configure_Thresholds.confirm -------------| (success)
//      |                         .                           |
//      |                         .                           |
//      |<--------- Link_Parameters_Report.indication --------| (Threshold crossed)
//      |---------- Link_Actions.request -------------------->| (power-down) Handler next msg=event_handler
//      |                         .                           |
//      |<--------- Link_Actions.confirm ---------------------| (success)
//      |                                                     |
//      |                                                     |
///////////////////////////////////////////////////////////////////////////////

static const char* const kConf_MIH_Commands = "user.commands";
///////////////////////////////////////////////////////////////////////////////

namespace po = boost::program_options;

using odtone::uint;
using odtone::ushort;
using odtone::sint8;

odtone::logger log_("[mih_usr]", std::cout);

///////////////////////////////////////////////////////////////////////////////
void __trim(odtone::mih::octet_string &str, const char chr)
{
    str.erase(std::remove(str.begin(), str.end(), chr), str.end());
}

//-----------------------------------------------------------------------------
template <class T> std::string StringOf(T object) {
//-----------------------------------------------------------------------------
    std::ostringstream os;
    os << object;
    return(os.str());
}
//-----------------------------------------------------------------------------
std::string getTimeStamp4Log()
//-----------------------------------------------------------------------------
{
    std::stringstream ss (std::stringstream::in | std::stringstream::out);
    struct timespec time_spec;
    unsigned int time_now_micros;
    unsigned int time_now_s;
    clock_gettime (CLOCK_REALTIME, &time_spec);
    time_now_s      = (unsigned int) time_spec.tv_sec % 3600;
    time_now_micros = (unsigned int) time_spec.tv_nsec/1000;
    //time_now_s = 110;
    //time_now_micros = 22;
    ss << time_now_s << ':' << time_now_micros;
    return ss.str();
}
//-----------------------------------------------------------------------------
std::string status2string(odtone::mih::status statusP){
//-----------------------------------------------------------------------------
    switch (statusP.get()) {
        case odtone::mih::status_success:                 return "SUCCESS";break;
        case odtone::mih::status_failure:                 return "UNSPECIFIED_FAILURE";break;
        case odtone::mih::status_rejected:                return "REJECTED";break;
        case odtone::mih::status_authorization_failure:   return "AUTHORIZATION_FAILURE";break;
        case odtone::mih::status_network_error:           return "NETWORK_ERROR";break;
        default:                                          return "UNKNOWN";
    }
}
//-----------------------------------------------------------------------------
std::string link_down_reason2string(odtone::mih::link_dn_reason reasonP){
//-----------------------------------------------------------------------------
    switch (reasonP.get()) {
        case odtone::mih::link_dn_reason_explicit_disconnect:       return "DN_REASON_EXPLICIT_DISCONNECT";break;
        case odtone::mih::link_dn_reason_packet_timeout:            return "DN_REASON_PACKET_TIMEOUT";break;
        case odtone::mih::link_dn_reason_no_resource:               return "DN_REASON_NO_RESOURCE";break;
        case odtone::mih::link_dn_reason_no_broadcast:              return "DN_REASON_NO_BROADCAST";break;
        case odtone::mih::link_dn_reason_authentication_failure:    return "DN_REASON_AUTHENTICATION_FAILURE";break;
        case odtone::mih::link_dn_reason_billing_failure:           return "DN_REASON_BILLING_FAILURE";break;
        default:                                                    return "DN_REASON_UNKNOWN";
    }
}
//-----------------------------------------------------------------------------
std::string link_going_down_reason2string(odtone::mih::link_gd_reason reasonP){
//-----------------------------------------------------------------------------
    switch (reasonP.get()) {
        case odtone::mih::link_gd_reason_explicit_disconnect:       return "GD_REASON_EXPLICIT_DISCONNECT";break;
        case odtone::mih::link_gd_reason_link_parameter_degrading:  return "GD_REASON_PARAMETER_DEGRADING";break;
        case odtone::mih::link_gd_reason_low_power:                 return "GD_REASON_LOW_POWER";break;
        case odtone::mih::link_gd_reason_no_resource:               return "GD_REASON_NO_RESOURCE";break;
        default:                                                    return "GD_REASON_UNKNOWN";
    }
}
//-----------------------------------------------------------------------------
std::string evt2string(odtone::mih::mih_evt_list evtP){
//-----------------------------------------------------------------------------
    std::string s;
    if(evtP.get(odtone::mih::mih_evt_link_detected))          s = std::string("DETECTED, ");
    if(evtP.get(odtone::mih::mih_evt_link_up))                s += "UP, ";
    if(evtP.get(odtone::mih::mih_evt_link_down))              s += "DOWN, ";
    if(evtP.get(odtone::mih::mih_evt_link_parameters_report)) s += "PARAMETERS_REPORT, ";
    if(evtP.get(odtone::mih::mih_evt_link_going_down))        s += "GOING_DOWN, ";
    if(evtP.get(odtone::mih::mih_evt_link_handover_imminent)) s += "HANDOVER_IMMINENT, ";
    if(evtP.get(odtone::mih::mih_evt_link_handover_complete)) s += "HANDOVER_COMPLETE, ";
    if(evtP.get(odtone::mih::mih_evt_link_pdu_transmit_status)) s += "PDU_TRANSMIT_STATUS, ";
    return s;
}
//-----------------------------------------------------------------------------
std::string link_id2string(odtone::mih::link_id linkP){
//-----------------------------------------------------------------------------
    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;
    std::string s;

    switch (linkP.type.get()) {
        case odtone::mih::link_type_gsm:
            s="GSM";
            break;
        case odtone::mih::link_type_gprs:
            s="GPRS";
            break;
        case odtone::mih::link_type_edge:
            s="EDGE";
            break;
        case odtone::mih::link_type_ethernet:
            s="Ethernet";
            break;
        case odtone::mih::link_type_wireless_other:
            s="Other";
            break;
        case odtone::mih::link_type_802_11:
            s="IEEE 802.11";
            break;
        case odtone::mih::link_type_lte:
            s="LTE";
            // address copied from UMTS
            local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(linkP.addr);
            s = s + std::string(local_l2_3gpp_addr.value);
            break;
        case odtone::mih::link_type_cdma2000:
            s="CDMA-2000";
            break;
        case odtone::mih::link_type_umts:
            s="UMTS:";
            local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(linkP.addr);
            /*index = 0;
            for(iter=0; iter < local_l2_3gpp_addr.value.length(); iter++) {
               index += sprintf(&buf[index],"-%hhx-", local_l2_3gpp_addr.value[iter]);
            }*/
            s = s + std::string(local_l2_3gpp_addr.value);
            break;
        case odtone::mih::link_type_cdma2000_hrpd:
            s="CDMA-2000-HRPD";
            break;
        case odtone::mih::link_type_802_16:
            s="IEEE 802.16";
            break;
        case odtone::mih::link_type_802_20:
            s="IEEE 802.20";
            break;
        case odtone::mih::link_type_802_22:
            s="IEEE 802.22";
            break;
    }
    return s;
}
//-----------------------------------------------------------------------------
std::string link_tupple_id2string(odtone::mih::link_tuple_id link_tuppleP){
//-----------------------------------------------------------------------------
    std::string               s;
    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;

    switch (link_tuppleP.type.get()) {
        case odtone::mih::link_type_gsm:
            s="GSM";
            break;
        case odtone::mih::link_type_gprs:
            s="GPRS";
            break;
        case odtone::mih::link_type_edge:
            s="EDGE";
            break;
        case odtone::mih::link_type_ethernet:
            s="Ethernet";
            break;
        case odtone::mih::link_type_wireless_other:
            s="Other";
            break;
        case odtone::mih::link_type_802_11:
            s="IEEE 802.11";
            break;
        case odtone::mih::link_type_lte:
            s="LTE";
            local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(link_tuppleP.addr);
            s = s + std::string(local_l2_3gpp_addr.value);
            break;
        case odtone::mih::link_type_cdma2000:
            s="CDMA-2000";
            break;
        case odtone::mih::link_type_umts:
            s="UMTS:";
            local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(link_tuppleP.addr);
            /*index = 0;
            for(iter=0; iter < local_l2_3gpp_addr.value.length(); iter++) {
               index += sprintf(&buf[index],"-%hhx-", local_l2_3gpp_addr.value[iter]);
            }*/
            break;
        case odtone::mih::link_type_cdma2000_hrpd:
            s="CDMA-2000-HRPD";
            break;
        case odtone::mih::link_type_802_16:
            s="IEEE 802.16";
            break;
        case odtone::mih::link_type_802_20:
            s="IEEE 802.20";
            break;
        case odtone::mih::link_type_802_22:
            s="IEEE 802.22";
            break;
    }
    return s;
}
//-----------------------------------------------------------------------------
std::string link_actions_req2string(odtone::mih::link_action_req link_act_reqP) {
//-----------------------------------------------------------------------------
    std::string s;

    s = link_id2string(link_act_reqP.id);

    if(link_act_reqP.action.type == odtone::mih::link_ac_type_none)                      s += ", AC_TYPE_NONE";
    if(link_act_reqP.action.type == odtone::mih::link_ac_type_disconnect)                s += ", AC_TYPE_DISCONNECT";
    if(link_act_reqP.action.type == odtone::mih::link_ac_type_low_power)                 s += ", AC_TYPE_LOW_POWER";
    if(link_act_reqP.action.type == odtone::mih::link_ac_type_power_down)                s += ", AC_TYPE_POWER_DOWN";
    if(link_act_reqP.action.type == odtone::mih::link_ac_type_power_up)                  s += ", AC_TYPE_POWER_UP";
    if(link_act_reqP.action.type == odtone::mih::link_ac_type_flow_attr)                 s += ", AC_TYPE_FLOW_ATTR";
    if(link_act_reqP.action.type == odtone::mih::link_ac_type_link_activate_resources)   s += ", AC_TYPE_ACTIVATE_RESOURCES";
    if(link_act_reqP.action.type == odtone::mih::link_ac_type_link_deactivate_resources) s += ", AC_TYPE_DEACTIVATE_RESOURCES";

    if(link_act_reqP.action.attr.get(odtone::mih::link_ac_attr_data_fwd_req))            s += ", AC_ATTR_DATA_FWD_REQ";
    if(link_act_reqP.action.attr.get(odtone::mih::link_ac_attr_scan))                    s += ", AC_ATTR_SCAN";
    if(link_act_reqP.action.attr.get(odtone::mih::link_ac_attr_res_retain))              s += ", AC_ATTR_RES_RETAIN";

    s += ", " + StringOf(link_act_reqP.ex_time) + " ms";
    return s;
}
//-----------------------------------------------------------------------------
std::string net_type_addr_list2string(boost::optional<odtone::mih::net_type_addr_list> ntalP) {
//-----------------------------------------------------------------------------
    std::string s;
    odtone::mih::link_id link_id;
    for (odtone::mih::net_type_addr_list::iterator i = ntalP->begin(); i != ntalP->end(); ++i)
    {
        link_id.addr = boost::get<odtone::mih::l2_3gpp_addr>(i->addr);
        link_id.type = boost::get<odtone::mih::link_type>(i->nettype.link);
        s += link_id2string(link_id);
    }
    return s;
}


/**
 * Parse supported commands.
 *
 * @param cfg Configuration options.
 * @return A bitmap mapping the supported commands.
 */
boost::optional<odtone::mih::mih_cmd_list> parse_supported_commands(const odtone::mih::config &cfg)
{
    using namespace boost;

    odtone::mih::mih_cmd_list commands;
    bool has_cmd = false;

    std::map<std::string, odtone::mih::mih_cmd_list_enum> enum_map;
    enum_map["mih_link_get_parameters"]       = odtone::mih::mih_cmd_link_get_parameters;
    enum_map["mih_link_configure_thresholds"] = odtone::mih::mih_cmd_link_configure_thresholds;
    enum_map["mih_link_actions"]              = odtone::mih::mih_cmd_link_actions;
    enum_map["mih_net_ho_candidate_query"]    = odtone::mih::mih_cmd_net_ho_candidate_query;
    enum_map["mih_net_ho_commit"]             = odtone::mih::mih_cmd_net_ho_commit;
    enum_map["mih_n2n_ho_query_resources"]    = odtone::mih::mih_cmd_n2n_ho_query_resources;
    enum_map["mih_n2n_ho_commit"]             = odtone::mih::mih_cmd_n2n_ho_commit;
    enum_map["mih_n2n_ho_complete"]           = odtone::mih::mih_cmd_n2n_ho_complete;
    enum_map["mih_mn_ho_candidate_query"]     = odtone::mih::mih_cmd_mn_ho_candidate_query;
    enum_map["mih_mn_ho_commit"]              = odtone::mih::mih_cmd_mn_ho_commit;
    enum_map["mih_mn_ho_complete"]            = odtone::mih::mih_cmd_mn_ho_complete;

    std::string tmp = cfg.get<std::string>(kConf_MIH_Commands);
    __trim(tmp, ' ');

    char_separator<char> sep1(",");
    tokenizer< char_separator<char> > list_tokens(tmp, sep1);

    BOOST_FOREACH(std::string str, list_tokens) {
        if(enum_map.find(str) != enum_map.end()) {
            commands.set((odtone::mih::mih_cmd_list_enum) enum_map[str]);
            has_cmd = true;
        }
    }

    boost::optional<odtone::mih::mih_cmd_list> supp_cmd;
    if(has_cmd)
        supp_cmd = commands;

    return supp_cmd;
}

///////////////////////////////////////////////////////////////////////////////
/**
 * This class provides an implementation of an IEEE 802.21 MIH-User.
 */
class mih_user : boost::noncopyable {
public:
    /**
     * Construct the MIH-User.
     *
     * @param cfg Configuration options.
     * @param io The io_service object that the MIH-User will use to
     * dispatch handlers for any asynchronous operations performed on the socket.
     */
    mih_user(const odtone::mih::config& cfg, boost::asio::io_service& io);

    /**
     * Destruct the MIH-User.
     */
    ~mih_user();

protected:
    /**
     * User registration handler.
     *
     * @param cfg Configuration options.
     * @param ec Error Code.
     */
    void user_reg_handler(const odtone::mih::config& cfg, const boost::system::error_code& ec);

    /**
     * Default MIH event handler.
     *
     * @param msg Received message.
     * @param ec Error code.
     */
    void event_handler(odtone::mih::message& msg, const boost::system::error_code& ec);

    /**
     * Capability Discover handler.
     *
     * @param msg Received message.
     * @param ec Error Code.
     */
    void receive_MIH_Capability_Discover_confirm(odtone::mih::message& msg, const boost::system::error_code& ec);

    /**
     * Event subscribe handler.
     *
     * @param msg Received message.
     * @param ec Error Code.
     */
    void receive_MIH_Event_Subscribe_response(odtone::mih::message& msg, const boost::system::error_code& ec);

    void send_MIH_Link_Configure_Thresholds_request(odtone::mih::message& msg, const boost::system::error_code& ec);

    void receive_MIH_Link_Configure_Thresholds_confirm(odtone::mih::message& msg, const boost::system::error_code& ec);

    void send_MIH_Link_Get_Parameters_request(odtone::mih::message& msg, const boost::system::error_code& ec);

    void receive_MIH_Link_Get_Parameters_confirm(odtone::mih::message& msg, const boost::system::error_code& ec);

    void receive_MIH_Link_Detected_indication(odtone::mih::message& msg, const boost::system::error_code& ec);

    void send_MIH_Link_Actions_request(odtone::mih::message& msg, const boost::system::error_code& ec);

    void receive_MIH_Link_Actions_confirm(odtone::mih::message& msg, const boost::system::error_code& ec);

    void receive_MIH_Link_Up_indication(odtone::mih::message& msg, const boost::system::error_code& ec);

    void receive_MIH_Link_Going_Down_indication(odtone::mih::message& msg, const boost::system::error_code& ec);

    void receive_MIH_Link_Down_indication(odtone::mih::message& msg, const boost::system::error_code& ec);

    void receive_MIH_Event_Unsubscribe_confirm(odtone::mih::message& msg, const boost::system::error_code& ec);

    void send_MIH_Event_Unsubscribe_request(odtone::mih::message& msg, const boost::system::error_code& ec);

    void receive_MIH_Link_Parameters_Report(odtone::mih::message& msg, const boost::system::error_code& ec);

    void print_l2_3gpp_addr(odtone::mih::link_type& link_type, odtone::mih::l2_3gpp_addr& l2_3gpp_addr);


private:
    odtone::sap::user _mihf;    /**< User SAP helper.        */
        odtone::mih::id   _mihfid;  /**< MIHF destination ID.   */
        odtone::mih::id   _mihuserid;  /**< MIH_USER ID.   */
    odtone::uint link_get_parameters_request, link_action_request, link_threshold_request;
    odtone::mih::net_type_addr rcv_net_type_addr;
    odtone::mih::link_id rcv_link_id;
};

/**
 * Construct the MIH-User.
 *
 * @param cfg Configuration options.
 * @param io The io_service object that the MIH-User will use to
 * dispatch handlers for any asynchronous operations performed on the socket.
 */
//-----------------------------------------------------------------------------
mih_user::mih_user(const odtone::mih::config& cfg, boost::asio::io_service& io)
    : _mihf(cfg, io, boost::bind(&mih_user::event_handler, this, _1, _2))
//-----------------------------------------------------------------------------
{
    odtone::mih::message m;
    boost::optional<odtone::mih::mih_cmd_list> supp_cmd = parse_supported_commands(cfg);

    link_get_parameters_request = 0;
    link_action_request         = 0;
    link_threshold_request      = 0;

    m << odtone::mih::indication(odtone::mih::indication::user_register)
        & odtone::mih::tlv_command_list(supp_cmd);

    log_(0, "User Register Indication - SENT ");
    log_(0, " - User Name: ", m.source().to_string());
    log_(0, " ");

    odtone::mih::octet_string user_id = cfg.get<odtone::mih::octet_string>(odtone::sap::kConf_MIH_SAP_id);
    _mihuserid.assign(user_id.c_str());

    odtone::mih::octet_string  dest_id = cfg.get<odtone::mih::octet_string>(odtone::sap::kConf_MIH_SAP_dest);
    _mihfid.assign("mihf1");

    log_(0, "[MSC_NEW][",getTimeStamp4Log(),"][MIH-USER=",_mihuserid.to_string(),"]\n");
    m.source(_mihuserid);
    m.destination(_mihfid);
    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ m.source().to_string() +"][--- MIH_User_Register.indication --->]["+m.destination().to_string()+"]\n");
    _mihf.async_send(m, boost::bind(&mih_user::user_reg_handler, this, boost::cref(cfg), _2));
}


/**
 * Destruct the MIH-User.
 */
//-----------------------------------------------------------------------------
mih_user::~mih_user()
//-----------------------------------------------------------------------------
{
}

/**
 * Print 3GPP Address.
 *
 * @param link_type Link Type.
 * @param l2_3gpp_addr 3GPP Address.
 */
//-----------------------------------------------------------------------------
void mih_user::print_l2_3gpp_addr(odtone::mih::link_type& link_type, odtone::mih::l2_3gpp_addr& l2_3gpp_addr)
//-----------------------------------------------------------------------------
{
    odtone::uint iter;
    if(link_type == odtone::mih::link_type_umts){ 
       log_(0, "\t   Link Type: UMTS"); 
    }
    if(link_type == odtone::mih::link_type_lte){ 
       log_(0, "\t   Link Type: LTE"); 
    }
    if((link_type == odtone::mih::link_type_umts)||(link_type == odtone::mih::link_type_lte)){ 
       //log_(0, "\t   3GPP Address: ");
       std::printf("[mih_usr]: \t   3GPP Address: ");
       for(iter=0; iter<l2_3gpp_addr.value.length(); iter++)
       {
          std::printf("-%hhx-", l2_3gpp_addr.value[iter]);
       }
       std::printf("\n");
    }
}

/**
 * User registration handler.
 *
 * @param cfg Configuration options.
 * @param ec Error Code.
 */
//-----------------------------------------------------------------------------
void mih_user::user_reg_handler(const odtone::mih::config& cfg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    log_(0, "MIH-User register result: ", ec.message());
    log_(0, "");

    odtone::mih::message msg;

    odtone::mih::octet_string destination = cfg.get<odtone::mih::octet_string>(odtone::sap::kConf_MIH_SAP_dest);
    _mihfid.assign(destination.c_str());

    //
    // Let's fire a capability discover request to get things moving
    //
    msg << odtone::mih::request(odtone::mih::request::capability_discover, _mihfid);

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ _mihuserid.to_string() +"][--- MIH_Capability_Discover.request --->]["+msg.destination().to_string()+"]\n");
    _mihf.async_send(msg, boost::bind(&mih_user::receive_MIH_Capability_Discover_confirm, this, _1, _2));

    log_(0, "MIH_Capability_Discover.request - SENT (towards its local MIHF)");
    log_(0, "");
}

/**
 * Default MIH event handler.
 *
 * @param msg Received message.
 * @param ec Error code.
 */
//-----------------------------------------------------------------------------
void mih_user::event_handler(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    if (ec) {
        log_(0, __FUNCTION__, " error: ", ec.message());
        return;
    }

    switch (msg.mid()) {
    case odtone::mih::indication::link_up:
        mih_user::receive_MIH_Link_Up_indication(msg, ec);
        break;

    case odtone::mih::indication::link_down:
        mih_user::receive_MIH_Link_Down_indication(msg, ec);
        //sleep(2);
        mih_user::send_MIH_Link_Get_Parameters_request(msg, ec);
        break;

    case odtone::mih::indication::link_detected:
        mih_user::receive_MIH_Link_Detected_indication(msg, ec);
        // monitor signal strength - do not change (up/down) signal strength report now
        //system ("sendip -d 0x00 -p ipv4 -is 127.0.0.1  -p udp -ud 22222 -us 65535 127.0.0.1");

        //sleep(2);
        link_action_request = 0;
        mih_user::send_MIH_Link_Actions_request(msg, ec);
        break;

    case odtone::mih::indication::link_going_down:
        mih_user::receive_MIH_Link_Going_Down_indication(msg,ec);
        //sleep(10);
        link_get_parameters_request = 2;
        mih_user::send_MIH_Link_Get_Parameters_request(msg, ec);
        break;

    case odtone::mih::indication::link_handover_imminent:
        log_(0, "MIH-User has received a local event \"link_handover_imminent\"");
        break;
    case odtone::mih::indication::link_handover_complete:
        log_(0, "MIH-User has received a local event \"link_handover_complete\"");
        break;

    case odtone::mih::confirm::link_configure_thresholds:
        mih_user::receive_MIH_Link_Configure_Thresholds_confirm(msg, ec);
        //sleep(2);
        if (link_get_parameters_request == 0) {
            link_get_parameters_request = 1;
            mih_user::send_MIH_Link_Get_Parameters_request(msg, ec);
        }
        break;

    case odtone::mih::confirm::link_get_parameters:
        switch(link_get_parameters_request){
            case 1: mih_user::receive_MIH_Link_Get_Parameters_confirm(msg, ec);
                   // monitor signal strength - increase signal strength report now
                    //system ("sendip -d 0x01 -p ipv4 -is 127.0.0.1  -p udp -ud 22222 -us 65535 127.0.0.1");
                    break;
            case 2: mih_user::receive_MIH_Link_Get_Parameters_confirm(msg, ec);
                    link_action_request = 1;
                    link_get_parameters_request = 3;
                    mih_user::send_MIH_Link_Actions_request(msg, ec);
                    break;
            case 3: mih_user::receive_MIH_Link_Get_Parameters_confirm(msg, ec);
                    mih_user::send_MIH_Event_Unsubscribe_request(msg, ec);
                    break;
        }
        break;

        mih_user::receive_MIH_Link_Get_Parameters_confirm(msg, ec);
        //sleep(1);
        break;

    case odtone::mih::confirm::link_actions:
        mih_user::receive_MIH_Link_Actions_confirm(msg, ec);
        if (link_action_request == 0) {
            link_action_request = 1;
            mih_user::send_MIH_Link_Actions_request(msg, ec);
        } else if (link_action_request == 1) {
            // monitor signal strength - increase signal strength report now
            //system ("sendip -d 0x01 -p ipv4 -is 127.0.0.1  -p udp -ud 22222 -us 65535 127.0.0.1");
        }

        break;

    case odtone::mih::confirm::event_unsubscribe:
        mih_user::receive_MIH_Event_Unsubscribe_confirm(msg, ec);
        break;

    case odtone::mih::indication::link_parameters_report:
        mih_user::receive_MIH_Link_Parameters_Report(msg, ec);
        if (link_threshold_request == 1) {
            // monitor signal strength - do not modify signal strength report now
            //system ("sendip -d 0x0x00 -p ipv4 -is 127.0.0.1  -p udp -ud 22222 -us 65535 127.0.0.1");
            link_action_request = 2;
            mih_user::send_MIH_Link_Actions_request(msg, ec);

        } else if (link_action_request == 1) {
            // monitor signal strength - decrease signal strength report now
            //system ("sendip -d 0xFF -p ipv4 -is 127.0.0.1  -p udp -ud 22222 -us 65535 127.0.0.1");
            // select decreasing down to low threshold
            link_threshold_request = 1;
            mih_user::send_MIH_Link_Configure_Thresholds_request(msg, ec);
        }
        break;
    }
}

//-----------------------------------------------------------------------------
void mih_user::receive_MIH_Link_Parameters_Report(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    odtone::mih::link_tuple_id link;
    odtone::mih::link_param_rpt_list lprl;
    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;

    msg >> odtone::mih::indication()
           & odtone::mih::tlv_link_identifier(link)
           & odtone::mih::tlv_link_param_rpt_list(lprl);

    log_(0, "");
    log_(0, "MIH_Link_Parameters_Report.indication - RECEIVED - Begin");
    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ msg.source().to_string() +"][--- MIH_Link_Parameters_Report.indication --->]["+msg.destination().to_string()+"]\n");
    local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(link.addr);
    print_l2_3gpp_addr(link.type, local_l2_3gpp_addr);
    log_(0, "MIH_Link_Parameters_Report.indication - End");
}


//-----------------------------------------------------------------------------
void mih_user::receive_MIH_Event_Unsubscribe_confirm(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    odtone::mih::status st;
    odtone::mih::link_tuple_id link;
    boost::optional<odtone::mih::mih_evt_list> evt;

    msg >>     odtone::mih::confirm()
            & odtone::mih::tlv_status(st)
            & odtone::mih::tlv_link_identifier(link)
            & odtone::mih::tlv_event_list(evt);

    log_(0, "");
    log_(0, "receive_MIH_Event_Unsubscribe_confirm - Begin");

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ msg.source().to_string() +"][--- MIH_Event_Unsubscribe.confirm\\n"+status2string(st.get()).c_str()+" --->]["+msg.destination().to_string()+"]\n");
    log_(0, "\t- STATUS: ", status2string(st.get()).c_str(), " ", st.get());
    log_(0, "\t- MIH_EVT_LIST - Event List:  ", evt2string(evt.get()).c_str());
    log_(0, "receive_MIH_Event_Unsubscribe_confirm - End");
}

//-----------------------------------------------------------------------------
void mih_user::send_MIH_Event_Unsubscribe_request(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    odtone::mih::message m;

    odtone::mih::link_tuple_id link;
    odtone::mih::mih_evt_list events;
    odtone::mih::l2_3gpp_addr l2_3gpp_addr;

    link.type = rcv_link_id.type;
    //address UMTS
    link.addr = rcv_link_id.addr;

    events.set(odtone::mih::mih_evt_link_detected);
    events.set(odtone::mih::mih_evt_link_up);
    events.set(odtone::mih::mih_evt_link_down);
    events.set(odtone::mih::mih_evt_link_parameters_report);
    events.set(odtone::mih::mih_evt_link_going_down);

    log_(0, "");
    log_(0, "send_MIH_Event_Unsubscribe_request - Begin");
    m << odtone::mih::request(odtone::mih::request::event_unsubscribe)
         & odtone::mih::tlv_link_identifier(link)
         & odtone::mih::tlv_event_list(events);

    m.destination(msg.source());

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ m.source().to_string() +"][--- MIH_Event_Unsubscribe.request --->]["+m.destination().to_string()+"]\n");
    _mihf.async_send(m, boost::bind(&mih_user::event_handler, this, _1, _2));

    log_(0, "  - MIH_EVT_LIST - Event List:  ", evt2string(events).c_str());
    log_(0, "send_MIH_Event_Unsubscribe_request - End");
}

//-----------------------------------------------------------------------------
void mih_user::receive_MIH_Link_Down_indication(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    odtone::mih::link_tuple_id link;
    odtone::mih::link_dn_reason ldr;
    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;

    log_(0, "");
    log_(0, "receive_MIH_Link_Down_indication - Begin");

    msg >> odtone::mih::indication()
           & odtone::mih::tlv_link_identifier(link)
           & odtone::mih::tlv_link_dn_reason(ldr);


    local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(link.addr);

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ msg.source().to_string() +"][--- MIH_Link_Down.indication\\n"+link_down_reason2string(ldr).c_str()+" --->]["+msg.destination().to_string()+"]\n");

    print_l2_3gpp_addr(link.type, local_l2_3gpp_addr);

    log_(0, "    Link down reason:  ", link_down_reason2string(ldr).c_str());
    log_(0, "receive_MIH_Link_Down_indication - End");
}

//-----------------------------------------------------------------------------
void mih_user::receive_MIH_Link_Going_Down_indication(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    odtone::mih::link_tuple_id link;
    odtone::mih::link_gd_reason lgd;
    odtone::mih::link_ac_ex_time ex_time;

    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;

    log_(0, "");
    log_(0, "receive_MIH_Link_Going_Down_indication - Begin");

    msg >> odtone::mih::indication()
           & odtone::mih::tlv_link_identifier(link)
           & odtone::mih::tlv_time_interval(ex_time)
           & odtone::mih::tlv_link_gd_reason(lgd);


    local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(link.addr);
    local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(link.addr);

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ msg.source().to_string() +"][--- MIH_Link_Going_Down.indication\\n"+link_going_down_reason2string(lgd).c_str()+" --->]["+msg.destination().to_string()+"]\n");

    print_l2_3gpp_addr(link.type, local_l2_3gpp_addr);
    log_(0, "\tTime Interval :", (ex_time/256));

    log_(0, "\tLink going down reason: ", link_going_down_reason2string(lgd).c_str());

    log_(0, "receive_MIH_Link_Going_Down_indication - End");
}

//-----------------------------------------------------------------------------
void mih_user::receive_MIH_Link_Up_indication(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    odtone::mih::link_tuple_id link;
    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;

    msg >>     odtone::mih::indication()
            & odtone::mih::tlv_link_identifier(link);

    log_(0, "");
    log_(0, "receive_MIH_Link_Up_indication - Begin");
    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ msg.source().to_string() +"][--- MIH_Link_Up.indication --->]["+msg.destination().to_string()+"]\n");

    local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(link.addr);
    print_l2_3gpp_addr(link.type, local_l2_3gpp_addr);
    log_(0, "receive_MIH_Link_Up_indication - End");
}

//-----------------------------------------------------------------------------
void mih_user::receive_MIH_Link_Actions_confirm(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{

    log_(0, "");
    log_(0, "receive_MIH_Link_Actions_confirm - Begin");

    if (ec) {
        log_(0, __FUNCTION__, " error: ", ec.message());
        return;
    }

    odtone::mih::status st;
    boost::optional<odtone::mih::link_action_rsp_list> larl;
    /*odtone::mih::link_action_rsp_list larl;
    odtone::mih::link_action_rsp lar;

    odtone::mih::link_scan_rsp_list lsrl;
    odtone::mih::link_scan_rsp lsr;

    odtone::uint i, l, n, p;
    odtone::mih::link_id link;
    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;
    odtone::mih::l2_3gpp_3g_cell_id local_l2_3gpp_3g_cell_id;
    odtone::mih::link_scan_rsp_list local_link_scan_rsp_list;
    odtone::sint8 local_sig_strength;*/

    msg >> odtone::mih::confirm()
           & odtone::mih::tlv_status(st)
           & odtone::mih::tlv_link_action_rsp_list(larl);

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ msg.source().to_string() +"][--- MIH_Link_Actions.confirm\\n"+status2string(st.get())+" --->]["+msg.destination().to_string()+"]\n");
    log_(0, "\t- STATUS: ", status2string(st.get()), " " , st.get());

    /*log_(0, "  - LINK ACTION RSP LIST - Length:", larl.size());

    for(i=0; i< larl.size(); i++)
    {
        local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(larl[i].id.addr);
        print_l2_3gpp_addr(larl[i].id.type, local_l2_3gpp_addr);

        if(larl[i].result == odtone::mih::link_ac_success){log_(0, "    Link Action Success");}
        if(larl[i].result == odtone::mih::link_ac_failure){log_(0, "    Link Action Failure");}
        if(larl[i].result == odtone::mih::link_ac_refused){log_(0, "    Link Action Refused");}
        if(larl[i].result == odtone::mih::link_ac_incapable){log_(0, "    Link Action Incapable");}

        local_link_scan_rsp_list = boost::get<odtone::mih::link_scan_rsp_list>(larl[i].scan_list);

        log_(0, "     Link Scan Rsp List - Element: ", local_link_scan_rsp_list.size());
        for(l=0; l<local_link_scan_rsp_list.size(); l++)
        {
            //link_addr
            local_l2_3gpp_3g_cell_id = boost::get<odtone::mih::l2_3gpp_3g_cell_id>(local_link_scan_rsp_list[l].id);

            log_(0, "        3GPP Cell ID Adress: ", local_link_scan_rsp_list.size());
            for(p=0; p<3; p++) {
                std::printf("[mih_usr]:             ");
                std::printf(" Plmn %d: %d", p, local_l2_3gpp_3g_cell_id.plmn_id[p]);
            }
            log_(0, "          Cell-ID: ", (local_l2_3gpp_3g_cell_id._cell_id/);
            //net_id network identifier
            log_(0, "        Network Identifier: ");
            std::printf("[mih_usr]:             ");
            for(n=0; n<local_link_scan_rsp_list[l].net_id.size(); n++)
            {
                std::printf("%c", local_link_scan_rsp_list[l].net_id[n]);
            }
            std::printf("\n");
            //sig_strength
            local_sig_strength = boost::get<odtone::sint8>(local_link_scan_rsp_list[l].signal);
            std::printf("[mih_usr]:             ");
            std::printf("Signal Strength: %d \n", local_sig_strength);
            //log_(0, "        Signal Strength: ", local_sig_strength);
        }
        log_(0, "   ");
    }
*/
    log_(0, "MIH_Link_Actions.confirm - End");
}

//-----------------------------------------------------------------------------
void mih_user::send_MIH_Link_Actions_request(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    odtone::mih::message m;

    odtone::mih::link_action_list lal;
    odtone::mih::link_action_req link_act_req;
    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;

    log_(0, "");
    log_(0, "send_MIH_Link_Actions_request - Begin");
    //address UMTS
    link_act_req.id.type = rcv_link_id.type;
    link_act_req.id.addr = rcv_link_id.addr;

    if(mih_user::link_action_request == 0){
            link_act_req.action.type = odtone::mih::link_ac_type_power_up;
            link_act_req.action.attr.set(odtone::mih::link_ac_attr_scan);
            //link_act_req.action.param...;
    } else if(mih_user::link_action_request == 1) {
            link_act_req.action.type = odtone::mih::link_ac_type_power_up;
    } else if(mih_user::link_action_request == 2) {
        link_act_req.action.type = odtone::mih::link_ac_type_power_down;
    }

    link_act_req.ex_time = 0;

    lal.push_back(link_act_req);


    m <<      odtone::mih::request(odtone::mih::request::link_actions)
            & odtone::mih::tlv_link_action_list(lal);


    log_(0, "\t- LINK_ID: ");
    local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(link_act_req.id.addr);
    print_l2_3gpp_addr(link_act_req.id.type, local_l2_3gpp_addr);

    log_(0, "\t- Link Actions: " + link_actions_req2string(link_act_req));

    m.destination(msg.source());

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ msg.destination().to_string() +"][--- MIH_Link_Actions.request\\n"+link_actions_req2string(link_act_req)+" --->]["+msg.source().to_string()+"]\n");
    _mihf.async_send(m, boost::bind(&mih_user::event_handler, this, _1, _2));

    log_(0, "send_MIH_Link_Actions_request - End");
    log_(0, "");
}

//-----------------------------------------------------------------------------
void mih_user::receive_MIH_Link_Detected_indication(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    odtone::mih::link_det_info_list ldil;

    log_(0, "");
    log_(0, "receive_MIH_Link_Detected_indication - Begin");

    msg >>     odtone::mih::indication()
            & odtone::mih::tlv_link_det_info_list(ldil);

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ msg.source().to_string() +"][--- MIH_Link_Detected.indication --->]["+msg.destination().to_string()+"]\n");

    log_(0, "receive_MIH_Link_Detected_indication - End");
}

//-----------------------------------------------------------------------------
void mih_user::send_MIH_Link_Get_Parameters_request(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    odtone::mih::message m;

    odtone::mih::dev_states_req dsr;
    odtone::uint iter;

    odtone::mih::link_id li;
    odtone::mih::link_id_list lil;

    odtone::mih::link_status_req lsr;

    log_(0,"");
    log_(0, "send_MIH_Link_Get_Parameters_request - Begin");

    //set values
    dsr.set(odtone::mih::dev_state_device_info); //optional

    li.type = rcv_link_id.type;
    li.addr = rcv_link_id.addr;
    lil.push_back(li);
    odtone::mih::link_param_gen lp1;
    //odtone::mih::link_param_gen lp2, lp3, lp4, lp5;

    lp1 = odtone::mih::link_param_gen_data_rate;
    lsr._param_type_list.push_back(lp1);

    /*lp2 = odtone::mih::link_param_gen_sinr;
    lsr._param_type_list.push_back(lp2);

    lp3 = odtone::mih::link_param_gen_packet_error_rate;
    lsr._param_type_list.push_back(lp3);

    lp4 = odtone::mih::link_param_gen_throughput;
    lsr._param_type_list.push_back(lp4);

    lp5 = odtone::mih::link_param_gen_signal_strength;
    lsr._param_type_list.push_back(lp5);*/

    lsr._states_req.set(odtone::mih::link_states_req_op_mode);
    lsr._states_req.set(odtone::mih::link_states_req_channel_id);
    lsr._desc_req.set(odtone::mih::link_desc_req_classes_of_service_supported);


    m << odtone::mih::request(odtone::mih::request::link_get_parameters)
            & odtone::mih::tlv_dev_states_req(dsr)
            & odtone::mih::tlv_link_id_list(lil)
            & odtone::mih::tlv_get_status_req_set(lsr);

    m.destination(msg.source());

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ msg.destination().to_string() +"][--- MIH_Link_Get_Parameters.request --->]["+msg.source().to_string()+"]\n");
    _mihf.async_send(m, boost::bind(&mih_user::event_handler, this, _1, _2));

    //log_(0, "  - DEVICE STATES REQUEST");

    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;

    log_(0, "\t- LINK ID LIST - Length: ", lil.size());

    for(iter=0; iter<lil.size(); iter++)
    {
        log_(0, "\t  LINK ID ", iter);
        local_l2_3gpp_addr =  boost::get<odtone::mih::l2_3gpp_addr>(lil[iter].addr);
        print_l2_3gpp_addr(lil[iter].type, local_l2_3gpp_addr);
    }

    log_(0, "\t- GET STATUS REQUEST ");
    if(lsr._states_req.get(odtone::mih::link_states_req_op_mode) == 1) {log_(0, "\t  Link power mode requested");}
    if(lsr._states_req.get(odtone::mih::link_states_req_channel_id) == 1) {log_(0, "\t  Channel identifier requested (as defined in the specific link technology)");}

    if(lp1 == odtone::mih::link_param_gen_data_rate) {log_(0, "\t  DATA RATE link parameter requested");}
    if(lp1 == odtone::mih::link_param_gen_signal_strength) {log_(0, "\t  SIGNAL STRENGTH link parameter is required");}
    if(lp1 == odtone::mih::link_param_gen_sinr) {log_(0, "\t  SINR link parameter is required");}
    if(lp1 == odtone::mih::link_param_gen_throughput) {log_(0, "\t  THROUGHPUT link parameter is required");}

    log_(0, "\t- LINK DESCRIPTOR REQUESTED ");

    if(lsr._desc_req.get(odtone::mih::link_desc_req_classes_of_service_supported) == 1) {log_(0, "\t  Number of CoS Supported ");}
    if(lsr._desc_req.get(odtone::mih::link_desc_req_queues_supported) == 1) {log_(0, "\t  Number of Queues Supported ");}
    log_(0, "send_MIH_Link_Get_Parameters_request - End");
    log_(0, "");

}

//-----------------------------------------------------------------------------
void mih_user::receive_MIH_Link_Get_Parameters_confirm(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
 
    odtone::mih::status st;
    odtone::mih::status_rsp_list srl;

    odtone::uint iter;

    log_(0, "");
    log_(0, "receive_MIH_Link_Get_Parameters_confirm - Begin");

    msg >> odtone::mih::confirm()
        & odtone::mih::tlv_status(st)
        & odtone::mih::tlv_get_status_rsp_list(srl);

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ _mihfid.to_string() +"][--- MIH_Link_Get_Parameters.confirm\\n"+status2string(st.get()).c_str()+" --->]["+_mihuserid.to_string()+"]\n");
    log_(0, "\t- STATUS: ", status2string(st.get()), " ", st.get());


    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;

    log_(0, "\t- STATUS RSP LIST - Length: ", srl.size());

    for(iter=0; iter<srl.size(); iter++)
    {
        log_(0, "\t  LINK ID ", iter+1, ":");
        local_l2_3gpp_addr =  boost::get<odtone::mih::l2_3gpp_addr>(srl[iter].id.addr);
        print_l2_3gpp_addr(srl[iter].id.type, local_l2_3gpp_addr);
        log_(0, "\t  LINK STATUS RSP List - Element: ", iter+1);

        /*odtone::uint iter1;

        log_(0, "        States Rsp List - Length: ", srl[iter].rsp.states_rsp_list.size());
        //States Rsp List (vector)
        for(iter1=0; iter1<srl[iter1].rsp.states_rsp_list.size(); iter1++)
        {
            //each element op_mode or channel_id
            log_(0, "        Element:", iter1+1, " - Value:", srl[iter1].rsp.states_rsp_list[iter1]);
        }

        log_(0, "        Param List - Length: ", srl[iter].rsp.param_list.size());
        //Param List
        for(iter1=0; iter1<srl[iter1].rsp.param_list.size(); iter1++)
        {
            //log_(0, "Element:", iter+1, "Type: ", srl[iter1].rsp.param_list[iter1].type ,"Value: ", srl[iter1].rsp.param_list[iter1]);
            // 1 - link_param_type
            // 2 - boost::variant<link_param_val, qos_param_val>
        }

        log_(0, "        Desc_rsp_list - Length: ", srl[iter].rsp.desc_rsp_list.size());
        //Desc_rsp_list
          for(iter1=0; iter1<srl[iter1].rsp.desc_rsp_list.size(); iter1++)
          {
            //log_(0, "Element:", iter+1, "Value", srl[iter1].rsp.desc_rsp_list[iter1]);
            //boost::variant<num_cos, num_queue>
            //uint8 num_cos - uint8 num_queue
          }*/
    }
    log_(0, "receive_MIH_Link_Get_Parameters_confirm - End");

}


/**
 * Capability Discover handler.
 *
 * @param msg Received message.
 * @param ec Error Code.
 */
//-----------------------------------------------------------------------------
void mih_user::receive_MIH_Capability_Discover_confirm(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    log_(0, "");
    log_(0, "receive_MIH_Capability_Discover_confirm - Begin");
    if (ec) {
        log_(0, __FUNCTION__, " error: ", ec.message());
        return;
    }

    odtone::mih::status st;
    boost::optional<odtone::mih::net_type_addr_list> ntal;
    boost::optional<odtone::mih::mih_evt_list> evt;

    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;
    odtone::mih::link_tuple_id li;

    msg >> odtone::mih::confirm()
        & odtone::mih::tlv_status(st)
        & odtone::mih::tlv_net_type_addr_list(ntal)
        & odtone::mih::tlv_event_list(evt);
        // MIH_TRANS_LIST could be added

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ _mihfid.to_string() ,"][--- MIH_Capability_Discover.confirm\\nstatus=", status2string(st.get()).c_str(),
                        "\\nEvent list=",evt2string(evt.get()).c_str(),
                        "\\nNet type addr list=" , net_type_addr_list2string(ntal).c_str(),
                        " --->][",_mihuserid.to_string(),"]\n");
    log_(0, "\t- STATUS: ", status2string(st.get()), " " ,st.get());

    if (evt) {
        log_(0, "\t- MIH_EVT_LIST - Event List: ", evt2string(evt.get()).c_str());
    }

    //log_(0, "MIH-User has received a Capability_Discover.response with status ",
    //        st.get(), " and the following capabilities:");

    log_(0, "\t- LIST(NET_TYPE_ADDR) - Network Types and Link Address:");

    if (ntal) {
        for (odtone::mih::net_type_addr_list::iterator i = ntal->begin(); i != ntal->end(); ++i)
        {
            local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(i->addr);
            li.type = boost::get<odtone::mih::link_type>(i->nettype.link);

            rcv_net_type_addr.addr = boost::get<odtone::mih::l2_3gpp_addr>(i->addr);
            rcv_net_type_addr.nettype.link = boost::get<odtone::mih::link_type>(i->nettype.link);

            rcv_link_id.addr = boost::get<odtone::mih::l2_3gpp_addr>(i->addr);
            rcv_link_id.type = boost::get<odtone::mih::link_type>(i->nettype.link);

            print_l2_3gpp_addr(li.type, local_l2_3gpp_addr);
            // should be the same print
            //print_l2_3gpp_addr(rcv_link_id.type, rcv_link_id.addr);

            //log_(0,  *i);
        }

    } else {
        log_(0,  "none");
    }

    log_(0, "");
    //
    // event subscription
    //
    // For every interface the MIHF sent in the
    // Capability_Discover.response send an Event_Subscribe.request
    // for all availabe events
    //
    if (ntal && evt) {
        for (odtone::mih::net_type_addr_list::iterator i = ntal->begin(); i != ntal->end(); ++i) {
            odtone::mih::message req;
            odtone::mih::link_tuple_id li;

            if (i->nettype.link.which() == 1)
            {
                li.addr = i->addr;
                li.type = boost::get<odtone::mih::link_type>(i->nettype.link);

                req << odtone::mih::request(odtone::mih::request::event_subscribe, _mihfid)
                    & odtone::mih::tlv_link_identifier(li)
                    & odtone::mih::tlv_event_list(evt);

                req.destination(msg.source());

                log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ _mihuserid.to_string() +"][--- MIH_Event_Subscribe.request"+
                                                      "\\nLink="+link_tupple_id2string(li).c_str()+
                                                      "\\nEvent list="+evt2string(evt.get()).c_str()+
                                                      " --->]["+_mihfid.to_string()+"]\n");
                _mihf.async_send(req, boost::bind(&mih_user::receive_MIH_Event_Subscribe_response, this, _1, _2));

                log_(0, "MIH-User has sent Event_Subscribe.request to ", req.destination().to_string());

                if (evt) {
                    log_(0, "\t- MIH_EVT_LIST - Event List:  ", evt2string(evt.get()).c_str());
                }
            }
        }
    }
    log_(0, "receive_MIH_Capability_Discover_confirm - End");
}

/**
 * Event subscribe handler.
 *
 * @param msg Received message.
 * @param ec Error Code.
 */
//-----------------------------------------------------------------------------
void mih_user::receive_MIH_Event_Subscribe_response(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    //log_(0, __FUNCTION__, "(", msg.tid(), ")");
    log_(0, "");
    log_(0, "receive_MIH_Event_Subscribe_response - Begin");

    if (ec) {
        log_(0, __FUNCTION__, " error: ", ec.message());
        return;
    }

    odtone::mih::status st;
    boost::optional<odtone::mih::mih_evt_list> evt;

    msg >> odtone::mih::response()
        & odtone::mih::tlv_status(st)
        & odtone::mih::tlv_event_list(evt);

    if (evt) {
        log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ _mihfid.to_string() +"][--- MIH_Event_Subscribe.response\\nstatus="+
                      status2string(st.get()).c_str()+
                      "\\nEvent list="+evt2string(evt.get()).c_str()+
                      " --->]["+_mihuserid.to_string()+"]\n");
    } else {
        log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ _mihfid.to_string() +"][--- MIH_Event_Subscribe.response\\nstatus="+
                      status2string(st.get()).c_str()+
                      " --->]["+_mihuserid.to_string()+"]\n");
    }
    log_(0, "  - STATUS: ", status2string(st.get()), " " ,st.get());

    mih_user::send_MIH_Link_Configure_Thresholds_request(msg, ec);
    log_(0, "receive_MIH_Event_Subscribe_response - End");
}

//-----------------------------------------------------------------------------
void mih_user::send_MIH_Link_Configure_Thresholds_request(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    odtone::mih::message m;

    odtone::mih::threshold th;
    std::vector<odtone::mih::threshold> thl;

    odtone::mih::link_tuple_id lti;
    odtone::mih::l2_3gpp_addr local_l2_3gpp_addr;

    log_(0,"");
    log_(0, "send_MIH_Link_Configure_Thresholds_request - Begin");

    //link_tuple_id
    lti.type = rcv_link_id.type;
    lti.addr = rcv_link_id.addr;

    local_l2_3gpp_addr = boost::get<odtone::mih::l2_3gpp_addr>(lti.addr);

    //List of the link threshold parameters
    odtone::mih::link_cfg_param_list lcpl;
    odtone::mih::link_cfg_param lcp;
    odtone::mih::link_param_gen lp;

    //link_param_gen_data_rate = 0,           /**< Data rate.         */
    //link_param_gen_signal_strength = 1,     /**< Signal strength.   */
    //link_param_gen_sinr = 2,                /**< SINR.              */
    //link_param_gen_throughput = 3,          /**< Throughput.        */
    //link_param_gen_packet_error_rate = 4,   /**< Packet error rate. */
    lp = odtone::mih::link_param_gen_signal_strength;

    lcp.type = lp;
    //th_action_normal = 0,   /**< Set normal threshold.      */
    //th_action_one_shot = 1, /**< Set one-shot threshold.    */
    //th_action_cancel = 2    /**< Cancel threshold.          */
    lcp.action = odtone::mih::th_action_one_shot;

    //above_threshold = 0,    /**< Above threshold.   */
    //below_threshold = 1,    /**< Below threshold.   */
    if (link_threshold_request == 0) {
        th.threshold_val = THRESHOLD_HIGH_VAL;
        th.threshold_x_dir = odtone::mih::threshold::above_threshold;
    } else if (link_threshold_request == 0) {
        th.threshold_val = THRESHOLD_LOW_VAL;
        th.threshold_x_dir = odtone::mih::threshold::below_threshold;
    } else  {
        th.threshold_val = THRESHOLD_LOW_VAL;
        th.threshold_x_dir = odtone::mih::threshold::below_threshold;
    }

    thl.push_back(th);
    lcp.threshold_list = thl;
    lcpl.push_back(lcp);

    m <<  odtone::mih::request(odtone::mih::request::link_configure_thresholds)
          & odtone::mih::tlv_link_identifier(lti)
          & odtone::mih::tlv_link_cfg_param_list(lcpl);

    m.destination(msg.source());

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ _mihuserid.to_string() +"][--- MIH_Link_Configure_Thresholds.request\\nlink="+
                 link_tupple_id2string(lti).c_str() +
                 " --->]["+_mihfid.to_string()+"]\n");
    _mihf.async_send(m, boost::bind(&mih_user::event_handler, this, _1, _2));


    log_(0, "\t- LINK TUPLE ID - Network Types and Link Address:");
    print_l2_3gpp_addr(lti.type, local_l2_3gpp_addr);

    log_(0, "\t- LINK CFG PARAM LIST - Length: ", lcpl.size());

    if(lp == odtone::mih::link_param_gen_data_rate) {log_(0, "\t  Generic link parameter DATA RATE ");}
    if(lp == odtone::mih::link_param_gen_signal_strength) {log_(0, "\t  Generic link parameter SIGNAL STRENGTH");}
    if(lp == odtone::mih::link_param_gen_sinr) {log_(0, "\t   Generic link parameter SINR");}
    if(lp == odtone::mih::link_param_gen_throughput) {log_(0, "\t  Generic link parameter THROUGHPUT");}

    if(lcp.action == odtone::mih::th_action_normal) {log_(0, "\t  Normal Threshold");}
    if(lcp.action == odtone::mih::th_action_one_shot) {log_(0, "\t  One Shot Threshold");}
    if(lcp.action == odtone::mih::th_action_cancel) {log_(0, "\t  Threshold to be canceled");}

    log_(0, "\t  Threshold value: ", th.threshold_val);

    if(th.threshold_x_dir == odtone::mih::threshold::below_threshold) {log_(0, "\t  Threshold direction BELOW");}
    if(th.threshold_x_dir == odtone::mih::threshold::above_threshold) {log_(0, "\t  Threshold direction ABOVE");}

    log_(0, "send_MIH_Link_Configure_Thresholds_request - End");
}

//-----------------------------------------------------------------------------
void mih_user::receive_MIH_Link_Configure_Thresholds_confirm(odtone::mih::message& msg, const boost::system::error_code& ec)
//-----------------------------------------------------------------------------
{
    log_(0, "");
    log_(0, "receive_MIH_Link_Configure_Thresholds_confirm - Begin");

    odtone::uint iter;
    odtone::mih::status st;

    //boost::optional<odtone::mih::link_cfg_status_list> lcsl;
    odtone::mih::link_cfg_status_list lcsl;
    odtone::mih::link_cfg_status lcp;
    odtone::mih::link_param_gen lp;

    odtone::mih::link_tuple_id lti;

    msg >> odtone::mih::confirm()
        & odtone::mih::tlv_status(st)
        & odtone::mih::tlv_link_identifier(lti)
        & odtone::mih::tlv_link_cfg_status_list(lcsl);

    log_(0, "[MSC_MSG]["+getTimeStamp4Log()+"]["+ _mihfid.to_string() +"][--- MIH_Link_Configure_Thresholds.confirm\\nstatus="+status2string(st.get()).c_str()+" --->]["+_mihuserid.to_string()+"]\n");
    log_(0, "\t- STATUS: ", status2string(st.get()), " " ,st.get());

    log_(0, "\t- LINK CFG STATUS LIST - Length: ", lcsl.size());

    for(iter=0; iter<lcsl.size(); iter++)
    {
        log_(0, "\t  Link Param Type: ", lcsl[0].type);
        log_(0, "\t  Threshold Val: ", (lcsl[0].thold.threshold_val/256));
        if(lcsl[0].thold.threshold_x_dir == odtone::mih::threshold::below_threshold) {log_(0, "\t  Threshold direction BELOW");}
        if(lcsl[0].thold.threshold_x_dir == odtone::mih::threshold::above_threshold) {log_(0, "\t  Threshold direction ABOVE");}
        if(lcsl[0].status == odtone::mih::status_success){log_(0, "\t  Config Status: Success ");}
        else {log_(0, "\t  Config Status: ", lcsl[0].status);}
    }

    log_(0, "receive_MIH_Link_Configure_Thresholds_confirm - End");
    log_(0,"");
}

//-----------------------------------------------------------------------------
int main(int argc, char** argv)
//-----------------------------------------------------------------------------
{
    odtone::setup_crash_handler();

    try {
        boost::asio::io_service ios;

        // declare MIH Usr available options
        po::options_description desc(odtone::mih::octet_string("MIH Usr Configuration"));
        desc.add_options()
            ("help", "Display configuration options")
            (odtone::sap::kConf_File, po::value<std::string>()->default_value("mih_usr.conf"), "Configuration file")
            (odtone::sap::kConf_Receive_Buffer_Len, po::value<uint>()->default_value(4096), "Receive buffer length")
            (odtone::sap::kConf_Port, po::value<ushort>()->default_value(1234), "Listening port")
            (odtone::sap::kConf_MIH_SAP_id, po::value<std::string>()->default_value("user"), "MIH-User ID")
            (kConf_MIH_Commands, po::value<std::string>()->default_value(""), "MIH-User supported commands")
            (odtone::sap::kConf_MIHF_Ip, po::value<std::string>()->default_value("127.0.0.1"), "Local MIHF IP address")
            (odtone::sap::kConf_MIHF_Local_Port, po::value<ushort>()->default_value(1025), "Local MIHF communication port")
            (odtone::sap::kConf_MIH_SAP_dest, po::value<std::string>()->default_value("mihf1"), "MIHF destination");

        odtone::mih::config cfg(desc);
        cfg.parse(argc, argv, odtone::sap::kConf_File);

        if (cfg.help()) {
            std::cerr << desc << std::endl;
            return EXIT_SUCCESS;
        }

        mih_user usr(cfg, ios);

        ios.run();

    } catch(std::exception& e) {
        log_(0, "exception: ", e.what());
    }
}

// EOF ////////////////////////////////////////////////////////////////////////
