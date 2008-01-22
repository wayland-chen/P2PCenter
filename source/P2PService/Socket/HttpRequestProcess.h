#if !defined(HttpRequestProcess_H)
#define HttpRequestProcess_H

#include "HttpRequest.h"
#include "HttpResponse.h"

#include <string>
using namespace std;

class HttpRequestProcess
{
	HttpRequest		_request;				// stores processed request info
	HttpResponse	_resp;
	long			_processState;			// state of processing 
	string			_buffer;				// buffer to process
	long			_length;				// length of buffer
	string			_line;					// current process line

public:

	HttpRequestProcess ();
	HttpRequestProcess ( LPTSTR buffer, long length );

	virtual ~HttpRequestProcess ();

	// create/release methods
	bool create  ( LPTSTR buffer, long length );
	void release ();

	// methods
	bool process		( LPTSTR buffer, long length );
	bool process		();
	bool startResponse();
	bool getLine		( int & index );
	void processLine	();
	void addToBody		( int nBytes, int index );
	bool bodySent		();

	// access methods for http buffer
	HttpResponse & getResponse ()
	{
		return _resp;
	}

	HttpRequest & getRequest ()
	{
		return _request;
	}

	string getBody ()
	{
		return _resp.getBody();
	}

	void getString ( string & set )
	{
		_resp.getString(set);
	}

	LPTSTR data ()
	{
		return (LPTSTR) _resp.c_str();
	}

	LPCTSTR c_str () 
	{
		return _resp.c_str();
	}

	long size ()
	{
		return _resp.size();
	}


};

#endif