#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "TcpClient.h"


TcpClient::TcpClient()
: currentHostName( "" )
, currentPort( 0 )
, currentSocketDescr( 0 )
, serverAddress ( )
, currentHostInfo( NULL )
, clientIsConnected( false )
, receiveBufferSize( 1024 )
{
}


TcpClient::~TcpClient()
{
  currentHostInfo = NULL;
}


void TcpClient::connectToServer( string &hostname, int port )
{
  currentHostInfo = gethostbyname( hostname.c_str( ) );

  if( currentHostInfo == NULL )
  {
    currentHostName   = "";
    currentPort       = 0;
    currentHostInfo   = NULL;
    clientIsConnected = false;
    printf("error connecting host\n" );
  }

  currentHostName = hostname;
  currentPort     = port;

  currentSocketDescr = socket(AF_INET, SOCK_STREAM, 0);

  if( currentSocketDescr == 0 )
  {
    currentHostName   = "";
    currentPort       = 0;
    currentHostInfo   = NULL;
    clientIsConnected = false;
    printf("can't create socket\n" );
  }

  serverAddress.sin_family = currentHostInfo->h_addrtype;
  serverAddress.sin_port   = htons( currentPort );

  memcpy( (char *) &serverAddress.sin_addr.s_addr, currentHostInfo->h_addr_list[0], currentHostInfo->h_length );

  if( connect( currentSocketDescr, ( struct sockaddr *) &serverAddress, sizeof( serverAddress ) ) < 0 )
  {
    throw string("can't connect server\n" );
  }

  clientIsConnected = true;
}


void TcpClient::disconnect( )
{
  if( clientIsConnected )
  {
    close( currentSocketDescr );
  }

  currentSocketDescr = 0;
  currentHostName    = "";
  currentPort        = 0;
  currentHostInfo    = NULL;
  clientIsConnected  = false;
}


void TcpClient::transmit( string &txString )
{
  if( !clientIsConnected )
  {
    throw string("connection must be established before any data can be sent\n");
  }

  char * transmitBuffer = new char[txString.length() +1];


  memcpy( transmitBuffer, txString.c_str(), txString.length() );
  transmitBuffer[txString.length()] = '\n'; //newline is needed!

  if( send( currentSocketDescr, transmitBuffer, txString.length() + 1, 0 ) < 0 )
  {
    throw string("can't transmit data\n");
  }

  delete [] transmitBuffer;

}


void TcpClient::receive( string &rxString )
{
  if( !clientIsConnected )
  {
    throw string("connection must be established before any data can be received\n");
  }

  char * receiveBuffer = new char[receiveBufferSize];

  memset( receiveBuffer, 0, receiveBufferSize );

  bool receiving = true;
  while( receiving )
  {
    int receivedByteCount = recv( currentSocketDescr, receiveBuffer, receiveBufferSize, 0 );
    if( receivedByteCount < 0 )
    {
      throw string("error while receiving data\n");
    }

    rxString += string( receiveBuffer );

    receiving = ( receivedByteCount == receiveBufferSize );
  }
  delete [] receiveBuffer;
}


string TcpClient::getCurrentHostName( ) const
{
  return currentHostName;
}


int TcpClient::getCurrentPort( ) const
{
  return currentPort;
}


