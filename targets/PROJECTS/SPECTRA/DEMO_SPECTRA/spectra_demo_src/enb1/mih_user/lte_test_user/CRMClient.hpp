//==============================================================================
// Brief   : MIH-User
// Authors : FATMA HRIZI <hrizi@eurecom.fr>
//------------------------------------------------------------------------------
// ODTONE - Open Dot Twenty One
//
// Copyright (C) 2009-2012 Universidade Aveiro
// Copyright (C) 2009-2012 Instituto de Telecomunicações - Pólo Aveiro
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//==============================================================================

#ifndef ODTONE_CRM_CLIENT_HPP
#define ODTONE_CRM_CLIENT_HPP


#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <iostream>
#include <ostream> 
#include <sstream>
#include "cpprest/basic_types.h"
#include "cpprest/asyncrt_utils.h"
#include "cpprest/uri.h"
#include <string>


/**
 *  CRM Client Class
 *  
 *  Defines the required functions to Get/Store Data from/in the CRRM
 * 
 **/
class CRMClient {

public: 

    CRMClient();
    CRMClient(char* url);
    ~CRMClient();
    
    void SetUrl(char* url);
    
    /**
     *  Get the data from the CRRM
     * 
     *  Returns JSON Format
    */
    pplx::task<void> RequestJSONValueAsync();
    
    /**
     *  Store Policies data from the CRRM
     * 
     *  Takes 4 parameters: P_ID, Name, Description, Value
     *  Returns the JSON format of the stored entity
    */
    pplx::task<void> StoreJSONValuePolicies(char * param1, char * param2, char * param3, char * param4);
    
    /**
     *  Store Measurements data from the CRRM
     * 
     *  Takes 5 parameters: M_ID, Name, Type, Unit, Value, Time
     *  Returns the JSON format of the stored entity
    */
    pplx::task<void> StoreJSONValuemeasurements(char * param1, char * param2, char * param3, char * param4, char * param5, char * param6);
    
    /**
     *  Store Decisions data from the CRRM
     * 
     *  Takes 4 parameters: D_ID, Name, Description, Value
     *  Returns the JSON format of the stored entity
    */
    pplx::task<void> StoreJSONValuedecisions(char * param1, char * param2, char * param3, char * param4, char * param5);
    
    
    
    /**
     *  Delete Entities from the CRRM
     * 
     *  Returns the JSON format of the deleted entity
    */
    pplx::task<void> Delete();
	

private:
    char * m_url;

};
#endif
