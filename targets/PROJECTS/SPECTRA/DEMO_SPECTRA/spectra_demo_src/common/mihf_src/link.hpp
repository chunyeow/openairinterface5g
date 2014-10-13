//=============================================================================
// Brief   : MIH Link Types
// Authors : Bruno Santos <bsantos@av.it.pt>
//           Simao Reis   <sreis@av.it.pt>
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

#ifndef ODTONE_MIH_TYPES_LINK__HPP_
#define ODTONE_MIH_TYPES_LINK__HPP_

///////////////////////////////////////////////////////////////////////////////
#include <odtone/mih/types/base.hpp>
#include <odtone/mih/types/address.hpp>
#include <odtone/mih/types/qos.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace odtone { namespace mih {

///////////////////////////////////////////////////////////////////////////////
/**
 * BATT_LEVEL data type.
 */
typedef uint8 batt_level;

/**
 * NUM_COS data type.
 */
typedef uint8 num_cos;

/**
 * NUM_QUEUE data type.
 */
typedef uint8 num_queue;

/**
 * CHANNEL_ID data type.
 */
typedef uint16 channel_id;

/**
 * CONFIG_STATUS data type.
 */
typedef bool config_status;

/**
 * DEVICE_INFO data type.
 */
typedef octet_string device_info;

/**
 * LINK_AC_EX_TIME data type.
 */
typedef uint16 link_ac_ex_time;

/**
 * SIG_STRENGTH data type.
 */
typedef boost::variant<sint8, percentage> sig_strength;

/**
 * LINK_RES_STATUS data type.
 */
typedef bool link_res_status;

/**
 * MAX_DELAY data type.
 */
typedef uint16 max_delay;

/**
 * BITRATE data type.
 */
typedef uint32 bitrate;

/**
 * JITTER data type.
 */
typedef uint16 jitter;

/**
 * PKT_LOSS_RATE data type.
 */
typedef uint16 pkt_loss_rate;

/**
 * COS data type.
 */
typedef uint16 cos;

/**
 * DROP_ELIGIBILITY data type.
 */
typedef bool drop_eligibility;

/**
 * MULTICAST_ENABLE data type.
 */
typedef bool multicast_enable;

/**
 * JUMBO_ENABLE data type.
 */
typedef bool jumbo_enable;

/**
 * PORT data type.
 */
typedef uint16 port;

/**
 * MARK data type.
 */
typedef uint8 mark;

/**
 * FLOW_ID data type.
 */
typedef uint32 flow_id;

///////////////////////////////////////////////////////////////////////////////
/**
 * PROTO data type enumeration.
 */
enum proto_enum {
	proto_tcp = 0,	/**< TCP.	*/
	proto_udp = 1	/**< UDP.	*/
};

/**
 * PROTO data type.
 */
typedef enumeration<proto_enum> proto;
///////////////////////////////////////////////////////////////////////////////
/**
 * OP_MODE data type enumeration.
 */
enum op_mode_enum {
	op_mode_normal = 0,			/**< Normal mode.		*/
	op_mode_power_saving = 1,	/**< Power saving mode.	*/
	op_mode_powered_down = 2,	/**< Powered Down.		*/
};

/**
 * OP_MODE data type.
 */
typedef enumeration<op_mode_enum> op_mode;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_STATES_RSP data type.
 */
typedef boost::variant<op_mode, channel_id> link_states_rsp;

/**
 * LIST(LINK_STATES_RSP) data type.
 */
typedef std::vector<link_states_rsp> link_states_rsp_list;

/**
 * LINK_DESC_RSP data type.
 */
typedef boost::variant<num_cos, num_queue> link_desc_rsp;

/**
 * LIST(LINK_DESC_RSP) data type.
 */
typedef std::vector<link_desc_rsp> link_desc_rsp_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * DEV_STATES_REQ data type enumeration.
 */
enum dev_states_req_enum {
	dev_state_device_info = 0,	/**< Device information.	*/
	dev_state_batt_level  = 1,	/**< Battery level.			*/
};

/**
 * DEV_STATES_REQ data type.
 */
typedef bitmap<16, dev_states_req_enum> dev_states_req;

///////////////////////////////////////////////////////////////////////////////
/**
 * DEV_STATES_RSP data type.
 */
typedef boost::variant<device_info, batt_level> dev_states_rsp;

/**
 * LIST(DEV_STATES_RSP) data type.
 */
typedef std::vector<dev_states_rsp> dev_states_rsp_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_STATES_REQ data type enumeration.
 */
enum link_states_req_enum {
	link_states_req_op_mode = 0,	/**< Operation mode.		*/
	link_states_req_channel_id = 1,	/**< Channel identifier.	*/
};

/**
 * LINK_STATES_REQ data type.
 */
typedef bitmap<16, link_states_req_enum> link_states_req;

/**
 * LIST(LINK_STATES_REQ) data type.
 */
typedef std::vector<link_states_req> link_states_req_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * THRESHOLD data type.
 */
struct threshold {
	/**
	 * Threshold direction enumeration.
	 */
	enum type_ip_enum {
		above_threshold = 0,	/**< Above threshold.	*/
		below_threshold = 1,	/**< Below threshold.	*/
	};

	/**
	 * Serialize/deserialize the THRESHOLD data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & threshold_val;
		ar & threshold_x_dir;
	}

	uint16                     threshold_val;	/**< Threshold value.		*/
	enumeration<type_ip_enum>  threshold_x_dir;	/**< Threshold Direction.	*/
};
///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_TYPE data type enumeration.
 */
enum link_type_enum {
	link_type_gsm = 1,				/**< Wireless - GSM.			*/
	link_type_gprs = 2,				/**< Wireless - GPRS.			*/
	link_type_edge = 3,				/**< Wireless - EDGE.			*/

	link_type_ethernet = 15,		/**< Ethernet.					*/

	link_type_wireless_other = 18,	/**< Wireless - Other.			*/
	link_type_802_11 = 19,			/**< Wireless - IEEE 802.11.	*/

	link_type_cdma2000 = 22,		/**< Wireless - CDMA-2000.		*/
	link_type_umts = 23,			/**< Wireless - UMTS.			*/
	link_type_cdma2000_hrpd = 24,	/**< Wireless - CDMA-2000-HRPD.	*/
	link_type_lte = 25,				/**< Wireless - LTE.	*/

	link_type_802_16 = 27,			/**< Wireless - IEEE 802.16.	*/
	link_type_802_20 = 28,			/**< Wireless - IEEE 802.20.	*/
	link_type_802_22 = 29			/**< Wireless - IEEE 802.22.	*/
};

/**
 * LINK_TYPE data type.
 */
typedef enumeration<link_type_enum> link_type;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_ID data type.
 */
struct link_id {
	/**
	 * Construct an empty LINK_ID data type.
	 */
	link_id() : type(link_type_enum(0))
	{ }

	/**
	 * Serialize/deserialize the LINK_ID data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & type;
		ar & addr;
	}

	/**
	 * Check if the LINK_ID data type is equal to another LINK_ID.
	 *
	 * @param other The LINK_ID to compare to.
	 * @return True if they are equal or false otherwise.
	 */
	bool operator==(const link_id& other) const
	{
		return ((type == other.type) && (addr == other.addr));
	}

    /**
     * LINK_ID data type output.
     *
     * @param out ostream.
     * @param addr LINK_ID data type.
     * @return ostream reference.
     */    friend std::ostream& operator<<(std::ostream& out, const link_id& lid)
    {
        out << "link id " << lid.type << " " << lid.addr;
        return out;
    }


	link_type type;	/**< Link address type.	*/
	link_addr addr;	/**< Link address.		*/
};

/**
 * LIST(LINK_ID) data type.
 */
typedef std::vector<link_id> link_id_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * IP_TUPLE data type.
 */
struct ip_tuple {
	/**
	 * Serialize/deserialize the IP_TUPLE data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & ip;
		ar & port_val;
	}

	ip_addr	ip;			/**< IP address.*/
	port	port_val;	/**< Port.		*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * Auxiliar QOS data type.
 */
struct qos_sequence {
	/**
	 * Serialize/deserialize the auxiliar QOS data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & delay_val;
		ar & bitrate_val;
		ar & jitter_val;
		ar & pkt_loss_val;
	}

	max_delay		delay_val;		/**< Maximum delay.	*/
	bitrate			bitrate_val;	/**< Bitrate.		*/
	jitter			jitter_val;		/**< Jitter.		*/
	pkt_loss_rate	pkt_loss_val;	/**< Packet loss.	*/
};

/**
 * QOS data type.
 */
struct qos {
	/**
	 * Serialize/deserialize the QOS data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & value;
	}

	boost::variant<qos_sequence, cos> value;	/**< QoS value. */
};

///////////////////////////////////////////////////////////////////////////////
/**
 * Auxiliar MARK_QoS data type.
 */
struct mark_qos_sequence {
	/**
	 * Serialize/deserialize the auxiliar FLOW_ATTRIBUTE data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & mark_val;
		ar & qos_val;
	}

	mark	mark_val;	/**< Mark value.	*/
	qos		qos_val;	/**< QoS value.		*/
};

/**
 * Auxiliar MARK_DROP_ELIG data type.
 */
struct mark_drop_elig_sequence {
	/**
	 * Serialize/deserialize the auxiliar FLOW_ATTRIBUTE data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & mark_val;
		ar & drop_val;
	}

	mark				mark_val;	/**< Mark value.				*/
	drop_eligibility	drop_val;	/**< Drop eligibility value.	*/
};

/**
 * FLOW_ATTRIBUTE data type.
 */
struct flow_attribute {
	/**
	 * Serialize/deserialize the FLOW_ATTRIBUTE data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & id;
		ar & multicast;
		ar & qos_val;
		ar & drop_elig_val;
	}

	flow_id											id;				/**< Flow ID.						*/
	boost::variant<null, multicast_enable>			multicast;		/**< Multicast enable.				*/
	boost::variant<null, mark_qos_sequence>			qos_val;		/**< Mask/QoS value.				*/
	boost::variant<null, mark_drop_elig_sequence>	drop_elig_val;	/**< Mask/drop eligibility value.	*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * RESOURCE_DESC data type.
 */
struct resource_desc {
	/**
	 * Serialize/deserialize the RESOURCE_DESC data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & lid;
		ar & fid;
		ar & data_rate;
		ar & qos_val;
		ar & jumbo;
		ar & multicast;
	}

	link_id									lid;		/**< Link ID.			*/
	flow_id									fid;		/**< Flow ID.			*/
	boost::variant<null, uint32>			data_rate;	/**< Link data rate.	*/
	boost::variant<null, qos>				qos_val;	/**< QoS value.			*/
	boost::variant<null, jumbo_enable>		jumbo;		/**< Jumbo enable.		*/
	boost::variant<null, multicast_enable>	multicast;	/**< Multicast enable.	*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_AC_RESULT data type enumeration.
 */
enum link_ac_result_enum {
	link_ac_success   = 0,	/**< Success.	*/
	link_ac_failure   = 1,	/**< Failure.	*/
	link_ac_refused   = 2,	/**< Refused.	*/
	link_ac_incapable = 3,	/**< Incapable.	*/
};

/**
 * LINK_AC_RESULT data type.
 */
typedef enumeration<link_ac_result_enum> link_ac_result;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_AC_TYPE data type enumeration.
 */
enum link_ac_type_enum {
	link_ac_type_none       = 0,	/**< None.				*/
	link_ac_type_disconnect = 1,	/**< Link disconnect.	*/
	link_ac_type_low_power  = 2,	/**< Link low power.	*/
	link_ac_type_power_down = 3,	/**< Link power down.	*/
	link_ac_type_power_up   = 4,	/**< Link power up.		*/
	link_ac_type_flow_attr  = 5,	/**< Flow Attribute.	*/
	link_ac_type_link_activate_resources   = 6,	/**< Link Activate Resources.	*/
	link_ac_type_link_deactivate_resources = 7,	/**< Link Deactivate Resources.	*/
};

/**
 * LINK_AC_TYPE data type.
 */
typedef enumeration<link_ac_type_enum> link_ac_type;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_AC_ATTR data type enumeration.
 */
enum link_ac_attr_enum {
	link_ac_attr_scan         = 0,	/**< Link scan.				*/
	link_ac_attr_res_retain   = 1,	/**< Link resource retain.	*/
	link_ac_attr_data_fwd_req = 2,	/**< Link data forward.		*/
};

/**
 * LINK_AC_ATTR data type.
 */
typedef bitmap<8, link_ac_attr_enum> link_ac_attr;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_AC_PARAM data type.
 */
struct link_ac_param {
	/**
	 * Serialize/deserialize the LINK_AC_PARAM data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & param;
	}

	boost::variant<null, flow_attribute, resource_desc> param; /**< Link action parameter.*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_ACTION data type.
 */
struct link_action {
	link_ac_type type;	/**< Link action type.		*/
	link_ac_attr attr;	/**< Link action attribute.	*/
	link_ac_param param;/**< Link action parameter.	*/

	/**
	 * Serialize/deserialize the LINK_ACTION data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{;
		ar & type;
		ar & attr;
		ar & param;
	}
};

///////////////////////////////////////////////////////////////////////////////
/**
 * TH_ACTION data type enumeration.
 */
enum th_action_enum {
	th_action_normal = 0,	/**< Set normal threshold.		*/
	th_action_one_shot = 1,	/**< Set one-shot threshold.	*/
	th_action_cancel = 2	/**< Cancel threshold.			*/
};

/**
 * TH_ACTION data type.
 */
typedef enumeration<th_action_enum> th_action;


///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_DN_REASON data type enumeration.
 */
enum link_dn_reason_enum {
	link_dn_reason_explicit_disconnect = 0,		/**< Explicit disconnect.		*/
	link_dn_reason_packet_timeout = 1,			/**< Packet timeout.			*/
	link_dn_reason_no_resource = 2,				/**< No resource.				*/
	link_dn_reason_no_broadcast = 3,			/**< No broadcast.				*/
	link_dn_reason_authentication_failure = 4,	/**< Authentication failure.	*/
	link_dn_reason_billing_failure = 5,			/**< Billing failure.			*/
};

/**
 * LINK_DN_REASON data type.
 */
typedef enumeration<link_dn_reason_enum> link_dn_reason;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_GD_REASON data type enumeration.
 */
enum link_gd_reason_enum {
	link_gd_reason_explicit_disconnect = 0,			/**< Explicit disconnect.		*/
	link_gd_reason_link_parameter_degrading = 1,	/**< Link parameter degrading.	*/
	link_gd_reason_low_power = 2,					/**< Low power.					*/
	link_gd_reason_no_resource = 3,					/**< No resource.				*/
};

/**
 * LINK_GD_REASON data type.
 */
typedef enumeration<link_gd_reason_enum> link_gd_reason;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_MIHCAP_FLAG data type enumeration.
 */
enum link_mihcap_flag_enum {
	link_mihcap_event_service = 1,			/**< Event service.			*/
	link_mihcap_command_service = 2,		/**< Command service.		*/
	link_mihcap_information_service = 3,	/**< Information service.	*/
};

/**
 * LINK_MIHCAP_FLAG data type.
 */
typedef bitmap<8, link_mihcap_flag_enum> link_mihcap_flag;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_GEN data type enumeration.
 */
enum link_param_gen_enum {
	link_param_gen_data_rate = 0,			/**< Data rate.			*/
	link_param_gen_signal_strength = 1,		/**< Signal strength.	*/
	link_param_gen_sinr = 2,				/**< SINR.				*/
	link_param_gen_throughput = 3,			/**< Throughput.		*/
	link_param_gen_packet_error_rate = 4,	/**< Packet error rate.	*/
};

/**
 * LINK_PARAM_GEM data type.
 */
typedef enumeration<link_param_gen_enum> link_param_gen;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_QOS data type.
 */
typedef uint8 link_param_qos;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_GG data type enumeration.
 */
enum link_param_gg_enum {
	link_param_gg_rx_qual = 0,		/**< RxQual.	*/
	link_param_gg_rs_lev = 1,		/**< RsLev.		*/
	link_param_gg_mean_bep = 2,		/**< Mean BEP.	*/
	link_param_gg_st_dev_bep = 3,	/**< StDev BEP.	*/
};

/**
 * LINK_PARAM_GG data type.
 */
typedef enumeration<link_param_gg_enum> link_param_gg;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_EDGE data type enumeration.
 */
enum link_param_edge_enum {
};

/**
 * LINK_PARAM_EDGE data type.
 */
typedef enumeration<link_param_edge_enum> link_param_edge;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_ETH data type enumeration.
 */
enum link_param_eth_enum {
};

/**
 * LINK_PARAM_ETH data type.
 */
typedef enumeration<link_param_eth_enum> link_param_eth;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_802_11 data type enumeration.
 */
enum link_param_802_11_enum {
	link_param_802_11_rssi = 0,		/**< RSSI.						*/
	link_param_802_11_no_qos = 1,	/**< No QoS resource available.	*/
	link_param_802_11_multicast_packet_loss_rate = 2,	/**< Multicast packet loss rate.*/
};

/**
 * LINK_PARAM_802_11 data type.
 */
typedef enumeration<link_param_802_11_enum> link_param_802_11;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_C2K data type enumeration.
 */
enum link_param_c2k_enum {
	link_param_c2k_pilot_strength = 0,	/**< Pilot strength.	*/
};

/**
 * LINK_PARAM_C2K data type.
 */
typedef enumeration<link_param_c2k_enum> link_param_c2k;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_FFD data type enumeration.
 */
enum link_param_ffd_enum {
	link_param_ffd_cpich_rscp = 0,				/**< CPICH RSCP.			*/
	link_param_ffd_pccpch_rscp = 1,				/**< PCCPCH RSCP.			*/
	link_param_ffd_ultra_carrie_rssi = 2,		/**< UTRA carrier RSSI.		*/
	link_param_ffd_gsm_carrie_rssi = 3,			/**< GSM carrier RSSI.		*/
	link_param_ffd_cpich_ec_no = 4,				/**< CPICH Ec/No.			*/
	link_param_ffd_transport_channel_bler = 5,	/**< Transport channel BLER.*/
	link_param_ffd_ue = 6,						/**< UE transmitted power.	*/
};

/**
 * LINK_PARAM_FFD data type.
 */
typedef enumeration<link_param_ffd_enum> link_param_ffd;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_HRPD data type enumeration.
 */
enum link_param_hrpd_enum {
	link_param_hrpd_pilot_strength = 0,	/**< HRPD pilot strength.	*/
};

/**
 * LINK_PARAM_HRPD data type.
 */
typedef enumeration<link_param_hrpd_enum> link_param_hrpd;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_802_16 data type enumeration.
 */
enum link_param_802_16_enum {
};

/**
 * LINK_PARAM_802_16 data type.
 */
typedef enumeration<link_param_802_16_enum> link_param_802_16;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_802_20 data type enumeration.
 */
enum link_param_802_20_enum {
};

/**
 * LINK_PARAM_802_20 data type.
 */
typedef enumeration<link_param_802_20_enum> link_param_802_20;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_802_22 data type enumeration.
 */
enum link_param_802_22_enum {
};

/**
 * LINK_PARAM_802_22 data type.
 */
typedef enumeration<link_param_802_22_enum> link_param_802_22;


///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_lte data type enumeration.
 */
enum link_param_lte_enum {
	link_param_lte_rsrp = 0,		/**< RSSI.						*/
	link_param_lte_rsrq = 1,		/**< No QoS resource available.	*/
	link_param_lte_cqi = 2,			/**< Multicast packet loss rate.*/
	link_param_lte_bandwidth = 3,	/**< System Load.					*/
	link_param_lte_pkt_delay = 4,	/**< Number of registered users.	*/
	link_param_lte_pkt_loss = 5,	/**< Number of active users.		*/
	link_param_lte_l2_buffer = 6,	/**< Congestion windows of users.	*/
	link_param_lte_MN_cap = 7,		/**< Congestion windows of users.	*/
	link_param_lte_embms = 8,		/**< Congestion windows of users.	*/
	link_param_lte_jumbo_feasibility = 9,	/**< Congestion windows of users.	*/
	link_param_lte_jumbo_setup = 10,		/**< Congestion windows of users.	*/
	link_param_lte_active_embms = 11,		/**< Transmission rate of users.	*/
	link_param_lte_link_congestion = 12,	/**< Link congestion.	*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_LTE data type.
 */
typedef enumeration<link_param_lte_enum> link_param_lte;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_TUPLE_ID data type.
 */
class link_tuple_id : public link_id {
public:
	/**
	 * Construct an empty LINK_TUPLE_ID data type.
	 */
	link_tuple_id() : poa_addr(null())
	{ }

	/**
	 * Serialize/deserialize the LINK_TUPLE_ID data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		link_id::serialize(ar);
		ar & poa_addr;
	}

	/**
	 * Check if the LINK_TUPLE_ID data type is equal to another LINK_TUPLE_ID.
	 *
	 * @param other The LINK_TUPLE_ID to compare to.
	 * @return True if they are equal or false otherwise.
	 */
	bool operator==(const link_tuple_id& other) const
	{
		return ((static_cast<const link_id&>(*this) == static_cast<const link_id&>(other))
				&& (addr == other.addr));
	}

	boost::variant<null, link_addr> poa_addr;	/**< PoA link address.	*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_SCAN_RSP data type.
 */
struct link_scan_rsp {
	link_addr    id;		/**< Link address.			*/
	octet_string net_id;	/**< Network identifier.	*/
	sig_strength signal;	/**< Signal strength.		*/

	/**
	 * Serialize/deserialize the LINK_SCAN_RSP data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & id;
		ar & net_id;
		ar & signal;
	}
};

/**
 * LIST(LINK_SCAN_RSP) data type.
 */
typedef std::vector<link_scan_rsp> link_scan_rsp_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_ACTION_REQ data type.
 */
struct link_action_req {
	link_id                         id;			/**< Link identifier.			*/
	boost::variant<null, link_addr> addr;		/**< PoA link address.			*/
	link_action                     action;		/**< Link action.				*/
	link_ac_ex_time                 ex_time;	/**< Link action execution time.*/

	/**
	 * Serialize/deserialize the LINK_ACTION_REQ data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & id;
		ar & addr;
		ar & action;
		ar & ex_time;
	}
};

/**
 * LIST(LINK_ACTION_REQ) data type.
 */
typedef std::vector<link_action_req> link_action_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_ACTION_RSP data type.
 */
struct link_action_rsp {
	link_id                                  id;		/**< Link identifier.	*/
	link_ac_result                           result;	/**< Link action result.*/
	boost::variant<null, link_scan_rsp_list> scan_list;	/**< Link action response list.*/

	/**
	 * Serialize/deserialize the LINK_ACTION_RSP data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & id;
		ar & result;
		ar & scan_list;
	}
};

/**
 * LIST(LINK_ACTION_RSP) data type.
 */
typedef std::vector<link_action_rsp> link_action_rsp_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_EVT_LIST data type enumeration.
 */
enum link_evt_list_enum {
	evt_link_detected = 0,				/**< Link detected				*/
	evt_link_up = 1,					/**< Link up					*/
	evt_link_down = 2,					/**< Link down					*/
	evt_link_parameters_report = 3,		/**< Link parameters report		*/
	evt_link_going_down = 4,			/**< Link doing down			*/
	evt_link_handover_imminent = 5,		/**< Link handover imminent		*/
	evt_link_handover_complete = 6,		/**< Link handover complete		*/
	evt_link_pdu_transmit_status = 7,	/**< Link PDU transmit status	*/
};

/**
 * LINK_EVT_LIST data type.
 */
typedef bitmap<32, link_evt_list_enum> link_evt_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_CMD_LIST data type enumeration.
 */
enum link_cmd_list_enum {
	cmd_link_event_subscribe      = 1,	/**< Event subscribe.		*/
	cmd_link_event_unsubscribe    = 2,	/**< Event unsubscribe.		*/
	cmd_link_get_parameters       = 3,	/**< Get parameters.		*/
	cmd_link_configure_thresholds = 4,	/**< Configure thresholds.	*/
	cmd_link_action               = 5,	/**< Action.				*/
};

/**
 * LINK_CMD_LIST data type.
 */
typedef bitmap<32, link_cmd_list_enum> link_cmd_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_DET_CFG data type.
 */
struct link_det_cfg {
	/**
	 * Serialize/deserialize the LINK_DET_CFG data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & network_id;
		ar & signal;
		ar & link_data_rate;
	}

	boost::variant<null, octet_string> network_id;		/**< Network identifier.*/
	boost::variant<null, sig_strength> signal;			/**< Signal strength.	*/
	boost::variant<null, uint32>       link_data_rate;	/**< Link data rate.	*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_TYPE data type.
 */
typedef boost::variant<link_param_gen,
			link_param_qos,
			link_param_gg,
			link_param_edge,
			link_param_eth,
			link_param_802_11,
			link_param_c2k,
			link_param_ffd,
			link_param_hrpd,
			link_param_802_16,
			link_param_802_20,
			link_param_802_22,
			link_param_lte> link_param_type;

/**
 * LIST(LINK_PARAM_TYPE) data type.
 */
typedef std::vector<link_param_type> link_param_type_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_DESC_REQ_LIST data type enumeration.
 */
enum link_desc_req_enum {
	link_desc_req_classes_of_service_supported = 0,	/**< Number of classes of services supported.*/
	link_desc_req_queues_supported = 1,				/**< Number of queues supported.*/
};

/**
 * LINK_DESC_REQ data type.
 */
typedef bitmap<16, link_desc_req_enum> link_desc_req;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_STATUS_REQ data type.
 */
struct link_status_req {
	link_states_req      _states_req;		/**< Link states to be requested.*/
	link_param_type_list _param_type_list;	/**< Link parameter type list.	 */
	link_desc_req        _desc_req;			/**< Link dsecriptors.	*/

	/**
	 * Serialize/deserialize the LINK_STATUS_REQ data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & _states_req;
		ar & _param_type_list;
		ar & _desc_req;
	}
};

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_VAL data type.
 */
typedef uint16 link_param_val;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM data type.
 */
struct link_param {
	/**
	 * Serialize/deserialize the LINK_PARAM data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & type;
		ar & value;
	}

	link_param_type type;					/**< Link parameter type.	*/
	boost::variant<link_param_val,
	               qos_param_val> value;	/**< Link parameter value.	*/
};

/**
 * LINK_PARAM_LIST data type.
 */
typedef std::vector<link_param> link_param_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_PARAM_RPT data type.
 */
struct link_param_report {
	/**
	 * Construct a LINK_PARAM_RPT data type.
	 */
	link_param_report() : thold(null())
	{ }

	/**
	 * Serialize/deserialize the LINK_PARAM_RPT data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & param;
		ar & thold;
	}


	link_param                      param;	/**< Link parameter.	*/
	boost::variant<null, threshold> thold;	/**< Threshold.			*/
};

/**
 * LIST(LINK_PARAM_RPT) data type.
 */
typedef std::vector<link_param_report> link_param_rpt_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_CFG_PARAM data type.
 */
struct link_cfg_param {
	/**
	 * Serialize/deserialize the LINK_CFG_PARAM data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & type;
		ar & timer_interval;
		ar & action;
		ar & threshold_list;
	}

	link_param_type              type;				/**< Link parameter type.*/
	boost::variant<null, uint16> timer_interval;	/**< Timer interval.	 */
	th_action                    action;			/**< Action to apply to the listed thresholds.*/
	std::vector<threshold>       threshold_list;	/**< Thresholds.		 */
};

/**
 * LIST(LINK_CFG_PARAM) data type.
 */
typedef std::vector<link_cfg_param> link_cfg_param_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_CFG_STATUS data type.
 */
struct link_cfg_status {
	link_param_type type;	/**< Link parameter type.					*/
	threshold       thold;	/**< Threshold.								*/
	config_status   status;	/**< Link parameter configuration status.	*/

	/**
	 * Serialize/deserialize the LINK_CFG_STATUS data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & type;
		ar & thold;
		ar & status;
	}
};

/**
 * LIST(LINK_CFG_STATUS) data type.
 */
typedef std::vector<link_cfg_status> link_cfg_status_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * NET_CAPS data type enumeration.
 */
enum net_caps_enum {
	net_caps_security = 0,				/**< Security.				*/
	net_caps_qos_0 = 1,					/**< QoS Class 0.			*/
	net_caps_qos_1 = 2,					/**< QoS Class 1.			*/
	net_caps_qos_2 = 3,					/**< QoS Class 2.			*/
	net_caps_qos_3 = 4,					/**< QoS Class 3.			*/
	net_caps_qos_4 = 5,					/**< QoS Class 4.			*/
	net_caps_qos_5 = 6,					/**< QoS Class 5.			*/
	net_caps_internet = 7,				/**< Internet access.		*/
	net_caps_emergency_services = 8,	/**< Emergency services.	*/
	net_caps_mih = 9,					/**< MIH capability.		*/
};

/**
 * NET_CAPS data type.
 */
typedef bitmap<32, net_caps_enum> net_caps;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_DET_INFO data type.
 */
struct link_det_info {
	/**
	 * Serialize/deserialize the LINK_DET_INFO data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & id;
		ar & network_id;
		ar & net_aux_id;
		ar & signal;
		ar & sinr;
		ar & data_rate;
		ar & mih_capabilities;
		ar & net_capabilities;
	}

	link_tuple_id    id;				/**< Link tuple identifier.			*/
	octet_string     network_id;		/**< Network identifier.			*/
	octet_string     net_aux_id;		/**< Auxiliar network identifier.	*/
	sig_strength     signal;			/**< Signal strength.				*/
	uint16           sinr;				/**< SINR.							*/
	uint32           data_rate;			/**< Link data rate.				*/
	link_mihcap_flag mih_capabilities;	/**< MIH capabilities.				*/
	net_caps         net_capabilities;	/**< Network capabilities.			*/
};

/**
 * LIST(LINK_DET_INFO) data type.
 */
typedef std::vector<link_det_info> link_det_info_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_POA_LIST data type.
 */
struct link_poa_list {
	/**
	 * Serialize/deserialize the LINK_POA_LIST data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & id;
		ar & addr_list;
	}

	link_id        id;			/**< Link identifier.	*/
	link_addr_list addr_list;	/**< Link address list.	*/
};

/**
 * LIST(LINK_POA_LIST) data type.
 */
typedef std::vector<link_poa_list> list_of_link_poa_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_STATUS_RSP data type.
 */
struct link_status_rsp {
	/**
	 * Serialize/deserialize the LINK_STATUS_RSP data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & states_rsp_list;
		ar & param_list;
		ar & desc_rsp_list;
	}


	link_states_rsp_list states_rsp_list;	/**< Link states response list.		*/
	link_param_list      param_list;		/**< Link parameter list.			*/
	link_desc_rsp_list   desc_rsp_list;		/**< Link descriptors response list.*/
};

/**
 * LIST(LINK_STATUS_RSP) data type.
 */
typedef std::vector<link_status_rsp> link_status_rsp_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * SEQUENCE(LINK_ID,LINK_STATUS_RSP) data type.
 */
struct status_rsp {
	/**
	 * Serialize/deserialize the SEQUENCE(LINK_ID,LINK_STATUS_RSP) data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & id;
		ar & rsp;
	}

	link_id         id;		/**< Link identifier.		*/
	link_status_rsp rsp;	/**< Link status response.	*/
};

/**
 * LIST(SEQUENCE(LINK_ID,LINK_STATUS_RSP)) data type.
 */
typedef std::vector<status_rsp> status_rsp_list;

///////////////////////////////////////////////////////////////////////////////
} /* namespace mih */ } /*namespace odtone */

// EOF ////////////////////////////////////////////////////////////////////////
#endif /* ODTONE_MIH_TYPES_LINK__HPP_ */
