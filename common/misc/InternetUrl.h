/********************************************************************
	created:	2008/01/11
	created:	11:1:2008   15:49
	author:		Kevin
	
	purpose:	

	email:		kan8888.com@163.com
	msn:		fuwenke@gmail.com
	QQ:		799383417
	site:		opensource.kan8888.com
*********************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

struct URL_t{
	char *url;
	char *protocol;
	char *hostname;
	char *file;
	unsigned int port;
	char *username;
	char *password;
};

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

class CInternetUrl
{
public:
	CInternetUrl( const char* url );
	virtual ~CInternetUrl(void);

	void Debug();

	string Url();
	string Protocol();
	string HostName();
	string File();
	unsigned int Port();
	string UserName();
	string Password();

	string operator [](string name);
	static void	url_escape_string_part(char *outbuf, const char *inbuf);

private:
	void url_free();
	void url_unescape_string(char *outbuf, const char *inbuf);
	void url_escape_string(char *outbuf, const char *inbuf);
	void url_escape_string_bak(char *outbuf, const char *inbuf);

	void SplitFilePar();

private:
	URL_t* m_url;
	map<string, string> m_mappar;
};
