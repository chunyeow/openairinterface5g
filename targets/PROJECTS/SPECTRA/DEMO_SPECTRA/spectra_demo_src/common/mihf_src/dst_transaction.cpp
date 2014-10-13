//==============================================================================
// Brief   : Destination Transaction
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
#include "log.hpp"
#include "dst_transaction.hpp"
#include "utils.hpp"
///////////////////////////////////////////////////////////////////////////////

namespace odtone { namespace mihf {

/**
 * Constructor a Destination State Machine transaction.
 *
 * @param f The transaction handler.
 * @param netsap The netsap module.
 */
dst_transaction_t::dst_transaction_t(handler_t &f, net_sap &netsap)
	: transaction_t(f, netsap)
{
	state = DST_INIT;
}

/**
 * Run Destination State Machine transaction.
 */
void dst_transaction_t::run()
{
	switch (state)
        {
        case DST_INIT: goto _init_lbl_;
        case DST_WAIT_RESPONSE_PRM: goto _wait_response_prm_lbl_;
        case DST_SEND_RESPONSE: goto _send_response_lbl_;
        case DST_FAILURE: goto _failure_lbl_;
        case DST_SUCCESS: goto _success_lbl_;
        }

  _init_lbl_:
	{
        ODTONE_LOG(1, "(dst_transaction_t) init tid ", in->tid());
		transaction_status    = ONGOING;
		opcode                = in->opcode();
		tid                   = in->tid();
		transaction_stop_when = 15; // TODO: read from config
		is_multicast          = utils::is_multicast(in);
		peer_mihf_id          = in->source();
		my_mihf_id            = in->destination();
		start_ack_responder   = (in->ackreq() && !is_multicast);
		msg_in_avail          = false;

		out.reset(new meta_message);
		msg_out_avail = process_message(in, out);

		if (start_ack_responder)
			ack_responder();

		if (opcode == mih::operation::request)
			goto _wait_response_prm_lbl_;
		else if ((opcode == mih::operation::indication)
			 || (opcode == mih::operation::response))
			goto _success_lbl_;
	}

  _wait_response_prm_lbl_:
	{
        ODTONE_LOG(1, "(dst_transaction_t) wait response tid ", tid);
		state = DST_WAIT_RESPONSE_PRM;

		if (transaction_stop_when == 0)
			goto _failure_lbl_;

		if (msg_out_avail && (!start_ack_responder || out->ackrsp()))
			goto _send_response_begin_lbl_;

		return;
	}

  _send_response_begin_lbl_:
	{
        ODTONE_LOG(1, "(dst_transaction_t) send response begin tid ", tid);
		state = DST_SEND_RESPONSE;

		start_ack_requestor  = out->ackreq();
		if(start_ack_requestor)
			ack_requestor();
		ack_requestor_status = ONGOING;

		_netsap.send(out);
	}

  _send_response_lbl_:
	{
        ODTONE_LOG(1, "(dst_transaction_t) send response tid ", tid);
		if (!start_ack_requestor || ack_requestor_status == SUCCESS)
			goto _success_lbl_;
		else if (ack_requestor_status == FAILURE)
			goto _failure_lbl_;

		return;
	}

  _failure_lbl_:
	{
        ODTONE_LOG(1, "(dst_transaction_t) failure tid ", tid);
		state              = DST_FAILURE;
		transaction_status = FAILURE;

		return;
	}

  _success_lbl_:
	{
        ODTONE_LOG(1, "(dst_transaction_t) success tid ", tid);
		state              = DST_SUCCESS;
		transaction_status = SUCCESS;
	}

	return;
}


} /* namespace mihf */ } /* namespace odtone */
