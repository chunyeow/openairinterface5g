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

using namespace std;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace utility;

// Retrieves a JSON value from an HTTP request.
pplx::task<void> RequestJSONValueAsync(char * url)
{
    http_client client (U(url));
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
pplx::task<void>  StoreJSONValuePolicies(char * url, char * param1, char * param2, char * param3, char * param4)
{

   http_client client (U(url));
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
pplx::task<void>  StoreJSONValuemeasurements(char * url, char * param1, char * param2, char * param3, char * param4, char * param5, char * param6)
{

   http_client client (U(url));
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
pplx::task<void>  StoreJSONValuedecisions(char * url, char * param1, char * param2, char * param3, char * param4, char * param5)
{

   http_client client (U(url));
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

pplx::task<void> Delete(char* url)
{
  return pplx::create_task([url]
  {
    http_client client(U(url));

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


int main(int argc,char *argv[])
{
    char * policiesurl = (char*) malloc( 100 * sizeof(char) ); 
    strcpy(policiesurl,"http://1.gae-spectra.appspot.com/policies");
    char * measurementsurl = (char*) malloc( 100* sizeof(char) ); 
    strcpy(measurementsurl,"http://1.gae-spectra.appspot.com/measurements");
    char * decisionsurl = (char*) malloc( 100 * sizeof(char)); 
    strcpy(decisionsurl,"http://1.gae-spectra.appspot.com/decisions");
    int c;

    if (strcmp(argv[1], "GET") == 0)
    {
	c = atoi(argv[2]);
	switch (c)
	{
	case 1: RequestJSONValueAsync(policiesurl).wait();
			 break;
	case 2: RequestJSONValueAsync(measurementsurl).wait();
			 break;
	case 3: RequestJSONValueAsync(decisionsurl).wait();
			 break;
	}
    }
    else if (strcmp(argv[1], "PUT") == 0)
    {
	c = atoi(argv[2]);
	switch (c)
	{
	case 1: StoreJSONValuePolicies(policiesurl, argv[3], argv[4], argv[5], argv[6]).wait();
			 break;
	case 2: StoreJSONValuemeasurements(measurementsurl, argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]).wait();
			 break;
	case 3: StoreJSONValuedecisions(decisionsurl, argv[3], argv[4], argv[5], argv[6], argv[7]).wait();
			 break;
	}
    }
    else if (strcmp(argv[1], "DEL") == 0)
    {
     	c = atoi(argv[2]);
	switch (c)
	{
	case 1: Delete(policiesurl).wait();
			 break;
	case 2: Delete(measurementsurl).wait();
			 break;
	case 3: Delete(decisionsurl).wait();
			 break;
	}
    }

    return 0;
}

