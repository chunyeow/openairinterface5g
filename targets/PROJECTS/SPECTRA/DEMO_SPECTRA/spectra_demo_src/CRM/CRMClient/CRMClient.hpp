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

// namespace odtone {
class CRMClient {

public: 

    CRMClient();
    ~CRMClient();
    CRMClient(char* url);
    
    void SetUrl(char* url);
    pplx::task<void> RequestJSONValueAsync();
    pplx::task<void> StoreJSONValuePolicies(char * param1, char * param2, char * param3, char * param4);
    pplx::task<void> StoreJSONValuemeasurements(char * param1, char * param2, char * param3, char * param4, char * param5, char * param6);
    pplx::task<void> StoreJSONValuedecisions(char * param1, char * param2, char * param3, char * param4, char * param5);
    pplx::task<void> Delete();
	

private:
    char * m_url;

};
// }


#endif
