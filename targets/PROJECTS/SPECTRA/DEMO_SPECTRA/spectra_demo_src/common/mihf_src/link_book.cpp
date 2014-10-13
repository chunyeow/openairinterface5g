//==============================================================================
// Brief   : Link Book
// Authors : Carlos Guimarães <cguimaraes@av.it.pt>
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
#include "link_book.hpp"
#include "log.hpp"

///////////////////////////////////////////////////////////////////////////////

namespace odtone { namespace mihf {

//-----------------------------------------------------------------------------
static std::string evt2string(mih::link_evt_list evtP){
//-----------------------------------------------------------------------------
    std::string s=std::string(" ");
    if(evtP.get(mih::evt_link_detected))            s += "DETECTED ";
    if(evtP.get(mih::evt_link_up))                  s += "UP ";
    if(evtP.get(mih::evt_link_down))                s += "DOWN ";
    if(evtP.get(mih::evt_link_parameters_report))   s += "PARAMETERS_REPORT ";
    if(evtP.get(mih::evt_link_going_down))          s += "GOING_DOWN ";
    if(evtP.get(mih::evt_link_handover_imminent))   s += "HANDOVER_IMMINENT ";
    if(evtP.get(mih::evt_link_handover_complete))   s += "HANDOVER_COMPLETE ";
    if(evtP.get(mih::evt_link_pdu_transmit_status)) s += "PDU_TRANSMIT_STATUS ";
    return s;
}
enum link_cmd_list_enum {
    cmd_link_event_subscribe      = 1,  /**< Event subscribe.       */
    cmd_link_event_unsubscribe    = 2,  /**< Event unsubscribe.     */
    cmd_link_get_parameters       = 3,  /**< Get parameters.        */
    cmd_link_configure_thresholds = 4,  /**< Configure thresholds.  */
    cmd_link_action               = 5,  /**< Action.                */
};
//-----------------------------------------------------------------------------
static std::string cmd2string(mih::link_cmd_list cmdP){
//-----------------------------------------------------------------------------
    std::string s=std::string(" ");
    if(cmdP.get(mih::cmd_link_event_subscribe))        s += "EVENT_SUBSCRIBE ";
    if(cmdP.get(mih::cmd_link_event_unsubscribe))      s += "EVENT_UNSUBSCRIBE";
    if(cmdP.get(mih::cmd_link_get_parameters))         s += "GET_PARAMETERS ";
    if(cmdP.get(mih::cmd_link_configure_thresholds))   s += "CONFIGURE_THRESHOLDS ";
    if(cmdP.get(mih::cmd_link_action))                 s += "ACTION ";
    return s;
}
/**
 * Add a new Link SAP entry in the link book.
 *
 * @param id Link SAP MIH Identifier.
 * @param ip Link SAP IP address.
 * @param port Link SAP listening port.
 * @param link_id interfaces that Link SAP manages.
 */
void link_book::add(const mih::octet_string &id,
		            mih::octet_string& ip,
		            uint16 port,
		            mih::link_id link_id)
{
	boost::mutex::scoped_lock lock(_mutex);
	// TODO: add thread safety
	link_entry a;

	a.ip.assign(ip);
	a.port = port;
	a.link_id = link_id;
	a.fail = 0;
	a.status = true;

	_lbook[id] = a;
	ODTONE_LOG(4, "(link_book) added: ", id, " ", ip, " ", port, " ", link_id);
}

/**
 * Set the IP address of an existing Link SAP entry.
 *
 * @param id Link SAP MIH Identifier.
 * @param ip The IP address to set.
 */
void link_book::set_ip(const mih::octet_string &id, std::string ip)
{
	boost::mutex::scoped_lock lock(_mutex);

	std::map<mih::octet_string, link_entry>::iterator it;
	it = _lbook.find(id);

	if (it != _lbook.end())
		it->second.ip = ip;
}

/**
 * Set the port of an existing Link SAP entry.
 *
 * @param id Link SAP MIH Identifier.
 * @param port The port to set.
 */
void link_book::set_port(const mih::octet_string &id, uint16 port)
{
	boost::mutex::scoped_lock lock(_mutex);

	std::map<mih::octet_string, link_entry>::iterator it;
	it = _lbook.find(id);

	if (it != _lbook.end())
		it->second.port = port;
}

/**
 * Update the events and commands supported by a Link SAP.
 *
 * @param id Link SAP MIH Identifier.
 * @param event_list Supported event list.
 * @param cmd_list Supported command list.
 */
void link_book::update_capabilities(const mih::octet_string &id,
									mih::link_evt_list event_list,
									mih::link_cmd_list cmd_list)

{
	boost::mutex::scoped_lock lock(_mutex);

	std::map<mih::octet_string, link_entry>::iterator it;
	it = _lbook.find(id);

	if (it != _lbook.end()) {
        ODTONE_LOG(4, "(link_book) update_capabilities() FOUND: ", id, " ", evt2string(event_list), " ", cmd2string(cmd_list));
		it->second.event_list = event_list;
		it->second.cmd_list = cmd_list;
	} else {
	    ODTONE_LOG(4, "(link_book) update_capabilities() NOT FOUND: ", id, " ", evt2string(event_list), " ", cmd2string(cmd_list));
	}
}

/**
 * Remove an existing Link SAP entry.
 *
 * @param id Link SAP MIH Identifier.
 */
void link_book::del(mih::octet_string &id)
{
	boost::mutex::scoped_lock lock(_mutex);

	_lbook.erase(id);
}

/**
 * Inactive an existing Link SAP entry.
 *
 * @param id Link SAP MIH Identifier.
 */
void link_book::inactive(mih::octet_string &id)
{
	boost::mutex::scoped_lock lock(_mutex);

	std::map<mih::octet_string, link_entry>::iterator it;
	it = _lbook.find(id);

	if (it != _lbook.end()) {
        ODTONE_LOG(4, "(link_book) inactive() : ", id);
		it->second.status = false;
	}
}

/**
 * Get the record for a given Link SAP.
 *
 * @param id Link SAP MIH Identifier.
 * @return The record for a given Link SAP.
 */
const link_entry& link_book::get(const mih::octet_string &id)
{
	boost::mutex::scoped_lock lock(_mutex);

	std::map<mih::octet_string, link_entry>::const_iterator it;
	it = _lbook.find(id);

	if (it == _lbook.end())
		boost::throw_exception(unknown_link_sap());

	return it->second;
}

/**
 * Get the list of all known Link SAPs.
 *
 * @return The list of all known Link SAPs.
 */
const std::vector<mih::octet_string> link_book::get_ids()
{
	boost::mutex::scoped_lock lock(_mutex);

	std::vector<mih::octet_string> ids;
	for(std::map<mih::octet_string, link_entry>::iterator it = _lbook.begin(); it != _lbook.end(); it++) {
		ids.push_back(it->first);
	}

	return ids;
}

/**
 * Search for the Link SAP MIH Identifier of a given interface.
 *
 * @param lt The link type of the Link SAP to search for.
 * @param la The link address of the Link SAP to search for.
 * @return The Link SAP MIH Identifier.
 */
const mih::octet_string link_book::search_interface(mih::link_type lt, mih::link_addr la)
{
	boost::mutex::scoped_lock lock(_mutex);

	mih::octet_string id;
    std::map<mih::octet_string, link_entry>::iterator it;
        for(it = _lbook.begin(); it != _lbook.end(); it++) {
            
                    ODTONE_LOG(4, "(link_book) ID LINK TYPE  : ", it->first, " ADDRESS ", it->second.link_id.addr, " STATUS", it->second.status);
        }
    
	for(/*std::map<mih::octet_string, link_entry>::iterator*/ it = _lbook.begin(); it != _lbook.end(); it++) {
        
             ODTONE_LOG(4, "(link_book) search 1st step  ID LINK TYPE comparison type:",it->second.link_id.type, "LT", lt);
		if(it->second.link_id.type == lt) {
             ODTONE_LOG(4, "(link_book) search 2nd step  ID LINK TYPE comparison addr:",it->second.link_id.addr, "LA", la);
			if(it->second.link_id.addr == la) {
             ODTONE_LOG(4, "(link_book) search 3rd step  ID LINK TYPE check status:",it->second.status);
             if(it->second.status) {
                    ODTONE_LOG(4, "(link_book) search 3rd step  ID LINK TYPE  : ", it->first, " ADDRESS ", it->second.link_id.addr, " STATUS", it->second.status);
					id = it->first;
					break;
				}
			}
		}
	}

    ODTONE_LOG(4, "(link_book) search_interface() : ", lt, " ", la, " Found:", id);
	return id;
}

/**
 * Update and return the number of fail responses of a given Link SAP.
 *
 * @param id Link SAP MIH Identifier.
 * @return The number of fails responses.
 */
uint16 link_book::fail(const mih::octet_string &id)
{
	boost::mutex::scoped_lock lock(_mutex);

	std::map<mih::octet_string, link_entry>::iterator it;
	it = _lbook.find(id);

	if (it == _lbook.end())
		boost::throw_exception(unknown_link_sap());

	(it->second.fail)++;
    ODTONE_LOG(4, "(link_book) fail() : ", id ," num fails:",it->second.fail);
	return it->second.fail;
}

/**
 * Reset the number of fail responses of a given Link SAP.
 *
 * @param id Link SAP MIH Identifier.
 */
void link_book::reset(const mih::octet_string &id)
{
	boost::mutex::scoped_lock lock(_mutex);

	std::map<mih::octet_string, link_entry>::iterator it;
	it = _lbook.find(id);

	if (it == _lbook.end())
		boost::throw_exception(unknown_link_sap());

    ODTONE_LOG(4, "(link_book) reset() : ", id);
	it->second.fail = 0;
	it->second.status = true;
}

} /* namespace mihf */ } /* namespace odtone */
