#include "stdafx.h"

#include "HttpServer.h"
#include "HttpSocketHandler.h"


/////////////////////////////////////////////////////////////////////////////
// HttpServer
//
// #DGH
// note:	server socket handler is newed, we may need to just
//			put in a full object, to stop deleting and make 
//			management easier. have to check thread shutdown.

HttpServer::HttpServer ( long noConnections, long port ) :

	HttpServerInfo(port),
	_requests(),
	_serverSocket(noConnections),
	_handler(0)

{
	// init startup
	_serverSocket.init();
}


HttpServer::~HttpServer()
{
	release();
}


void HttpServer::release ()
{
	stop();

	// release server info
	HttpServerInfo::release();

	// do our cleanup
	// handler is released by stop
	if ( !_requests.empty() )
		_requests.clear();

	_serverSocket.release();
}


bool HttpServer::create ( string & server, string & directory )
{
	// create server info
	HttpServerInfo::create(server,directory);

	// start new connections
	return start();
}



void HttpServer::stop ()
{
	// get rid of server socket handler
	if ( _handler != NULL )
	{
		// close socket
		_serverSocket.close();

		// release handler resources
		_handler->release();

		// remove handler
		delete _handler;
		_handler = 0;
	}
}

bool HttpServer::start ()
{
	// stop previous if needed
	stop();

	// open port
	if ( !_serverSocket.open( _port ) )
		return false;

	// create user server handler
	_handler = new ServerSocketHandler(_serverSocket,*this);
	_handler->create();

	return true;
}




/////////////////////////////////////////////////////////////////////////////
// HttpServer commands

void HttpServer::hit ( HttpRequest & request )
{
	// save the request object
	_requests.push_back( request );

	// increment the total hit count
	HttpServerStats::hit(request);
}

long HttpServer::run ()
{
	bool bMore = false;


	while ( true )
	{


		if ( !bMore )
			Sleep(100);
	}


	return 0;
}


/*


bool HttpServer::process ()
{
	bool bMore = false;

	// if there's still requests in the list
	if ( !_requests.IsEmpty() )
	{
		// pull off the first item
		HttpRequest request = _request.pop_front();

		// process it
		HttpRequestProcess processReq(request);
		processReq.process();
			
		// if it's done
		if ( request.getStatus() = REQ_DONE )
		{
			// process it for the stats
			getStats( request );

		}
	}

	return bMore;
}




bool HttpServer::urlToPath ( string & strFile )
{
	bool bLegal = FALSE;
	string & strRoot = strRoot;

	// start with the root, append the abs path
	string strTemp = strRoot + strFile;

	// now canonicalize it
	DWORD dwSize = GetFullPathName( strTemp, MAX_PATH, strFile.GetBuffer(MAX_PATH+1), NULL );
	strFile.ReleaseBuffer();

	// get the full path okay?
	if ( dwSize )
	{
		int cchRoot = strRoot.GetLength();
		int cchFile = strFile.GetLength();
		// must be the same or longer than the root
		if ( cchRoot == cchFile )
		{
			// must be exactly the same
			if ( strRoot.Compare( strFile ) == 0 )
				bLegal = TRUE;
		}
		else if ( cchRoot < cchFile )
		{
			// must have the root as the base
			if ( strRoot.Compare( strFile.Left(cchRoot) ) == 0
				&& strFile.GetAt( cchRoot ) == SEPCHAR )
				bLegal = TRUE;
		}
	}

	return bLegal;
}

bool HttpServer::pathToUrl ( string & strFile )
{
	int index;
	// a local reference to the root
	string& strRoot = m_pDoc->m_strRoot;
	// change all SEPCHARs to forward slashes
	while ( (index=strFile.Find( SEPCHAR )) != -1 )
		strFile = strFile.Left( index ) + '/' + strFile.Mid(index+1);
	// now add the prefix and server, and cut the root
	string strPort;
	UINT uPort = m_pDoc->m_uPort;
	if ( uPort != PORT_HTTP )
		strPort.Format( ":%d", uPort );

	strFile = string("http://")
		+ m_pDoc->m_strServer
		+ strPort
		+ strFile.Mid(strRoot.GetLength());

	return TRUE;
}

string HttpSocketHandler::stripLast ( string & strPath )
{
	string strExtra;
	if ( !strPath.IsEmpty() )
	{
		int index = strPath.rfind( SEPCHAR );
		if ( index < 0 )
			index = 0;
		strExtra = strPath.Mid( index );
		strPath = strPath.Left( index );
	}
	return strExtra;
}


bool HttpSocketHandler::isSvrApp( void )
{
	bool bOk = false;
	int index = m_pRequest->m_strFullPath.ReverseFind( '.' );
	if ( index != -1 )
	{
		string strExt = m_pRequest->m_strFullPath.Mid( index+1 );
		string strAvail;
		// check if CGI app
		strAvail.LoadString( IDS_APP_CGI );
		bOk = checkExt( strExt, strAvail, CRequest::APP_CGI );
		if ( !bOk )
		{
			strAvail.LoadString( IDS_APP_ISAPI );
			bOk = CheckExt( strExt, strAvail, CRequest::APP_ISAPI );
		}
	}

	return bOk;
}



bool HttpSocketHandler::checkExt( const string& strExt, string& strAvail, DWORD dwType )
{
	bool bMatch = false;
	string strPossible;
	// loop through all possible exts
	while( !strAvail.IsEmpty() )
	{
		int index = strAvail.ReverseFind('\n');
		if ( index == -1 )
		{
			strPossible = strAvail;
			strAvail.Empty();
		}
		else
		{
			strPossible = strAvail.Mid( index+1 );
			strAvail = strAvail.Left( index );
		}
		if ( strExt.CompareNoCase( strPossible ) == 0 )
		{
			m_pRequest->m_dwExecute = dwType;
			bMatch = true;
			break;
		}
	}
	return bMatch;
}


bool HttpServer::checkDefault( UINT uList, bool bExecute )
{
	bool bFound = false;
	DWORD dwAttr;
	string strDefault, strDefList;
	strDefList.LoadString( uList );
	while ( !strDefList.IsEmpty() )
	{
		int index;
		strDefault = m_pRequest->m_strFullPath;
		if ( (index=strDefList.Find('\n')) == -1 )
		{
			AddFile( strDefault, strDefList );
			strDefList.Empty();
		}
		else
		{
			AddFile( strDefault, strDefList.Left(index) );
			strDefList = strDefList.Mid( index+1 );
		}
		if ( (dwAttr=GetFileAttributes(strDefault)) != -1 &&
			(dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		{
			bFound = true;
			break;
		}
	}
	if ( bFound )
	{
		// redirect to the default file
		PathToURL( strDefault );
		if ( bExecute )
			strDefault += '?';

		StuffStatus( IDS_STATUS_MOVEDTEMP );
		StuffHeader( "Location", strDefault );
		StuffString( CRLF );
	}
	return bFound;
}


bool HttpServer::find ( string & strFile  )
{
	bool bFound = FALSE;
	strFile = m_pRequest->m_strURL;


	// change from URL to local file system path
	if ( URLtoPath( strFile ) )
	{
		string strExtra; // extra path info
		m_pRequest->m_dwAttr = GetFileAttributes( strFile );
		if ( m_pRequest->m_dwAttr != -1 )
			bFound = TRUE;
		else
		{
			// rip off the last portion
			strExtra = stripLast( strFile );
			while( !strFile.IsEmpty() )
			{
				// anything there?
				m_pRequest->m_dwAttr = GetFileAttributes( strFile );
				if ( m_pRequest->m_dwAttr != -1 )
				{
					// found something; better not be a folder
					if( (m_pRequest->m_dwAttr&FILE_ATTRIBUTE_DIRECTORY) == 0 )
						bFound = TRUE;
					break;
				}
				// rip off the next portion
				strExtra = StripLast( strFile ) + strExtra;
			}
		}

		if ( bFound )
		{
			// strip any trailing SEPCHAR
			if ( strFile.GetAt( strFile.GetLength()-1) == SEPCHAR )
				m_pRequest->m_strFullPath = strFile.Left( strFile.GetLength()-1 );
			else
				m_pRequest->m_strFullPath = strFile;

			// see if we need to set the extra path info
			if ( !strExtra.IsEmpty() )
			{
				m_pRequest->m_strPathInfo = strExtra;
				if ( URLtoPath( strExtra ) )
					m_pRequest->m_strPathTranslated = strExtra;
			}

			// if it's a folder, see if we can redirect to
			// one of the default docs or apps
			if ( m_pRequest->m_dwAttr & FILE_ATTRIBUTE_DIRECTORY )
			{
				// check for existence of a default doc or app
				if ( !checkDefault( IDS_DEFAULTDOC, FALSE ) )
					checkDefault( IDS_DEFAULTAPP, TRUE );
			}
			else if ( m_pRequest->m_dwExecute && !isSvrApp() )
			{
				StuffError( IDS_STATUS_BADREQUEST );
			}
		}
		else
			StuffError( IDS_STATUS_NOTFOUND );
	}
	else
		StuffError( IDS_STATUS_BADREQUEST );

	return bFound;
}

*/


/*
void HttpServer::StopListening( void )
{
	if ( _pListen != NULL )
	{
		_pListen->Close();
		delete _pListen;
		_pListen = NULL;
	}
}

bool HttpServer::StartListening( void )
{
	bool bOk = FALSE;
	StopListening();
	_pListen = new CListenSocket( this );
	if ( _pListen )
	{
		if ( _pListen->Create( _port, SOCK_STREAM, FD_ACCEPT ) )
			bOk = _pListen->Listen();

		if ( !bOk )
		{
			string strMsg;
			int nErr = _pListen->GetLastError();
			if ( nErr == WSAEADDRINUSE )
				strMsg.Format( IDS_LISTEN_INUSE, _port );
			else
				strMsg.Format( IDS_LISTEN_ERROR, _port );

			AfxMessageBox( strMsg, MB_OK|MB_ICONSTOP );
			_pListen->Close();
			delete _pListen;
			_pListen = NULL;
		}
	}
	else
		AfxMessageBox( IDS_CANT_LISTEN, MB_OK|MB_ICONSTOP );

	_timeStarted = CTime::GetCurrentTime();

	return bOk;
}

*/

/*



void HttpServer::OnSvrOptions()
{
	// add and initialize the general page
	CPropertySheet propOptions( IDS_OPTIONS );
	CGenPage genPage( this );
	genPage._bListIcon = _bListIcon;
	genPage._bAllowListing = _bAllowListing;
	genPage._logging = _logging;
	propOptions.AddPage( &genPage );

	// add and initialize the root dir page
	CRootPage rootPage( this );
	rootPage._strRoot = _strRoot;
	propOptions.AddPage( &rootPage );

	// add and initialize the name page
	CNamePage namePage( this );
	namePage._nNameSetting = _nSvrName;
	namePage._port = _port;
	if ( _nSvrName )
		namePage._strName = _server;
	propOptions.AddPage( &namePage );

	_bResetListen = FALSE;
	propOptions.DoModal();
	if ( _bResetListen )
	{
		StartListening();
		SetTitle( NULL );
	}
}



*/