#include "StdAfx.h"
#include ".\udpclient.h"
#include "P2PApp.h"

CUdpClient::CUdpClient(void) :
	m_bWantStop(false)
{
}

CUdpClient::~CUdpClient(void)
{
}

bool CUdpClient::Connect( const char* szServer, unsigned short usPort )
{
	m_addrServer.SetAddr( szServer, usPort );

	CKSockAddr addr("0.0.0.0", 0);
	if ( !m_udpSocket.Bind( addr ) )
		return false;

	return StartThread();
}

void CUdpClient::Release()
{
	m_bWantStop = true;
	WaitForStop();
}

void CUdpClient::Healthbeat()
{

}

bool CUdpClient::PushUdpCmd( CBaseCommand* pCmd )
{
	if ( pCmd )
	{
		m_listUdpCmd.Push( pCmd );
		return true;
	}
	else
		return false;
}

bool CUdpClient::PushUdpPeerCmd( P2P_COMMAND::CP2PBaseCommand* pCmd )
{
	if ( pCmd )
	{
		m_listP2PCmd.Push( pCmd );
		return true;
	}
	else
		return false;
}

void CUdpClient::ThreadProcMain(void)
{
	// 接到的都是客户端的东东
	char szRecvBuffer[UDP_MAX_BUFFER_LENGTH];
	int nRecvLength = 0;

	char szSendBuffer[UDP_MAX_BUFFER_LENGTH];
	int nSendLen = 0;

	CKSockAddr AddrClient;

	while ( !m_bWantStop )
	{
		while ( ( nRecvLength = m_udpSocket.RecvFrom( &AddrClient, szRecvBuffer, UDP_MAX_BUFFER_LENGTH, 0, 1000 )) > 0 )
		{
			if ( (SOCKERR_EOF != nRecvLength) && g_pNetwork )
			{
				g_pNetwork->OnRecvUdpData( szRecvBuffer, nRecvLength, AddrClient.GetHost().S_un.S_addr, AddrClient.GetPort() );
			}
			else
				break;
		}

		if ( g_pNetwork )
		{
			g_pNetwork->RequestP2PTask();

			// to server
			CBaseCommand* pSendCmd = m_listUdpCmd.Pop();
			while ( pSendCmd )
			{
				nSendLen = 0;
				pSendCmd->Create( szSendBuffer, nSendLen );
				m_udpSocket.SendTo( m_addrServer, szSendBuffer, nSendLen );
				g_pNetwork->m_transMonitorMgr.SendP2SUdpSendMonitor( szSendBuffer, nSendLen);
				delete pSendCmd;
				pSendCmd = m_listUdpCmd.Pop();
			}

			// to peer
			P2P_COMMAND::CP2PBaseCommand* pPeerCmd = m_listP2PCmd.Pop();
			while ( pPeerCmd )
			{
				/*CKLog::WriteLog( LOG_TYPE_DEBUG, "P2P:UDP:%s, Send: %s:%u", P2P_COMMAND::CP2PCommandQuery::CommandString( pPeerCmd->ID() ).c_str(), 
					inet_ntoa( *(in_addr *)(&pPeerCmd->m_dwIP) ), 
					pPeerCmd->m_usPort );*/

				AddrClient.SetAddr( *(in_addr *)(&pPeerCmd->m_dwIP), pPeerCmd->m_usPort );
				nSendLen = 0;
				pPeerCmd->Create( szSendBuffer, nSendLen );
				m_udpSocket.SendTo( AddrClient, szSendBuffer, nSendLen );
				g_pNetwork->m_transMonitorMgr.SendP2PUdpSendMonitor( szSendBuffer, nSendLen);
				delete pPeerCmd;
				pPeerCmd = m_listP2PCmd.Pop();
			}
		}
		// 发送所有 ...
	}
}


