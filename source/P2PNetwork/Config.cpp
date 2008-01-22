#include "StdAfx.h"
#include ".\config.h"

CConfig::CConfig(void) : 
m_usServerTcpPort(0),
m_usServerUdpPort(0)
{
}

CConfig::~CConfig(void)
{
}

bool CConfig::ReadAll( const char* szFilename )
{
	char szText[256];
	if ( ::GetPrivateProfileString( "CONFIG", "SERVERIP", "127.0.0.1", szText, sizeof(szText), szFilename ) <= 0 )
		return false;

	CKInetAddr inetAddr;
	inetAddr.SetHost( szText);
	if ( !inetAddr.IsValid())
		return false;

	m_sServerIP = ::inet_ntoa( inetAddr.GetHost());
	m_usServerTcpPort = ::GetPrivateProfileInt( "CONFIG", "TCPPORT", 8901, szFilename );
	m_usServerUdpPort = ::GetPrivateProfileInt( "CONFIG", "UDPPORT", 3344, szFilename );

	return true;
}
