//==============================================================================
// Brief   : Service Management
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
#include "service_management.hpp"
#include "log.hpp"
#include "utils.hpp"
#include "mihfid.hpp"
#include "transmit.hpp"

#include <odtone/mih/types/capabilities.hpp>
#include <odtone/mih/request.hpp>
#include <odtone/mih/response.hpp>
#include <odtone/mih/confirm.hpp>
#include <odtone/mih/indication.hpp>
#include <odtone/mih/tlv_types.hpp>

#include <boost/foreach.hpp>
///////////////////////////////////////////////////////////////////////////////

extern odtone::uint16 kConf_MIHF_Link_Response_Time_Value;
extern odtone::uint16 kConf_MIHF_Link_Delete_Value;

namespace odtone { namespace mihf {

/**
 * Construct the service management.
 *
 * @param io The io_service object that service management module will
 * use to dispatch handlers for any asynchronous operations performed on
 * the socket.
 * @param lpool The local transaction pool module.
 * @param link_abook The link book module.
 * @param user_abook The user book module.
 * @param address_abook The address book module.
 * @param t The transmit module.
 * @param lrpool The link response pool module.
 * @param dscv_order Ordered list of entities that will manage the
 * discovery of new PoS.
 * @param enable_unsolicited Allows unsolicited discovery.
 */
service_management::service_management(io_service &io,
										local_transaction_pool &lpool,
										link_book &link_abook,
										user_book &user_abook,
										address_book &address_book,
										transmit &t,
										link_response_pool &lrpool,
										std::vector<mih::octet_string> &dscv_order,
										bool enable_unsolicited)
	: _lpool(lpool),
	  _link_abook(link_abook),
	  _user_abook(user_abook),
	  _abook(address_book),
	  _transmit(t),
	  _lrpool(lrpool),
	  _discover(io, lpool, address_book, user_abook, t, dscv_order, enable_unsolicited)
{
	_dscv_order = dscv_order;
	_enable_unsolicited = enable_unsolicited;

	// Update MIHF capabilities
	utils::update_local_capabilities(_abook, _link_abook, _user_abook);

	// Get capabilities from statically configured Link SAPs
	const std::vector<mih::octet_string> link_sap_list = _link_abook.get_ids();

	BOOST_FOREACH(mih::octet_string id, link_sap_list) {
		meta_message_ptr out(new meta_message());

		*out << mih::request(mih::request::capability_discover);
		out->tid(0);
		out->destination(mih::id(id));

		ODTONE_LOG(1, "(mics) forwarding Link_Capability_Discover.request to ",
			out->destination().to_string());
		utils::forward_request(out, _lpool, _transmit);
	}
}

/**
 * Asks for the capabilities of all local Link SAPs.
 *
 * @param in The input message.
 * @param out The output message.
 * @return Always false, because it does not send any response directly.
 */
bool service_management::link_capability_discover_request(meta_message_ptr &in,
														  meta_message_ptr &out)
{
	// Asks for local Link SAPs capabilities
	ODTONE_LOG(1, "(mism) gathering information about local Link SAPs capabilities");

	*out << mih::request(mih::request::capability_discover);
	out->tid(in->tid());
	out->destination(in->source());

	// Check if the Link SAP is still active
	uint16 fails = _link_abook.fail(out->destination().to_string());
	if(fails > kConf_MIHF_Link_Delete_Value) {
		mih::octet_string dst = out->destination().to_string();
		_link_abook.inactive(dst);

		// Update MIHF capabilities
		utils::update_local_capabilities(_abook, _link_abook, _user_abook);
	}
	else {
		ODTONE_LOG(1, "(mics) forwarding Link_Capability_Discover.request to ",
			out->destination().to_string());
		utils::forward_request(out, _lpool, _transmit);
	}

	return false;
}

/**
 * Piggyback local MIHF Capabilities in request message.
 *
 * @param in input message.
 * @param out output message.
 */
void service_management::piggyback_capabilities(meta_message_ptr& in,
												meta_message_ptr& out)
{
	// Get local capabilities
	address_entry mihf_cap = _abook.get(mihfid_t::instance()->to_string());

	*out << mih::request(mih::request::capability_discover)
			& mih::tlv_net_type_addr_list(mihf_cap.capabilities_list_net_type_addr)
			& mih::tlv_event_list(mihf_cap.capabilities_event_list)
			& mih::tlv_command_list(mihf_cap.capabilities_cmd_list)
			& mih::tlv_query_type_list(mihf_cap.capabilities_query_type)
			& mih::tlv_transport_option_list(mihf_cap.capabilities_trans_list)
			& mih::tlv_mbb_ho_supp_list(mihf_cap.capabilities_mbb_ho_supp);

	out->tid(in->tid());
	out->source(in->source());
	out->destination(in->destination());
}

/**
 * Parse all capabilities from MIH Capability Discover message and stores
 * them.
 *
 * @param in input message.
 * @param out output message.
 */
void service_management::get_capabilities(meta_message_ptr& in,
										  meta_message_ptr& out)
{
	address_entry mihf_info;
	mihf_info.ip = in->ip();
	mihf_info.port = in->port();

	if(in->opcode() == mih::operation::request) {
		*in >> mih::request(mih::request::capability_discover)
				& mih::tlv_net_type_addr_list(mihf_info.capabilities_list_net_type_addr)
				& mih::tlv_event_list(mihf_info.capabilities_event_list)
				& mih::tlv_command_list(mihf_info.capabilities_cmd_list)
				& mih::tlv_query_type_list(mihf_info.capabilities_query_type)
				& mih::tlv_transport_option_list(mihf_info.capabilities_trans_list)
				& mih::tlv_mbb_ho_supp_list(mihf_info.capabilities_mbb_ho_supp);
	} else if(in->opcode() == mih::operation::response) {
		mih::status st;
		*in >> mih::response(mih::response::capability_discover)
			& mih::tlv_status(st)
			& mih::tlv_net_type_addr_list(mihf_info.capabilities_list_net_type_addr)
			& mih::tlv_event_list(mihf_info.capabilities_event_list)
			& mih::tlv_command_list(mihf_info.capabilities_cmd_list)
			& mih::tlv_query_type_list(mihf_info.capabilities_query_type)
			& mih::tlv_transport_option_list(mihf_info.capabilities_trans_list)
			& mih::tlv_mbb_ho_supp_list(mihf_info.capabilities_mbb_ho_supp);
	}

	_abook.add(in->source().to_string(), mihf_info);
}

/**
 * Set response to MIH Capability Discover message.
 *
 * @param in input message.
 * @param out output message.
 */
void service_management::set_capability_discover_response(meta_message_ptr& in,
														  meta_message_ptr& out)
{
	// Create and piggyback local capabilities in response message
	address_entry mihf_cap = _abook.get(mihfid_t::instance()->to_string());

	*out << mih::response(mih::response::capability_discover)
			& mih::tlv_status(mih::status_success)
			& mih::tlv_net_type_addr_list(mihf_cap.capabilities_list_net_type_addr)
			& mih::tlv_event_list(mihf_cap.capabilities_event_list)
			& mih::tlv_command_list(mihf_cap.capabilities_cmd_list)
			& mih::tlv_query_type_list(mihf_cap.capabilities_query_type)
			& mih::tlv_transport_option_list(mihf_cap.capabilities_trans_list)
			& mih::tlv_mbb_ho_supp_list(mihf_cap.capabilities_mbb_ho_supp);

	out->tid(in->tid());
	out->source(mihfid);
	out->destination(in->source());
}

/**
 * Send Capability Discover Indication message to all MIH Users.
 *
 * @param in input message.
 * @param out output message.
 */
void service_management::send_indication(meta_message_ptr& in,
										 meta_message_ptr& out)
{
	std::vector<mih::octet_string> ids = _user_abook.get_ids();
	in->opcode(mih::operation::indication);
	for (std::vector<mih::octet_string>::iterator it = ids.begin(); it < ids.end(); ++it) {
		if(std::find(_dscv_order.begin(), _dscv_order.end(), *it) == _dscv_order.end()) {
			in->destination(mih::id(*it));
			_transmit(in);
			ODTONE_LOG(3, "(mism) Capability_Discover.indication sent to ",
					  in->destination().to_string());
		}
	}
}

/**
 * Capability Discover Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool service_management::capability_discover_request(meta_message_ptr& in,
													 meta_message_ptr& out)
{
	ODTONE_LOG(1, "(mism) received Capability_Discover.request from ",
	    in->source().to_string(), " with destination ",
	    in->destination().to_string());

	// User requests the capabilities of a remote MIHF
	if (in->is_local() && !utils::this_mihf_is_destination(in)) {
		// Multicast && Discover Module ON
		if(utils::is_multicast(in) && _dscv_order.size() != 0) {
			_discover.request(in, out);
			return false;
		}
		// Multicast && Discover Module OFF
		piggyback_capabilities(in, out);
		utils::forward_request(out, _lpool, _transmit);
		return false;
	// User requets the capabilitties of the local MIHF
	} else if (in->is_local() && utils::this_mihf_is_destination(in)) {
		set_capability_discover_response(in, out);
		return true;
	// Remote requets received
	} else if (utils::this_mihf_is_destination(in)) {
		get_capabilities(in, out);
		send_indication(in, out);
		set_capability_discover_response(in, out);
		return true;
	// Multicast request received
	} else if (utils::is_multicast(in)) {
		get_capabilities(in, out);
		send_indication(in, out);
		set_capability_discover_response(in, out);
		return true;
	} else {
			ODTONE_LOG(3, "(mism) response to broadcast Capability_Discover.request disabled");
			return false;
	}

	return false;
}

/**
 * Capability Discover Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool service_management::capability_discover_response(meta_message_ptr &in,
						      meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mism) received Capability_Discover.response from ",
	    in->source().to_string());

	// Check if it is a discovery message
	if(in->is_local() && std::find(_dscv_order.begin(),
	                               _dscv_order.end(),
	                               in->source().to_string()) != _dscv_order.end()) {
		_discover.response(in, out);
		return false;
	}

	// Store remote MIHF capabilities
	get_capabilities(in, out);

	// do we have a request from a user?
	if (_lpool.set_user_tid(in)) {
		ODTONE_LOG(1, "forwarding Capability_Discover.response to ",
		    in->destination().to_string());
		in->opcode(mih::operation::confirm);
		_transmit(in);
		return false;
	}

	// set source id to broadcast id and check if there's a
	// broadcast request from a user
	mih::id tmp	= in->source();
	in->source(mih::id(""));
	if (_lpool.set_user_tid(in))  {
		ODTONE_LOG(1, "forwarding Capability_Discover.response to ",
		    in->destination().to_string());
		in->opcode(mih::operation::confirm);
		in->source(tmp);
		_transmit(in);
		return false;
	}

	if(_enable_unsolicited) {
		ODTONE_LOG(1, "forwarding Capability_Discover.response to all",
		    "MIH-Users");

		std::vector<mih::octet_string> user_id_list = _user_abook.get_ids();
		BOOST_FOREACH(mih::octet_string user, user_id_list) {
			ODTONE_LOG(3, "forwarding Capability_Discover.response to ",
				user);
			in->opcode(mih::operation::confirm);
			in->source(mihfid);
			in->destination(mih::id(user));
			_transmit(in);
		}

		return false;
	} else {
		ODTONE_LOG(1, "no pending transaction for this message, discarding");
		return false;
	}
}

/**
 * Capability Discover Confirm message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool service_management::capability_discover_confirm(meta_message_ptr &in,
						      meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mism) received Capability_Discover.confirm from ",
	    in->source().to_string());

	_link_abook.reset(in->source().to_string());

	if(!_lpool.set_user_tid(in)) {
		ODTONE_LOG(1, "no pending transaction for this message, discarding");
		return false;
	}

	mih::status st;
	boost::optional<mih::link_evt_list> event;
	boost::optional<mih::link_cmd_list> command;

	*in >> mih::confirm(mih::confirm::capability_discover)
	       & mih::tlv_status(st)
	       & mih::tlv_link_evt_list(event)
	       & mih::tlv_link_cmd_list(command);

	if(st == mih::status_success) {
		// Update Link SAP capabilities in the Link Book
		_link_abook.update_capabilities(in->source().to_string(), event.get(), command.get());

		// Update MIHF capabilities
		utils::update_local_capabilities(_abook, _link_abook, _user_abook);
	}

	return false;
}

/**
 * Register Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool service_management::register_request(meta_message_ptr &in,
                                          meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mism) received MIH_Register.request from ", in->source().to_string());

	if(utils::this_mihf_is_destination(in)) {
		if(in->is_local()) {
			in->source(mihfid);
		} else {
			// Set registration
			mih::link_id_list lil;
			*in >> mih::request(mih::request::mih_register)
				& mih::tlv_link_id_list(lil);
			_abook.set_registration(in->source().to_string(), lil);
			//
		}

		// Forward this message to MIH-User for handover as an indication
		in->opcode(mih::operation::indication);
		std::vector<mih::octet_string> user_list = _user_abook.get_ids();
		BOOST_FOREACH(mih::octet_string id, user_list) {
			in->destination(mih::id(id));
			_lpool.add(in);
			_transmit(in);

			ODTONE_LOG(1, "(mism) forwarding MIH_Register.indication to " , in->destination().to_string());
		}

		// Restore the original opcode after sending the indication message
		in->opcode(mih::operation::request);
		return false;
	} else {
		// Set registration
		mih::link_id_list lil;
		*in >> mih::request(mih::request::mih_register)
			& mih::tlv_link_id_list(lil);
		_abook.set_registration(in->destination().to_string(), lil);
		//

		// try to forward the message, this is to handle the
		// special case of the user handling MIH commands
		// sending some MIH command request to a peer mihf
		utils::forward_request(in, _lpool, _transmit);
		return false;
	}

	return false;
}

/**
 * Register Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool service_management::register_response(meta_message_ptr&in,
                                           meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mism) received MIH_Register.response from ", in->source().to_string());

	if(utils::this_mihf_is_destination(in)) {
		if(!_lpool.set_user_tid(in)) {
			ODTONE_LOG(1, "(mics) warning: no local transaction for this msg ",
				"discarding it");
			return false;
		}

		// Set registration time interval
		mih::status st;
		uint16 interval;
		*in >> mih::confirm(mih::confirm::mih_register)
	       & mih::tlv_status(st)
		   & mih::tlv_time_interval(interval);

		if(st == mih::status_success) {
			_abook.set_registration(in->source().to_string(), interval);
		}
		//

		ODTONE_LOG(1, "(mism) forwarding MIH_Register.confirm to " , in->destination().to_string());
		in->opcode(mih::operation::confirm);
		_transmit(in);
	} else {
		if(!_lpool.set_user_tid(in)) {
			ODTONE_LOG(1, "(mics) warning: no local transaction for this msg ",
				"discarding it");
			return false;
		}

		// Set registration time interval
		mih::status st;
		uint16 interval;
		*in >> mih::confirm(mih::confirm::mih_register)
	       & mih::tlv_status(st)
		   & mih::tlv_time_interval(interval);

		if(st == mih::status_success) {
			_abook.set_registration(in->destination().to_string(), interval);
		}
		//

		in->source(mihfid);
		_transmit(in);
	}

	return false;
}

/**
 * DeRegister Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool service_management::deregister_request(meta_message_ptr &in,
                                             meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mism) received MIH_DeRegister.request from ",
	               in->source().to_string());

	if(utils::this_mihf_is_destination(in)) {
		if(in->is_local())
			in->source(mihfid);

		// Forward this message to MIH-User for handover as an indication
		in->opcode(mih::operation::indication);
		std::vector<mih::octet_string> user_list = _user_abook.get_ids();
		BOOST_FOREACH(mih::octet_string id, user_list) {
			in->destination(mih::id(id));
			_lpool.add(in);
			_transmit(in);

			ODTONE_LOG(1, "(mism) forwarding MIH_DeRegister.indication to ",
			              in->destination().to_string());
		}

		// Restore the original opcode after sending the indication message
		in->opcode(mih::operation::request);
		return false;
	} else {
		// try to forward the message, this is to handle the
		// special case of the user handling MIH commands
		// sending some MIH command request to a peer mihf
		utils::forward_request(in, _lpool, _transmit);
		return false;
	}

	return false;
}

/**
 * DeRegister Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool service_management::deregister_response(meta_message_ptr&in,
                                             meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mism) received MIH_DeRegister.response from ", in->source().to_string());

	if(utils::this_mihf_is_destination(in)) {
		if(!_lpool.set_user_tid(in)) {
			ODTONE_LOG(1, "(mics) warning: no local transaction for this msg ",
				"discarding it");
			return false;
		}

		// Set registration
		mih::status st;
		*in >> mih::confirm(mih::confirm::mih_deregister)
	       & mih::tlv_status(st);

		if(st == mih::status_success) {
			// Set empty registered link id list
			boost::optional<mih::link_id_list> lil;
			boost::optional<uint16> interval;
			_abook.set_registration(in->source().to_string(), lil);
			_abook.set_registration(in->source().to_string(), interval);
		}
		//

		ODTONE_LOG(1, "(mism) forwarding MIH_DeRegister.confirm to " , in->destination().to_string());
		in->opcode(mih::operation::confirm);
		_transmit(in);
	} else {
		if(!_lpool.set_user_tid(in)) {
			ODTONE_LOG(1, "(mics) warning: no local transaction for this msg ",
				"discarding it");
			return false;
		}

		// Set registration
		mih::status st;
		*in >> mih::confirm(mih::confirm::mih_deregister)
	       & mih::tlv_status(st);

		if(st == mih::status_success) {
			// Set empty registered link id list
			boost::optional<mih::link_id_list> lil;
			boost::optional<uint16> interval;
			_abook.set_registration(in->destination().to_string(), lil);
			_abook.set_registration(in->destination().to_string(), interval);
		}
		//

		in->source(mihfid);
		_transmit(in);
	}

	return false;
}

/**
 * Link Register Indication message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool service_management::link_register_indication(meta_message_ptr &in,
	                                       meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mism) received Link_Register.indication from ",
	    in->source().to_string());

	// Add Link SAP to the list of known Link SAPs
	mih::status st;
	mih::link_id link_id;

	mih::octet_string ip(in->ip());

	*in >> odtone::mih::indication()
		& odtone::mih::tlv_interface_type_addr(link_id);

    ODTONE_LOG(4, "((mism) received Link_Register.indication: link_id", link_id);
	// Add Link SAP to the list of known Link SAPs
	_link_abook.add(in->source().to_string(), in->ip(), in->port(), link_id);

	// Update MIHF network type address
	address_entry mihf_cap = _abook.get(mihfid_t::instance()->to_string());

	// Set Network Type Address
	mih::net_type_addr nta;
	nta.nettype.link = link_id.type;
	nta.addr = link_id.addr;

	if(mihf_cap.capabilities_list_net_type_addr.is_initialized()) {
		mihf_cap.capabilities_list_net_type_addr.get().push_back(nta);
	} else {
		mih::net_type_addr_list ntal;
		ntal.push_back(nta);
		mihf_cap.capabilities_list_net_type_addr = ntal;
	}
	_abook.add(mihfid_t::instance()->to_string(), mihf_cap);

	// Request the Link SAP capabilities
	link_capability_discover_request(in, out);

	return false;
}

/**
 * User Register Indication message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool service_management::user_register_indication(meta_message_ptr &in,
	                                          meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mism) received User_Register.indication from ",
	    in->source().to_string());

	// Add MIH User to the list of known MIH Users
	boost::optional<mih::mih_cmd_list> supp_cmd;
	boost::optional<mih::iq_type_list> supp_iq;

	mih::octet_string ip(in->ip());

	*in >> odtone::mih::indication()
		& odtone::mih::tlv_command_list(supp_cmd)
		& odtone::mih::tlv_query_type_list(supp_iq);

	_user_abook.add(in->source().to_string(), ip, in->port(), supp_cmd, supp_iq);

	// Update MIHF capabilities
	utils::update_local_capabilities(_abook, _link_abook, _user_abook);

	return false;
}

} /* namespace mihf */ } /* namespace odtone */
