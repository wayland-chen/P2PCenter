#include "stdafx.h"
#include "HttpRequest.h"

//////////////////////////////////////////////////////////////////////////////////
// HttpRequest

HttpRequest::HttpRequest ( string & version ) :
	HttpBuffer(),
	
	_version(version),
	_headers()

{
	create();	
}


HttpRequest::~HttpRequest ()
{
	release();
}


// create/release

bool HttpRequest::create ()
{
	// create buffer
	HttpBuffer::create();

	// set defaults
	_url = "";
	_path = "";
	_pathInfo = "";
	_pathTranslated = "";
	_method = "";
	_args = "";
	_host = "";
	_version = "1.0";
	_execute = idAppNoExecute;
	_status = idHttpOk;
	_attributes = 0;
	_processState = REQ_DONE;
	_headers.clear();
	_length = 0;
	
	return true;
}


bool HttpRequest::create ( long request, string & file )
{
	addMethod( request, file );

	return true;
}





void HttpRequest::release ()
{
	// release buffer
	HttpBuffer::release();

	// release resources
	_url.erase();
	_path.erase();
	_pathInfo.erase();
	_pathTranslated.erase();
	_method.erase();
	_args.erase();
	_host.erase();
	_version.erase();
	_execute = idAppNoExecute;
	_status = idHttpOk;
	_attributes = 0;
	_processState = REQ_DONE;
	_headers.clear();
	_length = 0;
}


// operators
void HttpRequest::operator = ( const HttpRequest & request )
{
	// get request state except for
	// its contents
	_url = request._url;
	_path = request._path;
	_pathInfo = request._pathInfo;
	_pathTranslated = request._pathTranslated;
	_method = request._method;
	_args = request._args;
	_host = request._host;
	_version = request._version;
	_execute = request._execute;
	_status = request._status;
	_attributes = request._attributes;
	_processState = request._processState;
	_length = 0;

}

