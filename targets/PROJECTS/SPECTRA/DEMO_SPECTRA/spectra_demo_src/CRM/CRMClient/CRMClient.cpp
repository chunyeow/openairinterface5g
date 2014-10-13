#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <iostream>
#include <ostream> 
#include <sstream>
#include <fstream>
#include "cpprest/basic_types.h"
#include "cpprest/asyncrt_utils.h"
#include "cpprest/uri.h"
#include <string>

#include "CRMClient.hpp"

using namespace std;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace utility;

// namespace odtone {
    
CRMClient::CRMClient(char* url)
{
    m_url = (char*) (malloc (200* sizeof (char)));
    m_url = url;
}

CRMClient::CRMClient()
{
}

CRMClient::~CRMClient()
{
}

void CRMClient::SetUrl(char* url)
{
    m_url = url;
}

// Retrieves a JSON value from an HTTP request.
pplx::task<void> CRMClient::RequestJSONValueAsync()
{
    http_client client (U(m_url));
    return client.request(methods::GET).then([](http_response response) -> pplx::task<json::value>
    {
        if(response.status_code() == status_codes::OK)
        {
            return response.extract_json();
        }

        return pplx::task_from_result(json::value());
    })
        .then([](pplx::task<json::value> previousTask)
    {
        try
        {
  	    ofstream myfile;
  	    myfile.open ("outputGET.txt");
            const json::value& v = previousTask.get();
	    string_t jsonString = v.to_string();
            cout << U("Response...") << jsonString <<endl;
	    
//Parsing Begin
	    
	    cout << U("Start Parsing...") << endl;
	    for(auto iterArray = v.cbegin(); iterArray != v.cend(); ++iterArray)
	    {
	      const json::value &arrayValue = iterArray->second;
 
	      for(auto iterInner = arrayValue.cbegin(); iterInner != arrayValue.cend(); ++iterInner)
	      {
	      const json::value &propertyValue = iterInner->second;
 	      for(auto iterlast = propertyValue.cbegin(); iterlast != propertyValue.cend(); ++iterlast)
	      {
		const json::value &Name = iterlast->first;
		const json::value &Value = iterlast->second;
		cout<< U("Parameter: ") << Name.to_string()<<endl;
		cout<< U("Value: ") << Value.to_string()<< std::endl;
	      }
	      }      
	    cout << std::endl;
	    }	    
	    
//parsing End

            myfile << jsonString.c_str();
            myfile.close();
	    
        }
        catch (const http_exception& e)
        {
            wostringstream ss;
            ss << e.what() << endl;
            wcout << ss.str();
        }
    });

}

// Stores a JSON value (a Policy) from an HTTP request.
pplx::task<void>  CRMClient::StoreJSONValuePolicies(char * param1, char * param2, char * param3, char * param4)
{

   http_client client (U(m_url));
   json::value::field_map putvalue;
    
   putvalue.push_back(make_pair(json::value("pid"), json::value(param1)));
   putvalue.push_back(make_pair(json::value("name"), json::value(param2)));
   putvalue.push_back(make_pair(json::value("description"), json::value(param3)));
   putvalue.push_back(make_pair(json::value("value"), json::value(param4)));

    const string_t& s = "/";
    json::value object = json::value::object(putvalue);
    return client.request(methods::PUT, s, object).then([](http_response response)-> pplx::task<json::value>
    {
        if(response.status_code() == status_codes::OK)
        {
            return response.extract_json();
        }

        return pplx::task_from_result(json::value());
    })
        .then([](pplx::task<json::value> previousTask)
    {
        try
        {
            const json::value& v = previousTask.get();
	    string_t jsonString = v.to_string();
            cout << U("Response...") << jsonString <<endl;
        }
        catch (const http_exception& e)
        {
            wostringstream ss;
            ss << e.what() << endl;
            wcout << ss.str();
        }
    });
}


// Stores a JSON value (a Measurement) from an HTTP request.
pplx::task<void>  CRMClient::StoreJSONValuemeasurements(char * param1, char * param2, char * param3, char * param4, char * param5, char * param6)
{

   http_client client (U(m_url));
   json::value::field_map putvalue;
    
   putvalue.push_back(make_pair(json::value("key"), json::value(param1)));
   putvalue.push_back(make_pair(json::value("name"), json::value(param2)));
   putvalue.push_back(make_pair(json::value("type"), json::value(param3)));
   putvalue.push_back(make_pair(json::value("unit"), json::value(param4)));
   putvalue.push_back(make_pair(json::value("value"), json::value(param5)));
   putvalue.push_back(make_pair(json::value("time"), json::value(param6)));

    const string_t& s = "/";
    json::value object = json::value::object(putvalue);
    return client.request(methods::PUT, s, object).then([](http_response response)-> pplx::task<json::value>
    {
        if(response.status_code() == status_codes::OK)
        {
            return response.extract_json();
        }

        return pplx::task_from_result(json::value());
    })
        .then([](pplx::task<json::value> previousTask)
    {
        try
        {
            const json::value& v = previousTask.get();
	    string_t jsonString = v.to_string();
            cout << U("Response...") << jsonString <<endl;
        }
        catch (const http_exception& e)
        {
            wostringstream ss;
            ss << e.what() << endl;
            wcout << ss.str();
        }
    });

}

// Stores a JSON value (a Decision) from an HTTP request.
pplx::task<void>  CRMClient::StoreJSONValuedecisions(char * param1, char * param2, char * param3, char * param4, char * param5)
{

   http_client client (U(m_url));
   json::value::field_map putvalue;
    
   putvalue.push_back(make_pair(json::value("did"), json::value(param1)));
   putvalue.push_back(make_pair(json::value("name"), json::value(param2)));
   putvalue.push_back(make_pair(json::value("description"), json::value(param3)));
   putvalue.push_back(make_pair(json::value("value"), json::value(param4)));
   putvalue.push_back(make_pair(json::value("time"), json::value(param5)));

    const string_t& s = "/";
    json::value object = json::value::object(putvalue);
    return client.request(methods::PUT, s, object).then([](http_response response)-> pplx::task<json::value>
    {
        if(response.status_code() == status_codes::OK)
        {
            return response.extract_json();
        }

        return pplx::task_from_result(json::value());
    })
        .then([](pplx::task<json::value> previousTask)
    {
        try
        {
            const json::value& v = previousTask.get();
	    string_t jsonString = v.to_string();
            cout << U("Response...") << jsonString <<endl;
        }
        catch (const http_exception& e)
        {
            wostringstream ss;
            ss << e.what() << endl;
            wcout << ss.str();
        }
    });

}

pplx::task<void> CRMClient::Delete()
{
    char* url = m_url;
      return pplx::create_task([url]
//   return pplx::create_task([m_url]
  {
    http_client client (U(url));

    return client.request(methods::DEL);

  }).then([](http_response response)
  {
    if(response.status_code() == status_codes::OK)
    {
      auto body = response.extract_string();    

      std::wcout << L"Deleted: " << body.get().c_str() << std::endl;
    }
  });
}


// }