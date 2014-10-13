//==============================================================================
// Brief   : Event Service
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

#include "event_service.hpp"

#include "log.hpp"
#include "link_book.hpp"
#include "mihfid.hpp"
#include "transmit.hpp"
#include "utils.hpp"

#include <odtone/debug.hpp>
#include <odtone/mih/request.hpp>
#include <odtone/mih/response.hpp>
#include <odtone/mih/confirm.hpp>
#include <odtone/mih/indication.hpp>
#include <odtone/mih/tlv_types.hpp>

#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
///////////////////////////////////////////////////////////////////////////////

extern odtone::uint16 kConf_MIHF_Link_Response_Time_Value;
extern odtone::uint16 kConf_MIHF_Link_Delete_Value;

namespace odtone { namespace mihf {




/**
 * Construct the event service.
 *
 * @param io The io_service object that event service module will use to
 * dispatch handlers for any asynchronous operations performed on
 * the socket.
 * @param lpool The local transaction pool module.
 * @param t The transmit module.
 * @param abook The address book module.
 * @param lbook The link book module.
 * @param ubook The user book module.
 */
event_service::event_service(io_service &io, local_transaction_pool &lpool,
							 transmit &t, address_book &abook, link_book &lbook,
							 user_book &ubook)
	: _io(io),
	  _lpool(lpool),
	  _transmit(t),
	  _abook(abook),
	  _link_abook(lbook),
	  _user_abook(ubook)
{
}

/**
 * Handler responsible for setting a failure Link Event Subscribe
 * response.
 *
 * @param ec Error code.
 * @param in The input message.
 */
void event_service::link_event_subscribe_response_timeout(const boost::system::error_code &ec, meta_message_ptr &in)
{
	if(ec)
		return;

	{
		boost::mutex::scoped_lock lock(_mutex);
		_timer.erase(in->tid());
	}

	mih::link_tuple_id link;
	meta_message_ptr out(new meta_message());

	*in >> mih::request(mih::request::event_subscribe)
		& mih::tlv_link_identifier(link);

	// Send failure message to the user
	ODTONE_LOG(1, "(mism) setting failure response to Link_Event_Subscribe.request");
	*out << mih::response(mih::response::event_subscribe)
	    & mih::tlv_status(mih::status_failure)
	    & mih::tlv_link_identifier(link);

	out->tid(in->tid());
	out->destination(in->source());
	out->source(mihfid);

	_transmit(out);
}

/**
 * Make a subscription for a given user.
 *
 * @param user The MIH-User/MIHF that request the subscription.
 * @param link The link to make the subscription.
 * @param events The events to subscribe.
 * @return The status of the operation.
 */
mih::status event_service::subscribe(const mih::id &user,
				     mih::link_tuple_id &link,
				     mih::mih_evt_list &events)
{
	event_registration_t reg;
	reg.user.assign(user.to_string());
	reg.link = link;

	boost::mutex::scoped_lock lock(_event_mutex);

	for(int i = 0; i < 32; i++) {
		if (events.get((mih::mih_evt_list_enum) i)) {
			reg.event = (mih::mih_evt_list_enum) i;
			std::list<event_registration_t>::iterator tmp;
			tmp = std::find(_event_subscriptions.begin(), _event_subscriptions.end(), reg);
			if (tmp == _event_subscriptions.end()) {
				_event_subscriptions.push_back(reg);
				ODTONE_LOG(3, "(mies) added subscription ", reg.user,
					":", reg.link.addr, ":", reg.event);
			}
		}
	}

	return mih::status_success;
}

/**
 * Deserialize the message, subscribe the user and send a response immediatly
 * if the events are already subscribed with the Link SAP. Otherwise, the MIHF
 * sends a request to the Link SAP to subscribe the desired events.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::local_event_subscribe_request(meta_message_ptr &in,
						  meta_message_ptr &out)
{
	mih::mih_evt_list	events;
	mih::link_tuple_id	link;

	// TODO: optional is not take in cosideration yet
	*in >> mih::request(mih::request::event_subscribe)
		& mih::tlv_link_identifier(link)
		& mih::tlv_event_list(events);

	mih::octet_string link_id = _link_abook.search_interface(link.type, link.addr);
	
	// Check if the Link SAP exists
	// If not replies with a failure status
	if(link_id.compare("") == 0) {
		*out << mih::response(mih::response::event_subscribe)
				& mih::tlv_status(mih::status_failure)
				& mih::tlv_link_identifier(link);

		out->tid(in->tid());
		out->source(mihfid);
		out->destination(in->source());

		ODTONE_LOG(1, "(mies) forwarding Event_Subscribe.response to ",
		    out->destination().to_string());

		return true;
	}

	// Check if requested events have been already subscribed
	mih::mih_evt_list event_tmp;
	{
		boost::mutex::scoped_lock lock(_event_mutex);

		BOOST_FOREACH(event_registration_t item, _event_subscriptions)
		{
			if(item.link == link)
				event_tmp.set(item.event);
		}
	}

	if(events == event_tmp) {
		mih::status st = subscribe(in->source(), link, events);

		if(st == mih::status_success) {
			*out << mih::response(mih::response::event_subscribe)
				& mih::tlv_status(st)
				& mih::tlv_link_identifier(link)
				& mih::tlv_event_list(events);
		} else {
			*out << mih::response(mih::response::event_subscribe)
				& mih::tlv_status(st)
				& mih::tlv_link_identifier(link);
		}

		out->tid(in->tid());
		out->source(mihfid);
		out->destination(in->source());

		ODTONE_LOG(1, "(mies) forwarding Event_Subscribe.response to ",
		    out->destination().to_string());

		return true;
	} else { // Subscribe requested events with Link SAP
		mih::link_evt_list evt;
		// Since the two bitmaps have the same values
		// we can assign them directly
		for (size_t i = 0; i < 32; ++i) {
			if(events.get((mih::mih_evt_list_enum)i)) {
				evt.set((mih::link_evt_list_enum)i);
			}
		}
		//

		*out << mih::request(mih::request::event_subscribe)
			& mih::tlv_link_evt_list(evt);

		out->destination(mih::id(link_id));
		out->source(in->source());
		out->tid(in->tid());

		// Check if the Link SAP is still active
		uint16 fails = _link_abook.fail(out->destination().to_string());
		if(fails > kConf_MIHF_Link_Delete_Value) {
			mih::octet_string dst = out->destination().to_string();
			_link_abook.inactive(dst);

			// Update MIHF capabilities
			utils::update_local_capabilities(_abook, _link_abook, _user_abook);
		} else {
			ODTONE_LOG(1, "(mies) forwarding Event_Subscribe.request to ",
			    out->destination().to_string());
			utils::forward_request(out, _lpool, _transmit);

			// Set the timer that will be responsible for sending a failure
			// response if necessary
			boost::shared_ptr<boost::asio::deadline_timer> timer = boost::make_shared<boost::asio::deadline_timer>(_io);
			timer->expires_from_now(boost::posix_time::milliseconds(kConf_MIHF_Link_Response_Time_Value));
			timer->async_wait(boost::bind(&event_service::link_event_subscribe_response_timeout, this, _1, in));

			{
				boost::mutex::scoped_lock lock(_mutex);
				_timer[in->tid()] = timer;
			}
		}

		return false;
	}
}

/**
 * Event Subscribe Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::event_subscribe_request(meta_message_ptr &in,
					    meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Event_Subscribe.request from ",
	    in->source().to_string());

	if (utils::this_mihf_is_destination(in))  {
		return local_event_subscribe_request(in, out);
	} else {
		utils::forward_request(in, _lpool, _transmit);
		return false;
	}

	return false;
}

/**
 * Event Subscribe Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::event_subscribe_response(meta_message_ptr &in,
					     meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Event_Subscribe.response from ",
	    in->source().to_string());

	// do we have a request from a user?
	if (!_lpool.set_user_tid(in)) {
		ODTONE_LOG(1, "(mies) warning: no local transaction for this msg ",
		    "discarding it");
		return false;
	}

	mih::status        st;
	mih::link_tuple_id link;
	boost::optional<mih::mih_evt_list> events;

	// parse incoming message to (event_registration_t) reg
	*in >> mih::response()
		& mih::tlv_status(st)
		& mih::tlv_link_identifier(link)
		& mih::tlv_event_list(events);
        
        
    std::cout<< "(mies) The link received is  "<<link<<std::endl;
	// add a subscription
	if (st == mih::status_success) {
		st = subscribe(mih::id(in->destination().to_string()), link, events.get());
	}

	ODTONE_LOG(1, "(mies) forwarding Event_Subscribe.response to ",
	    in->destination().to_string());

	// forward to user
	_transmit(in);

	return false;
}

/**
 * Event Subscribe Confirm message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::event_subscribe_confirm(meta_message_ptr &in,
					    meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Event_Subscribe.confirm from ",
	    in->source().to_string());

	_link_abook.reset(in->source().to_string());

	// do we have a request from a user?
	out->source(in->source());
	if (!_lpool.set_user_tid(out)) {
		ODTONE_LOG(1, "(mies) warning: no local transaction for this msg ",
		    "discarding it");
		return false;
	}

	{
		boost::mutex::scoped_lock lock(_mutex);
		_timer.erase(in->tid());
	}

	mih::status st;
	boost::optional<mih::link_evt_list> events;

	*in >> mih::confirm(mih::confirm::event_subscribe)
		& mih::tlv_status(st)
		& mih::tlv_link_evt_list(events);

	mih::link_tuple_id link;
	link.type = _link_abook.get(in->source().to_string()).link_id.type;
	link.addr = _link_abook.get(in->source().to_string()).link_id.addr;

	if(st == mih::status_success) {
		mih::mih_evt_list evt;

		// Since the two bitmaps have the same values
		// we can assign them directly
		for (size_t i = 0; i < 32; ++i) {
			if(events.get().get((mih::link_evt_list_enum)i)) {
				evt.set((mih::mih_evt_list_enum)i);
			}
		}
		//

		*out << mih::response(mih::response::event_subscribe)
			& mih::tlv_status(st)
			& mih::tlv_link_identifier(link)
			& mih::tlv_event_list(evt);

		st = subscribe(mih::id(out->destination().to_string()), link, evt);
	} else {
		*out << mih::response(mih::response::event_subscribe)
				& mih::tlv_status(st)
				& mih::tlv_link_identifier(link);
	}

	ODTONE_LOG(1, "(mies) forwarding Event_Subscribe.confirm to ",
	    out->destination().to_string());

	// forward to user
	out->source(mihfid);
	_transmit(out);

	return false;
}

/**
 * Check if there is events subscribed to a given Link SAP, which
 * are not required anymore.
 *
 * @param in The input message.
 * @param link The link to make the unsubscription.
 * @param events The events to unsubscribed.
 */
void event_service::link_unsubscribe(meta_message_ptr &in,
                                     mih::link_tuple_id &link,
                                     mih::mih_evt_list &events)
{
	boost::mutex::scoped_lock lock(_event_mutex);

	// Check if requested events have been already subscribed
	mih::mih_evt_list event_unsubscribe = events;
	BOOST_FOREACH(event_registration_t item, _event_subscriptions)
	{
		if(item.link == link) {
			if(item.user.compare(in->source().to_string()) != 0) {
				event_unsubscribe.clear(item.event);
			}
		}
	}

	// Only send message to Link SAP if there is any event to unsubscribed
	// with it
	mih::mih_evt_list empty;
	if(!(empty == event_unsubscribe))
	{
		mih::link_evt_list evt;
		// Since the two bitmaps have the same values
		// we can assign them directly
		for (size_t i = 0; i < 32; ++i) {
			if(event_unsubscribe.get((mih::mih_evt_list_enum)i)) {
				evt.set((mih::link_evt_list_enum)i);
			}
		}
		//

		*in << mih::request(mih::request::event_unsubscribe)
				& mih::tlv_link_evt_list(evt);

		mih::octet_string link_id = _link_abook.search_interface(link.type, link.addr);
		in->destination(mih::id(link_id));
		in->source(mihfid);

		// Check if the Link SAP is still active
		uint16 fails = _link_abook.fail(in->destination().to_string());
		if(fails > kConf_MIHF_Link_Delete_Value) {
			mih::octet_string dst = in->destination().to_string();
			_link_abook.inactive(dst);

			// Update MIHF capabilities
			utils::update_local_capabilities(_abook, _link_abook, _user_abook);
		}
		else {
			ODTONE_LOG(1, "(mies) forwarding Event_Subscribe.request to ",
			    in->destination().to_string());
			utils::forward_request(in, _lpool, _transmit);
		}
	}
}

/**
 * Unsubscribe the events related to a given user.
 *
 * @param user The MIH-User/MIHF that request the unsubscription.
 * @param link The link to make the unsubscription.
 * @param events The events to unsubscribe.
 * @return The status of the operation.
 */
mih::status event_service::unsubscribe(const mih::id &user,
				       mih::link_tuple_id &link,
				       mih::mih_evt_list &events)
{
	boost::mutex::scoped_lock lock(_event_mutex);

	std::list<event_registration_t>::iterator it;

	it = _event_subscriptions.begin();
	while (it != _event_subscriptions.end())
	{
		if (it->link == link &&
		    (it->user.compare(user.to_string()) == 0) &&
		    events.get((mih::mih_evt_list_enum) it->event)) {
				ODTONE_LOG(3, "(mies) removed subscription ", it->user,
				    ":", it->link.addr ,":", it->event);
				_event_subscriptions.erase(it++);
		}
		else {
			it++;
		}
	}

	return mih::status_success;
}

/**
 * Deserialize message, unsubscribe user and send a response to the
 * requestor.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::local_event_unsubscribe_request(meta_message_ptr &in,
													meta_message_ptr &out)
{
	mih::status st;
	mih::link_tuple_id link;
	mih::mih_evt_list events;

	*in >> mih::request(mih::request::event_unsubscribe)
		& mih::tlv_link_identifier(link)
		& mih::tlv_event_list(events);

	st = unsubscribe(in->source(), link, events);

	*out << mih::response(mih::response::event_unsubscribe)
		& mih::tlv_status(st)
		& mih::tlv_link_identifier(link)
		& mih::tlv_event_list(events);

	out->tid(in->tid());
	out->source(mihfid);
	out->destination(in->source());

	// Check if there is any request for the events
	link_unsubscribe(in, link, events);

	ODTONE_LOG(1, "(mies) forwarding Event_Unsubscribe.response to ",
		    out->destination().to_string());

	return true;
}

/**
 * Event Unsubscribe Request message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::event_unsubscribe_request(meta_message_ptr &in,
					      meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Event_Unsubscribe.request from ",
	    in->source().to_string());

	if (utils::this_mihf_is_destination(in)) {
		return local_event_unsubscribe_request(in, out);
	} else {
		utils::forward_request(in, _lpool, _transmit);
		return false;
	}

	return false;
}

/**
 * Event Unsubscribe Response message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::event_unsubscribe_response(meta_message_ptr &in,
					       meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Event_Unsubscribe.response from ",
	    in->source().to_string());

	// do we have a request from a user?
	if (!_lpool.set_user_tid(in)) {
		ODTONE_LOG(1, "(mics) warning: no local transaction for this msg ",
		    "discarding it");

		return false;
	}

	mih::status	       st;
	mih::link_tuple_id link;
	boost::optional<mih::mih_evt_list> events;

	// parse incoming message to (event_registration_t) reg
	*in >>  mih::response(mih::response::event_unsubscribe)
		& mih::tlv_status(st)
		& mih::tlv_link_identifier(link)
		& mih::tlv_event_list(events);

	// remove subscription
	if (st == mih::status_success) {
		st = unsubscribe(mih::id(in->destination().to_string()), link, events.get());
	}

	ODTONE_LOG(1, "(mies) forwarding Event_Unsubscribe.response to ",
	    in->destination().to_string());

	// forward to user
	in->opcode(mih::operation::response);
	_transmit(in);

	return false;
}

/**
 * Event Unsubscribe Confirm message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::event_unsubscribe_confirm(meta_message_ptr &in,
					       meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Event_Unsubscribe.confirm from ",
	    in->source().to_string());

	_link_abook.reset(in->source().to_string());

	// do we have a request from a user?
	if (!_lpool.set_user_tid(in)) {
		ODTONE_LOG(1, "(mies) warning: no local transaction for this msg ",
		    "discarding it");
		return false;
	}

	mih::status	st;
	boost::optional<mih::link_evt_list> events;

	// parse incoming message to (event_registration_t) reg
	*in >> mih::confirm(mih::confirm::event_unsubscribe)
		& mih::tlv_status(st)
		& mih::tlv_link_evt_list(events);

	if (st == mih::status_success) {
		ODTONE_LOG(1, "(mies) Events successfully unsubscribed in Link SAP ",
			in->source().to_string());
	}

	return false;
}

/**
 * Forward the message for all users subscribed to event from the
 * Link SAP.
 *
 * @param msg The MIH Message.
 * @param li The link identifier.
 * @param event The related event.
 */
void event_service::msg_forward(meta_message_ptr &msg,
				mih::link_tuple_id &li,
				mih::mih_evt_list_enum event)
{
	std::list<event_registration_t>::iterator it;
	int i = 0; // for logging purposes

	if(msg->is_local())
		msg->source(mihfid);

	for(it = _event_subscriptions.begin();
	    it != _event_subscriptions.end();
	    it++, i++) {
        ODTONE_LOG(3, "(mies) msg_forward() comparing event ", event, " with event_subscription it->event ", it->event);
        ODTONE_LOG(3, "(mies) msg_forward() comparing link_tuple_id.type ", li.type, " with event_subscription it->link.type ", it->link.type);
        ODTONE_LOG(3, "(mies) msg_forward() comparing link_tuple_id.addr ", li.addr, " with event_subscription it->link.addr ", it->link.addr);
		if ((it->event == event) && (it->link == li)) {
			ODTONE_LOG(3, "(mies) found registration of user: ",
			    it->user, " for event type ", event);
			msg->destination(mih::id(it->user));
			_transmit(msg);
		}
	}
    ODTONE_LOG(3, "(mies) msg_forward() end");
}


/**
 * Parse the link identifier from incoming message and forwards the
 * message to subscribed users
 *
 * @param msg The MIH Message.
 * @param event The related event.
 */
void event_service::link_event_forward(meta_message_ptr &msg,
				       mih::mih_evt_list_enum event)
{
	mih::link_tuple_id li;
    ODTONE_LOG(1, "(mies) link_event_forward()");
	*msg >> mih::indication()
		& mih::tlv_link_identifier(li);

	msg_forward(msg, li, event);
}


/**
 * Link Up Indication message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::link_up_indication(meta_message_ptr &in, meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Link_Up.indication from ",
	    in->source().to_string());

	if(in->is_local()) {
	    ODTONE_LOG(1, "(mies) link_up_indication() reset ", in->source().to_string() ," in link_abook");
		_link_abook.reset(in->source().to_string());
	}

	link_event_forward(in, mih::mih_evt_link_up);

	return false;
}


/**
 * Link Down Indication message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::link_down_indication(meta_message_ptr &in, meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Link_Down.indication from ",
	    in->source().to_string());

	if(in->is_local())
		_link_abook.reset(in->source().to_string());

	link_event_forward(in, mih::mih_evt_link_down);

	return false;
}


/**
 * Link Detected Indication message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::link_detected_indication(meta_message_ptr &in,
					     meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Link_Detected.indication from ",
	    in->source().to_string());

	if(in->is_local()) {
		_link_abook.reset(in->source().to_string());
		// link detected info from incoming message
		mih::link_det_info		link_info;
		// link detected info on outgoing message
		mih::link_det_info_list		list_rsp;

		*in >> mih::indication()
			& mih::tlv_link_det_info(link_info);

		list_rsp.push_back(link_info);

		*in << mih::indication(mih::indication::link_detected)
			& mih::tlv_link_det_info_list(list_rsp);
	}

	std::list<event_registration_t>::iterator it;
	int i = 0; // for logging purposes

	if(in->is_local())
		in->source(mihfid);

	for(it = _event_subscriptions.begin();
	    it != _event_subscriptions.end();
	    it++, i++) {
		if (it->event == mih::mih_evt_link_detected) {
			ODTONE_LOG(3, i, " (mies) found registration of user: ",
			    it->user, " for event type ", mih::mih_evt_link_detected);
			in->destination(mih::id(it->user));
			_transmit(in);
		}
	}

	return false;
}

/**
 * Link Going Down Indication message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::link_going_down_indication(meta_message_ptr &in,
					       meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Link_Going_Down.indication from ",
	    in->source().to_string());

	if(in->is_local())
		_link_abook.reset(in->source().to_string());

	link_event_forward(in, mih::mih_evt_link_going_down);

	return false;
}

/**
 * Link Parameters Report Indication message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::link_parameters_report_indication(meta_message_ptr &in,
					       meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Link_Parameters_Report.indication from ",
	    in->source().to_string());

	if(in->is_local())
		_link_abook.reset(in->source().to_string());

	link_event_forward(in, mih::mih_evt_link_parameters_report);

	return false;
}

/**
 * Link Handover Imminent Indication message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::link_handover_imminent_indication(meta_message_ptr &in,
						      meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Link_Handover_Imminent.indication from ",
	    in->source().to_string());

	if(in->is_local())
		_link_abook.reset(in->source().to_string());

	std::list<event_registration_t>::iterator it;
	int i = 0; // for logging purposes

	if(in->is_local())
		in->source(mihfid);

	for(it = _event_subscriptions.begin();
	    it != _event_subscriptions.end();
	    it++, i++) {
		if (it->event == mih::mih_evt_link_handover_imminent) {
			ODTONE_LOG(3, i, " (mies) found registration of user: ",
			    it->user, " for event type ", mih::mih_evt_link_handover_imminent);
			in->destination(mih::id(it->user));
			_transmit(in);
		}
	}

	return false;
}

/**
 * Link Handover Complete Indication message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::link_handover_complete_indication(meta_message_ptr &in,
						      meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Link_Handover_Complete.indication from ",
	    in->source().to_string());

	if(in->is_local())
		_link_abook.reset(in->source().to_string());

	mih::link_tuple_id oli;
	mih::link_tuple_id nli;
	boost::optional<mih::link_addr> oar;
	boost::optional<mih::link_addr> nar;

	if(in->is_local()) {
		mih::status st;
		*in >> mih::indication(mih::indication::link_handover_complete)
			& mih::tlv_link_identifier(oli)
			& mih::tlv_new_link_identifier(nli)
			& mih::tlv_old_access_router(oar)
			& mih::tlv_new_access_router(nar)
			& mih::tlv_status(st);
	} else {
		*in >> mih::indication(mih::indication::link_handover_complete)
			& mih::tlv_link_identifier(oli)
			& mih::tlv_new_link_identifier(nli)
			& mih::tlv_old_access_router(oar)
			& mih::tlv_new_access_router(nar);
	}

	*in << mih::indication(mih::indication::link_handover_complete)
			& mih::tlv_link_identifier(oli)
			& mih::tlv_new_link_identifier(nli)
			& mih::tlv_old_access_router(oar)
			& mih::tlv_new_access_router(nar);

	std::list<event_registration_t>::iterator it;
	int i = 0; // for logging purposes

	if(in->is_local())
		in->source(mihfid);

	for(it = _event_subscriptions.begin();
	    it != _event_subscriptions.end();
	    it++, i++) {
		if (it->event == mih::mih_evt_link_handover_complete) {
			ODTONE_LOG(3, i, " (mies) found registration of user: ",
			    it->user, " for event type ", mih::mih_evt_link_handover_complete);
			in->destination(mih::id(it->user));
			_transmit(in);
		}
	}

	return false;
}

/**
 * Link PDU Transmit Status Indication message handler.
 *
 * @param in The input message.
 * @param out The output message.
 * @return True if the response is sent immediately or false otherwise.
 */
bool event_service::link_pdu_transmit_status_indication(meta_message_ptr &in,
							meta_message_ptr &out)
{
	ODTONE_LOG(1, "(mies) received Link_PDU_Transmit_Status.indication from ",
	    in->source().to_string());

	if(in->is_local())
		_link_abook.reset(in->source().to_string());

	link_event_forward(in, mih::mih_evt_link_pdu_transmit_status);

	return false;
}

} /* namespace mihf */ } /* namespace odtone */
// EOF ////////////////////////////////////////////////////////////////////////
