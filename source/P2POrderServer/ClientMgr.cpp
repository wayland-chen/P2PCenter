#include "StdAfx.h"
#include ".\clientmgr.h"
#include "log/KLog.h"

CClientMgr::CClientMgr(void)
{
	if ( m_db.ReadConnStrFromIni( "P2POrderServer.ini", "DBConfig", "ConnStr"))
	{
		DWORD dwTickCount = GetTickCount();
		printf( "now connectting db server...\n");
		if ( !m_db.Open())
		{
			printf( "connect db server failed. use time : %d ms.\n", GetTickCount() - dwTickCount);
		}
		else
		{
			printf( "connect db server succeed. use time : %d ms.\n", GetTickCount() - dwTickCount);
		}
	}
	else
	{
		CKLog::WriteLog( LOG_TYPE_WARNING, "ini file no exist, file:%s, section:%s, key:%s", 
			"P2POrderServer.ini", "DBConfig", "ConnStr");
	}
}

CClientMgr::~CClientMgr(void)
{
	if ( m_db.IsOpen())
		m_db.Close();
}

#define CHECK_SESSION_ID {if ( pClient->m_nSessionID != pCmd->m_nSessionID ) return;}

void CClientMgr::TransCommand( CBaseCommand* pCmd )
{
	switch( pCmd->ID() ) 
	{
	case P2S_CMD_CONNECT:
			OnConnect( reinterpret_cast<P2S_COMMAND::CCmdConnect*>(pCmd) );
		break;

	case P2S_CMD_GETGROUPS:
			OnGetGroups( reinterpret_cast<P2S_COMMAND::CCmdGetGroups*>(pCmd) );
		break;

	case P2S_CMD_GETNODES:
			OnGetNodes( reinterpret_cast<P2S_COMMAND::CCmdGetNodes*>(pCmd) );
		break;

	case P2S_CMD_LOGIN_CHANNEL:
			OnLoginChannel( reinterpret_cast<P2S_COMMAND::CCmdLoginChannel*>( pCmd ) );
		break;

	case P2S_CMD_LOGOUT_CHANNEL:
			OnLogoutChannel( reinterpret_cast<P2S_COMMAND::CCmdLogoutChannel*>( pCmd ) );
		break;

	case P2S_CMD_GETPEERS:
			OnGetPeers( reinterpret_cast<P2S_COMMAND::CCmdGetPeers*>(pCmd) );			
		break;

	case P2S_CMD_REQSHOOTTO:
			OnReqShootTo( reinterpret_cast<P2S_COMMAND::CCmdReqShootTo*>(pCmd) );
		break;

	case P2S_CMD_CLOSE:
			OnClose( reinterpret_cast<P2S_COMMAND::CCmdClose*>(pCmd) );			
		break;

	default:
		break;
	}
}

void CClientMgr::TransUdpCommand( CBaseCommand* pCmd, unsigned long ulIP, unsigned short usPort )
{
	switch( pCmd->ID() ) 
	{
	case P2S_CMD_SHOOTHOLE:
			OnUdpShootHole( reinterpret_cast<P2S_COMMAND::CCmdShootHole *>(pCmd), ulIP, usPort );
		break;

	case P2S_CMD_UDP_HEARTBEAT:
			OnUdpHeartbeat( reinterpret_cast<P2S_COMMAND::CCmdUdpHeartbeat *>(pCmd), ulIP, usPort );

	default:
		break;
	}
}

void CClientMgr::OnUdpShootHole( P2S_COMMAND::CCmdShootHole* pCmd ,unsigned long ulIP, unsigned short usPort)
{
	CKAutoLock l( &m_KCritSec );
	map<DWORD, CClient *>::iterator it = m_map.begin();
	if (it != m_map.end())
	{
		CClient* pClient = it->second;
		if ( pClient->m_peerInfo.ulID == pCmd->m_nUserID )
		{
			// 做简单的SESSION对比校验
			if ( pCmd->m_nSessionID == pClient->m_nSessionID )
			{
			}
			return;
		}
		it ++;
	}
}

void CClientMgr::OnUdpHeartbeat( P2S_COMMAND::CCmdUdpHeartbeat* pCmd ,unsigned long ulIP, unsigned short usPort)
{
	CKAutoLock l( &m_KCritSec );
	map<DWORD, CClient *>::iterator it = m_map.begin();
	while (it != m_map.end())
	{
		CClient* pClient = it->second;
		if ( pClient->m_peerInfo.ulID == pCmd->m_nUserID )
		{
			// 做简单的SESSION对比校验
			if ( pCmd->m_nSessionID == pClient->m_nSessionID )
			{
				pClient->m_peerInfo.ulExternalIP = ulIP;
				pClient->m_peerInfo.usExternalUDPPort = usPort;
				pClient->m_bUdpAvail = true;
			}
			else
			{
				printf("session is differ cmd(%d) -- client(%d)\n", pCmd->m_nSessionID, pClient->m_nSessionID );
			}
			return;
		}
		it ++;
	}
}

void CClientMgr::OnConnect( P2S_COMMAND::CCmdConnect* pCmd )
{
	P2S_COMMAND::CCmdConnectRet* pCmdConnectRet = NULL;

	{
		CKAutoLock l( &m_KCritSec );
		map<DWORD, CClient *>::iterator it = m_map.find( pCmd->m_ulSocketID );
		if (it != m_map.end())
		{
			CClient* pClient = it->second;
			static DWORD dwSession = 0;
			pClient->m_nSessionID = ++dwSession + GetTickCount();
			// 此UserID 将会成为数据库自动生成的ID,暂借用SESSION
			pClient->m_peerInfo.ulID = dwSession;
			pClient->m_peerInfo.ulLocalIP = pCmd->m_nLocalIP;
			pClient->m_peerInfo.usLocalUDPPort = pCmd->m_sLocalUdpPort;
			pClient->m_peerInfo.usLocalTCPPort = pCmd->m_sLocalTcpPort;
			pClient->m_bConnAvail = true;

			//create return
			pCmdConnectRet = new P2S_COMMAND::CCmdConnectRet();
			if ( pCmdConnectRet )
			{
				pCmdConnectRet->m_ulSocketID = pCmd->m_ulSocketID;
				pCmdConnectRet->m_nUserID = pClient->m_peerInfo.ulID;
				pCmdConnectRet->m_nSessionID = pClient->m_nSessionID;
				pCmdConnectRet->m_nResult = 0;
			}
		}
	}

	PushTcpResponseCmd( pCmdConnectRet );
}

void CClientMgr::OnGetGroups( P2S_COMMAND::CCmdGetGroups* pCmd )
{
}

void CClientMgr::OnGetNodes( P2S_COMMAND::CCmdGetNodes* pCmd )
{
}

void CClientMgr::OnLoginChannel( P2S_COMMAND::CCmdLoginChannel* pCmd )
{
	CKAutoLock l( m_channelMgr.GetCritSec() );

	CChannel* pChannel = m_channelMgr.FindNoLock( pCmd->m_sChannelID );
	if ( !pChannel )
	{
		if ( NULL == (pChannel = new CChannel()) )
			return;
		pChannel->m_sChannelID = pCmd->m_sChannelID;
		m_channelMgr.InsertNoLock( pCmd->m_sChannelID, pChannel );
	}

	if ( '1' == pCmd->m_cSource )
	{
		pChannel->m_sAgent = "";
		pChannel->m_nUpUserID = 0;
		pChannel->m_nUserID = pCmd->m_nUserID;
		pChannel->m_sNodeName = CheckSqlStr( pCmd->m_sNodeName);
		pChannel->m_sFilePath = CheckSqlStr( pCmd->m_sFilePath);
		pChannel->m_sFileName = CheckSqlStr( pCmd->m_sFileName);
		pChannel->m_sFileSize = CheckSqlStr( pCmd->m_sFileSize);
		pChannel->m_mapSourceID[pCmd->m_nUserID] = pCmd->m_nUserID;
		m_db.AppendChannelToDB( pChannel);
		m_channelMgr.SaveHtmlFile( pChannel->m_sNodeName );
	}

	pChannel->UserLogin( pCmd->m_nUserID );

	CBaseCommand* pCmdRet = P2S_COMMAND::CCommandFactory::CreateLoginChannelRetCmd( 
		pCmd->m_nUserID, pCmd->m_nSessionID, pCmd->m_sNodeName, pCmd->m_sChannelID, 
		pCmd->m_cSource == '1', pCmd->m_sFilePath, pCmd->m_sFileName, pCmd->m_sFileSize );

	if ( pCmdRet )
	{
		pCmdRet->m_ulSocketID = pCmd->m_ulSocketID;
		PushTcpResponseCmd( pCmdRet );
	}

	// 通知其他房间用户此用户进入房间

	CKAutoLock ll( &m_KCritSec );
	NotifyLoginChannel( pChannel, pCmd->m_nUserID );
}

void CClientMgr::OnLogoutChannel( P2S_COMMAND::CCmdLogoutChannel* pCmd )
{
	CKAutoLock l( m_channelMgr.GetCritSec() );

	CChannel* pChannel = m_channelMgr.FindNoLock( pCmd->m_sChannelID );
	if ( !pChannel ) return;

	pChannel->UserLogout( pCmd->m_nUserID );

	CBaseCommand* pCmdRet = P2S_COMMAND::CCommandFactory::CreateLogoutChannelRetCmd( pCmd->m_nUserID, pCmd->m_nSessionID, pCmd->m_sChannelID );
	if ( pCmdRet )
	{
		pCmdRet->m_ulSocketID = pCmd->m_ulSocketID;
		PushTcpResponseCmd( pCmdRet );
	}

	// 通知其他房间用户此用户离开房间
	{
		CKAutoLock ll( &m_KCritSec );
		NotifyLogoutChannel( pChannel, pCmd->m_nUserID );
	}

	if ( pChannel->m_mapSourceID.size() == 0 )
	{
		// 如无源,关闭频道并通知其他用户,待写
		string sNodeName = pChannel->m_sNodeName;
		m_db.RemoveChannelFromDB( pChannel);
		m_channelMgr.RemoveNoLock( pChannel->m_sChannelID );
		m_channelMgr.SaveHtmlFile( sNodeName );
	}
}

void CClientMgr::OnGetPeers( P2S_COMMAND::CCmdGetPeers* pCmd )
{
	map<int, unsigned char> mapPeers;
	{
		CKAutoLock l0( m_channelMgr.GetCritSec() );
		CChannel* pChannel = m_channelMgr.FindNoLock( pCmd->m_sChannelID );
		if ( !pChannel )	return; // out error
		pChannel->GetPeerList( pCmd->m_nUserID, mapPeers );
	}

	P2S_COMMAND::CCmdGetPeersRet* pCmdGetPeersRet = new P2S_COMMAND::CCmdGetPeersRet();

	if ( pCmdGetPeersRet )
	{
		pCmdGetPeersRet->m_ulSocketID = pCmd->m_ulSocketID;

		CKAutoLock l( &m_KCritSec );
		map<DWORD, CClient *>::iterator it = m_map.find( pCmd->m_ulSocketID );
		if (it == m_map.end()) return;
		CClient* pClient = it->second;
		//CHECK_SESSION_ID;
		{if ( pClient->m_nSessionID != pCmd->m_nSessionID ) return;}
		pCmdGetPeersRet->m_nUserID = pClient->m_peerInfo.ulID;
		pCmdGetPeersRet->m_nSessionID = pClient->m_nSessionID;
		pCmdGetPeersRet->m_sChannelID = pCmd->m_sChannelID;

		// 取得PEERLIST
		it = m_map.begin();
		while ( it != m_map.end() )
		{
			pClient = it->second;
			if ( pClient->m_peerInfo.ulID != pCmdGetPeersRet->m_nUserID && pClient->IsCanWork() )	//排除自己
			{
				map<int, unsigned char>::iterator itpeer = mapPeers.find( pClient->m_peerInfo.ulID );
				if ( itpeer != mapPeers.end() )
				{
					PEERINFO peerinfo = pClient->m_peerInfo;
					peerinfo.ucSource = itpeer->second;
					if ( PEER_TYPE_K8_SOURCE == pCmd->m_cSource )
					{
						//源只取得非源的客户端
						if ( peerinfo.ucSource != PEER_TYPE_K8_SOURCE)
							pCmdGetPeersRet->m_listPeers.push_back( peerinfo );
					}
					else
						pCmdGetPeersRet->m_listPeers.push_back( peerinfo );
				}
			}
			it ++;
		}
	}

	PushTcpResponseCmd( pCmdGetPeersRet );
}

void CClientMgr::OnReqShootTo( P2S_COMMAND::CCmdReqShootTo* pCmd )
{
	PEERINFO sorcePeerInfo;

	{
		P2S_COMMAND::CCmdReqShootToRet* pCmdShootToRet = NULL;
		CKAutoLock l( &m_KCritSec );
		map<DWORD, CClient *>::iterator it = m_map.find( pCmd->m_ulSocketID );
		if (it != m_map.end())
		{
			CClient* pClient = it->second;

			sorcePeerInfo = pClient->m_peerInfo;

			//create return
			pCmdShootToRet = new P2S_COMMAND::CCmdReqShootToRet();
			if ( pCmdShootToRet )
			{
				pCmdShootToRet->m_ulSocketID = pCmd->m_ulSocketID;
				pCmdShootToRet->m_nUserID = pClient->m_peerInfo.ulID;
				pCmdShootToRet->m_nSessionID = pClient->m_nSessionID;
				pCmdShootToRet->m_nTargetUserID = pCmd->m_nTargetUserID;
			}
		}
		PushTcpResponseCmd( pCmdShootToRet );
	}

	if ( sorcePeerInfo.ulID > 0 )
	{
		P2S_COMMAND::CCmdShootArrived* pCmdShootArrived = NULL;
		CKAutoLock l( &m_KCritSec );
		CClient* pClient = FindClientByUserID( pCmd->m_nTargetUserID );
		if ( pClient )
		{
			pCmdShootArrived = new P2S_COMMAND::CCmdShootArrived();
			if ( pCmdShootArrived )
			{
				pCmdShootArrived->m_ulSocketID = pClient->m_ulSocketID;
				pCmdShootArrived->m_nUserID = pClient->m_peerInfo.ulID;
				pCmdShootArrived->m_nSessionID = pClient->m_nSessionID;
				pCmdShootArrived->m_peerinfoSource = sorcePeerInfo;
			}
		}
		PushTcpResponseCmd( pCmdShootArrived );
	}
}

void CClientMgr::OnClose( P2S_COMMAND::CCmdClose* pCmd )
{
	P2S_COMMAND::CCmdCloseRet* pCmdCloseRet = NULL;

	{
		CKAutoLock l( &m_KCritSec );
		map<DWORD, CClient *>::iterator it = m_map.find( pCmd->m_ulSocketID );
		if (it != m_map.end())
		{
			CClient* pClient = it->second;

			//create return
			pCmdCloseRet = new P2S_COMMAND::CCmdCloseRet();
			if ( pCmdCloseRet )
			{
				pCmdCloseRet->m_ulSocketID = pCmd->m_ulSocketID;
				pCmdCloseRet->m_nUserID = pClient->m_peerInfo.ulID;
				pCmdCloseRet->m_nSessionID = pClient->m_nSessionID;
			}

			it = m_map.erase( it );
			delete pClient;
		}
	}

	PushTcpResponseCmd( pCmdCloseRet );
}

bool CClientMgr::PushTcpResponseCmd( CBaseCommand* pCmd )
{
	if ( pCmd )
	{
		m_listSendCmd.Push( pCmd );
		m_evSendCmd.Set();
		return true;
	}
	else
		return false;
}


CClient* CClientMgr::FindClientByUserID( unsigned long ulUserID )
{
	map<DWORD, CClient *>::iterator it = m_map.begin();
	while (it != m_map.end())
	{
		CClient* pClient = it->second;
		if ( ulUserID == pClient->m_peerInfo.ulID )
			return pClient;
		it ++;
	}

	return NULL;
}

void CClientMgr::LinkClose( unsigned long ulSockID )
{
	int nUserID = 0;

	{
		CKAutoLock ll( &m_KCritSec );
		CClient* pClient = FindNoLock( ulSockID );
		if ( !pClient ) return;
		nUserID = pClient->m_peerInfo.ulID;
	}

	CKAutoLock l( m_channelMgr.GetCritSec() );

	map<string, CChannel *>::iterator it = m_channelMgr.GetMapPtr()->begin();
	while ( it != m_channelMgr.GetMapPtr()->end() )
	{
		if ( it->second->IsInChannel( nUserID ) )
		{
			it->second->UserLogout( nUserID );
			NotifyLogoutChannel( it->second, nUserID );
			if ( it->second->m_mapSourceID.size() == 0 )
			{
				// 如无源,关闭频道并通知其他用户,待写
				string sNodeName = it->second->m_sNodeName;
				CChannel* pChannel = it->second;
				m_db.RemoveChannelFromDB( pChannel);
				it = m_channelMgr.GetMapPtr()->erase(it);
				m_channelMgr.SaveHtmlFile( sNodeName );
				delete pChannel;
				continue;
			}
		}
		it ++;
	}

	Remove( ulSockID );
}

void CClientMgr::NotifyLoginChannel( CChannel* pChannel, int nUserID )
{
	CClient* pClientSrc = FindClientByUserID( nUserID );
	if ( pClientSrc )
	{
		list<int> listPeers;
		pChannel->GetPeerList( nUserID, listPeers );
		while ( listPeers.size() > 0 )
		{
			int nDesUserID = listPeers.front();
			listPeers.pop_front();

			CClient* pClient = FindClientByUserID( nDesUserID );
			if ( pClient )
			{
				P2S_COMMAND::CCmdPeerLogin* pCmdPeerLogin = new P2S_COMMAND::CCmdPeerLogin();
				if ( pCmdPeerLogin )
				{
					assert( pClient->m_peerInfo.ulID == nDesUserID );

					pCmdPeerLogin->m_ulSocketID = pClient->m_ulSocketID;
					pCmdPeerLogin->m_nUserID = pClient->m_peerInfo.ulID;
					pCmdPeerLogin->m_nSessionID = pClient->m_nSessionID;
					pCmdPeerLogin->m_sChannelID = pChannel->m_sChannelID;

					PEERINFO peerinfo = pClientSrc->m_peerInfo;
					if ( pChannel->m_mapSourceID.find( peerinfo.ulID) != pChannel->m_mapSourceID.end())
						peerinfo.ucSource = PEER_TYPE_K8_SOURCE;
					else
						peerinfo.ucSource = PEER_TYPE_K8_GENERAL;

					pCmdPeerLogin->m_listPeers.push_back( peerinfo );

					PushTcpResponseCmd( pCmdPeerLogin );
				}
			}
		}
	}
}

void CClientMgr::NotifyLogoutChannel( CChannel* pChannel, int nUserID )
{
	CClient* pClientSrc = FindClientByUserID( nUserID );
	if ( pClientSrc )
	{
		list<int> listPeers;
		pChannel->GetPeerList( nUserID, listPeers );
		while ( listPeers.size() > 0 )
		{
			int nDesUserID = listPeers.front();
			listPeers.pop_front();

			CClient* pClient = FindClientByUserID( nDesUserID );
			if ( pClient )
			{
				P2S_COMMAND::CCmdPeerLogout* pCmdPeerLogout = new P2S_COMMAND::CCmdPeerLogout();
				if ( pCmdPeerLogout )
				{
					assert( pClient->m_peerInfo.ulID == nDesUserID );

					pCmdPeerLogout->m_ulSocketID = pClient->m_ulSocketID;
					pCmdPeerLogout->m_nUserID = pClient->m_peerInfo.ulID;
					pCmdPeerLogout->m_nSessionID = pClient->m_nSessionID;
					pCmdPeerLogout->m_sChannelID = pChannel->m_sChannelID;
					pCmdPeerLogout->m_listPeers.push_back( pClientSrc->m_peerInfo );
					PushTcpResponseCmd( pCmdPeerLogout );
				}
			}
		}
	}
}

void CClientMgr::MonitorUser( map<int, int>& mapUsers)
{
}

void CClientMgr::CancelMonitor( map<int, int>& mapUsers)
{
}

void CClientMgr::UpdateChannelTick()
{
	// 十分钟更新一次数据库Tick
	static DWORD dwTick = GetTickCount();
	if ( dwTick + 600 * 1000 < GetTickCount())
	{
		CKAutoLock l( m_channelMgr.GetCritSec() );
		map<string, CChannel *>::iterator it = m_channelMgr.GetMapPtr()->begin();
		while ( it != m_channelMgr.GetMapPtr()->end() )
		{
			m_db.UpdateChannelTick( it->second);
			it ++;
		}
		dwTick = GetTickCount();
	}
}

string CClientMgr::CheckSqlStr( string sStr)
{
	string sRet = sStr;
	const char* pOld = "'";
	const char* pNew = "''";
	int nPos =sRet.find( pOld);   
	while( nPos != string::npos)   
	{
		sRet.replace(nPos, strlen(pOld), pNew);   
		nPos =sRet.find( pOld ,nPos + strlen( pNew));   
	}

	return sRet;
}

