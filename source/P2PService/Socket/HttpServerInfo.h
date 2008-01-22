#if !defined(HttpServerInfo_H)
#define HttpServerInfo_H

#include "HttpRequest.h"
#include "HttpServerStats.h"
#include "HttpUtil.h"

#include <string>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////
// HttpServerInfo
//
// Purpose:		http server information


class HttpServerInfo :
	public HttpServerStats
{
protected:

	string					_strTitleBase;
	string					_strRoot;
	string					_strServer;
	TimeStamp				_timeStarted;
	long					_port;
	int						_server;
	bool					_bListIcon;
	bool					_bAllowListing;

public:

	HttpServerInfo ( long port = PORT_HTTP );
	virtual ~HttpServerInfo ();

	// create/release resources
	bool create		( string & server, string & directory );
	void release	();

	// access methods
	string & getTitleBase ()
	{
		return _strTitleBase;
	}

	string	getRoot ()
	{
		return _strRoot;
	}

	string getServer ()
	{
		return _strServer;
	}


	TimeStamp & getTimeStarted ()
	{
		return _timeStarted;
	}

	long getPort ()
	{
		return _port;
	}

	void setPort ( long port)
	{
		_port = port;
	}

	int	getServerIndex ()
	{
		return _server;
	}

	bool getListIcon ()
	{
		return _bListIcon;
	}

	bool getAllowListing ()
	{
		return _bAllowListing;
	}

	// utility methods
	bool	urlToPath		( string & strFile );
	bool	pathToUrl		( string & strFile );
	string	stripLast		( string & strPath );
	bool	find			( HttpRequest & request, string & strFile );
	bool	checkDefault	( HttpRequest & request, LPTSTR uList, bool bExecute );
	bool	isSeverApp		( HttpRequest & request );
	bool	checkExt		( HttpRequest & request, 
		                      const string & strExt, string & strAvail, 
							  DWORD dwType );



	// operators
	void operator = ( const HttpServerInfo & info )
	{
		_strTitleBase = info._strTitleBase;
		_strRoot = info._strRoot;
		_strServer = info._strServer;
		_timeStarted = info._timeStarted;
		_port = info._port;
		_server = info._server;
		_bListIcon = info._bListIcon;
		_bAllowListing = info._bAllowListing;
	}

};


#endif