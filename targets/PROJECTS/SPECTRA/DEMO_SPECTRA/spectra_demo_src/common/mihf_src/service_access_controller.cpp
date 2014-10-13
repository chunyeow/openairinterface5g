//==============================================================================
// Brief   : Service Access Controller
// Authors : Simao Reis <sreis@av.it.pt>
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
#include "service_access_controller.hpp"
#include "meta_message.hpp"
#include "log.hpp"
#include "mihfid.hpp"

#include <odtone/debug.hpp>
#include <odtone/mih/config.hpp>
#include <odtone/mih/request.hpp>
#include <odtone/mih/response.hpp>
#include <odtone/mih/indication.hpp>

#include <map>
///////////////////////////////////////////////////////////////////////////////


namespace odtone { namespace mihf {

static std::map<uint, handler_t> _callbacks;	/**< Callback map of the supported messages.*/

/**
 * Registering a callback handler for a MIH message identifier.
 * This should only be used on MIHF initialization because it's not thread safe.
 *
 * @param mid The MIH Message identifier.
 * @param func The handler function.
 */
void sac_register_callback(uint mid, handler_t func)
{
    ODTONE_LOG(1, "(sac) sac_register_callback(): mid ", mid, " handler ", func);
	_callbacks[mid] = func;
}

/**
 * Construct a Service Access Controller.
 *
 * @param t The transmit module.
 */
sac_dispatch::sac_dispatch(transmit &t)
	: _transmit(t)
{
}

/**
 * Check if there is a handler for this message and call it, else
 * discard message.
 *
 * @param in The input message.
 */
void sac_dispatch::operator()(meta_message_ptr& in)
{
	/** __no__ authentication at this point */

	uint mid = in->mid();

	ODTONE_LOG(1, "(sac) dispatching message with mid: ", mid);
	//
	// no thread safety because insertion should __only__ be made
	// on MIHF initialization
	//
	std::map<uint, handler_t>::iterator it;
	it = _callbacks.find(mid);

	if(it != _callbacks.end()) {
		handler_t process_message = it->second;
		meta_message_ptr out(new meta_message);

		out->tid(in->tid());
		// send response if it was generated
		try {

			if (process_message(in, out)) {
				_transmit(out);
			} else {
	            ODTONE_LOG(1, "sac_dispatch::operator() Message not transmitted out.");
			}

		} catch(mih::bad_tlv) {
			ODTONE_LOG(1, "Discarding malformed message.");
		} catch(unknown_link_sap) {
			ODTONE_LOG(1, "Received message from an unknown Link SAP. Discarding message.");
		} catch(unknown_mih_user) {
			ODTONE_LOG(1, "Received message from an unknown MIH-User. Discarding message.");
		}
	} else {
			ODTONE_LOG(1, "(sac) (warning) message with mid: ", mid,
						  " unknown, discarding.");
	}
}

/**
 * Check if there's a handler for this message and call it, else
 * discard message.
 *
 * @param in The input message.
 * @param out The output message.
 */
bool sac_process_message(meta_message_ptr& in, meta_message_ptr& out)
{
	// discard messages that this MIHF broadcasted to itself
	// discard messages that are not destined to this MIHF or if
	// multicast messages are not supported
	if(in->source() == mihfid) {
		ODTONE_LOG(1, "(sac) Discarding message! Reason: ",
					  "message was broadcasted to itself");
		return false;
	}

	if(!utils::this_mihf_is_destination(in) && !utils::is_multicast(in)) {
		ODTONE_LOG(1, "(sac) Discarding message! Reason: ",
					  "this is not the message destination");
		return false;
	}

	/** __no__ authentication at this point */

	uint mid = in->mid();

	//
	// no thread safety because insertion should __only__ be made
	// on MIHF initialization
	//
	std::map<uint, handler_t>::iterator it;
	it = _callbacks.find(mid);

	if(it != _callbacks.end()) {
		handler_t process_message = it->second;

		bool rsp;

		try {

			rsp = process_message(in, out);

		} catch(mih::bad_tlv) {
			ODTONE_LOG(1, "Discarding malformed message.");
			return false;
		}

		// set ip and port of response message
		out->ip(in->ip());
		out->scope(in->scope());
		out->port(in->port());

		// response message must have the same tid
		out->tid(in->tid());

		return rsp;
	} else {
		ODTONE_LOG(1, "(sac) (warning) message with mid: ", mid,
		    " unknown, discarding.");
	}

	return false;
}

} /* namespace mihf */ } /* namespace odtone */
