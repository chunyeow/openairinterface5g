//==============================================================================
// Brief   : ODTONE MIHF
// Authors : Simao Reis <sreis@av.it.pt>
//           Carlos Guimarães <cguimaraes@av.it.pt>
//------------------------------------------------------------------------------
// ODTONE - Open Dot Twenty One
//
// Copyright (C) 2009-2013 Universidade Aveiro
// Copyright (C) 2009-2013 Instituto de Telecomunicações - Pólo Aveiro
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//==============================================================================

///////////////////////////////////////////////////////////////////////////////

#include "mihfid.hpp"
#include "log.hpp"
// #include "transaction_ack_service.hpp"
// #include "transaction_manager.hpp"
#include "address_book.hpp"
#include "link_book.hpp"
#include "user_book.hpp"
#include "local_transaction_pool.hpp"
#include "transaction_pool.hpp"

#include "net_sap.hpp"
#include "message_out.hpp"
#include "transmit.hpp"
#include "service_management.hpp"
#include "event_service.hpp"
#include "command_service.hpp"
#include "information_service.hpp"
#include "service_access_controller.hpp"

#include "message_in.hpp"
#include "udp_listener.hpp"
#include "tcp_listener.hpp"

#include <odtone/base.hpp>
#include <odtone/debug.hpp>
#include <odtone/mih/config.hpp>
#include <odtone/mih/request.hpp>
#include <odtone/mih/response.hpp>
#include <odtone/mih/indication.hpp>
#include <odtone/mih/confirm.hpp>
#include <odtone/mih/types/capabilities.hpp>

#include <list>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/asio.hpp>
///////////////////////////////////////////////////////////////////////////////
// using namespace odtone::mihf;
using namespace odtone;
using namespace odtone::mihf;


namespace po = boost::program_options;

// available config options
static const char* const kConf_File                    = "conf.file";
static const char* const kConf_Receive_Buffer_Len      = "conf.recv_buff_len";
static const char* const kConf_MIHF_Id                 = "mihf.id";
static const char* const kConf_MIHF_Ip                 = "mihf.ip";
static const char* const kConf_MIHF_Peer_List          = "mihf.peers";
static const char* const kConf_MIHF_Users_List         = "mihf.users";
static const char* const kConf_MIHF_Links_List         = "mihf.links";
static const char* const kConf_MIHF_Transport_List     = "mihf.transport";
static const char* const kConf_MIHF_Remote_Port        = "mihf.remote_port";
static const char* const kConf_MIHF_Local_Port         = "mihf.local_port";
static const char* const kConf_MIHF_Link_Response_Time = "mihf.link_response_time";
static const char* const kConf_MIHF_Link_Delete        = "mihf.link_delete";
static const char* const kConf_MIHF_Discover           = "mihf.discover";
static const char* const kConf_MIHF_Multicast          = "enable_multicast";
static const char* const kConf_MIHF_Unsolicited        = "enable_unsolicited";
static const char* const kConf_MIHF_Verbosity          = "log";

uint16 kConf_MIHF_Link_Response_Time_Value;
uint16 kConf_MIHF_Link_Delete_Value;

/**
 * Remove a character from the string.
 */
void __trim(mih::octet_string &str, const char chr)
{
	str.erase(std::remove(str.begin(), str.end(), chr), str.end());
}

//
// list is a comma separated list of mihf id ip and port
//
// example: mihf2 192.168.0.1 4551
//
void set_list_peer_mihfs(mih::octet_string &list, address_book &abook)
{
	using namespace boost;

	char_separator<char> sep1(",");
	char_separator<char> sep2(" ");
	tokenizer< char_separator<char> > list_tokens(list, sep1);

	BOOST_FOREACH(mih::octet_string str, list_tokens)
	{
		tokenizer< char_separator<char> > tokens(str, sep2);
		tokenizer< char_separator<char> >::iterator it = tokens.begin();

		mih::octet_string id = *it;
		++it;
		mih::octet_string ip = *it;
		++it;
		mih::octet_string port = *it;

		uint16 port_;
		std::istringstream iss(port);
		if ((iss >> port_).fail())
			throw "invalid port";

		mih::transport_list trans;

		std::map<std::string, odtone::mih::transport_list_enum> enum_map;
		enum_map["udp"] = odtone::mih::transport_udp;
		enum_map["tcp"]	= odtone::mih::transport_tcp;

		while(it != tokens.end()) {
			if(enum_map.find(*it) != enum_map.end())
				trans.set(odtone::mih::transport_list_enum(enum_map[*it]));
			++it;
		}

		address_entry entry;
		entry.ip = ip;
		entry.port = port_;
		entry.capabilities_trans_list = trans;

		abook.add(id, entry);
	}
}

void set_users(mih::octet_string &list, user_book &ubook)
{
	using namespace boost;

	char_separator<char> sep1(",");
	char_separator<char> sep2(" ");
	tokenizer< char_separator<char> > list_tokens(list, sep1);

	BOOST_FOREACH(mih::octet_string str, list_tokens) {
		tokenizer< char_separator<char> > tokens(str, sep2);
		tokenizer< char_separator<char> >::iterator it = tokens.begin();

		mih::octet_string id = *it;
		++it;
		mih::octet_string ip("127.0.0.1");
		mih::octet_string port = *it;
		++it;

		uint16 port_;
		std::istringstream iss_port(port);
		if ((iss_port >> port_).fail())
			throw "invalid port";

		mih::mih_cmd_list supp_cmd_tmp;
		mih::iq_type_list supp_iq_tmp;
		bool has_cmd = false;
		bool has_iq = false;
		if(it != tokens.end()) {
			std::map<std::string, mih::mih_cmd_list_enum> enum_map_cmd;
			enum_map_cmd["mih_link_get_parameters"]       = mih::mih_cmd_link_get_parameters;
			enum_map_cmd["mih_link_configure_thresholds"] = mih::mih_cmd_link_configure_thresholds;
			enum_map_cmd["mih_link_actions"]              = mih::mih_cmd_link_actions;
			enum_map_cmd["mih_net_ho_candidate_query"]    = mih::mih_cmd_net_ho_candidate_query;
			enum_map_cmd["mih_net_ho_commit"]             = mih::mih_cmd_net_ho_commit;
			enum_map_cmd["mih_n2n_ho_query_resources"]    = mih::mih_cmd_n2n_ho_query_resources;
			enum_map_cmd["mih_n2n_ho_commit"]             = mih::mih_cmd_n2n_ho_commit;
			enum_map_cmd["mih_n2n_ho_complete"]           = mih::mih_cmd_n2n_ho_complete;
			enum_map_cmd["mih_mn_ho_candidate_query"]     = mih::mih_cmd_mn_ho_candidate_query;
			enum_map_cmd["mih_mn_ho_commit"]              = mih::mih_cmd_mn_ho_commit;
			enum_map_cmd["mih_mn_ho_complete"]            = mih::mih_cmd_mn_ho_complete;

			std::map<std::string, mih::iq_type_list_enum> enum_map_iq;
			enum_map_iq["iq_type_binary_data"]            = mih::iq_type_binary_data;
			enum_map_iq["iq_type_rdf_data"]               = mih::iq_type_rdf_data;
			enum_map_iq["iq_type_rdf_schema_url"]         = mih::iq_type_rdf_schema_url;
			enum_map_iq["iq_type_rdf_schema"]             = mih::iq_type_rdf_schema;
			enum_map_iq["iq_type_ie_network_type"]        = mih::iq_type_ie_network_type;
			enum_map_iq["iq_type_ie_operator_id"]         = mih::iq_type_ie_operator_id;
			enum_map_iq["iq_type_ie_service_provider_id"] = mih::iq_type_ie_service_provider_id;
			enum_map_iq["iq_type_ie_country_code"]        = mih::iq_type_ie_country_code;
			enum_map_iq["iq_type_ie_network_id"]          = mih::iq_type_ie_network_id;
			enum_map_iq["iq_type_ie_network_aux_id"]      = mih::iq_type_ie_network_aux_id;
			enum_map_iq["iq_type_ie_roaming_parteners"]   = mih::iq_type_ie_roaming_parteners;
			enum_map_iq["iq_type_ie_cost"]                = mih::iq_type_ie_cost;
			enum_map_iq["iq_type_ie_network_qos"]         = mih::iq_type_ie_network_qos;
			enum_map_iq["iq_type_ie_network_data_rate"]   = mih::iq_type_ie_network_data_rate;
			enum_map_iq["iq_type_ie_net_regult_domain"]   = mih::iq_type_ie_net_regult_domain;
			enum_map_iq["iq_type_ie_net_frequency_bands"] = mih::iq_type_ie_net_frequency_bands;
			enum_map_iq["iq_type_ie_net_ip_cfg_methods"]  = mih::iq_type_ie_net_ip_cfg_methods;
			enum_map_iq["iq_type_ie_net_capabilities"]    = mih::iq_type_ie_net_capabilities;
			enum_map_iq["iq_type_ie_net_supported_lcp"]   = mih::iq_type_ie_net_supported_lcp;
			enum_map_iq["iq_type_ie_net_mob_mgmt_prot"]   = mih::iq_type_ie_net_mob_mgmt_prot;
			enum_map_iq["iq_type_ie_net_emserv_proxy"]    = mih::iq_type_ie_net_emserv_proxy;
			enum_map_iq["iq_type_ie_net_ims_proxy_cscf"]  = mih::iq_type_ie_net_ims_proxy_cscf;
			enum_map_iq["iq_type_ie_net_mobile_network"]  = mih::iq_type_ie_net_mobile_network;
			enum_map_iq["iq_type_ie_poa_link_addr"]       = mih::iq_type_ie_poa_link_addr;
			enum_map_iq["iq_type_ie_poa_location"]        = mih::iq_type_ie_poa_location;
			enum_map_iq["iq_type_ie_poa_channel_range"]   = mih::iq_type_ie_poa_channel_range;
			enum_map_iq["iq_type_ie_poa_system_info"]     = mih::iq_type_ie_poa_system_info;
			enum_map_iq["iq_type_ie_poa_subnet_info"]     = mih::iq_type_ie_poa_subnet_info;
			enum_map_iq["iq_type_ie_poa_ip_addr"]        = mih::iq_type_ie_poa_ip_addr;

			while(it != tokens.end()) {
				if(enum_map_cmd.find(*it) != enum_map_cmd.end()) {
					supp_cmd_tmp.set((mih::mih_cmd_list_enum) enum_map_cmd[*it]);
					has_cmd = true;
				} else if(enum_map_iq.find(*it) != enum_map_iq.end()) {
					supp_iq_tmp.set((mih::iq_type_list_enum) enum_map_iq[*it]);
					has_iq = true;
				}

				++it;
			}
		}

		boost::optional<mih::mih_cmd_list> supp_cmd;
		boost::optional<mih::iq_type_list> supp_iq;

		if(has_cmd) {
			supp_cmd = supp_cmd_tmp;
		} else {
			supp_cmd_tmp.full();
			supp_cmd = supp_cmd_tmp;
		}

		if(has_iq)
			supp_iq = supp_iq_tmp;

		ubook.add(id, ip, port_, supp_cmd, supp_iq);
	}
}

void set_links(mih::octet_string &list, link_book &lbook)
{
	using namespace boost;

	char_separator<char> sep1(",");
	char_separator<char> sep2(" ");
	tokenizer< char_separator<char> > list_tokens(list, sep1);

	BOOST_FOREACH(mih::octet_string str, list_tokens) {
		tokenizer< char_separator<char> > tokens(str, sep2);
		tokenizer< char_separator<char> >::iterator it = tokens.begin();

		mih::octet_string id = *it;
		++it;
		mih::octet_string port  = *it;
		++it;
		mih::octet_string tec = *it;
		++it;
		mih::octet_string address = *it;
		mih::octet_string ip("127.0.0.1");

		uint16 port_;
		std::istringstream iss(port);
		if ((iss >> port_).fail()) {
			throw "invalid port";
		}

		if(tec == "" || address == "") {
			throw "invalid technology and/or address";
		}

		// Extract technology
		std::map<std::string, odtone::mih::link_type_enum> enum_map;
		enum_map["GSM"]           = odtone::mih::link_type_gsm;
		enum_map["GPRS"]          = odtone::mih::link_type_gprs;
		enum_map["EDGE"]          = odtone::mih::link_type_edge;
		enum_map["802_3"]         = odtone::mih::link_type_ethernet;
		enum_map["Other"]         = odtone::mih::link_type_wireless_other;
		enum_map["802_11"]        = odtone::mih::link_type_802_11;
		enum_map["CDMA2000"]      = odtone::mih::link_type_cdma2000;
		enum_map["UMTS"]          = odtone::mih::link_type_umts;
		enum_map["CDMA2000-HRPD"] = odtone::mih::link_type_cdma2000_hrpd;
		enum_map["LTE"]           = odtone::mih::link_type_lte;
		enum_map["802_16"]        = odtone::mih::link_type_802_16;
		enum_map["802_20"]        = odtone::mih::link_type_802_20;
		enum_map["802_22"]        = odtone::mih::link_type_802_22;

		mih::link_id lid;
		if(enum_map.find(tec) != enum_map.end())
			lid.type = odtone::mih::link_type(enum_map[tec]);

		// TODO: Parse the link address for all link types.
		switch(lid.type.get()) {
			case 1:
			case 2:
			case 3: {
				throw "not supported yet";
			}
			break;

			case 15:
			case 19:
			case 27:
			case 28:
			case 29: {
				mih::mac_addr mac;
				mac.address(address);
				lid.addr = mac;
			} break;

			case 18:
			case 22:
			case 23:
			case 24:
				throw "not supported yet";
			break;

			case 25: {
				++it;
				mih::octet_string plmn = *it;
				++it;
				mih::octet_string cell_id = *it;

				char_separator<char> plmn_sep(":");
				tokenizer< char_separator<char> > list_tokens(plmn, plmn_sep);

				mih::l2_3gpp_3g_cell_id lte;
				uint8 pos = 0;
				BOOST_FOREACH(mih::octet_string str, list_tokens) {
					uint8 byte = 0;
					std::istringstream iss(str);
				    iss >> std::hex >> byte;

					lte.plmn_id[pos] = byte % 0x100;
					++pos;
				}

				std::istringstream iss(port);
				if ((iss >> lte._cell_id).fail()) {
					throw "invalid cell_id";
				}

				lid.addr = lte;
			} break;
			default: {
				ODTONE_LOG(0, "Error: Invalid technology! Aborting...") ;
				throw "invalid technology";
			} break;
		}

		lbook.add(id, ip, port_, lid);
	}
}

void parse_mihf_information(mih::config &cfg, address_book &abook)
{
	using namespace boost;

	// Insert the MIHF itself in the address book
	mih::octet_string id = cfg.get<mih::octet_string>(kConf_MIHF_Id);
	mih::octet_string ip = cfg.get<mih::octet_string>(kConf_MIHF_Ip);
	uint16 port = cfg.get<uint16>(kConf_MIHF_Remote_Port);
	mih::transport_list trans;

	mih::octet_string tmp = cfg.get<mih::octet_string>(kConf_MIHF_Transport_List);
	boost::algorithm::to_lower(tmp);

	char_separator<char> sep(",");
	tokenizer< char_separator<char> > list_tokens(tmp, sep);

	std::map<std::string, odtone::mih::transport_list_enum> enum_map;
	enum_map["udp"] = odtone::mih::transport_udp;
	enum_map["tcp"]	= odtone::mih::transport_tcp;

	BOOST_FOREACH(mih::octet_string str, list_tokens) {
		__trim(str, ' ');
		if(enum_map.find(str) != enum_map.end())
			trans.set(odtone::mih::transport_list_enum(enum_map[str]));
	}

	address_entry entry;
	entry.ip = ip;
	entry.port = port;
	entry.capabilities_trans_list = trans;

	abook.add(id, entry);
}

void parse_peer_registrations(mih::config &cfg, address_book &abook)
{
	mih::octet_string mihfs	= cfg.get<mih::octet_string>(kConf_MIHF_Peer_List);

	set_list_peer_mihfs(mihfs, abook);
}

void parse_sap_registrations(mih::config &cfg, user_book &ubook, link_book &lbook)
{
	mih::octet_string users	= cfg.get<mih::octet_string>(kConf_MIHF_Users_List);
	mih::octet_string lsaps	= cfg.get<mih::octet_string>(kConf_MIHF_Links_List);

	set_users(users, ubook);
	set_links(lsaps, lbook);
}

std::vector<std::string> parse_discover_order(mih::config &cfg)
{
	using namespace boost;

	mih::octet_string order = cfg.get<mih::octet_string>(kConf_MIHF_Discover);
	std::vector<std::string> list;

	char_separator<char> sep(",");
	tokenizer< char_separator<char> > list_tokens(order, sep);

	BOOST_FOREACH(mih::octet_string str, list_tokens) {
		list.push_back(str);
	}

	return list;
}

void sm_register_callbacks(service_management &sm)
{
	sac_register_callback(mih::request::capability_discover,
			      boost::bind(&service_management::capability_discover_request,
					  boost::ref(sm), _1, _2));

	sac_register_callback(mih::response::capability_discover,
			      boost::bind(&service_management::capability_discover_response,
					  boost::ref(sm), _1, _2));

	sac_register_callback(mih::confirm::capability_discover,
			      boost::bind(&service_management::capability_discover_confirm,
					  boost::ref(sm), _1, _2));

	sac_register_callback(mih::request::mih_register,
			      boost::bind(&service_management::register_request,
					  boost::ref(sm), _1, _2));

	sac_register_callback(mih::response::mih_register,
			      boost::bind(&service_management::register_response,
					  boost::ref(sm), _1, _2));

	sac_register_callback(mih::request::mih_deregister,
			      boost::bind(&service_management::deregister_request,
					  boost::ref(sm), _1, _2));

	sac_register_callback(mih::response::mih_deregister,
			      boost::bind(&service_management::deregister_response,
					  boost::ref(sm), _1, _2));

	sac_register_callback(mih::indication::link_register,
			      boost::bind(&service_management::link_register_indication,
					  boost::ref(sm), _1, _2));

	sac_register_callback(mih::indication::user_register,
			      boost::bind(&service_management::user_register_indication,
					  boost::ref(sm), _1, _2));

}

void mies_register_callbacks(event_service &mies)
{
	sac_register_callback(mih::request::event_subscribe,
			      boost::bind(&event_service::event_subscribe_request,
					  boost::ref(mies), _1, _2));
	sac_register_callback(mih::response::event_subscribe,
			      boost::bind(&event_service::event_subscribe_response,
					  boost::ref(mies), _1,  _2));
	sac_register_callback(mih::confirm::event_subscribe,
			      boost::bind(&event_service::event_subscribe_confirm,
					  boost::ref(mies), _1,  _2));
	sac_register_callback(mih::indication::link_up,
			      boost::bind(&event_service::link_up_indication,
					  boost::ref(mies), _1, _2));
	sac_register_callback(mih::indication::link_down,
			      boost::bind(&event_service::link_down_indication,
					  boost::ref(mies), _1, _2));
	sac_register_callback(mih::indication::link_detected,
			      boost::bind(&event_service::link_detected_indication,
					  boost::ref(mies), _1, _2));
	sac_register_callback(mih::indication::link_going_down,
			      boost::bind(&event_service::link_going_down_indication,
					  boost::ref(mies), _1, _2));
	sac_register_callback(mih::indication::link_parameters_report,
			      boost::bind(&event_service::link_parameters_report_indication,
					  boost::ref(mies), _1, _2));
	sac_register_callback(mih::indication::link_handover_imminent,
			      boost::bind(&event_service::link_handover_imminent_indication,
			      boost::ref(mies), _1, _2));
	sac_register_callback(mih::indication::link_handover_complete,
			      boost::bind(&event_service::link_handover_complete_indication,
					  boost::ref(mies), _1, _2));
	sac_register_callback(mih::request::event_unsubscribe,
			      boost::bind(&event_service::event_unsubscribe_request,
			      boost::ref(mies), _1, _2));
	sac_register_callback(mih::response::event_unsubscribe,
			      boost::bind(&event_service::event_unsubscribe_response,
					  boost::ref(mies), _1, _2));
	sac_register_callback(mih::confirm::event_unsubscribe,
			      boost::bind(&event_service::event_unsubscribe_confirm,
					  boost::ref(mies), _1, _2));
}
// REGISTER(event_service::link_pdu_transmit_status_indication)

void mics_register_callbacks(command_service &mics)
{
	sac_register_callback(mih::request::link_get_parameters,
			      boost::bind(&command_service::link_get_parameters_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::link_get_parameters,
			      boost::bind(&command_service::link_get_parameters_response,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::confirm::link_get_parameters,
			      boost::bind(&command_service::link_get_parameters_confirm,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::request::link_configure_thresholds,
			      boost::bind(&command_service::link_configure_thresholds_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::link_configure_thresholds,
			      boost::bind(&command_service::link_configure_thresholds_response,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::confirm::link_configure_thresholds,
			      boost::bind(&command_service::link_configure_thresholds_confirm,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::request::link_actions,
			      boost::bind(&command_service::link_actions_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::link_actions,
			      boost::bind(&command_service::link_actions_response,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::confirm::link_actions,
			      boost::bind(&command_service::link_actions_confirm,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::request::net_ho_candidate_query,
			      boost::bind(&command_service::net_ho_candidate_query_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::net_ho_candidate_query,
			      boost::bind(&command_service::net_ho_candidate_query_response,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::request::mn_ho_candidate_query,
			      boost::bind(&command_service::mn_ho_candidate_query_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::mn_ho_candidate_query,
			      boost::bind(&command_service::mn_ho_candidate_query_response,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::request::n2n_ho_query_resources,
			      boost::bind(&command_service::n2n_ho_query_resources_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::n2n_ho_query_resources,
			      boost::bind(&command_service::n2n_ho_query_resources_response,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::request::mn_ho_commit,
			      boost::bind(&command_service::mn_ho_commit_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::mn_ho_commit,
			      boost::bind(&command_service::mn_ho_commit_response,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::request::net_ho_commit,
			      boost::bind(&command_service::net_ho_commit_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::net_ho_commit,
			      boost::bind(&command_service::net_ho_commit_response,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::request::n2n_ho_commit,
			      boost::bind(&command_service::n2n_ho_commit_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::n2n_ho_commit,
			      boost::bind(&command_service::n2n_ho_commit_response,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::request::n2n_ho_commit,
			      boost::bind(&command_service::n2n_ho_commit_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::n2n_ho_commit,
			      boost::bind(&command_service::n2n_ho_commit_response,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::request::mn_ho_complete,
			      boost::bind(&command_service::mn_ho_complete_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::mn_ho_complete,
			      boost::bind(&command_service::mn_ho_complete_response,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::request::n2n_ho_complete,
			      boost::bind(&command_service::n2n_ho_complete_request,
					  boost::ref(mics), _1, _2));
	sac_register_callback(mih::response::n2n_ho_complete,
			      boost::bind(&command_service::n2n_ho_complete_response,
					  boost::ref(mics), _1, _2));
}

void miis_register_callbacks(information_service &miis)
{
	sac_register_callback(mih::request::get_information,
			      boost::bind(&information_service::get_information_request,
					  boost::ref(miis), _1, _2));
	sac_register_callback(mih::response::get_information,
			      boost::bind(&information_service::get_information_response,
					  boost::ref(miis), _1, _2));
	sac_register_callback(mih::request::push_information,
			      boost::bind(&information_service::push_information_request,
					  boost::ref(miis), _1, _2));
	sac_register_callback(mih::indication::push_information,
			      boost::bind(&information_service::push_information_indication,
					  boost::ref(miis), _1, _2));
}

int main(int argc, char **argv)
{
	odtone::setup_crash_handler();

	boost::asio::io_service io;

	// declare MIHF supported options
	po::options_description desc(mih::octet_string("MIHF Configuration Options"));

	desc.add_options()
		("help", "Display configuration options")
		(kConf_File, po::value<std::string>()->default_value("odtone.conf"), "Configuration file")
		(kConf_Receive_Buffer_Len, po::value<uint16>()->default_value(4096), "Receive buffer length")
		(kConf_MIHF_Id, po::value<std::string>()->default_value("mihf"), "MIHF ID")
		(kConf_MIHF_Ip, po::value<std::string>()->default_value("127.0.0.1"), "MIHF IP")
		(kConf_MIHF_Remote_Port, po::value<uint16>()->default_value(4551), "Remote MIHF communication port")
		(kConf_MIHF_Local_Port, po::value<uint16>()->default_value(1025), "Local SAPs communications port")
		(kConf_MIHF_Peer_List, po::value<std::string>()->default_value(""), "List of peer MIHFs")
		(kConf_MIHF_Users_List, po::value<std::string>()->default_value(""), "List of local MIH-Users")
		(kConf_MIHF_Links_List, po::value<std::string>()->default_value(""), "List of local Links SAPs")
		(kConf_MIHF_Transport_List, po::value<std::string>()->default_value("udp"), "List of supported transport protocols")		
		(kConf_MIHF_Link_Response_Time, po::value<uint16>()->default_value(7000), "Link SAP response time (milliseconds)")
		(kConf_MIHF_Link_Delete, po::value<uint16>()->default_value(2), "Link SAP response fails to forget")
		(kConf_MIHF_Discover, po::value<std::string>()->default_value(""), "MIHF Discovery Mechanisms Order")
		(kConf_MIHF_Multicast,  "Allows multicast messages")
		(kConf_MIHF_Unsolicited,  "Allows unsolicited discovery")
		(kConf_MIHF_Verbosity, po::value<uint16>()->default_value(1), "Log level [0-4]")
	;

	odtone::mih::config cfg(desc);
	if (!cfg.parse(argc, argv, kConf_File) && !cfg.help()) {
		std::cerr <<  "Error: Couldn't open config file: " << cfg.get<std::string>(kConf_File) << std::endl;
		return EXIT_FAILURE;
	}

	if (cfg.help()) {
		std::cerr << desc << std::endl;
		return EXIT_SUCCESS;
	}

	// get command line parameters
	bool enable_multicast = (cfg.count(kConf_MIHF_Multicast) == 1);
	bool enable_unsolicited = (cfg.count(kConf_MIHF_Unsolicited) == 1);

	uint16 buff_size = cfg.get<uint16>(kConf_Receive_Buffer_Len);
	uint16 lport = cfg.get<uint16>(kConf_MIHF_Local_Port);
	uint16 rport = cfg.get<uint16>(kConf_MIHF_Remote_Port);
	mih::octet_string id = cfg.get<mih::octet_string>(kConf_MIHF_Id);
	uint16 loglevel = cfg.get<uint16>(kConf_MIHF_Verbosity);
	kConf_MIHF_Link_Response_Time_Value = cfg.get<uint16>(kConf_MIHF_Link_Response_Time);
	kConf_MIHF_Link_Delete_Value = cfg.get<uint16>(kConf_MIHF_Link_Delete);
	std::vector<std::string> dscv_order = parse_discover_order(cfg);
	//

	// set this mihf id
	mihfid_t::instance()->assign(id.c_str());
	// set log level
	ODTONE_LOG.level(loglevel);

	// create address books that stores info on how to contact mih
	// saps and peer mihfs
	address_book mihf_abook(io);
	user_book user_abook;
	link_book link_abook;
	parse_mihf_information(cfg, mihf_abook);
	parse_sap_registrations(cfg, user_abook, link_abook);
	parse_peer_registrations(cfg, mihf_abook);
	//

	// pool of pending transactions with peer mihfs
	transaction_pool	tpool(io);

	// pool of pending transactions with local mih saps (user and links)
	local_transaction_pool	lpool;

	// pool of pending capability discover requests
	link_response_pool lrpool;

	// handler for remote messages
	handler_t process_message = boost::bind(&sac_process_message, _1, _2);

	// wrapper for sending messages
	net_sap			netsap(io, mihf_abook, rport);

	// transaction manager for outgoing messages
	message_out		msgout(tpool, lpool, process_message, netsap);
	transmit		trnsmt(io, user_abook, link_abook, msgout, lport);

	// transaction manager for incoming messages
	message_in msgin(tpool, process_message, netsap);

	// sac dispatch is for handling messages from local mih saps
	// (users and links)
	sac_dispatch sacd(trnsmt);

	// handler of messages received on local port
	dispatch_t ldispatch = boost::bind(&sac_dispatch::operator(), sacd, _1);
	// handler of messages received from peer mihfs
	dispatch_t rdispatch = boost::bind(&message_in::operator(), msgin, _1);

	// create and bind to port 'lport' on loopback interface and
	// call ldispatch when a message is received
	udp_listener commhandv4(io, buff_size, ip::udp::v4(), "127.0.0.1", lport, ldispatch, true);
	udp_listener commhandv6(io, buff_size, ip::udp::v6(), "::1", lport, ldispatch, true);

	// create and bind to port rport and call rdispatch when a
	// message is received
	udp_listener remotelistener_udp(io, buff_size, ip::udp::v6(), "::", rport, rdispatch, enable_multicast);

	// create and bind to port rport and call rdispatch when a
	// message is received
	tcp_listener remotelistener_tcp(io, buff_size, ip::tcp::v6(), "::", rport, rdispatch, enable_multicast);

	// start listening on local and remote ports
	commhandv4.start();
	commhandv6.start();
	remotelistener_udp.start();

	if(mihf_abook.get(mihfid_t::instance()->to_string()).capabilities_trans_list->get(mih::transport_tcp) == 1)
		remotelistener_tcp.start();

	// instantiate mihf services
	event_service		mies(io, lpool, trnsmt, mihf_abook, link_abook, user_abook);
	command_service		mics(io, lpool, trnsmt, mihf_abook, link_abook, user_abook, lrpool);
	information_service	miis(lpool, trnsmt, user_abook);
	service_management	sm(io, lpool, link_abook, user_abook, mihf_abook, trnsmt, lrpool, dscv_order, enable_unsolicited);

	// register callbacks with service access controller
	sm_register_callbacks(sm);
	mies_register_callbacks(mies);
	mics_register_callbacks(mics);
	miis_register_callbacks(miis);
	std::cout << "Boot complete" << std::endl << std::flush;

	io.run();

	return EXIT_SUCCESS;
}
