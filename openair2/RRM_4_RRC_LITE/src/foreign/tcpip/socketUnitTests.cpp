#ifdef UNITTESTS

#include "MiniCppUnit.hxx"
#include "storage.h"
#include "socket.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <limits.h>
#include <float.h>

#define ADDRINUSE 
#define PORT 8888

#define TEST_CHAR UCHAR_MAX
#define TEST_BYTE UCAHR_MAX
#define TEST_STRING "Test-String(ÄÖÜäöü?ß&%$§!+#~*')"
#define TEST_SHORT SHRT_MAX
#define TEST_INT INT_MAX
#define TEST_FLOAT FLT_MAX
#define TEST_DOUBLE DBL_MAX


class socketUnitTests : public TestFixture<socketUnitTests>
{
public:
	TEST_FIXTURE( socketUnitTests )
   , currentMode_(undefined),
   server_socket_( PORT ) ,
	 client_socket_( "localhost" , PORT )
									  
	{
		TEST_CASE( openConnection );

		TEST_CASE( receiveStorage );
		TEST_CASE( sendStorage );

		TEST_CASE( closeConnection );
	}

	void openConnection()
	{
		try {
      server_socket_.set_blocking(false);
			server_socket_.accept();
			currentMode_ = server;
		}
		catch(tcpip::SocketException e) {
			if (e.what() == "tcpip::Socket::accept() Unable to create listening socket: Address already in use"){
				currentMode_ = client;
			}else{
				FAIL( "Server socket accept() failed!" );
			}
		}
		
		if (currentMode_ == server)
		{
      server_socket_.set_blocking(true);
      std::cout << Assert::yellow() << "Listening on port " << PORT << ". Please start another instance of this program on host.\n" << Assert::normal();
      server_socket_.accept();
		}
		if (currentMode_ == client){
			try {
				client_socket_.connect();
			} 
			catch (tcpip::SocketException e){
				FAIL( "Client socket.connect() failed!" );
			}
		}
	}

	void closeConnection()
	{
		try {
			if (currentMode_ == client) client_socket_.close();
		}
		catch (tcpip::SocketException e){
				FAIL( "Client socket close() failed!" );
		}

		try {
			if (currentMode_ == server) server_socket_.close();
		}
		catch (tcpip::SocketException e){
				FAIL( "Server socket close() failed!" );
		}
	}

	void receiveStorage()
	{
		if (currentMode_ == client) return;

		tcpip::Storage received_message;

		try {
			server_socket_.receiveExact(received_message);

			ASSERT_EQUALS( static_cast<int>(TEST_CHAR), 
						   static_cast<int>(received_message.readChar()) );
			ASSERT_EQUALS( TEST_STRING, received_message.readString() );
			ASSERT_EQUALS( TEST_SHORT, received_message.readShort() );
			ASSERT_EQUALS( TEST_INT, received_message.readInt() );
			ASSERT_EQUALS( TEST_FLOAT, received_message.readFloat() );
			ASSERT_EQUALS( TEST_DOUBLE, received_message.readDouble() );
		}
		catch (tcpip::SocketException e) {
			FAIL( "Server socket receiveExact() failed!" );
		}
	}

	void sendStorage()
	{
		if (currentMode_ == server) return;

		tcpip::Storage send_message;
		
		try {
			send_message.writeChar( TEST_CHAR );
			send_message.writeString( TEST_STRING );
			send_message.writeShort( TEST_SHORT );
			send_message.writeInt( TEST_INT );
			send_message.writeFloat( TEST_FLOAT );
			send_message.writeDouble( TEST_DOUBLE );

			client_socket_.sendExact(send_message);
		}
		catch (tcpip::SocketException e) {
			FAIL( "Client socket sendExact() failed!" );
		}
	}

	//...
private:
	enum mode{undefined = -1, client, server};
	mode currentMode_;
	tcpip::Socket server_socket_;
	tcpip::Socket client_socket_;
};

//Register fixture
REGISTER_FIXTURE( socketUnitTests );

#endif
