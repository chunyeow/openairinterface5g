//==============================================================================
// Brief   : Transaction Pool
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
#include "transaction_pool.hpp"
///////////////////////////////////////////////////////////////////////////////

namespace odtone { namespace mihf {

/**
 * Construct a transaction pool.
 *
 * @param io The io_service object that transaction pool module will
 * use to dispatch handlers for any asynchronous operations performed on
 * the socket.
 */
transaction_pool::transaction_pool(io_service &io)
	: _timer(io, boost::posix_time::seconds(1)),
	  _dst_mutex(),
	  _src_mutex()
{
	// start timer
	_timer.expires_from_now(boost::posix_time::seconds(1));
	_timer.async_wait(boost::bind(&transaction_pool::tick, this));
}

/**
 * Decrements the timer of each transaction only if its value is
 * greater than 0.
 *
 * @param set The transaction type.
 * @param it The transaction.
 * @param mutex The mutex.
 */
template <class Set, class SetIterator>
void transaction_pool::dec(Set &set,
			    SetIterator &it,
			    boost::mutex &mutex)
{
	Set del_these;

	{
		boost::mutex::scoped_lock lock(mutex);

		for(it = set.begin(); it != set.end(); it++) {
			(*it)->transaction_stop_when--;
			ODTONE_LOG(1, "(transaction_pool) decrementing tid ", (*it)->tid, " counter down to ", (*it)->transaction_stop_when);
			if ((*it)->transaction_stop_when == 0) {
				(*it)->run();

				if ((*it)->transaction_status == ONGOING)
					del_these.insert(*it);
			}

			if ((*it)->ack_requestor_status == ONGOING) {
				(*it)->retransmission_when--;
				if ((*it)->retransmission_when == 0) {
					(*it)->ack_requestor();
					(*it)->run();
				}
			}

			if ((*it)->transaction_status != ONGOING)
				del_these.insert(*it);
		}
	}

	// delete finished transactions
	for (it = del_these.begin(); it != del_these.end(); it++)
		del(*it);
}

/**
 * Decrements each transaction timer existente in the transaction pool.
 */
void transaction_pool::tick()
{
    //ODTONE_LOG(1, "(transaction_pool) tick");
	_timer.expires_from_now(boost::posix_time::seconds(1));
	_timer.async_wait(boost::bind(&transaction_pool::tick, this));

	src_transaction_set::iterator src_it;
	if (_src.size() > 0)
		dec(_src, src_it, _src_mutex);

	dst_transaction_set::iterator dst_it;
	if (_dst.size() > 0)
		dec(_dst, dst_it, _dst_mutex);
}

/**
 * Add a new source transaction entry in the transaction pool.
 *
 * @param t The source transaction pointer.
 */
void transaction_pool::add(src_transaction_ptr &t)
{
    ODTONE_LOG(1, "(transaction_pool) add src tid ", t->tid);
	boost::mutex::scoped_lock lock(_src_mutex);
	_src.insert(t);
}

/**
 * Add a new destination transaction entry in the transaction pool.
 *
 * @param t The destination transaction pointer.
 */
void transaction_pool::add(dst_transaction_ptr &t)
{
    ODTONE_LOG(1, "(transaction_pool) add dst tid ", t->tid);
	boost::mutex::scoped_lock lock(_dst_mutex);
	_dst.insert(t);
}

/**
 * Remove an existing source transaction entry from the transaction pool.
 *
 * @param t The source transaction pointer to be removed.
 */
void transaction_pool::del(const src_transaction_ptr &t)
{
    ODTONE_LOG(1, "(transaction_pool) del src tid ", t->tid);
	boost::mutex::scoped_lock lock(_src_mutex);
	_src.erase(t);
}

/**
 * Remove a existing destination transaction entry from the transaction pool.
 *
 * @param t destination transaction pointer to be removed.
 */
void transaction_pool::del(const dst_transaction_ptr &t)
{
    ODTONE_LOG(1, "(transaction_pool) del dst tid ", t->tid);
	boost::mutex::scoped_lock lock(_dst_mutex);
	_dst.erase(t);
}

/**
 * Searchs for a source transaction in the transaction pool.
 *
 * @param id The MIH destination identifier to search for.
 * @param tid The transaction identifier to search for.
 * @param t The source transaction pointer.
 */
void transaction_pool::find(const mih::id &id, uint16 tid, src_transaction_ptr &t)
{
	boost::mutex::scoped_lock lock(_src_mutex);
	src_transaction_set::iterator it;

	for(it = _src.begin(); it != _src.end(); it++) {
		if(((*it)->peer_mihf_id == id) && (*it)->tid == tid) {
			t = *it;
			return;
		}
        }
}

/**
 * Searchs for a destination transaction in the transaction pool.
 *
 * @param id The MIH source identifier to search for.
 * @param tid The transaction identifier to search for.
 * @param t The destination transaction pointer.
 */
void transaction_pool::find(const mih::id &id, uint16 tid, dst_transaction_ptr &t)
{
	boost::mutex::scoped_lock lock(_dst_mutex);
	dst_transaction_set::iterator it;

	for(it = _dst.begin(); it != _dst.end(); it++) {
		if(((*it)->peer_mihf_id == id) && (*it)->tid == tid) {
			t = *it;
			return;
		}
        }
}


} /* namespace mihf */ } /* namespace odtone */
