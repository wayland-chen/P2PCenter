#include "stdafx.h"


#include "HttpResponse.h"
#include "TimeStamp.h"

#pragma warning( disable:4800 )



// static link to server
HttpServerInfo *  HttpResponse::_server = NULL;


//////////////////////////////////////////////////////////////////////////////////
// HttpResponse

HttpResponse::HttpResponse () :
	HttpRequest(),
	_file(),
	_contentLength(0x7FFFFFFF),
	_rangestart(0),
	_ispartial(false)
{}


HttpResponse::HttpResponse ( HttpRequest & request ) :
	HttpRequest(),
	_file(),
	_contentLength(0x7FFFFFFF),
	_rangestart(0),
	_ispartial(false)

{
	setState(request);
}


HttpResponse::~HttpResponse ()
{
	release();
}

// create/release
bool HttpResponse::create ()
{
	// perform init
	//_hFile = 0;
	_file.create();

	return true;
}

void HttpResponse::release ()
{
	// cleanup request
	HttpRequest::release();

	// cleanup file
	_file.release();
	/*
	if ( _hFile )
	{
		// close file
		CloseHandle(_hFile);
	}
	*/
}

bool HttpResponse::startServerApp ()
{
	addError( idHttpInternalError );
	return false;
}

bool HttpResponse::startResponse ()
{
	bool bWait = false;

	string method = getMethod();
	if ( HttpUtil::isGetCmd(method) )
	{
		if( getStatus() == idHttpOk )
		{
			if ( addHeading2() )
			{
				bWait = true;
			}
		}
	}

	_server->hit( *this );

	return bWait;
}


bool HttpResponse::addHeading ()
{
	bool more = false;

	if ( getAttributes() & FILE_ATTRIBUTE_HIDDEN )
	{
		// never show hidden files
		addError( idHttpForbidden );
	}
	else if ( getAttributes() & FILE_ATTRIBUTE_DIRECTORY )
	{
		if ( _server->getAllowListing() )
		{
			// create a directory listing
			addStatus( idHttpOk );
			addString( CRLF );
			more = true;
		}
		else
			addError( idHttpForbidden );
	}
	else
	{
		LPCTSTR fileName = getFullPath().c_str();

		if ( _file.openFastRead(getFullPath()) )
		{
			if ( getStatus() != REQ_SIMPLE )
			{
				TimeStamp timeIfMod;

				string strIfMod = getHeaderValue( idParamIfModifiedSince );

				if ( strIfMod.size() > 0 &&
					HttpUtil::fromHttpTime( strIfMod, timeIfMod ) &&
					!HttpUtil::ifModSince( _file, timeIfMod ) )
				{
					// eh, it hasn't been modified
					addStatus( idHttpNotModified );

					// don't need it anymore
					_file.close();
				}
				else
				{
					// send it off
					addStatus( idHttpOk );

					// set content type and length
					long contentLength = _file.size();

					addContentType();
					addHeader( idParamContentLength, contentLength );

					// get the last modified time
					FILETIME ft;
					if ( GetFileTime( _file, NULL, NULL, &ft ) )
					{
						string strTime = HttpUtil::getHttpDate( &ft );
						addHeader( idParamLastModified, strTime );
					}

					more = true;
				}

				// blank line
				addString( CRLF );
			}
			else
			{
				more = true;
			}
		}
		else
		{
			// couldn't open; try again later
			addError( idHttpUnavailable );
		}
	}
	return more;
}

bool HttpResponse::addHeading2()
{
	if ( _ispartial )
	{
		addStatus( idHttpPartial );
		addHeader( idParamContentLength, _contentLength - _rangestart );
		char szRange[256];
		sprintf( szRange, "bytes %u-%u/%u", _rangestart, _contentLength -1, _contentLength );
		addHeader( idParamContentRange, szRange );
	}
	else
	{
		addStatus( idHttpOk );
		addHeader( idParamContentLength, _contentLength );
	}

	addContentType2();

	//addHeader( idParamKeepAlive, "timeout=5, max=100" );
	addHeader( idParamConnection, "close" );
	addHeader( idParamAcceptRanges, "bytes" );

	string strTime = HttpUtil::getHttpDate( NULL );
	addHeader( idParamLastModified, strTime );

	char szText[32];
	sprintf( szText, "0x%.8x", (DWORD)this);
	addHeader( idParamSetCookie, "this_pointer=" + string(szText) + "; path=/" );

	addString( CRLF );

	return true;
}

void HttpResponse::addTagetInfo ()
{
	// #DGH
	// need to get readfile corrrect

	//if ( _hFile != INVALID_HANDLE_VALUE)
	if ( _file.isValid() )
	{
		DWORD dwRead = 0;

		// get file size
		DWORD dwSize =
			//::GetFileSize(_hFile, NULL);
			_file.size();

		// if bytes in file
		if ( dwSize != 0 )
		{
			// set size to send
			//setSizeToSend( dwSize );

			// add null terminator to size (easier for debugging)
			//dwSize++;

			// get buffer
			string tmp;
			LPTSTR buffer = StringUtil::getBufferString(tmp,dwSize);

			// null terminate
			//buffer[dwSize] = '\0';

			// read the first chunk
			dwRead = _file.read( buffer, dwSize );
			//ReadFile( _hFile, buffer, dwSize, &dwRead, NULL );

			if ( dwRead > 0 )
			{
				addString( tmp );
				addString( CRLF );
			}
			else
			{
				// nothing read close the file
				_file.close();
				//CloseHandle( _hFile );
				//_hFile = INVALID_HANDLE_VALUE;
			}
		}
	}
	else if ( getAttributes() & FILE_ATTRIBUTE_DIRECTORY )
		addListing();
	else
		addString( CRLF );
}

void HttpResponse::addTagetInfo2 ()
{
	if ( _file.isValid() )
	{
		DWORD dwRead = 0;
		DWORD dwSize = _file.size();

		if ( dwSize != 0 )
		{
			string tmp;
			LPTSTR buffer = StringUtil::getBufferString(tmp,dwSize);

			dwRead = _file.read( buffer, dwSize );

			if ( dwRead > 0 )
			{
				addString( tmp );
				addString( CRLF );
			}
			else
			{
				_file.close();
			}
		}
	}
	else if ( getAttributes() & FILE_ATTRIBUTE_DIRECTORY )
		addListing();
	else
		addString( CRLF );
}

void HttpResponse::addListing ()
{
	bool bRoot = false;
	bool bIcons = _server->getListIcon();
	string strIcon;

	string strLine = 
	string("http://") 
		+ _server->getServer()
		+ getUrl();

	string strDir  = getUrl();
	string strMask = getFullPath();

	// make sure URL ends in a slash
	if ( strDir[ strDir.size()-1 ] != '/' )
		strDir += '/';

	// is this the server's root folder?
	else if ( strDir.compare( "/" ) == 0 )
		bRoot = true;

	// create the file search mask
	HttpUtil::addFile( strMask, IDS_DIRMASK );

	addString( IDS_CONTENTS_PRE );
	addString( strLine );
	addString( IDS_CONTENTS_POST );

	if ( bRoot )
		addString( IDS_CONTENTS_DESC );

	if ( bIcons )
		StringUtil::loadString( strIcon, IDS_ICON_BLANK );

	StringUtil::formatString( strLine, IDS_CONTENTS_HEADING, strIcon );
	addString( strLine );

	int nFiles = 0;

	WIN32_FIND_DATA fd;

	// find the first file that matches the mask
	HANDLE fh = FindFirstFile( strMask.c_str(), &fd );
	if ( fh != INVALID_HANDLE_VALUE )
	{
		// create a line for the found file
		nFiles += addListingFile( &fd, strDir, bIcons );

		// loop through all other files
		while ( FindNextFile( fh, &fd ) )
			nFiles += addListingFile( &fd, strDir, bIcons );
	}

	if ( nFiles == 0 )
		addString( IDS_CONTENTS_EMPTY );

	addString( IDS_CONTENTS_FOOTER );
	// only add the parent link if there is one
	if ( !bRoot )
	{
		if ( bIcons )
		{
			StringUtil::loadString( strIcon, IDS_ICON_PARENT );
		}

		StringUtil::formatString( strLine, IDS_CONTENTS_PARENT, strIcon );
		addString( strLine );
	}
	// add the note and end it
	addString( IDS_CONTENTS_NOTE );
	addString( CRLF );
}

int HttpResponse::addListingFile ( WIN32_FIND_DATA * pfd, 
								   const string& strDir, 
								   bool bIcons )
{
	int nFile = 0;

	// don't include '.', '..' or hidden files
	if ( lstrcmp( pfd->cFileName, "." ) && lstrcmp( pfd->cFileName, ".." )
		&& (pfd->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0 )
	{
		string strSize, strIcon = "";
		string strLine, strFile = pfd->cFileName;

		TimeStamp timeFile( pfd->ftLastWriteTime );

		bool bFolder = ((pfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
		if ( bIcons && bFolder )
			StringUtil::loadString( strIcon, IDS_ICON_FOLDER );
		else if ( bIcons )
			StringUtil::loadString( strIcon, IDS_ICON_FILE );

		// create the link string
		string strLink = strDir + strFile;

		// make sure spaces are replaced with '%20'
		int index;
		while ( (index=strLink.find(' ')) != -1 )
			strLink = StringUtil::left(strLink,index) + "%20" + StringUtil::mid(strLink,index+1);

		// format the size string
		if ( bFolder )
			strSize = "  Folder";
		else if ( pfd->nFileSizeHigh > 0 )
			strSize = "   &gt; 4GB"; // yeah, right.
		else if ( pfd->nFileSizeLow < 1024 )
			strSize = "    &lt; 1K";
		else
			StringUtil::formatString( strSize, "%7dK", pfd->nFileSizeLow/1024 );

		
		StringUtil::formatString( strLine, 
								  IDS_CONTENTS_FORMAT,
								  timeFile.format( IDS_FILETIMEFMT ),
								  strSize, strLink, strIcon, strFile );
		

		addString( strLine );

		nFile = 1;
	}
	return nFile;
}

void HttpResponse::addContentType ()
{
	// get the extension
	string fullPath = getFullPath();
	string strExt   = StringUtil::mid( fullPath, fullPath.rfind('.') );

	// add content type
	string strType = "";
	if ( StringUtil::compareNoCase(strExt,".html") )
		strType = "text/html";
	else if ( StringUtil::compareNoCase(strExt,".xml") )
		strType = "text/xml";
	else if ( StringUtil::compareNoCase(strExt,".rmvb") )
		strType = "application/vnd.rn-realmedia-vbr";
	else if ( StringUtil::compareNoCase(strExt,".rm") )
		strType = "application/vnd.rn-realmedia";
	else if ( StringUtil::compareNoCase(strExt,".wmv") )
		strType = "video/x-ms-wmv";
	else if ( StringUtil::compareNoCase(strExt,".swf") )
		strType = "application/x-shockwave-flash";

	if ( strType.size() > 0)
		addHeader( idParamContentType, strType );

}

void HttpResponse::addContentType2 ()
{
	// get the extension
	string fullPath = getUrl();
	string strExt;
	int pos = fullPath.rfind('.');
	if ( pos != -1 )
		strExt = StringUtil::mid( fullPath, pos );

	// add content type
	string strType = "";
	if ( StringUtil::compareNoCase(strExt,".html") )
		strType = "text/html";
	else if ( StringUtil::compareNoCase(strExt,".xml") )
		strType = "text/xml";
	else if ( StringUtil::compareNoCase(strExt,".rmvb") )
		strType = "application/vnd.rn-realmedia-vbr";
	else if ( StringUtil::compareNoCase(strExt,".rm") )
		strType = "application/vnd.rn-realmedia";
	else if ( StringUtil::compareNoCase(strExt,".wmv") )
		strType = "video/x-ms-wmv";
	else if ( StringUtil::compareNoCase(strExt,".swf") )
		strType = "application/x-shockwave-flash";

	if ( strType.size() > 0)
		addHeader( idParamContentType, strType );
}

void HttpResponse::setContentLength( unsigned long contentlength )
{
	_contentLength = contentlength;
	_rangestart = min(_rangestart, _contentLength);
}

void HttpResponse::setRangeStart( unsigned long rangestart )
{
	_rangestart = min(rangestart, _contentLength);
	if ( _rangestart > 0 )
		_ispartial = true;
	else
		_ispartial = false;
}

unsigned long HttpResponse::getContentLength()
{
	return _contentLength;
}

unsigned long HttpResponse::getRangeStart()
{
	return _rangestart;
}

/*
// create/release

bool HttpResponse::create ()
{
	// get response stream
	string & srchStr = _responseStrm.str();
	cout << srchStr;

	// set token string
	string tokenStr = "\r\n";

	// get response
	long first = 0; long last = 0;
	if ( !getParam(first,last,tokenStr,srchStr,_response) )
		return false;

	// get header
	if ( !getParam(first,last,tokenStr,srchStr,_header) )
		return true;


	// get response header
	if ( !getParam(first,last,tokenStr,srchStr,_responseHeader) )
		return true;

	// get response header
	if ( !getParam(first,last,tokenStr,srchStr,_entityHeader) )
		return true;

	// get response header
	if ( !getParam(first,last,tokenStr,srchStr,_entityBody) )
		return true;


	return true;
}


*/

