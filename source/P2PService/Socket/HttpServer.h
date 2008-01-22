#if !defined(HttpServer_H)
#define HttpServer_H

#include "HttpRequest.h"
#include "HttpServerInfo.h"
#include "ServerSocketHandler.h"
#include "HttpSocketHandler.h"
#include "TextFile.h"
#include "TimeStamp.h"


#include <vector>
using namespace std;


///////////////////////////////////////////////////////////////////////////////////////
// HttpServer
//
// Purpose:		http server implementation

class HttpServer :
	public HttpServerInfo,
	public CreateSocketHandler
{

	HttpRequestList			_requests;		// list of outstanding requests
	ServerSocket			_serverSocket;
	ServerSocketHandler	*	_handler;

public:

	HttpServer( long noConnections = 10, long port = 80 );
	virtual ~HttpServer();


	// create/release 
	bool	create	( string & server, string & directory );
	void	release	();



	// responsible for create a connection
	SocketHandler * createHandler () 
	{
		// create socket handler attach to server info
		// a ref to http server info is passed in
		return (SocketHandler *) new HttpSocketHandler(*this);
	}


	bool	start			();
	void	stop			();

	void    hit				( HttpRequest & request );
	long    run				();

	bool	urlToPath		( string & strFile );
	bool	pathToUrl		( string & strFile );
	bool	checkDefault	( UINT uList, bool bExecute );
	bool	find			( string & strFile );

	bool	isSvrApp		( void );
	bool	checkExt		( const string& strExt, string& strAvail, DWORD dwType );

	string & getServer ()
	{
		return _strServer;
	}

	bool getListIcon ()
	{
		return _bListIcon;
	}

	bool getAllowListing ()
	{
		return _bAllowListing;
	}

};





#endif