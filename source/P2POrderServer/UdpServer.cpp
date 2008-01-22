#include "StdAfx.h"
#include ".\udpserver.h"

CUdpServer::CUdpServer(void) :
m_bWantStop(false),
m_pApp(NULL)
{
}

CUdpServer::~CUdpServer(void)
{
}

void CUdpServer::SetApp( COrderApp* pApp )
{
	m_pApp = pApp;
}

bool CUdpServer::Connect( unsigned short usPort )
{
	CKSockAddr addr( "0.0.0.0", usPort );

	if ( !m_udpSocket.Bind( addr ))
		return false;

	printf( "bind udp port %d succeed\n", usPort );

	return StartThread();
}

void CUdpServer::Release(void)
{
	m_bWantStop = true;
	m_evUdpSend.Set();
	WaitForStop();

	m_udpSocket.Close();
}

void CUdpServer::ThreadProcMain(void)
{
	char szRecvBuffer[UDP_MAX_BUFFER_LENGTH];
	int nRecvLength = 0;

	CKSockAddr AddrClient;
	while ( !m_bWantStop )
	{
		if ( ( nRecvLength = m_udpSocket.RecvFrom( &AddrClient, szRecvBuffer, UDP_MAX_BUFFER_LENGTH, 1, 1 )) > 0 )
		{
			if ( (SOCKERR_EOF != nRecvLength) && m_pApp )
			{
				m_pApp->OnRecvUdpData( szRecvBuffer, nRecvLength, AddrClient.GetHost().S_un.S_addr, AddrClient.GetPort() );
			}
		}
	}
}
