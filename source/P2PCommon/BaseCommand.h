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

#include "p2sdefine.h"
#include "p2pdefine.h"
#include <string>
#include <list>
#include <map>
using namespace std;

//-----------------------------------------------------------------------
class CBaseCommand
{
public:
	CBaseCommand( unsigned int nCommandID );
	virtual ~CBaseCommand(void);

	static unsigned int getID( const char* pBuffer );

	virtual int Create( char* pBuffer, int& nLen ); 
	virtual int Parse( char* pBuffer, int nLen );

	unsigned int ID();

public:
	// TCP MOUDLE 返回的sessionID,用于服务器端,客户端接收CMD无效
	unsigned long m_ulSocketID;

protected:
	unsigned int m_nCommandID;
	unsigned int m_nVersion;

protected:
	//------------tools function-------------------//
	inline char* pop_int8( char *buf, char &v);
	inline char* pop_int16( char *buf, short &v);
	inline char* pop_int32( char *buf, int &v);
	inline char* pop_str( char *buf, string &v);
	inline char* pop_buf( char *buf, char* data, short &len);
	inline char* pop_timestamp( char *buf, unsigned long &v);

	inline char* push_int8_2( char *buf, const char& s, int &len);
	inline char* push_int16_2( char *buf, const short& s, int &len);
	inline char* push_int32_2( char *buf, const int& s, int &len);
	inline char* push_str_2( char *buf, const string& s, int &len);
	inline char* push_buf_2( char *buf, const char* data, const short datalen, int &len);
	inline char* push_timestamp_2( char *buf, const unsigned long& s, int &len);
};

inline 
char* CBaseCommand::pop_int8( char *buf, char &v)
{
	v =*(reinterpret_cast<char*>(buf));
	return (buf+sizeof( char));
}

inline 
char* CBaseCommand::pop_int16( char *buf, short &v)
{
	v =*(reinterpret_cast<short*>(buf));
	return (buf+sizeof( short));
}

inline 
char* CBaseCommand::pop_int32( char *buf, int &v)
{
	v =*(reinterpret_cast<int*>(buf));
	return (buf+sizeof( int));
}

inline 
char* CBaseCommand::pop_timestamp( char *buf, unsigned long &v)
{
	v =*(reinterpret_cast<unsigned long*>(buf));
	return (buf+sizeof( unsigned long));
}

inline 
char* CBaseCommand::pop_str( char *buf, string &v)
{
	short strlen =*(reinterpret_cast<short*>(buf));
	buf = buf + sizeof( short);

	v.clear();
	v.append( buf, strlen);

	return (buf+strlen);
}

inline 
char* CBaseCommand::pop_buf( char *buf, char* data, short &len)
{
	len =*(reinterpret_cast<short*>(buf));
	buf = buf + sizeof( short);

	memcpy(data, buf, len);

	return (buf+len);
}

inline 
char* CBaseCommand::push_int8_2( char *buf, const char& s, int &len)
{
	*((char*)buf) =s;
	len += sizeof( char);

	return ( buf+sizeof( char));
}

inline 
char* CBaseCommand::push_int16_2( char *buf, const short& s, int &len)
{
	*((short*)buf) =s;
	len += sizeof( short);

	return ( buf+sizeof( short));
}

inline 
char* CBaseCommand::push_int32_2( char *buf, const int& s, int &len)
{
	*((int*)buf) =s;
	len += sizeof( int);

	return ( buf+sizeof( int));
}

inline 
char* CBaseCommand::push_timestamp_2( char *buf, const unsigned long& s, int &len)
{
	*((unsigned long*)buf) =s;
	len += sizeof( unsigned long);

	return ( buf+sizeof( unsigned long));
}

inline 
char* CBaseCommand::push_str_2( char *buf, const string& s, int &len)
{
	*((short*)buf) =(short)s.size();
	buf =buf +sizeof( short);
	len += sizeof( short);

	memcpy( buf, s.c_str(), s.size());
	len += (int)s.size();

	return ( buf + (int)s.size() );
}

inline 
char* CBaseCommand::push_buf_2( char *buf, const char* data, const short datalen, int &len)
{
	*((short*)buf) = datalen;
	buf =buf +sizeof( short);
	len += sizeof( short);

	memcpy( buf, data, datalen);
	len += (int)datalen;

	return ( buf + (int)datalen );
}
//------------------------------------------------------------------------

