#include "stdafx.h"

#include "HttpServerInfo.h"



HttpServerInfo::HttpServerInfo ( long port ) :

	HttpServerStats(),

	_strTitleBase(""),
	_strRoot(""),
	_strServer(),
	_timeStarted(),
	_port(port),
	_server(0),
	_bListIcon(false),
	_bAllowListing(true)

{}



HttpServerInfo::~HttpServerInfo ()
{
	release();
}


bool HttpServerInfo::create ( string & server, string & directory )
{
	// reset stats
	reset();

	// show logging
	setLogging( true );

	// assign default values
	_strServer = server;

	// get the default root path
	_strRoot = directory;

	// ensure no final separator character
	if ( _strRoot[_strRoot.size()-1] == SEPCHAR )
		_strRoot.erase( _strRoot.size() -  1 );

	// make into a full path string
	char buffer[MAX_PATH+1];
	GetFullPathName( _strRoot.c_str(), MAX_PATH, buffer, NULL);
	_strRoot = buffer;

	return true;
}

void HttpServerInfo::release ()
{
	// release stats
	HttpServerStats::release();

	// release objects
	_strTitleBase.erase();
	_strRoot.erase();
	_strServer.erase();

}


bool HttpServerInfo::urlToPath ( string & strFile )
{
	bool bLegal = false;

	// remove leading indicators
	StringUtil::trimLeft( strFile, "/" );


	// start with the root, append the abs path
	string & strRoot = getRoot();
	string strTemp   = strRoot + SEPCHAR + strFile;

	// now canonicalize it
	char buffer[MAX_PATH+1];

	DWORD dwSize = 
	GetFullPathName( strTemp.c_str(), MAX_PATH, buffer, NULL );
	strFile = buffer;

	// get the full path okay?
	if ( dwSize )
	{
		int cchRoot = strRoot.size();
		int cchFile = strFile.size();

		// must be the same or longer than the root
		if ( cchRoot == cchFile )
		{
			// must be exactly the same
			if ( strRoot.compare( strFile ) == 0 )
				bLegal = true;
		}
		else if ( cchRoot < cchFile )
		{
			// must have the root as the base
			if ( strRoot.compare( StringUtil::left(strFile,cchRoot) ) == 0 &&
				 strFile[cchRoot] == SEPCHAR )
				bLegal = true;
		}
	}

	return bLegal;
}

bool HttpServerInfo::pathToUrl ( string & strFile )
{
	int index;

	// a local reference to the root
	string & strRoot = getRoot();

	// change all SEPCHARs to forward slashes
	while ( (index=strFile.find( SEPCHAR )) != -1 )
		strFile = StringUtil::left( strFile,index ) + '/' + StringUtil::mid(strFile,index+1);

	// now add the prefix and server, and cut the root
	string strPort;
	UINT uPort = getPort();
	if ( uPort != PORT_HTTP )
		StringUtil::formatString(strPort, ":%d", uPort );

	
	strFile = string("http://")
		+ getServer()
		+ strPort
		+ StringUtil::mid(strFile,strRoot.size());
	

	return true;
}

string HttpServerInfo::stripLast ( string & strPath )
{
	string strExtra;
	if ( !strPath.empty() )
	{
		int index = strPath.rfind( SEPCHAR );
		if ( index < 0 )
			index = 0;

		strExtra = StringUtil::mid(strPath,index);
		strPath  = StringUtil::left( strPath, index );
	}
	return strExtra;
}


bool HttpServerInfo::find ( HttpRequest & request, 
						    string & strFile )
{
	bool found = false;

	// get url
	strFile = request.getUrl();

	// remove leading indicators
	StringUtil::trimLeft( strFile, "/" );


	// change from URL to local file system path
	if ( urlToPath( strFile ) )
	{
		string strExtra; // extra path info

		request.setAttributes( GetFileAttributes( strFile.c_str() ) );

		if ( request.getAttributes() != -1 )
			found = true;
		else
		{
			// rip off the last portion
			strExtra = stripLast( strFile );

			while( !strFile.empty() )
			{
				// anything there?
				request.setAttributes( GetFileAttributes( strFile.c_str() ) );

				if ( request.getAttributes() != -1 )
				{
					// found something; better not be a folder
					if( ( request.getAttributes() & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
						found = true;
					break;
				}

				// rip off the next portion
				strExtra = stripLast( strFile ) + strExtra;
			}
		}
	

		
		// #DGH
		// come back to later
		// this code exists in above if statement

		
		if ( found )
		{
			// strip any trailing SEPCHAR
			if ( strFile[strFile.size()-1] == SEPCHAR )
				request.setFullPath( StringUtil::left( strFile, strFile.size()-1 ) );
			else
				request.setFullPath( strFile );

			// see if we need to set the extra path info
			if ( !strExtra.empty() )
			{
				request.setPathInfo( strExtra );

				if ( urlToPath( strExtra ) )
					request.setPathTranslated( strExtra );
			}

			// if it's a folder, see if we can redirect to
			// one of the default docs or apps
			if ( request.getAttributes() & FILE_ATTRIBUTE_DIRECTORY )
			{
				// check for existence of a default doc or app
				if ( !checkDefault( request, IDS_DEFAULTDOC, false ) )
					checkDefault( request, IDS_DEFAULTAPP, true );
			}
			else if ( request.getExecute() && !isSeverApp(request) )
			{
				request.addError( idHttpBadRequest );
			}
		}
		else
		{
			request.addError( idHttpNotFound );
		}
	}
	else
	{
		request.addError( idHttpBadRequest );
	}

	
	return found;
}


bool HttpServerInfo::checkDefault ( HttpRequest & request, 
								    LPTSTR uList, 
									bool bExecute )
{
	bool found = false;
	DWORD dwAttr;
	string strDefault;

	// get default list
	string strDefList;
	StringUtil::loadString( strDefList,  uList );

	// test list
	while ( !strDefList.empty() )
	{
		// get path
		int index;
		strDefault = request.getFullPath();

		index = strDefList.find('\n');
		if ( index == -1 )
		{
			HttpUtil::addFile( strDefault, strDefList );
			strDefList.erase();
		}
		else
		{
			HttpUtil::addFile( strDefault, StringUtil::left( strDefList, index) );
			strDefList = StringUtil::mid( strDefList,  index+1 );
		}

		if ( (dwAttr=GetFileAttributes(strDefault.c_str())) != -1 &&
			 (dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		{
			found = true;
			break;
		}
	}

	if ( found )
	{
		// redirect to the default file
		pathToUrl( strDefault );
		if ( bExecute )
			strDefault += '?';

		request.addStatus( idHttpMovedTemp );
		request.addHeader( "Location", strDefault );
		request.addString( CRLF );
		
	}
	return found;
}

bool HttpServerInfo::isSeverApp ( HttpRequest & request )
{
	bool ok = false;

	// find ext terminator
	int index = request.getFullPath().rfind( '.' );

	// if found
	if ( index != -1 )
	{
		// get extension
		string strExt = StringUtil::mid( request.getFullPath(), index+1 );
		string strAvail;

		// check if CGI app
		StringUtil::loadString( strAvail, IDS_APP_CGI );

		ok = checkExt( request, strExt, strAvail, idAppCgi );
		if ( !ok )
		{
			StringUtil::loadString( strAvail, IDS_APP_ISAPI );
			ok = checkExt( request, strExt, strAvail, idAppIsapi );
		}
	}

	return ok;
}


bool HttpServerInfo::checkExt ( HttpRequest & request,
							    const string & strExt, 
								string & strAvail, 
								DWORD dwType )
{
	// show match not found
	bool match = false;

	// stores test extension
	string strPossible;

	// loop through all possible exts
	while( !strAvail.empty() )
	{
		int index = strAvail.rfind('\n');
		if ( index == -1 )
		{
			strPossible = strAvail;
			strAvail.erase();
		}
		else
		{
			strPossible = StringUtil::mid( strAvail, index+1 );
			strAvail    = StringUtil::left( strAvail,  index );
		}

		// compare extension
		if ( StringUtil::compareNoCase( strExt, strPossible ) == 0 )
		{
			request.setExecute( dwType );
			match = true;

			break;
		}
	}

	return match;
}
