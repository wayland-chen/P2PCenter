#include "StdAfx.h"
#include ".\p2papp.h"

CP2PApp::CP2PApp(void) :
	m_pClient(NULL),
	m_bConnected(false),
	m_bAuthSucceed(false),
	m_bWantStop(false),
	m_nUserID(0),
	m_nSessionID(0),
	m_nLocalIP(0),
	m_sLocalUdpPort(0),
	m_sLocalTcpPort(0)
{
}

CP2PApp::~CP2PApp(void)
{
}

bool CP2PApp::Connect( const char* szServerIP, unsigned short usPort )
{
	if ( (m_pClient = CreateTcpClient( this )) == NULL )
		return false;

	if ( !m_pClient->ConnectServer( szServerIP, usPort ) )
	{
		m_pClient->ReleaseConnection();
		m_pClient = NULL;
		return false;
	}

	if ( !m_evConnect.Wait( 5 * 1000 ) )
	{
		m_pClient->ReleaseConnection();
		m_pClient = NULL;
		return false;
	}

	if ( !m_bConnected )
	{
		m_pClient->ReleaseConnection();
		m_pClient = NULL;
		return false;
	}

	if ( !StartThread() ) return false;

	// 发送登录指令
	PushRequestCmd( P2S_COMMAND::CCommandFactory::CreateConnectCmd( 
		"anonymouse", "123456", m_nLocalIP, m_sLocalUdpPort, m_sLocalTcpPort ) );

	// 等待认证成功
#ifdef _DEBUG
	if ( !m_evAuth.Wait( 2 * 1000 ) )
#else
	if ( !m_evAuth.Wait( 10 * 1000 ) )
#endif
	{
		m_bConnected = false;
		m_bAuthSucceed = false;
		m_pClient->ReleaseConnection();
		m_pClient = NULL;
		return false;
	}

	return true;
}

void CP2PApp::Release()
{
	m_bWantStop = true;
	m_evTcpCmd.Set();
	WaitForStop();

	if ( m_pClient )
	{
		m_pClient->Disconnect();
		m_pClient->ReleaseConnection();
		m_pClient = NULL;
	}
}

void CP2PApp::OnReceivedData( const char* pData,int nLen)
{
	CKLog::WriteLog( LOG_TYPE_DEBUG, "TCP:len:%d,ID=%u,remark=> %s\n", nLen, CBaseCommand::getID( pData ), 
		P2S_COMMAND::CP2SCommandQuery::CommandString( CBaseCommand::getID( pData ) ).c_str() );

	CBaseCommand* pCmd = NULL;

	switch( CBaseCommand::getID( pData ) ) 
	{
	case P2S_CMD_CONNECT_RET:
		pCmd = new P2S_COMMAND::CCmdConnectRet();
		break;

	case P2S_CMD_GETGROUPS_RET:
		pCmd = new P2S_COMMAND::CCmdGetGroupsRet();
		break;

	case P2S_CMD_GETNODES_RET:
		pCmd = new P2S_COMMAND::CCmdGetNodesRet();
		break;

	case P2S_CMD_GETPEERS_RET:
		pCmd = new P2S_COMMAND::CCmdGetPeersRet();
		break;

	case P2S_CMD_PEERLOGIN:
		pCmd = new P2S_COMMAND::CCmdPeerLogin();
		break;

	case P2S_CMD_PEERLOGOUT:
		pCmd = new P2S_COMMAND::CCmdPeerLogout();
		break;

	case P2S_CMD_REQSHOOTTO_RET:
		pCmd = new P2S_COMMAND::CCmdReqShootToRet();
		break;

	case P2S_CMD_SHOOTARRIVED:
		pCmd = new P2S_COMMAND::CCmdShootArrived();
		break;

	case P2S_CMD_CLOSE_RET:
		pCmd = new P2S_COMMAND::CCmdCloseRet();
		break;

	// room begin
	case P2S_CMD_ROOM_CREATE_RET:
		pCmd = new P2S_COMMAND::CCmdRoomCreateRet();
		break;

	case P2S_CMD_ROOM_LOGIN_RET:
		pCmd = new P2S_COMMAND::CCmdRoomLoginRet();
		break;

	case P2S_CMD_ROOM_LOGIN_NTF:
		pCmd = new P2S_COMMAND::CCmdRoomLoginNtf();
		break;

	case P2S_CMD_ROOM_LOGOUT_RET:
		pCmd = new P2S_COMMAND::CCmdRoomLogoutRet();
		break;

	case P2S_CMD_ROOM_LOGOUT_NTF:
		pCmd = new P2S_COMMAND::CCmdRoomLogoutNtf();
		break;

	case P2S_CMD_ROOM_DELETE_RET:
		pCmd = new P2S_COMMAND::CCmdRoomDeleteRet();
		break;

	case P2S_CMD_ROOM_MODIFY_RET:
		pCmd = new P2S_COMMAND::CCmdRoomModifyRet();
		break;

	case P2S_CMD_ROOM_POST_DATA_RET:
		pCmd = new P2S_COMMAND::CCmdRoomPostDataRet();
		break;

	case P2S_CMD_ROOM_POST_DATA_BRD:
		pCmd = new P2S_COMMAND::CCmdRoomPostDataBrd();
		break;

	case P2S_CMD_ROOM_GET_DATA_RET:
		pCmd = new P2S_COMMAND::CCmdRoomGetDataRet();
		break;

	case P2S_CMD_ROOM_CLOSE_RET:
		pCmd = new P2S_COMMAND::CCmdRoomCloseRet();
		break;
	// room end

	// monitor begin
	case P2S_CMD_MONITOR_TRANSDATA:
		pCmd = new P2S_COMMAND::CCmdMonitorTransdata();
	// monitor end

	default:
		break;
	}

	if ( pCmd )
	{
		try
		{
			int nLength = pCmd->Parse( (char *)pData, nLen );
			assert( nLen == nLength );
			m_transMonitorMgr.SendP2STcpRecvMonitor( pData, nLen);
			TransCommand( pCmd );
		}
		catch ( ... ) 
		{
		}

		delete pCmd;
	}
}

void CP2PApp::OnConnected(void)
{
	m_bConnected = true;
	m_evConnect.Set();
}

void CP2PApp::OnDisconnected(void)
{
	printf("Connect disconnected.\n");
	m_bConnected = false;
	m_evConnect.Set();
}

void CP2PApp::OnConnectFailed(void)
{
	printf("Connect failed.\n");
	m_bConnected = false;
	m_evConnect.Set();
}

void CP2PApp::ThreadProcMain(void)
{
	char szSendBuffer[TCP_MAX_BUFFER_LENGTH];
	int  nSendLen = 0;

	while ( !m_bWantStop )
	{
		static DWORD dwLastTcpHeartbeatTime = GetTickCount();

		CBaseCommand* pSendCmd = m_listTcpCmd.Pop();	// 非连接信令要求已认证后再行发送

		while ( pSendCmd )
		{
			nSendLen = 0;
			pSendCmd->Create( szSendBuffer, nSendLen );
			if ( m_pClient )
			{
				if ( m_pClient->SendData( szSendBuffer, nSendLen ) )
				{
					m_transMonitorMgr.SendP2STcpSendMonitor( szSendBuffer, nSendLen);
				}
			}
			delete pSendCmd;
			dwLastTcpHeartbeatTime = GetTickCount();
			pSendCmd = m_listTcpCmd.Pop();
		}

		// 发送TCP心跳
		if ( m_bAuthSucceed && (dwLastTcpHeartbeatTime + TCP_HEARTBEAT_INTERVAL) < GetTickCount() )
		{
			dwLastTcpHeartbeatTime = GetTickCount();
			//PushRequestCmd( P2S_COMMAND::CCommandFactory::CreateTcpHeartbeatCmd( UserID(), SessionID() , "" ));
		}

		OnThreadIdle();

		m_evTcpCmd.Wait( 1 );
	}
}

void CP2PApp::TransCommand( CBaseCommand* pCmd )
{
}

bool CP2PApp::PushRequestCmd( CBaseCommand* pCmd )
{
	if ( !m_bConnected )
		return false;

	if ( pCmd )
	{
		m_listTcpCmd.Push( pCmd );
		m_evTcpCmd.Set();
		return true;
	}
	else
		return false;
}

int CP2PApp::UserID()
{
	return m_nUserID;
}

int CP2PApp::SessionID()
{
	return m_nSessionID;
}

int CP2PApp::GetRandSession()
{
	srand(GetTickCount());
	return rand() * GetTickCount();
}

int CP2PApp::CalcCheckSum( const char* pBuffer, int nLen )
{
	int nRet = 0x8f46ccdd;
	for ( unsigned int i = 0; i < nLen / sizeof(int); i++ )
	{
		nRet ^= *(int *)(pBuffer + i * sizeof(int));
	}

	return nRet;
}

