#include <iostream>
#include "TcpClient.h"

void printUsage()
{
  cout<<"usage: EthernetRawCommand <server-ip> [scpi-command]"<<endl;
}


int main( int argc, char *argv[] )
{
  int errorCode         = 0; //no error

  bool useSingleCommand = false;
  string singleCommand  = "";
  string hostname       = "";
  int    port           = 5025;

  string input          = "";
  TcpClient client;


  switch( argc )
  {
    case 3:
      useSingleCommand = true;
      singleCommand    = argv[2];
    case 2:
      hostname         = argv[1];
      break;
    default:
        printUsage();
        return(-1);
  }

  try
  {
    client.connectToServer( hostname, port );

    bool terminate = false;

    while( !terminate )
    {
      char buffer[1024];

      if( useSingleCommand )
      {
        input =  singleCommand; //send string
      }
      else
      {
        cin.getline( buffer, 1024 );
        input = buffer;

        if( input == "end" )
        {
          terminate = true;
        }

      }

      if( !terminate)
      {
        client.transmit( input ); //send string

        int qPos = input.find( "?", 0 );
        //receive string only when needed
        if( qPos > 0 )
        {
          string rcStr = "";
          client.receive( rcStr );
          cout << rcStr << endl;
        }
      }

      if( useSingleCommand )
      {
        terminate = true;
      }
    }

  }catch( const string errorString )
  {
    cout<<errorString<<endl;
  }

  client.disconnect( );

  return errorCode;
}
