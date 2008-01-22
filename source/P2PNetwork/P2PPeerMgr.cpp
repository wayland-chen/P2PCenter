#include "StdAfx.h"
#include ".\p2ppeermgr.h"
#include "misc/MD5.h"
#include "P2PApp.h"

CP2PPeerMgr::CP2PPeerMgr(void)
{
}

CP2PPeerMgr::~CP2PPeerMgr(void)
{
}

void CP2PPeerMgr::QueryExistBlocks( const char* pChannelID )
{
	static DWORD dwTime = GetTickCount();
	if ( dwTime + P2P_QUERYBLOCK_INTERVAL < GetTickCount() )
	{
		CKAutoLock l( &m_KCritSec );

		map<DWORD, CP2PPeer *>::iterator it = m_map.begin();
		while ( it != m_map.end() )
		{
			P2P_COMMAND::CP2PBaseCommand* pCmd = P2P_COMMAND::CP2PCmdFactory::CreateGetBlockAreaCmd( g_pNetwork->UserID(), g_pNetwork->SessionID(), pChannelID );
			pCmd->m_dwIP = it->second->m_peerInfo.ulExternalIP;
			pCmd->m_usPort = it->second->m_peerInfo.usExternalUDPPort;
			g_pNetwork->PushUdpPeerCmd( pCmd );

			it ++;
		}

		dwTime = GetTickCount();
	}
}

bool CP2PPeerMgr::GetNeedSendCmd( list<P2P_COMMAND::CCmdGetSegmentRet *>& listCmd )
{
	CKAutoLock l( &m_KCritSec );

	map<DWORD, CP2PPeer *>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		static DWORD dwSendCount = 0;
		for ( int i = 0; i < (( 16 / m_map.size()) + 1); i ++)
		{
			if ( it->second->m_mapNeedBlocks.size() > 0 )
			{
				map<DWORD, DWORD>::iterator itBlock = it->second->m_mapNeedBlocks.begin();
				P2P_COMMAND::CCmdGetSegmentRet* pCmd = new P2P_COMMAND::CCmdGetSegmentRet();
				if ( pCmd )
				{
					pCmd->m_nUserID = g_pNetwork->UserID();
					pCmd->m_nSessionID = g_pNetwork->GetRandSession();
					pCmd->m_dwIP = it->second->m_peerInfo.ulExternalIP;
					pCmd->m_usPort = it->second->m_peerInfo.usExternalUDPPort;
					pCmd->m_nBlockNum = itBlock->first;
					listCmd.push_back( pCmd );
					it->second->CalcSendBlock( itBlock->first);
				}
				it->second->m_mapNeedBlocks.erase(itBlock);
				dwSendCount ++;
			}
			else
				break;
		}

		static DWORD dwTime = GetTickCount();
		if ( dwTime + 5000 < GetTickCount())
		{
			CKLog::WriteLog( LOG_TYPE_DEBUG, "NeedBlocks = %d, WantSend %d", 
				it->second->m_mapNeedBlocks.size(), dwSendCount);
			dwTime = GetTickCount();
		}

		it ++;
	}

	return true;
}

void CP2PPeerMgr::OnPeerHandshake( int nUserID, unsigned long ulIP, unsigned short usPort )
{
	CKAutoLock l( &m_KCritSec );

	CP2PPeer* pPeer = FindNoLock( nUserID );
	if ( pPeer )
	{
		// 如果未建立链接,即未收到过握手回复包,继续向对方发起握手
		if ( CONNNECT_STATUS_CONNECTED != pPeer->m_connStatus )
		{
			P2P_COMMAND::CP2PBaseCommand* pCmd = P2P_COMMAND::CP2PCmdFactory::CreateHandshakeCmd( g_pNetwork->UserID(), g_pNetwork->GetRandSession() );
			if ( pCmd )
			{
				pCmd->m_dwIP = ulIP;
				pCmd->m_usPort = usPort;
				g_pNetwork->PushUdpPeerCmd( pCmd );
			}
		}

		pPeer->m_peerInfo.ulExternalIP = ulIP;
		pPeer->m_peerInfo.usExternalUDPPort = usPort;
	}
}

void CP2PPeerMgr::OnPeerHandshakeRet( int nUserID, unsigned long ulIP, unsigned short usPort )
{
	CKAutoLock l( &m_KCritSec );

	CP2PPeer* pPeer = FindNoLock( nUserID );
	if ( pPeer )
		pPeer->m_connStatus = CONNNECT_STATUS_CONNECTED;
}

void CP2PPeerMgr::OnGetBlockAreasRet( int nUserID, const map<DWORD, DWORD>& mapAreas )
{
	CKAutoLock l( &m_KCritSec );

	CP2PPeer* pPeer = FindNoLock( nUserID );
	if ( pPeer )
	{
		pPeer->m_mapBlockAreas = mapAreas;
	}
}

void CP2PPeerMgr::OnGetSegment( int nUserID, const list<DWORD>& listBlock )
{
	CKAutoLock l( &m_KCritSec );

	CP2PPeer* pPeer = FindNoLock( nUserID );
	if ( pPeer )
	{
		list<DWORD>::const_iterator it = listBlock.begin();
		while ( it != listBlock.end() )
		{
			pPeer->m_mapNeedBlocks.insert( map<DWORD, DWORD>::value_type( *it, *it ) );
			pPeer->CalcGetBlock( *it);
			it ++;
		}
	}
}

void CP2PPeerMgr::SendGetSegment( const char* pChannelID, DWORD dwStartBlock, DWORD dwEndBlock, map<DWORD, DWORD>& mapRequests)
{
	////测试段
	//if ( m_map.size() > 0)
	//{
	//	map<DWORD, CP2PPeer *>::iterator it = m_map.begin();
	//	if ( it->second->IsHaveBlock( dwStartBlock ) && it->second->IsHaveBlock( dwEndBlock )  )
	//	{
	//		P2P_COMMAND::CP2PBaseCommand* pCmd = P2P_COMMAND::CP2PCmdFactory::CreateGetSegmentCmd( 
	//			g_pNetwork->UserID(), g_pNetwork->GetRandSession(), pChannelID, dwStartBlock, dwEndBlock );
	//		pCmd->m_dwIP = it->second ->m_peerInfo.ulExternalIP;
	//		pCmd->m_usPort = it->second ->m_peerInfo.usExternalUDPPort;
	//		it->second->CalcRequestBlock( dwEndBlock - dwStartBlock);
	//		g_pNetwork->PushUdpPeerCmd( pCmd );
	//	}
	//}
	//return;
	////测试段

	if ( dwEndBlock < dwStartBlock ) return;

	CKAutoLock l( &m_KCritSec );

	static DWORD dwAlreadySends = 0;
	DWORD dwStartPos = dwStartBlock;

	map<DWORD, CP2PPeer*> mapPeers;
	if ( GetSortBySpeed( mapPeers))
	{
		map<DWORD, CP2PPeer*>::iterator it = mapPeers.begin();
		while ( it != mapPeers.end())
		{
			CP2PPeer* pPeer = it->second;
			list<DWORD> listRequests;
			for ( DWORD dwEndPos = dwStartPos; 
				dwEndPos < (dwStartPos + P2P_TIMEREQUEST_MAX_NUM) && dwEndPos <= dwEndBlock; dwEndPos++)
			{
				if ( pPeer->IsHaveBlock( dwEndPos))
				{
					listRequests.push_back( dwEndPos);
					if ( dwEndPos >= dwEndBlock)
						break;
				}
				else
					break;
			}

			if ( listRequests.size() > 0)
			{
				DWORD dwStart = *listRequests.begin();
				DWORD dwEnd = *listRequests.rbegin();
				//==
				P2P_COMMAND::CP2PBaseCommand* pCmd = P2P_COMMAND::CP2PCmdFactory::CreateGetSegmentCmd( 
					g_pNetwork->UserID(), g_pNetwork->GetRandSession(), pChannelID, dwStartPos, dwEnd );
				mapRequests[dwStart] = dwEnd;
				pCmd->m_dwIP = it->second ->m_peerInfo.ulExternalIP;
				pCmd->m_usPort = it->second ->m_peerInfo.usExternalUDPPort;
				it->second->CalcRequestBlock( dwEnd - dwStart + 1);
				g_pNetwork->PushUdpPeerCmd( pCmd );
				dwAlreadySends += dwEnd - dwStart + 1;
				//CKLog::WriteLog( LOG_TYPE_DEBUG, "已发送请求数： %d", dwAlreadySends);
				//==

				dwStartPos = dwEnd + 1;
			}

			it++;
		}
	}
}

void CP2PPeerMgr::SendHeartbeat( const char* pChannelID )
{
	CKAutoLock l( &m_KCritSec );

	map<DWORD, CP2PPeer *>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		if ( it->second->m_dwLastHeartbeatTime + P2P_HEARTBEAT_INTERVAL < GetTickCount() )
		{
			P2P_COMMAND::CP2PBaseCommand* pCmd = P2P_COMMAND::CP2PCmdFactory::CreateHeartbeat( g_pNetwork->UserID(), g_pNetwork->GetRandSession(), pChannelID );
			if ( pCmd )
			{
				pCmd->m_dwIP = it->second->m_peerInfo.ulExternalIP;
				pCmd->m_usPort = it->second->m_peerInfo.usExternalUDPPort;
				g_pNetwork->PushUdpPeerCmd( pCmd );
			}
			it->second->m_dwLastHeartbeatTime = GetTickCount();
		}
		it ++;
	}
}

void CP2PPeerMgr::OnCancelSegment( int nUserID, DWORD nStartBlock, DWORD nEndBlock )
{
	CKAutoLock l( &m_KCritSec );

	CP2PPeer* pPeer = FindNoLock( nUserID );
	if ( pPeer )
	{
		map<DWORD, DWORD>::iterator it = pPeer->m_mapNeedBlocks.begin();
		while ( it != pPeer->m_mapNeedBlocks.end() )
		{
			if ( nStartBlock <= it->first && it->first <= nEndBlock )
			{
				it = pPeer->m_mapNeedBlocks.erase( it );
			}
			else
				it ++;
		}
	}
}

void CP2PPeerMgr::OnGetSegmentRet( int nUserID, DWORD dwBlock )
{
	CKAutoLock l( &m_KCritSec );

	CP2PPeer* pPeer = FindNoLock( nUserID );
	if ( pPeer )
	{
		pPeer->CalcRecvBlock( dwBlock);
	}
}

bool CP2PPeerMgr::GetMonitorInfo( stMonitorInfo& monInfo)
{
	stPeerInfo peerInfo;

	CKAutoLock l( &m_KCritSec );

	map<DWORD, CP2PPeer *>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		memset( &peerInfo, 0, sizeof(peerInfo));
		CP2PPeer * pPeer = it->second;
		peerInfo.dwID = pPeer->m_peerInfo.ulID;
		peerInfo.sExternalIP = inet_ntoa( *(in_addr *)(&pPeer->m_peerInfo.ulExternalIP));
		peerInfo.wExternalUDPPort = pPeer->m_peerInfo.usExternalUDPPort;
		peerInfo.wExternalTCPPort = pPeer->m_peerInfo.usExternalTCPPort;
		peerInfo.ConnType = CONNTYPE_NONE;
		peerInfo.bSource = (pPeer->m_peerInfo.ucSource == PEER_TYPE_K8_SOURCE);
		peerInfo.dwUploadBytes = pPeer->m_dwUploadBytes;
		peerInfo.dwRecvBytes = pPeer->m_dwRecvBytes;
		peerInfo.dwRequestBlocks = pPeer->m_dwRequestBlocks;
		peerInfo.dwReceiveBlocks = pPeer->m_dwReceiveBlocks;
		peerInfo.dwGetBlocks = pPeer->m_dwGetBlocks;
		peerInfo.dwSendBlocks = pPeer->m_dwSendBlocks;
		peerInfo.dbHealthRate = pPeer->HealthRate();
		peerInfo.dbReturnRate = pPeer->ReturnRate();
		monInfo.Peers.push_back( peerInfo);
		it ++;
	}

	return true;
}

bool CP2PPeerMgr::GetSortBySpeed( map<DWORD, CP2PPeer*>& mapPeers)
{
	// 目前暂不排序,以后优化
	DWORD dwIndex = 0;
	list<CP2PPeer *> listSources;	// 把源总是排在最后，源拥有所有块
	map<DWORD, CP2PPeer *>::iterator it = m_map.begin();
	while ( it != m_map.end())
	{
		if ( it->second->m_mapBlockAreas.size() > 0)
		{
			if ( PEER_TYPE_K8_SOURCE == it->second->m_peerInfo.ucSource )
				listSources.push_back( it->second);
			else
				mapPeers[dwIndex++] = it->second;
		}
		it ++;
	}

	list<CP2PPeer *>::iterator it2 = listSources.begin();
	while ( it2 != listSources.end())
	{
		mapPeers[dwIndex++] = *it2;
		it2 ++;
	}

	return (mapPeers.size() > 0);
}

