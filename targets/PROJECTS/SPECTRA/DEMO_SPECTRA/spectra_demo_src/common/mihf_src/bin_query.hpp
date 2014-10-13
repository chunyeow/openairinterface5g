//=============================================================================
// Brief   : MIH Binary Query Types
// Authors : Bruno Santos <bsantos@av.it.pt>
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

#ifndef ODTONE_MIH_TYPES_BIN_QUERY__HPP_
#define ODTONE_MIH_TYPES_BIN_QUERY__HPP_

///////////////////////////////////////////////////////////////////////////////
#include <odtone/mih/types/base.hpp>
#include <odtone/mih/types/information.hpp>
#include <odtone/mih/types/location.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace odtone { namespace mih {

///////////////////////////////////////////////////////////////////////////////
typedef cost_curr 				curr_pref;	/**< CURR_PREG data type value.	 */
typedef std::vector<network_id> netwk_inc;	/**< NETWK_INC data type value.	 */
typedef uint32 					nghb_radius;/**< NGHB_RADIUS data type value.*/
typedef uint32                  ie_type;	/**< IE_TYPE data type value.*/
typedef std::vector<ie_type>    rpt_templ;	/**< RTP_TEMPL data type value.*/

///////////////////////////////////////////////////////////////////////////////
/**
 * NET_TYPE_INC data type enumeration.
 */
enum net_type_inc_enum {
	net_type_inc_gsm = 0,				/**< GSM			*/
	net_type_inc_gprs = 1,				/**< GPRS			*/
	net_type_inc_edge = 2,				/**< EDGE			*/
	net_type_inc_ethernet = 3,			/**< Ethernet		*/
	net_type_inc_wireless_other = 4,	/**< Wireless other	*/
	net_type_inc_ieee802_11 = 5,		/**< IEEE 802.11	*/
	net_type_inc_cdma2000 = 6,			/**< CDMA 2000		*/
	net_type_inc_umts = 7,				/**< UMTS			*/
	net_type_inc_cdma2000_hrpd = 8,		/**< CDMA 2000 HRPD	*/
	net_type_inc_ieee802_16 = 9,		/**< IEEE 802.16	*/
	net_type_inc_ieee802_20 = 10,		/**< IEEE 802.20	*/
	net_type_inc_ieee802_22 = 11,		/**< IEEE 802.22	*/
	net_type_inc_lte = 12,				/**< LTE			*/
};

/**
 * NET_TYPE_INC data type.
 */
typedef bitmap<32, net_type_inc_enum> net_type_inc;

///////////////////////////////////////////////////////////////////////////////
/**
 * QUERIER_LOC data type.
 */
struct querier_loc {
	/**
	 * Construct an empty QUERIER_LOC data type.
	 */
	querier_loc() : _location(null()), _link_addr(null()), _nghb_radius(null())
	{ }

	/**
	 * Serialize/deserialize the QUERIER_LOC data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & _location;
		ar & _link_addr;
		ar & _nghb_radius;
	}

	boost::variant<null, location>    _location;	/**< Location information.	*/
	boost::variant<null, link_addr>   _link_addr;	/**< Link address.			*/
	boost::variant<null, nghb_radius> _nghb_radius;	/**< Radius from the center point of
														 querier’s location.	*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * RPT_LIMIT data type.
 */
struct rpt_limit {
	/**
	 * Construct an empty RPT_LIMIT data type.
	 */
	rpt_limit() : _max_ies(0), _start_entry(0)
	{ }

	/**
	 * Serialize/deserialize the RPT_LIMIT data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & _max_ies;
		ar & _start_entry;
	}

	uint16 _max_ies;		/**< Maximum number of IEs.	*/
	uint16 _start_entry;	/**< Start entry.	*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * IQ_BIN_DATA data type.
 */
struct iq_bin_data {
	/**
	 * Construct an empty IQ_BIN_DATA data type.
	 */
	iq_bin_data()
		: _querier_loc(null()), _net_type_inc(null()), _netwk_inc(null()),
		_rpt_templ(null()), _rpt_limit(null()), _currency(null())
	{ }

	/**
	 * Serialize/deserialize the IQ_BIN_DATA data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & _querier_loc;
		ar & _net_type_inc;
		ar & _netwk_inc;
		ar & _rpt_templ;
		ar & _rpt_limit;
		ar & _currency;
	}

	boost::variant<null, querier_loc>  _querier_loc;	/**< Querier's location.*/
	boost::variant<null, net_type_inc> _net_type_inc;	/**< Set of link types.	*/
	boost::variant<null, netwk_inc>    _netwk_inc;		/**< List of network identifiers.*/
	boost::variant<null, rpt_templ>    _rpt_templ;		/**< List of IEs types.	*/
	boost::variant<null, rpt_limit>    _rpt_limit;		/**< Report limitation.	*/
	boost::variant<null, curr_pref>    _currency;		/**< Currency preference.*/
};

/**
 * LIST(IQ_BIN_DATA) data type.
 */
typedef std::vector<iq_bin_data> iq_bin_data_list;

///////////////////////////////////////////////////////////////////////////////
} /* namespace mih */ } /*namespace odtone */

// EOF ////////////////////////////////////////////////////////////////////////
#endif /* ODTONE_MIH_TYPES_BIN_QUERY__HPP_ */
