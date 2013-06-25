#include "Json.h"
#include <iostream>


int main(int argc, char *argv[])
{
  try
  {
    Json js;

    std::string str("Test\\ string\\ end\\\nAnders was here\n\"Quoted string inside string\"\n");
    std::cout << " --------------- original\n";
    std::cout << str << std::endl;

    js.encode(str);
    std::cout << " --------------- encoded\n";
    std::cout << str << std::endl;

    js.decode(str);
    std::cout << " --------------- decoded\n";
    std::cout << str << std::endl;

    js["String"] = str;
    str = js.ToString();
    std::cout << " --------------- from ToString()\n";
    std::cout << str << std::endl;

    Json js2 = Json::Parse(str);
    std::cout << js2.ToString() << std::endl;

    std::cout << (std::string)js2["String"] << std::endl;

    str = "{\"no-value\":null}";
    Json js3 = Json::Parse(str);
    std::cout << js3.ToString() << std::endl;

    std::cout << " --------------- request\n";
    Json req;
    req["method"] = "test";
    Json params;
    params["value1"] = 1L;
    params["value2"] = "2";
    req["params"] = params;
    std::cout << req.ToString() << std::endl;
  }
  catch (const Exception& e)
  {
    std::cerr << e.ToString() << std::endl;
  }
}


