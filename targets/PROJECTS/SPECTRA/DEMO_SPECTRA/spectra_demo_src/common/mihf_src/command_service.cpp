//==============================================================================
// Brief   : Command Service
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
#include "command_service.hpp"

#include "log.hpp"
#include "utils.hpp"
#include "mihfid.hpp"
#include "transmit.hpp"
#include "link_book.hpp"

#include <odtone/base.hpp>
#include <odtone/debug.hpp>
#include <odtone/mih/request.hpp>
#include <odtone/mih/confirm.hpp>
#include <odtone/mih/response.hpp>
#include <odtone/mih/tlv_types.hpp>

#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
///////////////////////////////////////////////////////////////////////////////

extern odtone::uint16 kConf_MIHF_Link_Response_Time_Value;
extern odtone::uint16 kConf_MIHF_Link_Delete_Value;

namespace odtone { namespace mihf {

/**
 * Construct the command service.
 *
 * @param io The io_service object that command service module will use to
 * dispatch handlers for any asynchronous operations performed on
 * the socket.
 * @param lpool The local transaction pool module.
 * @param t The transmit module.
 * @param abook The address book module.
 * @param link_abook The link book module.
 * @param user_abook The user book module.
 * @param lrpool The link response pool module.
 */
command_service::command_service(io_service &io,
								 local_transaction_pool &lpool,
								 transmit &t,
								 address_book &abook,
								 link_book &link_abook,
								 user_book &user_abook,
								 link_response_pool &lrpool)
	: _io(io),
	  _lpool(lpool),
	  _transmit(t),
	  _abook(abook),
	  _link_abook(link_abook),
	  _user_abook(user_abook),
	  _lrpool(lrpool)
{
}

/**
 * Handler responsible for processing the received Link Get Parameters
 * responses from Link SAPs.
 *
 * @param ec Error code.
 * @param in The input message.
 */
void command_service::link_get_parameters_response_handler(const boost::system::error_code &ec, meta_message_ptr &in)
{
	if(ec)
		return;

	{
		boost::mutex::scoped_lock lock(_mutex);
		_timer.erase(in->tid());
	}

	mih::status st = mih::status_failure;
	mih::link_id             lid;
	mih::link_status_rsp	 lsr;
	mih::status_rsp          sr;
	mih::status_rsp_list     srl;
	mih::dev_states_rsp_list dsrl;
	meta_message_ptr out(new meta_message());

	std::vector<mih::octet_string> ids = _link_abook.get_ids();
	std::vector<mih::octet_string>::iterator it_link;
	for(it_link = ids.begin(); it_link != ids.end(); it_link++) {
		if(_lrpool.check(in->tid(), *it_link)) {
			// fill GetStatusResponseList
			link_entry a;
			mih::link_id lid;

			a = _link_abook.get(*it_link);

			lid.type = a.link_id.type;
			lid.addr = a.link_id.addr;

			// fill capabilities
			pending_link_response tmp = _lrpool.find(in->tid(), *it_link);
			_lrpool.del(in->tid(), *it_link);

			mih::link_status_rsp& link_status = boost::get<mih::link_status_rsp>(tmp.response);
			lsr = link_status;

			sr.id = lid;
			sr.rsp = lsr;

			srl.push_back(sr);

			// If one or more responses are successful the status
			// is set to success
			st = mih::status_success;
		}
	}

	// Send Link_Get_Parameters.confirm to the user
	if(st == mih::status_success) {
		ODTONE_LOG(1, "(micm) setting response to Link_Get_Parameters.request");
		*out << mih::response(mih::response::link_get_parameters)
			& mih::tlv_status(mih::status_success)
	//	    & mih::tlv_dev_states_rsp_list(dsrl)
			& mih::tlv_get_status_rsp_list(srl);
	} else {
		ODTONE_LOG(1, "(micm) setting failure response to Link_Get_Parameters.request");
		*out << mih::response(mih::response::link_get_parameters)
		    & mih::tlv_status(st);
	}

	out->tid(in->tid());
	out->destination(in->source());
	out->source(mihfid);

	out->ip(in->ip());
	out->scope(in->scope());
	out->port(in->port());
	_transmit(out);
}

/**
 * Link Get Parameters Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::link_get_parameters_request(meta_message_ptr &in,
						  meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mics) received a Link_Get_Parameters.request from ",
	    in->source().to_string());


	if(utils::this_mihf_is_destination(in)) {
		//
		// Kick this message to MIH_Link SAP.
		//
		// local_transactions was made to handle request's
		// from users to peer mihf's but in this case we add an
		// entry to handle the MIH_Link_Get_Parameters and
		// Link_Get_Parameters.
		//
		boost::optional<mih::dev_states_req> dsr;
		mih::link_id_list                    lil;
		mih::link_status_req                 lsr;

		*in >> mih::request(mih::request::link_get_parameters)
		       & mih::tlv_dev_states_req(dsr)
		       & mih::tlv_link_id_list(lil)
		       & mih::tlv_get_status_req_set(lsr);

		*out << mih::request(mih::request::link_get_parameters)
		    & mih::tlv_link_parameters_req(lsr._param_type_list)
		    & mih::tlv_link_states_req(lsr._states_req)
		    & mih::tlv_link_descriptor_req(lsr._desc_req);

		out->tid(in->tid());
		out->source(mihfid);

		// For each Link_ID in request message
		std::vector<mih::link_id>::iterator lid;
		for(lid = lil.begin(); lid != lil.end(); lid++) {
			out->destination(mih::id(_link_abook.search_interface((*lid).type, (*lid).addr)));
			// If the Link SAP it is known send message
			if (out->destination().to_string().compare("") != 0) {
				// Check if the Link SAP is still active
				uint16 fails = _link_abook.fail(out->destination().to_string());
				if(fails > kConf_MIHF_Link_Delete_Value) {
					mih::octet_string dst = out->destination().to_string();
					_link_abook.inactive(dst);

					// Update MIHF capabilities
					utils::update_local_capabilities(_abook, _link_abook, _user_abook);
				} else {
					ODTONE_LOG(1, "(mics) forwarding Link_Get_Parameters.request to ",
						out->destination().to_string());
					utils::forward_request(out, _lpool, _transmit);
				}
			}
		}

		// Set the timer that will be responsible for aggregate and
		// response to this resquest
		boost::shared_ptr<boost::asio::deadline_timer> timer = boost::make_shared<boost::asio::deadline_timer>(_io);
		timer->expires_from_now(boost::posix_time::milliseconds(kConf_MIHF_Link_Response_Time_Value));
		timer->async_wait(boost::bind(&command_service::link_get_parameters_response_handler, this, _1, in));

		{
			boost::mutex::scoped_lock lock(_mutex);
			_timer[in->tid()] = timer;
		}

		// Do not respond to the request. The response handler will be
		// responsible for that.
		return false;
	} else {
		utils::forward_request(in, _lpool, _transmit);
		return false;
	}

	return false;
}

/**
 * Link Get Parameters Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::link_get_parameters_response(meta_message_ptr &in,
						   meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mics) received Link_Get_Parameters.response from ",
	    in->source().to_string());

	if(!_lpool.set_user_tid(in)) {
		ODTONE_LOG(1, "(mics) warning: no local transaction for this msg ",
			"discarding it");
		return false;
	}

	ODTONE_LOG(1, "(mics) forwarding Link_Get_Parameters.response to ",
	    in->destination().to_string());

	_transmit(in);

	return false;
}

/**
 * Link Get Parameters Confirm message handler.
 *
 * @param in input message.
 * @param out output message.
 * @return true if the response is sent immediately or false otherwise.
 */
bool command_service::link_get_parameters_confirm(meta_message_ptr &in,
						   meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mics) received Link_Get_Parameters.confirm from ",
	    in->source().to_string());

	_link_abook.reset(in->source().to_string());

	if(_lpool.set_user_tid(in)) {
		mih::status st;
		boost::optional<mih::link_param_list> lpl;
		boost::optional<mih::link_states_rsp_list> lsrl;
		boost::optional<mih::link_desc_rsp_list> ldrl;

		*in >> mih::confirm(mih::confirm::link_get_parameters)
		       & mih::tlv_status(st)
		       & mih::tlv_link_parameters_status_list(lpl)
		       & mih::tlv_link_states_rsp(lsrl)
		       & mih::tlv_link_descriptor_rsp(ldrl);

		if(st == mih::status_success) {
			mih::link_status_rsp link_status;

			link_status.states_rsp_list = lsrl.get();
			link_status.param_list = lpl.get();
			link_status.desc_rsp_list = ldrl.get();

			_lrpool.add(in->source().to_string(),
			           in->tid(),
			           link_status);
		}

		return false;
	}

	ODTONE_LOG(1, "no pending transaction for this message, discarding");
	return false;
}

/**
 * Handler responsible for processing the received Link Get Parameters
 * responses from Link SAPs.
 *
 * @param ec Error code.
 * @param in The input message.
 */
void command_service::link_configure_thresholds_response_timeout(const boost::system::error_code &ec, meta_message_ptr &in)
{
	if(ec)
		return;

	{
		boost::mutex::scoped_lock lock(_mutex);
		_timer.erase(in->tid());
	}

	mih::link_tuple_id link;
	meta_message_ptr out(new meta_message());

	*in >> mih::request(mih::request::link_configure_thresholds)
		& mih::tlv_link_identifier(link);

	// Send failure message to the user
	ODTONE_LOG(1, "(mics) setting failure response to Link_Configure_Thresholds.request");
	*out << mih::response(mih::response::link_configure_thresholds)
	    & mih::tlv_status(mih::status_failure)
	    & mih::tlv_link_identifier(link);

	out->tid(in->tid());
	out->destination(in->source());
	out->source(mihfid);

	_transmit(out);
}

/**
 * Link Configure Thresholds Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::link_configure_thresholds_request(meta_message_ptr &in,
							meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mics) received a Link_Configure_Thresholds.request from ",
	    in->source().to_string());

	if(utils::this_mihf_is_destination(in)) {
		//
		// Kick this message to MIH_Link SAP.
		//
		// local_transactions was made to handle request's
		// from users to peer mihf's but in this case we add an
		// entry to handle the MIH_Link_Get_Parameters and
		// Link_Get_Parameters.
		//
		mih::link_tuple_id       lti;
		mih::link_cfg_param_list lcpl;

		*in >> mih::request(mih::request::link_configure_thresholds)
		       & mih::tlv_link_identifier(lti)
		       & mih::tlv_link_cfg_param_list(lcpl);
		
		*out << mih::request(mih::request::link_configure_thresholds)
		       & mih::tlv_link_cfg_param_list(lcpl);

		out->destination(mih::id(_link_abook.search_interface(lti.type, lti.addr)));
		out->source(in->source());
		out->tid(in->tid());

		// If the Link SAP it is known continue
		if (out->destination().to_string().compare("") == 0) {
			*out << mih::response(mih::response::link_configure_thresholds)
				& mih::tlv_status(mih::status_failure)
				& mih::tlv_link_identifier(lti);

			out->tid(in->tid());
			out->source(mihfid);
			out->destination(in->source());

			ODTONE_LOG(1, "(mies) forwarding Link_Configure_Thresholds.response to ",
				out->destination().to_string());

			return true;
		}

		// Check if the Link SAP is still active
		uint16 fails = _link_abook.fail(out->destination().to_string());
		if(fails > kConf_MIHF_Link_Delete_Value) {
			mih::octet_string dst = out->destination().to_string();
			_link_abook.inactive(dst);

			// Update MIHF capabilities
			utils::update_local_capabilities(_abook, _link_abook, _user_abook);
		} else {
			ODTONE_LOG(1, "(mics) forwarding Link_Configure_Thresholds.request to ",
			    out->destination().to_string());
			utils::forward_request(out, _lpool, _transmit);

			// Set the timer that will be responsible for sending a failure
			// response if necessary
			boost::shared_ptr<boost::asio::deadline_timer> timer = boost::make_shared<boost::asio::deadline_timer>(_io);
			timer->expires_from_now(boost::posix_time::milliseconds(kConf_MIHF_Link_Response_Time_Value));
			timer->async_wait(boost::bind(&command_service::link_configure_thresholds_response_timeout, this, _1, in));

			{
				boost::mutex::scoped_lock lock(_mutex);
				_timer[in->tid()] = timer;
			}
		}

		return false;
	} else {
		utils::forward_request(in, _lpool, _transmit);
		return false;
	}

	return false;
}

/**
 * Link Configure Thresholds Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::link_configure_thresholds_response(meta_message_ptr &in,
							 meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mics) received Link_Configure_Thresholds.response from ",
	    in->source().to_string());

	if(!_lpool.set_user_tid(in)) {
		ODTONE_LOG(1, "(mics) warning: no local transaction for this msg ",
		    "discarding it");
		return false;
	}

	ODTONE_LOG(1, "(mics) forwarding Link_Configure_Thresholds.response to ", in->destination().to_string());

	_transmit(in);

	return false;
}

/**
 * Link Configure Thresholds Confirm message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::link_configure_thresholds_confirm(meta_message_ptr &in,
							 meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mics) received Link_Configure_Thresholds.confirm from ",
	    in->source().to_string());

	_link_abook.reset(in->source().to_string());

	out->source(in->source());
	if (!_lpool.set_user_tid(out)) {
		ODTONE_LOG(1, "(mics) warning: no local transaction for this msg ",
		    "discarding it");
		return false;
	}

	{
		boost::mutex::scoped_lock lock(_mutex);
		_timer.erase(in->tid());
	}

	mih::status st;
	boost::optional<mih::link_cfg_status_list> lcsl;

	mih::link_tuple_id li;
	li.type = _link_abook.get(in->source().to_string()).link_id.type;
	li.addr = _link_abook.get(in->source().to_string()).link_id.addr;

	*in >> mih::confirm(mih::confirm::link_configure_thresholds)
		& mih::tlv_status(st)
		& mih::tlv_link_cfg_status_list(lcsl);

	*out << mih::response(mih::response::link_configure_thresholds)
		& mih::tlv_status(st)
		& mih::tlv_link_identifier(li)
		& mih::tlv_link_cfg_status_list(lcsl);

	out->source(mihfid);

	ODTONE_LOG(1, "(mics) forwarding Link_Configure_Thresholds.confirm to ", out->destination().to_string());

	_transmit(out);

	return false;
}

/**
 * Handler responsible for setting a failure Link Action
 * responses.
 *
 * @param ec Error code.
 * @param in The input message.
 */
void command_service::link_actions_response_handler(const boost::system::error_code &ec,
													meta_message_ptr &in)
{
	if(ec)
		return;

	{
		boost::mutex::scoped_lock lock(_mutex);
		_timer.erase(in->tid());
	}

	mih::status st = mih::status_failure;
	mih::link_action_rsp_list larl;
	mih::link_action_rsp      lar;
	mih::link_ac_result       laresult;
	mih::link_scan_rsp_list   lsrl;
	meta_message_ptr out(new meta_message());

	std::vector<mih::octet_string> ids = _link_abook.get_ids();
	std::vector<mih::octet_string>::iterator it_link;
	for(it_link = ids.begin(); it_link != ids.end(); it_link++) {
		if(_lrpool.check(in->tid(), *it_link)) {
			// fill LinkActionsResultList
			link_entry a;
			mih::link_id lid;

			a = _link_abook.get(*it_link);

			lar.id.type = a.link_id.type;
			lar.id.addr = a.link_id.addr;

			// fill action result
			pending_link_response tmp = _lrpool.find(in->tid(), *it_link);
			_lrpool.del(in->tid(), *it_link);

			action& ac = boost::get<action>(tmp.response);

			if(ac.link_scan_rsp_list.is_initialized()) {
				lar.scan_list = ac.link_scan_rsp_list.get();
			}
			lar.result = ac.link_ac_result.get();
			larl.push_back(lar);

			// If one or more responses are successful the status
			// is set to success
			st = mih::status_success;
		}
	}

	// Send Link_Actions.confirm to the user
	if(st == mih::status_success) {
		ODTONE_LOG(1, "(mics) setting response to Link_Actions.request");
		*out << mih::response(mih::response::link_actions)
		    & mih::tlv_status(st)
		    & mih::tlv_link_action_rsp_list(larl);
	} else {
		ODTONE_LOG(1, "(mics) setting failure response to Link_Actions.request");
		*out << mih::response(mih::response::link_actions)
		    & mih::tlv_status(st);
	}

	out->tid(in->tid());
	out->destination(in->source());
	out->source(mihfid);

	out->ip(in->ip());
	out->scope(in->scope());
	out->port(in->port());
	_transmit(out);
}

/**
 * Link Actions Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::link_actions_request(meta_message_ptr &in,
										   meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mics) received a Link_Actions.request from ",
	    in->source().to_string());

	if(utils::this_mihf_is_destination(in)) {
		//
		// Kick this message to MIH_Link SAP.
		//
		// The solution found to handle this corner case in the
		// 802.21 standard was to send the message, as is, to the
		// link sap.
		//
		mih::link_action_list lal;

		*in >> mih::request(mih::request::link_actions)
		       & mih::tlv_link_action_list(lal);

		// For each Link_ID in request message
		std::vector<mih::link_action_req>::iterator lar;
		for(lar = lal.begin(); lar != lal.end(); lar++) {
            std::cout<<"ADDR in MIHF "<<(*lar).id.addr<<std::endl;
			out->destination(mih::id(_link_abook.search_interface((*lar).id.type, (*lar).id.addr)));
			// If the Link SAP it is known send message
			if (out->destination().to_string().compare("") != 0) {
				// Check if the Link SAP is still active
				uint16 fails = _link_abook.fail(out->destination().to_string());
				if(fails > kConf_MIHF_Link_Delete_Value) {
					mih::octet_string dst = out->destination().to_string();
					_link_abook.inactive(dst);

					// Update MIHF capabilities
					utils::update_local_capabilities(_abook, _link_abook, _user_abook);
				} else {
					mih::link_addr* a = boost::get<mih::link_addr>(&(*lar).addr);
					if (a && ((*lar).action.attr.get(mih::link_ac_attr_data_fwd_req)) ) {
						*out << mih::request(mih::request::link_actions)
									& mih::tlv_link_action((*lar).action)
									& mih::tlv_time_interval((*lar).ex_time)
									& mih::tlv_poa(*a);
					} else {
						*out << mih::request(mih::request::link_actions)
									& mih::tlv_link_action((*lar).action)
									& mih::tlv_time_interval((*lar).ex_time);
					}

					out->tid(in->tid());
					out->source(mihfid);

					ODTONE_LOG(1, "(mics) forwarding Link_Actions.request to ",
						out->destination().to_string());
					utils::forward_request(out, _lpool, _transmit);
				}
			}
		}

		// Set the timer that will be responsible for aggregate and
		// response to this resquest
		boost::shared_ptr<boost::asio::deadline_timer> timer = boost::make_shared<boost::asio::deadline_timer>(_io);
		timer->expires_from_now(boost::posix_time::milliseconds(kConf_MIHF_Link_Response_Time_Value));
		timer->async_wait(boost::bind(&command_service::link_actions_response_handler, this, _1, in));

		{
			boost::mutex::scoped_lock lock(_mutex);
			_timer[in->tid()] = timer;
		}

		// Do not respond to the request. The thread response handler will be
		// responsible for that.
		return false;
	} else {
		utils::forward_request(in, _lpool, _transmit);
		return false;
	}

	return false;
}

/**
 * Link Actions Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::link_actions_response(meta_message_ptr &in,
					    meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mics) received Link_Actions.response from ",
	    in->source().to_string());

	if(!_lpool.set_user_tid(in)) {
		ODTONE_LOG(1, "(mics) no local pending transaction for this message, discarding");
		return false;
	}

	ODTONE_LOG(1, "(mics) forwarding Link_Actions.response to ", in->destination().to_string());

	_transmit(in);

	return false;
}

/**
 * Link Actions Confirm message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::link_actions_confirm(meta_message_ptr &in,
					    meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mics) received Link_Actions.confirm from ",
	    in->source().to_string());

	_link_abook.reset(in->source().to_string());

	if(_lpool.set_user_tid(in)) {
		mih::status st;
		boost::optional<mih::link_scan_rsp_list> lsrl;
		boost::optional<mih::link_ac_result> lar;

		*in >> mih::confirm(mih::confirm::link_actions)
		       & mih::tlv_status(st)
		       & mih::tlv_link_scan_rsp_list(lsrl)
		       & mih::tlv_link_ac_result(lar);

		if(st == mih::status_success) {
			_lrpool.add(in->source().to_string(),
					   in->tid(),
					   lsrl,
					   lar.get());
		}

		return false;
	}

	ODTONE_LOG(1, "no pending transaction for this message, discarding");
	return false;

}

/**
 * Currently command service handover related messages are handled by
 * a single MIH-user. If this MIHF is the destination of the message,
 * forward it to the MIH-User with mobility role.
 *
 * @param recv_msg The receive message output.
 * @param send_msg The send message output.
 * @param in The input message.
 * @param out The output message.
 * @param cmd The command that the MIH-Users must support in order to
 * receive an indication about the reception opf this message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::generic_command_request(const char *recv_msg,
					      const char *send_msg,
					      meta_message_ptr &in,
					      meta_message_ptr &out,
					      mih::mih_cmd_list_enum cmd)
{
	ODTONE_LOG(1, recv_msg, in->source().to_string());

	if(utils::this_mihf_is_destination(in)) {
		// Forward this message to MIH-User for handover as an indication
		in->opcode(mih::operation::indication);
		std::vector<mih::octet_string> user_list = _user_abook.get_ids();
		BOOST_FOREACH(mih::octet_string id, user_list) {
			user_entry user = _user_abook.get(id);
			if(user.supp_cmd.is_initialized()) {
				if(user.supp_cmd->get(cmd)) {
					in->destination(mih::id(id));
					_lpool.add(in);

					if(in->is_local())
						in->source(mihfid);

					ODTONE_LOG(1, send_msg , in->destination().to_string());
					_transmit(in);
				}
			}
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
 * Currently command service handover related messages are handled by
 * a single MIH-user. If this MIHF is the destination of the message,
 * check for a pending transaction and forwards the message.
 *
 * @param recv_msg The receive message output.
 * @param send_msg The send message output.
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::generic_command_response(const char *recv_msg,
					       const char *send_msg,
					       meta_message_ptr &in,
					       meta_message_ptr &out)
{
	ODTONE_LOG(1, recv_msg, in->source().to_string());

	if(utils::this_mihf_is_destination(in)) {
		if(!_lpool.set_user_tid(in)) {
			ODTONE_LOG(1, "(mics) warning: no local transaction for this msg ",
				"discarding it");
			return false;
		}

		ODTONE_LOG(1, send_msg , in->destination().to_string());
		in->opcode(mih::operation::confirm);
		_transmit(in);
	} else {
		if(!_lpool.set_user_tid(in)) {
			ODTONE_LOG(1, "(mics) warning: no local transaction for this msg ",
				"discarding it");
			return false;
		}

		ODTONE_LOG(1, send_msg , in->destination().to_string());
		in->source(mihfid);
		_transmit(in);
	}

	return false;
}

/**
 * Net Handover Candidate Query Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::net_ho_candidate_query_request(meta_message_ptr &in,
						     meta_message_ptr &out)
{
	return generic_command_request("(mics) received Net_HO_Candidate_Query.request from ",
					"(mics) sending a Net_HO_Candidate_Query.indication to ",
					in, out, mih::mih_cmd_net_ho_candidate_query);
}

/**
 * Net Handover Candidate Query Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::net_ho_candidate_query_response(meta_message_ptr &in,
						      meta_message_ptr &out)
{
	return generic_command_response("(mics) received Net_HO_Candidate_Query.response from ",
					"(mics) sending a Net_HO_Candidate_Query.confirm to ",
					in, out);
}

/**
 * MN Handover Query Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::mn_ho_candidate_query_request(meta_message_ptr &in,
						    meta_message_ptr &out)
{
	return generic_command_request("(mics) received a MN_HO_Candidate_Query.request from ",
				       "(mics) sending a MN_HO_Candidate_Query.indication to ",
				       in, out, mih::mih_cmd_mn_ho_candidate_query);
}

/**
 * MN Handover Query Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::mn_ho_candidate_query_response(meta_message_ptr &in,
						     meta_message_ptr &out)
{
	return generic_command_response("(mics) received MN_HO_Candidate_Query.response from ",
					"(mics) sending a MN_HO_Candidate_Query.confirm to ",
					in, out);
}

/**
 * N2N Handover Query Resources Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::n2n_ho_query_resources_request(meta_message_ptr &in,
						     meta_message_ptr &out)
{
	return generic_command_request("(mics) received a MN_N2N_HO_Query_Resources.request from ",
				       "(mics) sending a MN_N2N_HO_Query_Resources.indication to ",
				       in, out, mih::mih_cmd_n2n_ho_query_resources);
}

/**
 * N2N Handover Query Resources Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::n2n_ho_query_resources_response(meta_message_ptr &in,
						      meta_message_ptr &out)
{
	return generic_command_response("(mics) received MN_N2N_HO_Query_Resources.response from ",
					"(mics) sending a MN_N2N_HO_Query_Resources.confirm to ",
					in, out);
}


/**
 * MN Handover Commit Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::mn_ho_commit_request(meta_message_ptr &in,
					   meta_message_ptr &out)
{
	return generic_command_request("(mics) received a MN_HO_Commit.request from ",
				       "(mics) sending a MN_HO_Commit.indication to ",
				       in, out, mih::mih_cmd_mn_ho_commit);
}

/**
 * MN Handover Commit Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::mn_ho_commit_response(meta_message_ptr &in,
					    meta_message_ptr &out)
{
	return generic_command_response("(mics) received MN_HO_Commit.response from ",
					"(mics) sending a MN_HO_Commit.confirm to ",
					in, out);
}

/**
 * Net Handover Commit Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::net_ho_commit_request(meta_message_ptr &in,
					    meta_message_ptr &out)
{
	return generic_command_request("(mics) received a Net_HO_Commit.request from ",
				       "(mics) sending a Net_HO_Commit.indication to ",
				       in, out, mih::mih_cmd_net_ho_commit);
}

/**
 * Net Handover Commit Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::net_ho_commit_response(meta_message_ptr &in,
					     meta_message_ptr &out)
{
	return generic_command_response("(mics) received Net_HO_Commit.response from ",
					"(mics) sending a Net_HO_Commit.confirm to ",
					in, out);
}

/**
 * MN Handover Complete Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::mn_ho_complete_request(meta_message_ptr &in,
					     meta_message_ptr &out)
{
	return generic_command_request("(mics) received a MN_HO_Complete.request from ",
				       "(mics) sending a MN_HO_Complete.indication to ",
				       in, out, mih::mih_cmd_mn_ho_complete);
}

/**
 * MN Handover Complete Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::mn_ho_complete_response(meta_message_ptr &in,
					      meta_message_ptr &out)
{
	return generic_command_response("(mics) received MN_HO_Complete.response from ",
					"(mics) sending a MN_HO_Complete.confirm to ",
					in, out);
}

/**
 * N2N Handover Commit Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::n2n_ho_commit_request(meta_message_ptr &in,
					    meta_message_ptr &out)
{
	return generic_command_request("(mics) received a N2N_HO_Commit.request from ",
				       "(mics) sending a N2N_HO_Commit.indication to ",
				       in, out, mih::mih_cmd_n2n_ho_commit);
}

/**
 * N2N Handover Commit Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::n2n_ho_commit_response(meta_message_ptr &in,
					     meta_message_ptr &out)
{
	return generic_command_response("(mics) received N2N_HO_Commit.response from ",
					"(mics) sending a N2N_HO_Commit.confirm to ",
					in, out);
}


/**
 * N2N Handover Complete Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::n2n_ho_complete_request(meta_message_ptr &in,
					      meta_message_ptr &out)
{
	return generic_command_request("(mics) received a N2N_HO_Complete.request from ",
				       "(mics) sending a N2N_HO_Complete.indication to ",
				       in, out, mih::mih_cmd_n2n_ho_complete);
}

/**
 * N2N Handover Complete Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool command_service::n2n_ho_complete_response(meta_message_ptr &in,
					       meta_message_ptr &out)
{
	return generic_command_response("(mics) received N2N_HO_Complete.response from ",
					"(mics) sending a N2N_HO_Complete.confirm to ",
					in, out);
}


} /* namespace mihf */ } /* namespace odtone */

