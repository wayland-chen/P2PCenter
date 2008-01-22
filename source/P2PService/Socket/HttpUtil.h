#if !defined(HttpUtil_H)
#define HttpUtil_H

#include "HttpStrings.h"
#include "StringUtil.h"
#include "TimeStamp.h"

#include <wininet.h>


// http://www.w3.org/pub/WWW/Protocols

/*
 * The following methods are used to access the header fields and 
 * the contents after the connection is made to the remote object:
 * <ul>
 *   <li><code>getContent</code>
 *   <li><code>getHeaderField</code>
 *   <li><code>getInputStream</code>
 *   <li><code>getOutputStream</code>
 * </ul>
 * <p>
 * Certain header fields are accessed frequently. The methods:
 * <ul>
 *   <li><code>getContentEncoding</code>
 *   <li><code>getContentLength</code>
 *   <li><code>getContentType</code>
 *   <li><code>getDate</code>
 *   <li><code>getExpiration</code>
 *   <li><code>getLastModifed</code>
 * </ul>
 * <p>
 * provide convenient access to these fields. The 
 * <code>getContentType</code> method is used by the 
 * <code>getContent</code> method to determine the type of the remote 
 * object; subclasses may find it convenient to override the 
 * <code>getContentType</code> method. 
 * <p>
 * In the common case, all of the pre-connection parameters and 
 * general request properties can be ignored: the pre-connection 
 * parameters and request properties default to sensible values. For 
 * most clients of this interface, there are only two interesting 
 * methods: <code>getInputStream</code> and <code>getObject</code>, 
 * which are mirrored in the <code>URL</code> class by convenience methods.
 * <p>
 * More information on the request properties and header fields of 
 * an <code>http</code> connection can be found at:
 * <blockquote><pre>
 * http://www.w3.org/hypertext/WWW/Protocols/HTTP1.0/draft-ietf-http-spec.html
 * </pre></blockquote>
*/

#define MAX_DIR_FILENAME    24



// http commands 
const string httpCommands[] =
{
	"GET", "POST", "HEAD", "OPTIONS", "PUT", "DELETE", "TRACE"
};

// http header param names

#define idParamContentLength	"Content-Length"
#define idParamLastModified		"Last-Modified"
#define idParamIfModifiedSince	"If-Modified-Since"
#define idParamContentType		"Content-Type"
#define idParamAcceptRanges		"Accept-Ranges"
#define idParamKeepAlive		"Keep-Alive"
#define idParamContentRange		"Content-Range"
#define idParamConnection		"Connection"
#define idParamSetCookie		"Set-Cookie"
#define idParamContentDispos    "Content-Disposition"

//: bytes
//: timeout=5, max=100

// common definitions

#define CRLF			"\x0d\x0a"
#define DOUBLE_CRLF		"\x0d\x0a\x0d\x0a"
#define SEPCHAR			'\\'
#define PORT_HTTP		80
#define HTTP_SEPCHAR	'/'

// http command ids 

enum
{
	idHttpGet,
	idHttpPost,
	idHttpHead,
	idHttpOptions,
	idHttpPut,
	idHttpDelete,
	idHttpTrace
};



// http response codes
enum
{
    // 2XX: generally "OK"
    idHttpOk = 200,
    idHttpCreated = 201,
    idHttpAccepted = 202,
    idHttpNotAuthoritative = 203, 
    idHttpNoContent = 204,
    idHttpReset = 205,
    idHttpPartial = 206,

    // 3XX: relocation/redirect
    idHttpMultChoice = 300,
    idHttpMovedPerm = 301,
    idHttpMovedTemp = 302,
    idHttpSeeOther = 303,
    idHttpNotModified = 304,
    idHttpUseProxy = 305,

    // 4XX: client error
    idHttpBadRequest = 400,
    idHttpUnauthorized = 401,
    idHttpPaymentRequired = 402,
    idHttpForbidden = 403,
    idHttpNotFound = 404,
    idHttpBadMethod = 405,
    idHttpNotAcceptable = 406,
    idHttpProxyAuth = 407,
    idHttpClientTimeout = 408,
    idHttpConflict = 409,
    idHttpGone = 410,
    idHttpLengthRequired = 411,
    idHttpPreconFailed = 412,
    idHttpEntityTooLarge = 413,
    idHttpRequestTooLong = 414,
    idHttpUnsupportedType = 415,
    
    // 5XX: server error
    idHttpSeverError = 500,
    idHttpInternalError = 501,
    idHttpBadGateway = 502,
    idHttpUnavailable = 503,
    idHttpGatewayTimeout = 504,
    idHttpVersion = 505,

};


enum
{
	idAttrHidden,
	idAttrDirectory
};




/////////////////////////////////////////////////////////////////////////////////////////
// HttpUtil
//
// Purpose:		provides common http utility methods


class HttpUtil
{
public:

	HttpUtil ()
	{}

	virtual ~HttpUtil ()
	{}

	// create/release
	static
	bool getParam ( long & first, long & last, 
		            string & tokenStr, string & srchStr,
		            string & paramStr )
	{
		// set first pos for search
		last = srchStr.find( tokenStr, first );
		if ( last == -1 )
			return false;

		// get param
		paramStr = srchStr.substr( first, last );

		// skip token
		last += tokenStr.length();
	
		// reset srch index
		first = last;

		return true;
	}


	static
	void addFile ( string & strPath, const string & strFile )
	{
		if ( strPath[ strPath.size()- 1 ] != SEPCHAR )
			strPath += SEPCHAR;

		strPath += strFile;
	}

	static
	void addFile ( string & strPath, LPTSTR uStr )
	{
		string strFile;
		StringUtil::loadString( strFile, uStr );

		addFile( strPath, strFile );
	}

	static
	bool isGetCmd ( string & cmd )
	{
		if ( StringUtil::compareNoCase(cmd,httpCommands[idHttpGet]) )
			return true;
		else 
			return false;
	}

	static
	bool isPostCmd ( string & cmd )
	{
		if ( StringUtil::compareNoCase(cmd,httpCommands[idHttpPost]) )
			return true;
		else 
			return false;
	}

	static
	bool isHeadCmd ( string & cmd )
	{
		if ( StringUtil::compareNoCase(cmd,httpCommands[idHttpHead]) )
			return true;
		else 
			return false;
	}

	static
	bool isOptionsCmd ( string & cmd )
	{

		if ( StringUtil::compareNoCase(cmd,httpCommands[idHttpOptions]) )
			return true;
		else 
			return false;
	}

	static
	bool isPutCmd ( string & cmd )
	{
		if ( StringUtil::compareNoCase(cmd,httpCommands[idHttpPut]) )
			return true;
		else 
			return false;
	}

	static
	bool isDeleteCmd ( string & cmd )
	{
		if ( StringUtil::compareNoCase(cmd,httpCommands[idHttpDelete]) )
			return true;
		else 
			return false;
	}

	static
	bool isTraceCmd ( string & cmd )
	{
		if ( StringUtil::compareNoCase(cmd,httpCommands[idHttpTrace]) )
			return true;
		else 
			return false;
	}


	static
	long getFileAttribute ( long winFileAttr )
	{
		long attr = 0;

		if ( winFileAttr & FILE_ATTRIBUTE_HIDDEN )
			attr |= idAttrHidden;
		if ( winFileAttr & FILE_ATTRIBUTE_DIRECTORY )
			attr |= idAttrDirectory;

		return attr;

	}

	static
	string getHttpDate ( LPFILETIME pft = NULL )
	{
		SYSTEMTIME st;
		if ( pft )
			FileTimeToSystemTime( pft, &st );
		else
			GetSystemTime( &st );

		return toHttpDate(st);
	}

	static
	string toHttpDate ( SYSTEMTIME & st )
	{
		// get internet time
		char buffer[INTERNET_RFC1123_BUFSIZE];
		InternetTimeFromSystemTime( &st, INTERNET_RFC1123_FORMAT,
									buffer, INTERNET_RFC1123_BUFSIZE );

		return string(buffer);
	}

	static
	bool ifModSince ( HANDLE hFile, const TimeStamp & timeIfMod )
	{
		// assume it has been modified....
		bool bOk = true;
		FILETIME ft;
		if ( GetFileTime( hFile, NULL, NULL, &ft ) )
		{
			SYSTEMTIME st;
			if ( FileTimeToSystemTime( &ft, &st ) )
			{
				TimeStamp timeFile( st );
				if ( timeFile <= timeIfMod )
					bOk = false;
			}
		}
		return bOk;
	}


	// Dow, dd Mon year hh:mm:ss GMT
	static
	bool fromHttpTime ( const string & strHttp, TimeStamp & timeHttp )
	{
		// assume we couldn't get a good time conversion....
		bool bOk = false;
		SYSTEMTIME st = {0};
		int index;
		switch( strHttp[3] )
		{
			case ',':
				// read RFC-1123 (preferred)....
				st.wDay    = StringUtil::intVal( StringUtil::mid(strHttp,5,2) );
				st.wMonth  = StringUtil::monthFromStr( StringUtil::mid(strHttp,8,3) );
				st.wYear   = StringUtil::intVal( StringUtil::mid(strHttp,12,4) );
				st.wHour   = StringUtil::intVal( StringUtil::mid(strHttp,17,2) );
				st.wMinute = StringUtil::intVal( StringUtil::mid(strHttp,20,2) );
				st.wSecond = StringUtil::intVal( StringUtil::mid(strHttp,23,2) );
				break;
			case ' ':
				// read ANSI-C time format....
				st.wDay    = StringUtil::intVal( StringUtil::mid(strHttp,8,2) );
				st.wMonth  = StringUtil::monthFromStr( StringUtil::mid(strHttp,4,3) );
				st.wYear   = StringUtil::intVal( StringUtil::mid(strHttp,20,4) );
				st.wHour   = StringUtil::intVal( StringUtil::mid(strHttp,11,2) );
				st.wMinute = StringUtil::intVal( StringUtil::mid(strHttp,14,2) );
				st.wSecond = StringUtil::intVal( StringUtil::mid(strHttp,17,2) );
				break;
			default:
				if ( (index = strHttp.find( ", " )) != -1 )
				{
					st.wDay    = StringUtil::intVal( StringUtil::mid(strHttp,index+2,2) );
					st.wMonth  = StringUtil::monthFromStr( StringUtil::mid(strHttp,index+5,3) );
					st.wYear   = StringUtil::intVal( StringUtil::mid(strHttp,index+9,2) );
					st.wHour   = StringUtil::intVal( StringUtil::mid(strHttp,index+12,2) );
					st.wMinute = StringUtil::intVal( StringUtil::mid(strHttp,index+15,2) );
					st.wSecond = StringUtil::intVal( StringUtil::mid(strHttp,index+18,2) );

					// add the correct century....
					st.wYear += (st.wYear > 50)?1900:2000;
				}
				break;
		}

		// if year not zero, we pulled the info out of the string....
		if ( st.wYear != 0 )
		{
			// assume GMT....
			TimeStamp strTime( st );

			// check to see if the minutes are the same....
			if ( strTime.getMinute() == st.wMinute )
			{
				// assume it worked....
				timeHttp = strTime;
				bOk = true;
			}
		}
		return bOk;
	}


	static
	void getStatusString ( long status, string & str )
	{


		switch ( status )
		{
			// 2XX: generally "OK"
			case idHttpOk:
				str = IDS_STATUS_OK;
				break;
			case idHttpCreated:
				str = IDS_STATUS_CREATED;
				break;
			case idHttpAccepted:
				str = IDS_STATUS_ACCEPTED;
				break;
			
			//case idHttpNotAuthoritative:

			case idHttpNoContent:
				str = IDS_STATUS_NOCONTENT;
				break;

			//case idHttpReset:
			case idHttpPartial:
				str = IDS_STATUS_PARTIAL;
				break;
			

			// 3XX: relocation/redirect
			//case idHttpMultChoice:
			case idHttpMovedPerm:
				str = IDS_STATUS_MOVEDPERM;
				break;
			case idHttpMovedTemp:
				str = IDS_STATUS_MOVEDTEMP;
				break;
			//case idHttpSeeOther
			case idHttpNotModified:
				str = IDS_STATUS_NOTMODIFIED;
				break;
			//case idHttpUseProxy:

			// 4XX: client error
			case idHttpBadRequest:
				str = IDS_STATUS_BADREQUEST;
				break;
			case idHttpUnauthorized:
				str = IDS_STATUS_UNAUTH;
				break;
			//case idHttpPaymentRequired:
			case idHttpForbidden:
				str = IDS_STATUS_FORBIDDEN;
				break;
			case idHttpNotFound:
				str = IDS_STATUS_NOTFOUND;
				break;

			//case idHttpBadMethod = 405,
			//case idHttpNotAcceptable = 406,
			//case idHttpProxyAuth = 407,
			//case idHttpClientTimeout = 408,
			//case idHttpConflict = 409,
			//case idHttpGone = 410,
			//case idHttpLengthRequired = 411,
			//case idHttpPreconFailed = 412,
			//case idHttpEntityTooLarge = 413,
			//case idHttpRequestTooLong = 414,
			//case idHttpUnsupportedType = 415,
    
			// 5XX: server error
			case idHttpSeverError:
				str = IDS_STATUS_SVRERROR;
				break;

			case idHttpInternalError:
				str = IDS_STATUS_NOTIMPL;
				break;

			case idHttpBadGateway:
				str = IDS_STATUS_BADGATEWAY;
				break;

			case idHttpUnavailable:
				str = IDS_STATUS_SVCUNAVAIL;
				break;

			//case idHttpGatewayTimeout = 504,
			//case idHttpVersion = 505,
		}
	}



};


#endif