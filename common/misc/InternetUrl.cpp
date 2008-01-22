
#include ".\interneturl.h"

#pragma warning(disable:4244)
#pragma warning(disable:4267)

CInternetUrl::CInternetUrl( const char* url )
{
	m_url = NULL;
	int pos1, pos2,v6addr = 0;
	char *escfilename=NULL;
	char *ptr1=NULL, *ptr2=NULL, *ptr3=NULL, *ptr4=NULL;
	int jumpSize = 3;

	if( url==NULL ) return;

	if (strlen(url) > (SIZE_MAX / 3 - 1)) {
		//mp_msg(MSGT_NETWORK,MSGL_FATAL,MSGTR_MemAllocFailed);
		goto err_out;
	}
	escfilename=(char *)malloc(strlen(url)*3+1);
	if (!escfilename ) {
		//mp_msg(MSGT_NETWORK,MSGL_FATAL,MSGTR_MemAllocFailed);
		goto err_out;
	}

	// Create the URL container
	m_url = (URL_t*)malloc(sizeof(URL_t));
	memset( m_url, 0, sizeof(URL_t) );
	if( m_url==NULL ) {
		//mp_msg(MSGT_NETWORK,MSGL_FATAL,MSGTR_MemAllocFailed);
		goto err_out;
	}

	// Initialisation of the URL container members
	memset( m_url, 0, sizeof(URL_t) );

	url_unescape_string(escfilename,url);

	// Copy the url in the URL container
	char* urls = strdup(escfilename);
	if( urls==NULL ) {
		//		mp_msg(MSGT_NETWORK,MSGL_FATAL,MSGTR_MemAllocFailed);
		goto err_out;
	}
	m_url->url = (char *)malloc( strlen(urls) + 1 );
	strcpy( m_url->url, urls);
	//mp_msg(MSGT_OPEN,MSGL_V,"Filename for url is now %s\n",escfilename);

	// extract the protocol
	ptr1 = strstr(escfilename, "://");
	if( ptr1==NULL ) {
		// Check for a special case: "sip:" (without "//"):
		if (strstr(escfilename, "sip:") == escfilename) {
			ptr1 = (char *)&url[3]; // points to ':'
			jumpSize = 1;
		} else {
			//mp_msg(MSGT_NETWORK,MSGL_V,"Not an URL!\n");
			goto err_out;
		}
	}
	pos1 = ptr1-escfilename;
	m_url->protocol = (char*)malloc(pos1+1);
	if( m_url->protocol==NULL ) {
		//		mp_msg(MSGT_NETWORK,MSGL_FATAL,MSGTR_MemAllocFailed);
		goto err_out;
	}
	strncpy(m_url->protocol, escfilename, pos1);
	m_url->protocol[pos1] = '\0';

	// jump the "://"
	ptr1 += jumpSize;
	pos1 += jumpSize;

	// check if a username:password is given
	ptr2 = strstr(ptr1, "@");
	ptr3 = strstr(ptr1, "/");
	if( ptr3!=NULL && ptr3<ptr2 ) {
		// it isn't really a username but rather a part of the path
		ptr2 = NULL;
	}
	if( ptr2!=NULL ) {
		// We got something, at least a username...
		int len = ptr2-ptr1;
		m_url->username = (char*)malloc(len+1);
		if( m_url->username==NULL ) {
			//			mp_msg(MSGT_NETWORK,MSGL_FATAL,MSGTR_MemAllocFailed);
			goto err_out;
		}
		strncpy(m_url->username, ptr1, len);
		m_url->username[len] = '\0';

		ptr3 = strstr(ptr1, ":");
		if( ptr3!=NULL && ptr3<ptr2 ) {
			// We also have a password
			int len2 = ptr2-ptr3-1;
			m_url->username[ptr3-ptr1]='\0';
			m_url->password = (char*)malloc(len2+1);
			if( m_url->password==NULL ) {
				//mp_msg(MSGT_NETWORK,MSGL_FATAL,MSGTR_MemAllocFailed);
				goto err_out;
			}
			strncpy( m_url->password, ptr3+1, len2);
			m_url->password[len2]='\0';
		}
		ptr1 = ptr2+1;
		pos1 = ptr1-escfilename;
	}

	// before looking for a port number check if we have an IPv6 type numeric address
	// in IPv6 URL the numeric address should be inside square braces.
	ptr2 = strstr(ptr1, "[");
	ptr3 = strstr(ptr1, "]");
	ptr4 = strstr(ptr1, "/");
	if( ptr2!=NULL && ptr3!=NULL && ptr2 < ptr3 && (!ptr4 || ptr4 > ptr3)) {
		// we have an IPv6 numeric address
		ptr1++;
		pos1++;
		ptr2 = ptr3;
		v6addr = 1;
	} else {
		ptr2 = ptr1;

	}

	// look if the port is given
	ptr2 = strstr(ptr2, ":");
	// If the : is after the first / it isn't the port
	ptr3 = strstr(ptr1, "/");
	if(ptr3 && ptr3 - ptr2 < 0) ptr2 = NULL;
	if( ptr2==NULL ) {
		// No port is given
		// Look if a path is given
		if( ptr3==NULL ) {
			// No path/filename
			// So we have an URL like http://www.hostname.com
			pos2 = strlen(escfilename);
		} else {
			// We have an URL like http://www.hostname.com/file.txt
			pos2 = ptr3-escfilename;
		}
	} else {
		// We have an URL beginning like http://www.hostname.com:1212
		// Get the port number
		m_url->port = atoi(ptr2+1);
		pos2 = ptr2-escfilename;
	}
	if( v6addr ) pos2--;
	// copy the hostname in the URL container
	m_url->hostname = (char*)malloc(pos2-pos1+1);
	if( m_url->hostname==NULL ) {
		//mp_msg(MSGT_NETWORK,MSGL_FATAL,MSGTR_MemAllocFailed);
		goto err_out;
	}
	strncpy(m_url->hostname, ptr1, pos2-pos1);
	m_url->hostname[pos2-pos1] = '\0';

	// Look if a path is given
	ptr2 = strstr(ptr1, "/");
	if( ptr2!=NULL ) {
		// A path/filename is given
		// check if it's not a trailing '/'
		if( strlen(ptr2)>1 ) {
			// copy the path/filename in the URL container
			char* file = strdup(ptr2);
			if( file==NULL ) {
				//mp_msg(MSGT_NETWORK,MSGL_FATAL,MSGTR_MemAllocFailed);
				goto err_out;
			}
			m_url->file = (char *)malloc( strlen(file) + 1 );
			strcpy( m_url->file, strdup(file));
		}
	} 
	// Check if a filename was given or set, else set it with '/'
	if( m_url->file==NULL ) {
		m_url->file = (char*)malloc(2);
		if( m_url->file==NULL ) {
			//mp_msg(MSGT_NETWORK,MSGL_FATAL,MSGTR_MemAllocFailed);
			goto err_out;
		}
		strcpy(m_url->file, "/");
	}

	free(escfilename);
	escfilename = NULL;

	SplitFilePar();

	return;

err_out:
	if (escfilename) free(escfilename);
	url_free();
	return;
}

CInternetUrl::~CInternetUrl(void)
{
	url_free();
}

void CInternetUrl::url_free()
{
	if( m_url)
	{
		if(m_url->url) free(m_url->url);
		if(m_url->protocol) free(m_url->protocol);
		if(m_url->hostname) free(m_url->hostname);
		if(m_url->file) free(m_url->file);
		if(m_url->username) free(m_url->username);
		if(m_url->password) free(m_url->password);
		free(m_url);
		m_url = NULL;
	}
}

/* Replace escape sequences in an URL (or a part of an URL) */
/* works like strcpy(), but without return argument */
void CInternetUrl::url_unescape_string(char *outbuf, const char *inbuf)
{
	unsigned char c,c1,c2;
	int i,len=strlen(inbuf);
	for (i=0;i<len;i++){
		c = inbuf[i];
		if (c == '%' && i<len-2) { //must have 2 more chars
			c1 = toupper(inbuf[i+1]); // we need uppercase characters
			c2 = toupper(inbuf[i+2]);
			if (	((c1>='0' && c1<='9') || (c1>='A' && c1<='F')) &&
				((c2>='0' && c2<='9') || (c2>='A' && c2<='F')) ) {
					if (c1>='0' && c1<='9') c1-='0';
					else c1-='A'-10;
					if (c2>='0' && c2<='9') c2-='0';
					else c2-='A'-10;
					c = (c1<<4) + c2;
					i=i+2; //only skip next 2 chars if valid esc
				}
		}
		*outbuf++ = c;
	} 
	*outbuf++='\0'; //add nullterm to string
}

void CInternetUrl::url_escape_string_part(char *outbuf, const char *inbuf) {
	unsigned char c,c1,c2;
	int i,len=strlen(inbuf);

	for  (i=0;i<len;i++) {
		c = inbuf[i];
		if ((c=='%') && i<len-2 ) { //need 2 more characters
			c1=toupper(inbuf[i+1]); c2=toupper(inbuf[i+2]); // need uppercase chars
		} else {
			c1=129; c2=129; //not escape chars
		}

		if(	(c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			(c >= '0' && c <= '9') ||
			(c >= 0x7f)) {
				*outbuf++ = c;
			} else if ( c=='%' && ((c1 >= '0' && c1 <= '9') || (c1 >= 'A' && c1 <= 'F')) &&
				((c2 >= '0' && c2 <= '9') || (c2 >= 'A' && c2 <= 'F'))) {
					// check if part of an escape sequence
					*outbuf++=c;                      // already

					// dont escape again
					// mp_msg(MSGT_NETWORK,MSGL_ERR,MSGTR_MPDEMUX_URL_StringAlreadyEscaped,c,c1,c2);
					// error as this should not happen against RFC 2396
					// to escape a string twice
				} else {
					/* all others will be escaped */
					c1 = ((c & 0xf0) >> 4);
					c2 = (c & 0x0f);
					if (c1 < 10) c1+='0';
					else c1+='A'-10;
					if (c2 < 10) c2+='0';
					else c2+='A'-10;
					*outbuf++ = '%';
					*outbuf++ = c1;
					*outbuf++ = c2;
				}
	}
	*outbuf++='\0';
}

/* Replace specific characters in the URL string by an escape sequence */
/* works like strcpy(), but without return argument */
void CInternetUrl::url_escape_string_bak(char *outbuf, const char *inbuf) 
{
	strcpy( outbuf, inbuf ); //
}

void CInternetUrl::url_escape_string(char *outbuf, const char *inbuf) {
	unsigned char c;
	int i = 0,j,len = strlen(inbuf);
	char* tmp,*unesc = NULL, *in;

	// Look if we have an ip6 address, if so skip it there is
	// no need to escape anything in there.
	tmp = strstr(inbuf,"://[");
	if(tmp) {
		tmp = strchr(tmp+4,']');
		if(tmp && (tmp[1] == '/' || tmp[1] == ':' ||
			tmp[1] == '\0')) {
				i = tmp+1-inbuf;
				strncpy(outbuf,inbuf,i);
				outbuf += i;
				tmp = NULL;
			}
	}

	while(i < len) {
		// look for the next char that must be kept
		for  (j=i;j<len;j++) {
			c = inbuf[j];
			if(c=='-' || c=='_' || c=='.' || c=='!' || c=='~' ||	/* mark characters */
				c=='*' || c=='\'' || c=='(' || c==')' || 	 	/* do not touch escape character */
				c==';' || c=='/' || c=='?' || c==':' || c=='@' || 	/* reserved characters */
				c=='&' || c=='=' || c=='+' || c=='$' || c==',') 	/* see RFC 2396 */
				break;
		}
		// we are on a reserved char, write it out
		if(j == i) {
			*outbuf++ = c;
			i++;
			continue;
		}
		// we found one, take that part of the string
		if(j < len) {
			if(!tmp) tmp = (char *)malloc(len+1);
			strncpy(tmp,inbuf+i,j-i);
			tmp[j-i] = '\0';
			in = tmp;
		} else // take the rest of the string
			in = (char*)inbuf+i;

		if(!unesc) unesc = (char *)malloc(len+1);
		// unescape first to avoid escaping escape
		url_unescape_string(unesc,in);
		// then escape, including mark and other reserved chars
		// that can come from escape sequences
		url_escape_string_part(outbuf,unesc);
		outbuf += strlen(outbuf);
		i += strlen(in);
	}
	*outbuf = '\0';
	if(tmp) free(tmp);
	if(unesc) free(unesc);
}

void CInternetUrl::Debug() {
	if( m_url==NULL ) {
		printf("URL pointer NULL\n");
		return;
	}
	if( m_url->url!=NULL ) {
		printf("url=%s\n", m_url->url );
	}
	if( m_url->protocol!=NULL ) {
		printf("protocol=%s\n", m_url->protocol );
	}
	if( m_url->hostname!=NULL ) {
		printf("hostname=%s\n", m_url->hostname );
	}
	printf("port=%d\n", m_url->port );
	if( m_url->file!=NULL ) {
		printf("file=%s\n", m_url->file );
	}
	if( m_url->username!=NULL ) {
		printf("username=%s\n", m_url->username );
	}
	if( m_url->password!=NULL ) {
		printf("password=%s\n", m_url->password );
	}
	map<string, string>::iterator it = m_mappar.begin();
	while ( it != m_mappar.end() )
	{
		printf("PAR: %s = %s\n", it->first.c_str(), it->second.c_str() );
		it ++;
	}
}

void CInternetUrl::SplitFilePar()
{
	if ( !m_url || !m_url->file ) return; 

	m_url->port = Port();

	char* p = strstr( m_url->file, "?" );
	if ( p )
	{
		*p = 0;
		p++;
		while ( p )
		{
			char* pPar = strstr( p, "&" );
			if ( pPar )
			{
				*pPar = 0;
				// p = "xxx = bbb"
				string name, par;
				char* pTemp = strstr( p, "=");
				if ( pTemp )
				{
					name.append( p, pTemp - p );
					par.append( pTemp + 1 );
				}
				if ( name.length() > 0 )
					m_mappar.insert( map<string, string>::value_type(name, par));
				p = ++pPar;
			}
			else
			{
				string name, par;
				char* pTemp = strstr( p, "=");
				if ( pTemp )
				{
					name.append( p, pTemp - p );
					par.append( pTemp + 1 );
				}
				if ( name.length() > 0 )
					m_mappar.insert( map<string, string>::value_type(name, par));
				break;
			}
		}
	}
	int i = 0;
}

string CInternetUrl::Url()
{
	if ( m_url && m_url->url )
		return m_url->url;

	return "";
}

string CInternetUrl::Protocol()
{
	if ( m_url && m_url->protocol )
		return m_url->protocol;

	return "";
}

string CInternetUrl::HostName()
{
	if ( m_url && m_url->hostname )
		return m_url->hostname;

	return "";
}

string CInternetUrl::File()
{
	if ( m_url && m_url->file )
		return m_url->file;

	return "";
}

unsigned int CInternetUrl::Port()
{
	if ( m_url && m_url->port )
		return m_url->port;

	if ( m_url && m_url->protocol )
	{
		if ( strnicmp( m_url->protocol, "http",4 ) == 0 )
			return 80;
		if ( strnicmp( m_url->protocol, "ftp",3 ) == 0 )
			return 21;
		if ( strnicmp( m_url->protocol, "P2P",3 ) == 0 )
			return 727;
	}

	return 0;
}

string CInternetUrl::UserName()
{
	if ( m_url && m_url->username )
		return m_url->username;

	return "";
}

string CInternetUrl::Password()
{
	if ( m_url && m_url->password )
		return m_url->password;

	return "";
}

string CInternetUrl::operator [](string name)
{
	map<string, string>::iterator it = m_mappar.find( name );
	if ( it != m_mappar.end() )
	{
		return it->second;
	}

	return "";
}



