//=============================================================================
// Brief   : MIH Address Types
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

#ifndef ODTONE_MIH_TYPES_ADDRESS__HPP_
#define ODTONE_MIH_TYPES_ADDRESS__HPP_

///////////////////////////////////////////////////////////////////////////////
#include <odtone/mih/types/base.hpp>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
namespace odtone { namespace mih {

///////////////////////////////////////////////////////////////////////////////
typedef uint32   cell_id;	/**< CELL_ID data type.		*/
typedef uint16   lac;		/**< LAC data type.			*/
typedef uint16   ci;		/**< CI data type.			*/

///////////////////////////////////////////////////////////////////////////////
/**
 * TRANSPORT_ADDR data type.
 */
class transport_addr {
protected:
	/**
	 * Construct a TRANSPORT_ADDR data type.
	 *
	 * @param type Transport address type.
	 */
	transport_addr(uint16 type) : _type(type)
	{ }

	/**
	 * Construct a TRANSPORT_ADDR data type.
	 *
	 * @param type Transport address type.
	 * @param raw Raw bytes of the transport address.
	 * @param len Size of the transport address raw bytes.
	 */
	transport_addr(uint16 type, const void* raw, size_t len)
		: _type(type), _addr(reinterpret_cast<const char*>(raw), len)
	{ }

public:

	/**
	 * Get the TRANSPORT_ADDR type.
	 *
	 * @return The transport address type.
	 */
	uint16 type() const
	{
		return _type;
	}

	/**
	 * Set the TRANSPORT_ADDR type.
	 *
	 * @param type The transport address type.
	 */
	void type(const uint16 type)
	{
		_type = type;
	}

	/**
	 * Get the pointer to an array of characters which contains the
	 * transport address.
	 *
	 * @return Pointer to an internal array containing the transport address.
	 */
	const void* get() const
	{
		return _addr.data();
	}

	/**
	 * Get the lenght of the transport address string.
	 *
	 * @return The lenght of the transport address string.
	 */
	size_t length() const
	{
		return _addr.length();
	}

	/**
	 * Serialize/deserialize the TRANSPORT_ADDR data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & _type;
		ar & _addr;
	}

protected:
	uint16       _type;	/**< Transport address type.	*/
	octet_string _addr;	/**< Transport address string.	*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * TRANSPORT_TYPE data type enumeration.
 */
enum transport_type_enum {
	l2           = 0,		/**< Layer 2 transport address.				*/
	l3_or_higher = 1,		/**< Layer 3 or higher transport address.	*/
};

/**
 * TRANSPORT_TYPE data type.
 */
typedef enumeration<transport_type_enum> transport_type; 

///////////////////////////////////////////////////////////////////////////////
/**
 * MAC_ADDR data type.
 */
class mac_addr : public transport_addr {
public:
	/**
	 * Construct an empty MAC_ADDR data type.
	 * @note Transport address type = 6
	 */
	mac_addr() : transport_addr(6)
	{ }

	/**
	 * Construct an empty MAC_ADDR data type.
	 * @note Transport address type = 6
	 *
	 * @param addr address MAC address string (format: XX:XX:XX:XX:XX:XX).
	 */
	explicit mac_addr(const octet_string& addr) : transport_addr(6)
	{
		this->address(addr);
	}

	/**
	 * Construct an empty MAC_ADDR data type.
	 * @note Transport address type = 6
	 *
	 * @param raw Raw bytes of the MAC address.
	 * @param len Lenght of the MAC address raw bytes.
	 */
	mac_addr(const void* raw, size_t len) : transport_addr(6, raw, len)
	{ }

	/**
	 * Get the MAC address string (format: XX:XX:XX:XX:XX:XX).
	 *
	 * @return The MAC address string.
	 */
	octet_string address() const;

	/**
	 * Set the MAC address.
	 *
	 * @param addr The MAC address string (format: XX:XX:XX:XX:XX:XX).
	 */
	void address(const octet_string& addr);

	/**
	 * MAC_ADDR data type output.
	 *
	 * @param out ostream.
	 * @param tp MAC_ADDR data type.
	 * @return ostream reference.
	 */
	friend std::ostream& operator<<(std::ostream& out, const mac_addr& tp)
	{
		out << "\ntype: " << tp.type();
		out << "\naddress: " << tp.address();

		return out;
	}

	/**
	 * Check if the MAC_ADDR is equal to another MAC_ADDR.
	 *
	 * @param other The MAC_ADDR to compare with.
	 * @return True if they are equal or false otherwise.
	 */
	bool operator==(const mac_addr& other) const
	{
		return ((type() == other.type()) && (address().compare(other.address()) == 0));
	}
};

///////////////////////////////////////////////////////////////////////////////
/**
 * L2_3GPP_2G_CELL_ID data type.
 */
struct l2_3gpp_2g_cell_id {
	uint8  plmn_id[3];	/**< PLMN_ID data type.	*/
	lac    _lac;		/**< LAC data type.		*/
	ci     _ci;			/**< CI data type.		*/

	/**
	 * Serialize/deserialize the L2_3GPP_2G_CELL_ID data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & plmn_id[0];
		ar & plmn_id[1];
		ar & plmn_id[2];
		ar & _lac;
		ar & _ci;
	}

	/**
	 * L2_3GPP_2G_CELL_ID data type output.
	 *
	 * @param out ostream.
	 * @param addr L2_3GPP_2G_CELL_ID data type.
	 * @return ostream reference.
	 */
	friend std::ostream& operator<<(std::ostream& out, const l2_3gpp_2g_cell_id& addr)
	{
        out << "l2_3gpp_2g_cell_id: CI:" << addr._ci << " LAC:" << addr._lac << " PLMN id:" << addr.plmn_id[0]<< addr.plmn_id[1]<< addr.plmn_id[2];
		return out;
	}

	/**
	 * Check if the L2_3GPP_2G_CELL_ID is equal to another L2_3GPP_2G_CELL_ID.
	 *
	 * @param other The L2_3GPP_2G_CELL_ID to compare with.
	 * @return True if they are equal or false otherwise.
	 */
	bool operator==(const l2_3gpp_2g_cell_id& other) const
	{
        std::cerr << "l2_3gpp_2g_cell_id()==" << std::endl;
        std::cerr << "_lac " << _lac << " " << other._lac << std::endl;
        std::cerr << "_ci " << _ci << " " << other._ci << std::endl;
        std::cerr << "plmn_id " << plmn_id[0] << " " << other.plmn_id[0] << std::endl;
        std::cerr << "plmn_id " << plmn_id[1] << " " << other.plmn_id[1] << std::endl;
        std::cerr << "plmn_id " << plmn_id[2] << " " << other.plmn_id[2] << std::endl;
		return ((_lac == other._lac)
				&& (_ci == other._ci)
				&& (plmn_id[0] == other.plmn_id[0])
				&& (plmn_id[1] == other.plmn_id[1])
				&& (plmn_id[2] == other.plmn_id[2]));
	}
};

/**
 * L2_3GPP_3G_CELL_ID data type.
 */
struct l2_3gpp_3g_cell_id {
	uint8    plmn_id[3];	/**< PLMN_ID data type.	*/
	cell_id  _cell_id;		/**< CELL_ID data type.	*/

	/**
	 * Serialize/deserialize the L2_3GPP_3G_CELL_ID data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & plmn_id[0];
		ar & plmn_id[1];
		ar & plmn_id[2];
		ar & _cell_id;
	}

	/**
	 * L2_3GPP_3G_CELL_ID data type output.
	 *
	 * @param out ostream.
	 * @param addr L2_3GPP_3G_CELL_ID data type.
	 * @return ostream reference.
	 */
	friend std::ostream& operator<<(std::ostream& out, const l2_3gpp_3g_cell_id& addr)
	{
        out << "l2_3gpp_3g_cell_id: Cell id:" << addr._cell_id << " PLMN id:" << addr.plmn_id[0]<< addr.plmn_id[1]<< addr.plmn_id[2];
		return out;
	}

	/**
	 * Check if the L2_3GPP_3G_CELL_ID is equal to another L2_3GPP_3G_CELL_ID.
	 *
	 * @param other The L2_3GPP_3G_CELL_ID to compare with.
	 * @return True if they are equal or false otherwise.
	 */
	bool operator==(const l2_3gpp_3g_cell_id& other) const
	{
        std::cerr << "l2_3gpp_3g_cell_id()==" << std::endl;
        std::cerr << "_cell_id " << _cell_id << " " << other._cell_id << std::endl;
        std::cerr << "plmn_id " << plmn_id[0] << " " << other.plmn_id[0] << std::endl;
        std::cerr << "plmn_id " << plmn_id[1] << " " << other.plmn_id[1] << std::endl;
        std::cerr << "plmn_id " << plmn_id[2] << " " << other.plmn_id[2] << std::endl;
		return ((_cell_id == other._cell_id)
				&& (plmn_id[0] == other.plmn_id[0])
				&& (plmn_id[1] == other.plmn_id[1])
				&& (plmn_id[2] == other.plmn_id[2]));
	}
};

///////////////////////////////////////////////////////////////////////////////
/**
 * L2_3GPP_ADDR data type.
 */
struct l2_3gpp_addr  {
	/**
	 * Serialize/deserialize the L2_3GPP_ADDR data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & value;
	}

	/**
	 * L2_3GPP_ADDR data type output.
	 *
	 * @param out ostream.
	 * @param addr L2_3GPP_ADDR data type.
	 * @return ostream reference.
	 */
	friend std::ostream& operator<<(std::ostream& out, const l2_3gpp_addr& addr)
	{
        out << "l2_3gpp_addr " << addr.value;
		return out;
	}

	/**
	 * Check if the L2_3GPP_ADDR is equal to another L2_3GPP_ADDR.
	 *
	 * @param other The L2_3GPP_ADDR to compare with.
	 * @return True if they are equal or false otherwise.
	 */
	bool operator==(const l2_3gpp_addr& other) const
	{
		return ((value.compare(other.value) == 0));
	}

	octet_string value;	/**< L2_3GPP_ADDR data type.	*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * Define L2_3GPP2_ADDR data type.
 */
struct l2_3gpp2_addr {
	/**
	 * Serialize/deserialize the L2_3GPP2_ADDR data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & value;
	}

	/**
	 * L2_3GPP2_ADDR data type output.
	 *
	 * @param out ostream.
	 * @param addr L2_3GPP2_ADDR data type.
	 * @return ostream reference.
	 */
	friend std::ostream& operator<<(std::ostream& out, const l2_3gpp2_addr& addr)
	{
        out << "l2_3gpp2_addr:" << addr.value;
		return out;
	}

	/**
	 * Check if the L2_3GPP2_ADDR is equal to another L2_3GPP2_ADDR.
	 *
	 * @param other The L2_3GPP2_ADDR to compare with.
	 * @return True if they are equal or false otherwise.
	 */
	bool operator==(const l2_3gpp2_addr& other) const
	{
		return ((value == other.value));
	}

	octet_string value;	/**< L2_3GPP2_ADDR data type.	*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * Define OTHER_L2_ADDR data type.
 */
struct other_l2_addr {
	/**
	 * Serialize/deserialize the OTHER_L2_ADDR data type.
	 *
	 * @param ar The archive to/from where serialize/deserialize the data type.
	 */
	template<class ArchiveT>
	void serialize(ArchiveT& ar)
	{
		ar & value;
	}

	/**
	 * OTHER_L2_ADDR data type output.
	 *
	 * @param out ostream.
	 * @param addr OTHER_L2_ADDR data type.
	 * @return ostream reference.
	 */
	friend std::ostream& operator<<(std::ostream& out, const other_l2_addr& addr)
	{
	    out << "other_l2_addr";
		return out;
	}

	/**
	 * Check if the OTHER_L2_ADDR is equal to another OTHER_L2_ADDR.
	 *
	 * @param other The OTHER_L2_ADDR to compare with.
	 * @return True if they are equal or false otherwise.
	 */
	bool operator==(const other_l2_addr& other) const
	{
		return ((value == other.value));
	}

	octet_string value;	/**< OTHER_L2_ADDR data type.	*/
};

///////////////////////////////////////////////////////////////////////////////
/**
 * LINK_ADDR data type.
 */
typedef boost::variant<mac_addr,
			l2_3gpp_3g_cell_id,
			l2_3gpp_2g_cell_id,
			l2_3gpp_addr,
			l2_3gpp2_addr,
			other_l2_addr
		      > link_addr;

/**
 * LIST(LINK_ADDR) data type.
 */
typedef std::vector<link_addr> link_addr_list;

///////////////////////////////////////////////////////////////////////////////
/**
 * IP_ADDR data type.
 */
class ip_addr : public transport_addr {
public:
	/**
	 * IP_ADDR type enumeration.
	 */
	enum type_ip_enum {
		none = 0,		/**< None.					*/
		ipv4 = 1,		/**< IP address version 4.	*/
		ipv6 = 2,		/**< IP address version 6.	*/
	};

	/**
	 * Construct a IP_ADDR data type.
	 *
	 * @param tp IP address type.
	 */
	ip_addr(type_ip_enum tp = none) : transport_addr(tp)
	{ }

	/**
	 * Construct a IP_ADDR data type.
	 *
	 * @param tp IP address type.
	 * @param raw Raw bytes of the IP address.
	 * @param len Size of the IP address raw bytes.
	 */
	ip_addr(type_ip_enum tp, const void* raw, size_t len)
		: transport_addr(tp, raw, len)
	{ }

	/**
	 * Construct a IP_ADDR data type.
	 *
	 * @param tp IP address type.
	 * @param raw IP address string format.
	 */
	explicit ip_addr(type_ip_enum tp, const octet_string& addr)
		: transport_addr(tp)
	{
		this->address(addr);
	}

	/**
	 * Get the IP address string.
	 *
	 * @return The IP address string.
	 */
	octet_string address() const;

	/**
	 * Set the IP address.
	 *
	 * @param addr The IP address string.
	 */
	void address(const octet_string& addr);

	/**
	 * IP_ADDR data type output.
	 *
	 * @param out ostream.
	 * @param tp IP_ADDR data type.
	 * @return ostream reference.
	 */
	friend std::ostream& operator<<(std::ostream& out, const ip_addr& tp)
	{
		out << "\ntype: " << tp.type();
		out << "\naddress: " << tp.address();

		return out;
	}

	/**
	 * Check if the IP_ADDR is equal to another IP_ADDR.
	 *
	 * @param other The IP_ADDR to compare with.
	 * @return True if they are equal or false otherwise.
	 */
	bool operator==(const ip_addr& other) const
	{
		return ((type() == other.type()) && (address().compare(other.address()) == 0));
	}
};

typedef ip_addr dhcp_serv;	 /**< DHCP_SERV data type.	*/
typedef ip_addr fn_agent;	 /**< FN_AGENT data type.	*/
typedef ip_addr acc_rtr;	 /**< ACC_RTR data type.	*/

///////////////////////////////////////////////////////////////////////////////
} /* namespace mih */ } /*namespace odtone */

// EOF ////////////////////////////////////////////////////////////////////////
#endif /* ODTONE_MIH_TYPES_ADDRESS__HPP_ */
