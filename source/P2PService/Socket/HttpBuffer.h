#if !defined(HttpBuffer_H)
#define HttpBuffer_H


#include "HttpUtil.h"

#include <string>
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////
// HttpBufffer
//
// Purpose:	represents an buffer which stores an http formatted buffer


class HttpBuffer
{

	string			_contents;			// stores entire http stream

public:

	HttpBuffer () :
		_contents("")
	{}

	virtual ~HttpBuffer ()
	{
		release();
	}

	// create/release
	bool create ()
	{
		// reset contents
		_contents = "";

		return true;
	}


	bool create  ( LPCTSTR buffer, long length )
	{
		if ( buffer && length > 0 )
			_contents = buffer;
	}

	void release ()
	{
		// clear contents
		_contents.erase();
	}

	// access methods
	string getBody ()
	{
		// get pos of body in request
		long pos = _contents.find(DOUBLE_CRLF);

		// if valid return it
		if ( pos != -1 )
		{
			// just get the body
			pos += strlen(DOUBLE_CRLF);

			return _contents.substr(pos);
		}
		else
			return string("");
	}

	// add methods
	int addString( const string & str )
	{
		_contents += str;
		return _contents.size();
	}


	int addString( LPCTSTR id )
	{
		string str;
		StringUtil::loadString( str, id );

		return addString( str );
	}

	int addStatus( LPCTSTR strStatus )
	{
		string strVer = "HTTP/1.1 ";
		addString( strVer );
		addString( strStatus );
		addString( CRLF );

		// stuff the server name
		string strServer;
		if ( StringUtil::loadString( strServer, IDS_SERVER_NAME ) && !strServer.empty() )
			addHeader( "Server", strServer );

		// stuff the date
		string strDate;

		return addHeader( "Date", HttpUtil::getHttpDate() );
	}

	virtual
	int addStatus( long status )
	{
		// get status string
		string strStatus;
		HttpUtil::getStatusString(status,strStatus);
		
		return addStatus(strStatus.c_str());
	}

	virtual
	int addError( long idError )
	{
		string strError;
		HttpUtil::getStatusString( idError, strError );

		addStatus( idError );

		return addString( CRLF );
	}

	int addHeader( string strName, string strValue )
	{
		addString( strName );
		addString( ": " );
		addString( strValue );
		return addString( CRLF );
	}

	int addHeader( string strName, int nValue )
	{
		string strValue;
		addString( strName );
		addString( ": " );

		StringUtil::formatString( strValue, "%d", nValue );
		addString( strValue );
		return addString( CRLF );
	}

	void addMethod ( long method, string & version )
	{
		// add command
		addString( httpCommands[method] );
		addString( " / HTTP/" );
		addString( version );
		addString( CRLF );
		addString( CRLF );
	}

	void addMethod ( long method, string & version, string & file )
	{
		// add command
		addString( httpCommands[method] );
		addString( " /" );
		if ( !file.empty() )
			addString( file );
		addString( " HTTP/" );
		addString( version );
		addString( CRLF );
		addString( CRLF );
	}


	// operators
	void operator = ( const HttpBuffer & buffer )
	{
		_contents = buffer._contents;
	}

	operator string & ()
	{
		return _contents;
	}

	LPTSTR data ()
	{
		return (LPTSTR) _contents.c_str();
	}

	LPCTSTR c_str () 
	{
		return _contents.c_str();
	}

	long size ()
	{
		return _contents.size();
	}
	
	void getString ( string & set )
	{
		set = _contents;
	}

	string & getContents ()
	{
		return _contents;
	}
};



#endif