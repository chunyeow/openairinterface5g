/*
 * This file provides test cases for the tcpip package, that can be executed by MiniCppUnit
 *
 * Author: Axel Wegener
 */

#ifdef UNITTESTS

#include "MiniCppUnit.hxx"
#include "storage.h"
//#include "socket.h"
#include <math.h>


class tcpipUnitTests : public TestFixture<tcpipUnitTests>
{
public:
	TEST_FIXTURE( tcpipUnitTests )
	{
		TEST_CASE( testStorageChar );
		TEST_CASE( testStorageByte );
		TEST_CASE( testStorageString );
		TEST_CASE( testStorageShort );
		TEST_CASE( testStorageInt );
		TEST_CASE( testStorageFloat );
		TEST_CASE( testStorageDouble );
		TEST_CASE( testStorageMixed );
		TEST_CASE( testStorageReset );
		TEST_CASE( testStorageValidPos );
		TEST_CASE( testStorageLoadCharArray );
		TEST_CASE( testStorageCharToInt );
		TEST_CASE( testStorageByteShortInt );
	}
	
	void testStorageChar()
	{
		tcpip::Storage s1;

		s1.writeChar('\0');
		s1.writeChar('Ü');
		s1.writeChar('\n');

		ASSERT_EQUALS( true, s1.valid_pos() );
		ASSERT( s1.readChar() == '\0' ); 
		ASSERT( (char) s1.readChar() == 'Ü' ); 
		ASSERT( s1.readChar() == '\n' ); 
		ASSERT_EQUALS( false, s1.valid_pos() );
	}

	void testStorageByte()
	{
		tcpip::Storage s1;
       
		s1.writeByte(0);
		s1.writeByte(-128);
		s1.writeByte(127);
/*
		bool err = false;
		try
		{
			s1.writeByte(128);
		}
		catch (std::invalid_argument e)
		{
			err = true;
		}
		catch ( ... ) {}
		if (!err) FAIL("Storage::writeByte allowed an input value of 128");
		
		err = false;
		try
		{
			s1.writeByte(-129);
		}
		catch (std::invalid_argument e)
		{
			err = true;
		}
		if (!err) FAIL("Storage::writeByte allowed an input value of -129");
*/		
		ASSERT_EQUALS( true, s1.valid_pos() );
		ASSERT_EQUALS( 0, s1.readByte() ); 
		ASSERT_EQUALS( -128, s1.readByte() ); 
		ASSERT_EQUALS( 127, s1.readByte() );
		ASSERT_EQUALS( false, s1.valid_pos() );
	}

	void testStorageString()
	{
		tcpip::Storage s1;
       
		s1.writeString("Dies ist ein TestString");
		s1.writeString("");
		s1.writeString("2. Text mit Sonderzeichen ü ö ß \n\0");

		ASSERT_EQUALS( true, s1.valid_pos() );
		ASSERT_EQUALS( "Dies ist ein TestString", s1.readString() ); 
		ASSERT_EQUALS( "", s1.readString() ); 
		ASSERT_EQUALS( "2. Text mit Sonderzeichen ü ö ß \n\0", s1.readString() ); 
		ASSERT_EQUALS( false, s1.valid_pos() );
	}

	void testStorageShort()
	{
		tcpip::Storage s1;
       
		s1.writeShort(0);
		s1.writeShort(32767);
		s1.writeShort(-32768);

		ASSERT_EQUALS( true, s1.valid_pos() );
		ASSERT_EQUALS( 0, s1.readShort() ); 
		ASSERT_EQUALS( 32767, s1.readShort() );
		ASSERT_EQUALS( -32768, s1.readShort() ); 
		ASSERT_EQUALS( false, s1.valid_pos() );
	}

	void testStorageInt()
	{
		tcpip::Storage s1;
       
		s1.writeInt(0);
		s1.writeInt(-32768);
		s1.writeInt(32767);
		s1.writeInt(INT_MIN);
		s1.writeInt(INT_MAX);

		ASSERT_EQUALS( true, s1.valid_pos() );
		ASSERT_EQUALS( 0, s1.readInt() ); 
		ASSERT_EQUALS( -32768, s1.readInt() ); 
		ASSERT_EQUALS( 32767, s1.readInt() );
		ASSERT_EQUALS( INT_MIN, s1.readInt() );
		ASSERT_EQUALS( INT_MAX, s1.readInt() );
		ASSERT_EQUALS( false, s1.valid_pos() );
	}

	void testStorageFloat()
	{
		tcpip::Storage s1;
       
		float f1 = (float) 0.000002;
		float f2 = (float) 0.000001;

		s1.writeFloat(0.0);
		s1.writeFloat(f1);
		s1.writeFloat(f2);

		ASSERT_EQUALS( true, s1.valid_pos() );
		ASSERT_EQUALS( (float) 0.0, s1.readFloat() ); 
		ASSERT_EQUALS( f1, s1.readFloat() ); 
		ASSERT_EQUALS( f2, s1.readFloat() );
		ASSERT_EQUALS( false, s1.valid_pos() );
	}

	void testStorageDouble()
	{
		tcpip::Storage s1;
       
		double d1 = 0.000002;
		double d2 = 0.000001;

		s1.writeDouble(0.0);
		s1.writeDouble(d1);
		s1.writeDouble(d2);

		ASSERT_EQUALS( true, s1.valid_pos() );
		ASSERT_EQUALS( 0.0, s1.readDouble() ); 
		ASSERT_EQUALS( d1, s1.readDouble() ); 
		ASSERT_EQUALS( d2, s1.readDouble() );
		ASSERT_EQUALS( false, s1.valid_pos() );
	}	

	void testStorageMixed()
	{
		tcpip::Storage s1;
       
		double d1 = 0.000002;

		s1.writeFloat(0.0);
		s1.writeDouble(d1);
		s1.writeString("\0");
		s1.writeShort(42);

		ASSERT_EQUALS( true, s1.valid_pos() );
		ASSERT_EQUALS( (float) 0.0, s1.readFloat() ); 
		ASSERT_EQUALS( d1, s1.readDouble() ); 
		ASSERT_EQUALS( "\0", s1.readString() );
		ASSERT_EQUALS( 42, s1.readShort() ); 
		ASSERT_EQUALS( false, s1.valid_pos() );
	}

	void testStorageReset()
	{
		tcpip::Storage s1;
       
		double d1 = 0.000002;

		s1.reset();
		ASSERT_EQUALS( false, s1.valid_pos() );

		s1.writeFloat(0.0);
		s1.writeDouble(d1);
		s1.writeString("\0");
		s1.writeShort(42);

		ASSERT_EQUALS( true, s1.valid_pos() );
		s1.reset();
		ASSERT_EQUALS( false, s1.valid_pos() );

		s1.writeString("Test");

		ASSERT_EQUALS( true, s1.valid_pos() );
		ASSERT_EQUALS( "Test", s1.readString() ); 
		ASSERT_EQUALS( false, s1.valid_pos() );

		s1.reset();
		ASSERT( 0 == s1.size() );
	}

	void testSocketConnect()
	{
		
	}

	void testStorageValidPos()
	{
		tcpip::Storage s1;
		ASSERT_EQUALS( false, s1.valid_pos() );

		tcpip::Storage s2;
		s2.writeFloat(1.0);
		ASSERT_EQUALS( true, s2.valid_pos() );
		
		s2.reset();
		ASSERT_EQUALS( false, s2.valid_pos() );
	}
	
  void testStorageLoadCharArray()
  {
    tcpip::Storage sHallo;
    sHallo.writeString("Hallo du!");

    unsigned char cBuffer[100];
    unsigned int i=0;
    while ( sHallo.valid_pos() && i < sizeof(cBuffer) )
    {
      cBuffer[i] = sHallo.readChar();
      i++;
    }

		tcpip::Storage s1(cBuffer, i);

		ASSERT_EQUALS( "Hallo du!", s1.readString() );

  }
  void testStorageCharToInt()
  {
    tcpip::Storage s;
	
	ASSERT_EQUALS(0, (int)s.position());
	ASSERT_EQUALS(0, (int)s.size());

	s.writeChar(0);
	ASSERT_EQUALS(0, (int)s.position());
	ASSERT_EQUALS(1, (int)s.size());

	s.writeChar(0);
	ASSERT_EQUALS(0, (int)s.position());
	ASSERT_EQUALS(2, (int)s.size());

	s.writeChar(0);
	ASSERT_EQUALS(0, (int)s.position());
	ASSERT_EQUALS(3, (int)s.size());

	s.writeChar(4);
	ASSERT_EQUALS(0, (int)s.position());
	ASSERT_EQUALS(4, (int)s.size());
	
	int n = s.readInt();
	
	ASSERT_EQUALS(4, (int)s.position());
	ASSERT_EQUALS(4, (int)s.size());
	ASSERT_EQUALS(4, n);
	
	s.reset();

	ASSERT_EQUALS(0, (int)s.position());
	ASSERT_EQUALS(0, (int)s.size());

	s.writeChar(0);
	ASSERT_EQUALS(0, (int)s.position());
	ASSERT_EQUALS(1, (int)s.size());

	s.writeChar(0);
	ASSERT_EQUALS(0, (int)s.position());
	ASSERT_EQUALS(2, (int)s.size());

	s.writeChar(0);
	ASSERT_EQUALS(0, (int)s.position());
	ASSERT_EQUALS(3, (int)s.size());

	s.writeChar(4);
	ASSERT_EQUALS(0, (int)s.position());
	ASSERT_EQUALS(4, (int)s.size());
	
	n = s.readInt();
	
	ASSERT_EQUALS(4, (int)s.position());
	ASSERT_EQUALS(4, (int)s.size());
	ASSERT_EQUALS(4, n);

  }
  
  void testStorageByteShortInt()
  {
    tcpip::Storage s;
	return;
	try {
//	s.writeByte(-128);
	s.writeByte(127);
	
//	ASSERT_EQUALS(-128, s.readByte());
	ASSERT_EQUALS(127, s.readByte());
	
	}
	catch (std::string s)
	{
		std::cerr << s << std::endl;
	}

	/*	int max = 255;
	int min = 0;
	
	byte b;
	int i;
	
	b = (byte) min;
	i = b;
	ASSERT_EQUALS(0, i);

	b = (byte) max;
	i = b;
	ASSERT_EQUALS(255, i);
  */
  }
};

REGISTER_FIXTURE( tcpipUnitTests );

#endif
