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

// P2PNetwork.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include ".\p2pnetwork.h"
#include "misc/InternetUrl.h"

CP2PNetwork* g_pNetwork = NULL;

IP2PNetwork* CreateP2PNetwork()
{
#if defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	CKLog::SetDebugType( LOG_TYPE_DEBUG);
#else
	CKLog::SetDebugType( LOG_TYPE_WARNING);
#endif

	g_pNetwork =  new CP2PNetwork();
	return g_pNetwork;
}

bool InitP2PSocket()
{
	return InitSocket();
}

void UninitP2PStocket()
{
	UninitSocket();
}

CP2PNetwork::CP2PNetwork(void) :
	m_pNotify(NULL)
{
	printf( "Client Current version is %s\n", CLIENT_VERSION );
}

CP2PNetwork::~CP2PNetwork(void)
{
}

CString GetModuleFolderName()
{
	TCHAR szFolderName[MAX_PATH];
	GetModuleFileName(_AtlBaseModule.m_hInstResource, szFolderName, sizeof(szFolderName));
	(_tcsrchr(szFolderName,'\\'))[1] = 0;
	return szFolderName;
}

bool CP2PNetwork::InitModule( IP2PNetworkNotify* pNotify )
{
#ifndef _DEBUG
	//CKLog::SetDebugType( LOG_TYPE_WARNING);
#endif

	CKLog::WriteLog( LOG_TYPE_FORCE, "Init P2PNetwork pNotify=%d", pNotify);

	g_pNetwork = this;

	m_pNotify = pNotify;

	if ( !m_Config.ReadAll( GetModuleFolderName() + "P2PNetwork.ini") )
	{
		printf( "P2PNetwork.ini is not exist.\n" );
		return false;
	}

	if ( !m_udpClient.Connect( m_Config.m_sServerIP.c_str(), m_Config.m_usServerUdpPort ) )
		return false;

	return CP2PApp::Connect( m_Config.m_sServerIP.c_str(), m_Config.m_usServerTcpPort );
}

void CP2PNetwork::ReleaseModule()
{
	CKLog::WriteLog( LOG_TYPE_FORCE, "Release P2PNetwork. Channel Count=%d", m_channelMgr.GetCount());
	m_channelMgr.Flush();
	g_pNetwork = NULL;
	m_udpClient.Release();
	CP2PApp::Release();

	delete this;
}

DWORD CP2PNetwork::OpenSource( const char* szUrl, bool bSource )
{
	CInternetUrl url( szUrl );
	//url.Debug();

	if ( bSource )
	{
		if ( url["filepath"].length() == 0 || 
			url["filehash"].length() != 32 || 
			url["filesize"].length() == 0 || 
			url["nodename"].length() == 0 )
		{
			return 0;
		}
	}
	else
	{
		if ( url["filehash"].length() != 32 || 
			url["filesize"].length() == 0 || 
			url["nodename"].length() == 0 )
		{
			return 0;
		}
	}
 
	{
		CKAutoLock l( m_channelMgr.GetCritSec() );
		delete m_channelMgr.RemoveChannelByID( url["filehash"].c_str() );
	}

	static DWORD dwCount = 0x0001000;
	CChannel* pChannel = new CChannel();
	if ( pChannel )
	{
		if ( !pChannel->OpenSource( url["nodename"].c_str(), url["filepath"].c_str(), 
			url["filename"].c_str(), url["filehash"].c_str(), _atoi64(url["filesize"].c_str()), bSource ) )
			return 0;
		m_channelMgr.Insert( ++dwCount, pChannel );

		CKLog::WriteLog( LOG_TYPE_FORCE, "Open Channel %u, %s;\n\t%s.", dwCount, url["filehash"].c_str(), url["filename"].c_str());
		return dwCount;
	}

	return 0;
}

void CP2PNetwork::CloseSource( DWORD dwChannelID )
{
	m_channelMgr.Remove( dwChannelID );
	CKLog::WriteLog( LOG_TYPE_FORCE, "Close Channel %u", dwChannelID);
}

void CP2PNetwork::RequestSegment( DWORD dwChannelID, DWORD dwStartPos, DWORD dwLength )
{
	m_channelMgr.RequestSegment( dwChannelID, dwStartPos, dwLength );
}

bool CP2PNetwork::ReadSegment( DWORD dwChannelID, DWORD dwStartPos, char* pBuffer, DWORD& dwLength )
{
	DWORD dwNeedLength = dwLength;

	if ( m_channelMgr.ReadSegment( dwChannelID, dwStartPos, pBuffer, dwLength) )
	{
		//DPRINTF( "------------>> succeed startpos = %u, dwLength = %u, alreadyread = %u\n", dwStartPos, dwNeedLength, dwLength );
		return true;
	}
	//else
	//	DPRINTF( "ReadSegment failed startpos = %u, dwLength = %u\n", dwStartPos, dwNeedLength );

	return false;
}

LONGLONG CP2PNetwork::GetChannelSize( DWORD dwChannelID )
{
	return m_channelMgr.GetChannelSize( dwChannelID );
}

bool CP2PNetwork::GetChannelMonitorInfo( DWORD dwChannelID, stMonitorInfo& monInfo)
{
	return m_channelMgr.GetChannelMonitorInfo( dwChannelID, monInfo);
}

bool CP2PNetwork::GetChannelMonitorInfo( const char* szChannelHash, stMonitorInfo& monInfo)
{
	return m_channelMgr.GetChannelMonitorInfo( szChannelHash, monInfo);
}

bool CP2PNetwork::GetAllChannelID( list<DWORD>& listChannels)
{
	return m_channelMgr.GetAllChannelID( listChannels);
}

void CP2PNetwork::OnConnected(void)
{
	CP2PApp::OnConnected();

	if ( m_pNotify)
		m_pNotify->OnConnect();
}

void CP2PNetwork::OnDisconnected(void)
{
	CP2PApp::OnDisconnected();

	if ( m_pNotify)
		m_pNotify->OnDisConnect();
}

void CP2PNetwork::OnThreadIdle()
{
	// 发送 UDP 心跳
	static DWORD dwLastUdpHeartbeatTime = GetTickCount();
	if ( m_bAuthSucceed && (dwLastUdpHeartbeatTime + UDP_HEARTBEAT_INTERVAL) < GetTickCount() )
	{
		dwLastUdpHeartbeatTime = GetTickCount();
		m_udpClient.PushUdpCmd( P2S_COMMAND::CCommandFactory::CreateUdpHeartbeatCmd( UserID(), SessionID() , "" ));
	}
}

void CP2PNetwork::TransCommand( CBaseCommand* pCmd )
{
	switch( pCmd->ID() ) 
	{
	case P2S_CMD_CONNECT_RET:
		{
			P2S_COMMAND::CCmdConnectRet* pCmdConnectRet = reinterpret_cast<P2S_COMMAND::CCmdConnectRet *>(pCmd);
			m_nUserID = pCmdConnectRet->m_nUserID;
			m_nSessionID = pCmdConnectRet->m_nSessionID;
			m_bAuthSucceed = true;
			m_evAuth.Set();
			// send UDP Hole Packet
			m_udpClient.PushUdpCmd( P2S_COMMAND::CCommandFactory::CreateUdpHeartbeatCmd( g_pNetwork->UserID(), g_pNetwork->SessionID() , "" ));
		}
		break;

	case P2S_CMD_GETGROUPS_RET:
		{
			P2S_COMMAND::CCmdGetGroupsRet* pCmdGetGroupsRet = reinterpret_cast<P2S_COMMAND::CCmdGetGroupsRet *>(pCmd);
		}
		break;

	case P2S_CMD_GETNODES_RET:
		{
			P2S_COMMAND::CCmdGetNodesRet* pCmdGetNodesRet = reinterpret_cast<P2S_COMMAND::CCmdGetNodesRet *>(pCmd);
		}
		break;

	case P2S_CMD_GETPEERS_RET:
		{
			P2S_COMMAND::CCmdGetPeersRet* pCmdGetPeersRet = reinterpret_cast<P2S_COMMAND::CCmdGetPeersRet *>(pCmd);
			CKAutoLock l( m_channelMgr.GetCritSec() );
			CChannel* pChannel = m_channelMgr.FindChannelByID( pCmdGetPeersRet->m_sChannelID );
			assert( pChannel );
			if ( pChannel )
			{
				list<PEERINFO>::iterator it = pCmdGetPeersRet->m_listPeers.begin();
				while ( it != pCmdGetPeersRet->m_listPeers.end() )
				{
					PEERINFO& peerinfo = *it;
					pChannel->AddPeer( peerinfo );
					it++;
				}

				CKLog::WriteLog( LOG_TYPE_FORCE, 
					"Get Peers Return, Peer Count: %u;\n\t%s.", 
					pCmdGetPeersRet->m_listPeers.size(), pChannel->ChannelName().c_str());
			}
		}
		break;

	case P2S_CMD_PEERLOGIN:
		{
			P2S_COMMAND::CCmdPeerLogin* pCmdPeerLogin = reinterpret_cast<P2S_COMMAND::CCmdPeerLogin *>(pCmd);
			CKAutoLock l( m_channelMgr.GetCritSec() );
			CChannel* pChannel = m_channelMgr.FindChannelByID( pCmdPeerLogin->m_sChannelID );
			assert( pChannel );
			if ( pChannel )
			{
				list<PEERINFO>::iterator it = pCmdPeerLogin->m_listPeers.begin();
				while ( it != pCmdPeerLogin->m_listPeers.end() )
				{
					PEERINFO& peerinfo = *it;
					pChannel->AddPeer( peerinfo );
					CKLog::WriteLog( LOG_TYPE_DEBUG, 
						"%s Add Peer UserID=%d, UDPIP=%s, UDPPort=%d, TCPPort=%d", 
						pCmdPeerLogin->m_sChannelID.c_str(), peerinfo.ulID, 
						inet_ntoa( *(in_addr *)(&peerinfo.ulExternalIP)), 
						peerinfo.usExternalUDPPort, peerinfo.usExternalTCPPort);
					it++;
				}
			}
		}
		break;

	case P2S_CMD_PEERLOGOUT:
		{
			P2S_COMMAND::CCmdPeerLogout* pCmdPeerLogout = reinterpret_cast<P2S_COMMAND::CCmdPeerLogout *>(pCmd);
			CKAutoLock l( m_channelMgr.GetCritSec() );
			CChannel* pChannel = m_channelMgr.FindChannelByID( pCmdPeerLogout->m_sChannelID );
			assert( pChannel );
			if ( pChannel )
			{
				list<PEERINFO>::iterator it = pCmdPeerLogout->m_listPeers.begin();
				while ( it != pCmdPeerLogout->m_listPeers.end() )
				{
					PEERINFO& peerinfo = *it;
					pChannel->DeletePeer( peerinfo.ulID );
					it++;
				}
			}
		}
		break;

	case P2S_CMD_REQSHOOTTO_RET:
		{
			P2S_COMMAND::CCmdReqShootToRet* pCmdReqShootToRet = reinterpret_cast<P2S_COMMAND::CCmdReqShootToRet *>(pCmd);
		}
		break;

	case P2S_CMD_SHOOTARRIVED:
		{
			P2S_COMMAND::CCmdShootArrived* pCmdShootArrived = reinterpret_cast<P2S_COMMAND::CCmdShootArrived *>(pCmd);
			// 查询是否已与此用户连接,是,返回,否,发起握手连接
			// 先忽略是否连接判断
			CKLog::WriteLog( LOG_TYPE_DEBUG, "--------ShootArrived ip: %s:%u\n",
				inet_ntoa( *(in_addr *)(&pCmdShootArrived->m_peerinfoSource.ulExternalIP) ), pCmdShootArrived->m_peerinfoSource.usExternalUDPPort);

			P2P_COMMAND::CP2PBaseCommand* pCmd = P2P_COMMAND::CP2PCmdFactory::CreateHandshakeCmd( g_pNetwork->UserID(), g_pNetwork->GetRandSession() );
			if ( pCmd )
			{
				pCmd->m_dwIP = pCmdShootArrived->m_peerinfoSource.ulExternalIP;
				pCmd->m_usPort = pCmdShootArrived->m_peerinfoSource.usExternalUDPPort;
				g_pNetwork->PushUdpPeerCmd( pCmd );
			}
		}
		break;

	case P2S_CMD_CLOSE_RET:
		{
			P2S_COMMAND::CCmdCloseRet* pCmdCloseRet = reinterpret_cast<P2S_COMMAND::CCmdCloseRet *>(pCmd);
		}
		break;

		// room begin
	case P2S_CMD_ROOM_CREATE_RET:
		{
			P2S_COMMAND::CCmdRoomCreateRet* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdRoomCreateRet *>(pCmd);
		}
		break;

	case P2S_CMD_ROOM_LOGIN_RET:
		{
			P2S_COMMAND::CCmdRoomLoginRet* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdRoomLoginRet *>(pCmd);
		}
		break;

	case P2S_CMD_ROOM_LOGIN_NTF:
		{
			P2S_COMMAND::CCmdRoomLoginNtf* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdRoomLoginNtf *>(pCmd);
		}
		break;

	case P2S_CMD_ROOM_LOGOUT_RET:
		{
			P2S_COMMAND::CCmdRoomLogoutRet* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdRoomLogoutRet *>(pCmd);
		}
		break;

	case P2S_CMD_ROOM_LOGOUT_NTF:
		{
			P2S_COMMAND::CCmdRoomLogoutNtf* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdRoomLogoutNtf *>(pCmd);
		}
		break;

	case P2S_CMD_ROOM_DELETE_RET:
		{
			P2S_COMMAND::CCmdRoomDeleteRet* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdRoomDeleteRet *>(pCmd);
		}
		break;

	case P2S_CMD_ROOM_MODIFY_RET:
		{
			P2S_COMMAND::CCmdRoomModifyRet* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdRoomModifyRet *>(pCmd);
		}
		break;

	case P2S_CMD_ROOM_POST_DATA_RET:
		{
			P2S_COMMAND::CCmdRoomPostDataRet* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdRoomPostDataRet *>(pCmd);
		}
		break;

	case P2S_CMD_ROOM_POST_DATA_BRD:
		{
			P2S_COMMAND::CCmdRoomPostDataBrd* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdRoomPostDataBrd *>(pCmd);
		}
		break;

	case P2S_CMD_ROOM_GET_DATA_RET:
		{
			P2S_COMMAND::CCmdRoomGetDataRet* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdRoomGetDataRet *>(pCmd);
		}
		break;

	case P2S_CMD_ROOM_CLOSE_RET:
		{
			P2S_COMMAND::CCmdRoomCloseRet* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdRoomCloseRet *>(pCmd);
		}
		break;
		// room end

		// monitor begin
	case P2S_CMD_MONITOR_TRANSDATA:
		{
			P2S_COMMAND::CCmdMonitorTransdata* pCmdRet = reinterpret_cast<P2S_COMMAND::CCmdMonitorTransdata *>(pCmd);
			if ( 1 == pCmdRet->m_cRemoveAllFlag)
				m_transMonitorMgr.RemoveAll();
			else
			{
				CTransMonitor* pMonitor = new CTransMonitor();
				if ( pMonitor)
				{
					pMonitor->m_cTransType = pCmdRet->m_cTransType;
					pMonitor->m_cMonitorType = pCmdRet->m_cMonitorType;
					pMonitor->m_cTransWeight = pCmdRet->m_cTransWeight;
					pMonitor->m_cSendInterval = pCmdRet->m_cSendInterval;
					pMonitor->m_nDstIP = pCmdRet->m_nDstIP;
					pMonitor->m_usDstPort = pCmdRet->m_usDstPort;
					m_transMonitorMgr.Push( pMonitor);
				}
			}
		}
		// monitor end

	default:
		break;
	}
}

bool CP2PNetwork::PushUdpPeerCmd( P2P_COMMAND::CP2PBaseCommand* pCmd )
{
	if ( !m_bAuthSucceed )
		return false;

	return m_udpClient.PushUdpPeerCmd( pCmd );
}

void CP2PNetwork::OnRecvUdpData( char* pData, int nLen, unsigned long ulIP, unsigned short usPort )
{
	//printf("Receive udp data len = %d, ip:%s, port = %d\n", nLen, inet_ntoa( *(in_addr *)(&ulIP) ), usPort );

	P2P_COMMAND::CP2PBaseCommand* pCmd = NULL;

	switch( CBaseCommand::getID( pData ) ) 
	{
		//------------------------------------------------------------------------
	case P2P_CMD_HANDSHAKE:
		pCmd = new P2P_COMMAND::CCmdHandshake();
		break;

	case P2P_CMD_GETBLOCK_AREA:
		pCmd = new P2P_COMMAND::CCmdGetBlockArea();
		break;

	case P2P_CMD_GETSEGMENT:
		pCmd = new P2P_COMMAND::CCmdGetSegment();
		break;

	case P2P_CMD_STAT_UPLOAD:
		pCmd = new P2P_COMMAND::CCmdStatUpload();
		break;

	case P2P_CMD_CANCELSEGMENT:
		pCmd = new P2P_COMMAND::CCmdCancelSegment();
		break;

	case P2P_CMD_EXIT:
		pCmd = new P2P_COMMAND::CCmdExit();
		break;

	case P2P_CMD_HEARTBEAT:
		pCmd = new P2P_COMMAND::CCmdHeartbeat();
		break;
		//------------------------------------------------------------------------
	case P2P_CMD_HANDSHAKE_RET:
		pCmd = new P2P_COMMAND::CCmdHandshakeRet();
		break;

	case P2P_CMD_GETBLOCK_AREA_RET:
		pCmd = new P2P_COMMAND::CCmdGetBlockAreaRet();
		break;

	case P2P_CMD_GETSEGMENT_RET:
		pCmd = new P2P_COMMAND::CCmdGetSegmentRet();
		break;

	case P2P_CMD_STAT_UPLOAD_RET:
		pCmd = new P2P_COMMAND::CCmdStatUploadRet();
		break;

	case P2P_CMD_CANCELSEGMENT_RET:
		pCmd = new P2P_COMMAND::CCmdCancelSegmentRet();
		// ------------------------------------------------------------------------

	default:
		break;
	}

	if ( pCmd )
	{
		try
		{
			int nLength = pCmd->Parse( pData, nLen );
			assert( nLen == nLength );
			pCmd->m_dwIP = ulIP;
			pCmd->m_usPort = usPort;
			m_transMonitorMgr.SendP2PUdpRecvMonitor( pData, nLen);
			m_channelMgr.TransUdpCommand( pCmd );
		}
		catch ( ... ) 
		{
		}

		delete pCmd;
	}
	else
	{
		printf("pCmd is null id => %u\n", CBaseCommand::getID( pData ) );
	}
}

void CP2PNetwork::RequestP2PTask()
{
	if ( m_channelMgr.GetCount() > 0 )
	{
		m_channelMgr.Loop();
	}
}

