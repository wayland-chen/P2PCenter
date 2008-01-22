#if !defined(HttpRequest_H)
#define HttpRequest_H


#include "HttpBuffer.h"
#include "TimeStamp.h"

#include <deque>
#include <map>
#include <string>
#include <sstream>
using namespace std;


#pragma warning( disable:4172 )

enum
{
	idAppNoExecute  = 0x0000,
	idAppExecute = 0x0001,
	idAppCgi     = 0x0002,
	idAppIsapi   = 0x0004 
};


enum REQSTATUS
{
	REQ_REQUEST=0, 
	REQ_HEADER,
	REQ_BODY, 
	REQ_SIMPLE, 
	REQ_DONE
};



/////////////////////////////////////////////////////////////////////////////////////
// HttpRequest
//
// Purpose:	represent an http request

typedef map<string,string>				StringMap;
typedef StringMap::iterator				itrStringMap;
typedef StringMap::reverse_iterator		revItrStringMap;


class HttpRequest :
	public HttpBuffer
{
public:

	//////////////////////////////////////////////////////////
	// http request format 
	//
	// Request Line Required
	// General Header Optional
	// Request Header Optional
	// Entity Header Optional
	// Entity Body Optional

	string			_url;				// url for request
	string			_path;				// full path for request
	string			_pathInfo;			// path information
	string			_pathTranslated;	// path translated
	string			_method;			// request method
	string			_args;				// args for request
	string			_host;				// host for request
	string			_version;			// http version
	long			_execute;			// shows if execute app
	long			_status;			// shows status of request
	long			_attributes;		// request attributes
	long			_processState;		// request processing state
	StringMap		_headers;			// param-value pairs
	long			_length;			// lenght of request

	StringMap       _pars;

public:

	HttpRequest ( string & version = string("1.0") );
	virtual ~HttpRequest ();

	// create/release
	bool create  ();
	bool create  ( long request, string & file = string("") );
	void release ();


	// get/set state
	void setState ( HttpRequest & request )
	{
		*this = request;
	}

	void getState ( HttpRequest & request )
	{
		request = *this;
	}

	// operators
	void operator = ( const HttpRequest & request );


	// access methods
	string & getUrl ()
	{
		return _url;
	}

	void setUrl ( string & set )
	{
		_url = set;
	}

	string & getFullPath ()
	{
		return _path;
	}

	void setFullPath ( string & set )
	{
		_path = set;
	}

	string & getPathInfo ()
	{
		return _pathInfo;
	}

	void setPathInfo ( string & set )
	{
		_pathInfo = set;
	}

	string & getPathTranslated ()
	{
		return _pathTranslated;
	}

	void setPathTranslated ( string & set )
	{
		_pathTranslated = set;
	}

	string & getMethod ()
	{
		return _method;
	}

	void setMethod ( string & set )
	{
		_method = set;
	}

	string & getVersion ()
	{
		return _version;
	}

	void setVersion ( string & set )
	{
		set = _version;
	}

	long getExecute ()
	{
		return _execute;
	}

	void setExecute ( long set )
	{
		_execute = set;
	}

	long getStatus ()
	{
		return _status;
	}

	void setStatus ( long set )
	{
		_status = set;
	}

	long getAttributes ()
	{
		return _attributes;
	}

	void setAttributes ( long set )
	{
		_attributes = set;
	}

	long getProcessState ()
	{
		return _processState;
	}

	void setProcessState ( long set )
	{
		_processState = set;
	}

	string & getArgs ()
	{
		return _args;
	}

	void setArgs ( string & set )
	{
		_args = set;
		string spar = _args;

		int index = spar.find( '&');
		while ( index != -1 )
		{
			string element = StringUtil::left( spar, index );
			int pos = element.find('=');
			if ( pos != -1 )
			{
				string name = StringUtil::left( element, pos );
				string value = StringUtil::mid( element, pos + 1 );
				if ( name.length() > 0 )
					_pars[name] = value;
			}

			spar.erase( 0, index + 1 );
			index = spar.find( '&');
		}

		string element = spar;
		int pos = element.find('=');
		if ( pos != -1 )
		{
			string name = StringUtil::left( element, pos );
			string value = StringUtil::mid( element, pos + 1 );
			if ( name.length() > 0 )
				_pars[name] = value;
		}
	}
	
	string & getHost ()
	{
		return _host;
	}

	void setHost ( string & set )
	{
		_host = set;
	}

	long getLength ()
	{
		return _length;
	}

	void setLength ( long set )
	{
		_length = set;
	}


	bool addHeaderValue ( LPTSTR param, string & value )
	{
		if ( param == 0 || strlen(param) == 0 || value.empty() )
			return false;

		// add value
		_headers[ string(param)] = value;

		return true;
	}

	bool addHeaderValue ( string & param, string & value )
	{
		if ( param.empty() || value.empty() )
			return false;

		// add value
		_headers[param] = value;

		return true;
	}

	string getHeaderValue ( string & param )
	{
		// if find value return it
		itrStringMap itr = _headers.find(param);
		if ( itr != _headers.end() )
			return (*itr).second;
		else
			return string("");
	}

	string getHeaderValue ( LPTSTR param )
	{
		// if find value return it
		itrStringMap itr = _headers.find(string(param));
		if ( itr != _headers.end() )
			return (*itr).second;
		else
			return string("");
	}

	string getParamentValue ( string & param )
	{
		// if find value return it
		itrStringMap itr = _pars.find(param);
		if ( itr != _pars.end() )
			return (*itr).second;
		else
			return string("");
	}

	string getParamentValue ( LPTSTR param )
	{
		// if find value return it
		itrStringMap itr = _pars.find(string(param));
		if ( itr != _pars.end() )
			return (*itr).second;
		else
			return string("");
	}

	// add status
	int addStatus( long status )
	{
		// save the status for this request
		setStatus(status);

		// store status in buffer
		return HttpBuffer::addStatus( status );
	}
};



/////////////////////////////////////////////////////////////////////////////////////
// HttpRequestList
//
// Purpose:	list of http requests


class HttpRequestList :
	public deque<HttpRequest>
{
public:

	HttpRequestList ()
	{}

	virtual ~HttpRequestList ()
	{
		clear();
	}
};

typedef deque<HttpRequest>::iterator	itrHttpRequest;




#endif