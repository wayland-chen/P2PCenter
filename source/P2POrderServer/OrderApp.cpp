#include "StdAfx.h"
#include ".\orderapp.h"
#include "misc/IniFile.h"
#include "log/KLog.h"

COrderApp::COrderApp(void) :
m_pTcpServer(NULL),
m_bWantStop(false)
{
	printf( "Server Current version is %s\n", SERVER_VERSION );
}

COrderApp::~COrderApp(void)
{
	if ( m_pTcpServer )
	{
		m_pTcpServer->Destroy();
		m_pTcpServer = NULL;
	}
}

bool COrderApp::Connect( unsigned short usPort )
{
#ifdef _DEBUG
	m_pTcpServer = CreateTcpServer( this, 20 );
#else
	m_pTcpServer = CreateTcpServer( this, 2000 );
#endif

	if ( ! m_pTcpServer )
		return false;

	if ( !m_pTcpServer->Create( usPort ) )
		return false;

	printf( "listen tcp port %d succeed\n", usPort );

	return StartThread();
}

void COrderApp::Release(void)
{
	m_bWantStop = true;
	m_clientMgr.m_evSendCmd.Set();
	WaitForStop();

	if ( m_pTcpServer )
	{
		m_pTcpServer->Destroy();
		m_pTcpServer = NULL;
	}
}

void COrderApp::OnAccept(unsigned int nLinkNo)
{
	printf( "Accept new link session id: %d\n", nLinkNo);

	CClient* pClient = new CClient();
	if ( pClient )
	{
		pClient->m_ulSocketID = nLinkNo;
		m_clientMgr.Insert( nLinkNo, pClient );
		printf( "peer count = %d\n", m_clientMgr.GetCount() );
	}
}

void COrderApp::OnClosed(unsigned int nLinkNo)
{
	printf( "Close link session id: %d\n", nLinkNo);

	m_clientMgr.LinkClose( nLinkNo );
}

void COrderApp::OnRecvData(const char* pData, int nLen, unsigned int nLinkNo)
{
	CKLog::WriteLog( LOG_TYPE_DEBUG, "TCP:session:%d,datalen: %d, remark=> %s client count=> %u\n", 
		nLinkNo, nLen, 
		P2S_COMMAND::CP2SCommandQuery::CommandString( CBaseCommand::getID( pData ) ).c_str(), 
		m_clientMgr.GetCount() );

	if ( nLen <= 0)
		return;

	CBaseCommand* pCmd = NULL;

	switch( CBaseCommand::getID( pData ) ) 
	{
	case P2S_CMD_CONNECT:
		pCmd = new P2S_COMMAND::CCmdConnect();
		break;

	case P2S_CMD_GETGROUPS:
		pCmd = new P2S_COMMAND::CCmdGetGroups();
		break;

	case P2S_CMD_GETNODES:
		pCmd = new P2S_COMMAND::CCmdGetNodes();
		break;

	case P2S_CMD_LOGIN_CHANNEL:
		pCmd = new P2S_COMMAND::CCmdLoginChannel();
		break;

	case P2S_CMD_LOGOUT_CHANNEL:
		pCmd = new P2S_COMMAND::CCmdLogoutChannel();
		break;

	case P2S_CMD_GETPEERS:
		pCmd = new P2S_COMMAND::CCmdGetPeers();
		break;

	case P2S_CMD_REQSHOOTTO:
		pCmd = new P2S_COMMAND::CCmdReqShootTo();
		break;

	case P2S_CMD_CLOSE:
		pCmd = new P2S_COMMAND::CCmdClose();
		break;

	default:
		break;
	}

	if ( pCmd )
	{
		try
		{
			int nLen2 = pCmd->Parse( (char *)pData, nLen );
			assert( nLen2 == nLen );
			pCmd->m_ulSocketID = nLinkNo;
			m_clientMgr.TransCommand( pCmd );
		}
		catch ( ... ) 
		{
		}

		delete pCmd;
	}
}

void COrderApp::OnRecvUdpData( char* pData, int nLen, unsigned long ulIP, unsigned short usPort )
{
	CKLog::WriteLog( LOG_TYPE_DEBUG, "UDP:dlen:%d,ip:%s:%d => %s,ccount:%u\n", 
		nLen, inet_ntoa( *(in_addr *)(&ulIP) ), usPort, P2S_COMMAND::CP2SCommandQuery::CommandString(CBaseCommand::getID( pData )).c_str(), m_clientMgr.GetCount() );

	CBaseCommand* pCmd = NULL;

	switch( CBaseCommand::getID( pData ) ) 
	{
	case P2S_CMD_SHOOTHOLE:
		pCmd = new P2S_COMMAND::CCmdShootHole();
		break;

	case P2S_CMD_UDP_HEARTBEAT:
		pCmd = new P2S_COMMAND::CCmdUdpHeartbeat();
		break;

	default:
		break;
	}

	if ( pCmd )
	{
		try
		{
			int nLength = pCmd->Parse( pData, nLen );
			assert( nLen == nLength );
			pCmd->m_ulSocketID = -1;
			m_clientMgr.TransUdpCommand( pCmd, ulIP, usPort );
		}
		catch ( ... ) 
		{
		}

		delete pCmd;
	}
}

void COrderApp::ThreadProcMain(void)
{
	char szSendBuffer[TCP_MAX_BUFFER_LENGTH];
	int nSendLength = 0;

	while ( !m_bWantStop )
	{
		CBaseCommand* pCmd = m_clientMgr.m_listSendCmd.Pop();
		while( pCmd )
		{
			nSendLength = 0;
			if ( pCmd->Create( szSendBuffer, nSendLength ) > 0 )
			{
				assert( pCmd->m_ulSocketID >= 0 );
				m_pTcpServer->SendData( szSendBuffer, nSendLength, pCmd->m_ulSocketID );
			}

			delete pCmd;
			pCmd = m_clientMgr.m_listSendCmd.Pop();
		}

		m_clientMgr.m_evSendCmd.Wait( 1000 );

		m_clientMgr.UpdateChannelTick();
	}
}

void COrderApp::TransInputCommand( string sCmd)
{
	static map<int, int> mapMonitorUsers;

	if ( sCmd == "monitor")
	{
		m_clientMgr.CancelMonitor( mapMonitorUsers);
		mapMonitorUsers.clear();

		CIniFile ini;
		if ( -1 == ini.LoadFromFile("Monitor.ini") )
			return;

		int nCount = atoi(ini.GetString( "MonitorID", "Count").GetBuffer());
		for ( int i = 0; i < nCount; i ++)
		{
			CString szKey;
			szKey.Format("User%d", i);
			int nUserID = atoi(ini.GetString( "MonitorID", szKey.GetBuffer()).GetBuffer());
			if ( nUserID > 0) mapMonitorUsers[nUserID] = nUserID;
		}

		if ( mapMonitorUsers.size() > 0)
		{
			m_clientMgr.MonitorUser( mapMonitorUsers);
			CKLog::WriteLog( LOG_TYPE_FORCE, "Monitor User Count %d", mapMonitorUsers.size() );
		}
	}

	if ( sCmd == "cancelmonitor")
	{
		m_clientMgr.CancelMonitor( mapMonitorUsers);
		mapMonitorUsers.clear();
		CKLog::WriteLog( LOG_TYPE_FORCE, "Cancel Monitor User Count %d", mapMonitorUsers.size() );
	}
}


