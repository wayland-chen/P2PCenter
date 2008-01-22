#include "StdAfx.h"
#include ".\channelmgr.h"
#include "misc/MD5.h"
#include "P2PApp.h"

CChannelMgr::CChannelMgr(void)
{
}

CChannelMgr::~CChannelMgr(void)
{
}

void CChannelMgr::RequestSegment( DWORD dwChannelID, DWORD dwStartPos, DWORD dwLength )
{
	CKAutoLock l(&m_KCritSec);

	map<DWORD, CChannel*>::iterator it = m_map.find( dwChannelID );
	if ( it != m_map.end() )
	{
		CChannel* pChannel = it->second;
		pChannel->RequestSegment( dwStartPos, dwLength );
	}
}

bool CChannelMgr::ReadSegment( DWORD dwChannelID, DWORD dwStartPos, char* pBuffer, DWORD& dwLength )
{
	CKAutoLock l(&m_KCritSec);

	map<DWORD, CChannel*>::iterator it = m_map.find( dwChannelID );
	if ( it != m_map.end() )
	{
		CChannel* pChannel = it->second;
		return pChannel->ReadSegment( dwStartPos, pBuffer, dwLength );
	}

	return false;
}

LONGLONG CChannelMgr::GetChannelSize( DWORD dwChannelID )
{
	CKAutoLock l(&m_KCritSec);

	map<DWORD, CChannel*>::iterator it = m_map.find( dwChannelID );
	if ( it != m_map.end() )
	{
		return it->second->Length();
	}

	return 0;
}

bool CChannelMgr::GetChannelMonitorInfo( DWORD dwChannelID, stMonitorInfo& monInfo)
{
	CKAutoLock l(&m_KCritSec);

	map<DWORD, CChannel*>::iterator it = m_map.find( dwChannelID );
	if ( it != m_map.end() )
	{
		monInfo.sChannelName = it->second->ChannelName();
		return it->second->m_peerMgr.GetMonitorInfo( monInfo );
	}

	return false;
}

bool CChannelMgr::GetChannelMonitorInfo( const char* szChannelHash, stMonitorInfo& monInfo)
{
	CKAutoLock l(&m_KCritSec);

	CChannel* pChannel = FindChannelByID( szChannelHash );
	if ( pChannel )
	{
		monInfo.sChannelName = pChannel->ChannelName();
		return pChannel->m_peerMgr.GetMonitorInfo( monInfo );
	}

	return false;
}

bool CChannelMgr::GetAllChannelID( list<DWORD>& listChannels)
{
	CKAutoLock l(&m_KCritSec);

	map<DWORD, CChannel*>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		listChannels.push_back( it->first);
		it++;
	}

	return true;
}

CChannel* CChannelMgr::FindChannelByID( string sID )
{
	map<DWORD, CChannel*>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		CChannel* pChannel = it->second;
		if ( pChannel->GetID() == sID )
		{
			return pChannel;
		}
		it++;
	}

	return NULL;
}

CChannel* CChannelMgr::RemoveChannelByID( string sID )
{
	map<DWORD, CChannel*>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		CChannel* pChannel = it->second;
		if ( pChannel->GetID() == sID )
		{
			it = m_map.erase(it);
			return pChannel;
		}
		it++;
	}

	return NULL;
}

void CChannelMgr::Loop()
{
	CKAutoLock l(&m_KCritSec);

	map<DWORD, CChannel*>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		CChannel* pChannel = it->second;
		pChannel->Loop();
		it++;
	}
}

void CChannelMgr::TransUdpCommand( P2P_COMMAND::CP2PBaseCommand* pCmd )
{
	/*CKLog::WriteLog( LOG_TYPE_DEBUG, "UDP:%s:%d,%s", 
		inet_ntoa( *(in_addr *)(&pCmd->m_dwIP) ), pCmd->m_usPort, 
		P2P_COMMAND::CP2PCommandQuery::CommandString( pCmd->ID() ).c_str() );*/

	switch( pCmd->ID() ) 
	{
		//------------------------------------------------------------------------
	case P2P_CMD_HANDSHAKE:
		OnHandshake( reinterpret_cast<P2P_COMMAND::CCmdHandshake*>(pCmd) );
		break;

	case P2P_CMD_GETBLOCK_AREA:
		OnGetBlockArea( reinterpret_cast<P2P_COMMAND::CCmdGetBlockArea*>(pCmd) );
		break;

	case P2P_CMD_GETSEGMENT:
		OnGetSegment( reinterpret_cast<P2P_COMMAND::CCmdGetSegment*>(pCmd) );
		break;

	case P2P_CMD_STAT_UPLOAD:
		OnStatUpload( reinterpret_cast<P2P_COMMAND::CCmdStatUpload*>(pCmd) );
		break;

	case P2P_CMD_CANCELSEGMENT:
		OnCancelSegment( reinterpret_cast<P2P_COMMAND::CCmdCancelSegment*>(pCmd) );
		break;

	case P2P_CMD_HEARTBEAT:
		OnHeartbeat( reinterpret_cast<P2P_COMMAND::CCmdHeartbeat*>(pCmd) );
		break;

	case P2P_CMD_EXIT:
		OnExit( reinterpret_cast<P2P_COMMAND::CCmdExit*>(pCmd) );
		break;
		//------------------------------------------------------------------------
	case P2P_CMD_HANDSHAKE_RET:
		OnHandshakeRet( reinterpret_cast<P2P_COMMAND::CCmdHandshakeRet*>(pCmd) );
		break;

	case P2P_CMD_GETBLOCK_AREA_RET:
		OnGetBlockAreaRet( reinterpret_cast<P2P_COMMAND::CCmdGetBlockAreaRet*>(pCmd) );
		break;

	case P2P_CMD_GETSEGMENT_RET:
		OnGetSegmentRet( reinterpret_cast<P2P_COMMAND::CCmdGetSegmentRet*>(pCmd) );
		break;

	case P2P_CMD_STAT_UPLOAD_RET:
		OnStatUploadRet( reinterpret_cast<P2P_COMMAND::CCmdStatUploadRet*>(pCmd) );
		break;

	case P2P_CMD_CANCELSEGMENT_RET:
		OnCancelSegmentRet( reinterpret_cast<P2P_COMMAND::CCmdCancelSegmentRet*>(pCmd) );
		break;
		// ------------------------------------------------------------------------

	default:
		break;
	}
}

void CChannelMgr::OnHandshake( P2P_COMMAND::CCmdHandshake* pCmd )
{
	P2P_COMMAND::CP2PBaseCommand* pCmdRet = P2P_COMMAND::CP2PCmdFactory::CreateHandshakeRetCmd( g_pNetwork->UserID(), pCmd->m_nSessionID );
	if ( pCmdRet )
	{
		pCmdRet->m_dwIP = pCmd->m_dwIP;
		pCmdRet->m_usPort = pCmd->m_usPort;
		g_pNetwork->PushUdpPeerCmd( pCmdRet );
	}

	CKAutoLock l(&m_KCritSec);

	map<DWORD, CChannel*>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		CChannel* pChannel = it->second;
		pChannel->m_peerMgr.OnPeerHandshake( pCmd->m_nUserID, pCmd->m_dwIP, pCmd->m_usPort );
		it++;
	}
}

void CChannelMgr::OnHandshakeRet( P2P_COMMAND::CCmdHandshakeRet* pCmd )
{
	CKAutoLock l(&m_KCritSec);

	map<DWORD, CChannel*>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		CChannel* pChannel = it->second;
		pChannel->m_peerMgr.OnPeerHandshakeRet( pCmd->m_nUserID, pCmd->m_dwIP, pCmd->m_usPort );
		it++;
	}
}

void CChannelMgr::OnGetBlockArea( P2P_COMMAND::CCmdGetBlockArea* pCmd )
{
	CKAutoLock l(&m_KCritSec);

	CChannel* pChannel = FindChannelByID( CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	if ( pChannel )
	{
		map<DWORD, DWORD> mapAreas;
		if ( pChannel->IsSource() )
		{
			mapAreas.insert( map<DWORD, DWORD>::value_type( 0, (pChannel->Length() + UDP_PACKET_LENGTH - 1) / UDP_PACKET_LENGTH ) );
		}
		else
		{
			pChannel->m_packetMgr.GetExistBlockAreas( mapAreas );
			CKLog::WriteLog( LOG_TYPE_DEBUG, "OnGetBlockArea areasize = %d", mapAreas.size());
		}
		P2P_COMMAND::CP2PBaseCommand* pCmdRet = P2P_COMMAND::CP2PCmdFactory::CreateGetBlockAreaRetCmd( g_pNetwork->UserID(), pCmd->m_nSessionID, pCmd->m_szChannelID, mapAreas );
		if ( pCmdRet )
		{
			pCmdRet->m_dwIP = pCmd->m_dwIP;
			pCmdRet->m_usPort = pCmd->m_usPort;
			g_pNetwork->PushUdpPeerCmd( pCmdRet );
		}
	}
	else
	{
		printf("channel is not exist. channelid = %s\n", CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	}
}

void CChannelMgr::OnGetBlockAreaRet( P2P_COMMAND::CCmdGetBlockAreaRet* pCmd )
{
	CKAutoLock l(&m_KCritSec);

	CChannel* pChannel = FindChannelByID( CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	if ( pChannel )
	{
		pChannel->m_peerMgr.OnGetBlockAreasRet( pCmd->m_nUserID, pCmd->m_mapAreas );
	}
	else
	{
		printf("channel is not exist. channelid = %s\n", CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	}
}

void CChannelMgr::OnGetSegment( P2P_COMMAND::CCmdGetSegment* pCmd )
{
	CKAutoLock l(&m_KCritSec);

	static DWORD dwTime = GetTickCount();
	static int nCount = 0;
	++nCount;
	if ( dwTime + 1000 < GetTickCount() )
	{
		CKLog::WriteLog( LOG_TYPE_DEBUG, "CChannelMgr::OnGetSegment:%s:%d,%s Count = %d", 
			inet_ntoa( *(in_addr *)(&pCmd->m_dwIP) ), pCmd->m_usPort, 
			P2P_COMMAND::CP2PCommandQuery::CommandString( pCmd->ID() ).c_str(), nCount );
		dwTime = GetTickCount();
	}

	CChannel* pChannel = FindChannelByID( CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	if ( pChannel )
	{
		int nEndPos = min(pCmd->m_nEndPos,  pCmd->m_nStartPos + P2P_MAX_GETSEGMENT_INTERVAL );
		int nStartPos = min(pCmd->m_nStartPos, pCmd->m_nEndPos);
		list<DWORD> listBlock;

		if ( pChannel->IsSource() )
		{
			nEndPos = min( nEndPos, pChannel->Length() / UDP_PACKET_LENGTH + 1 );
			for( DWORD dwIndex = nStartPos; dwIndex <= nEndPos; dwIndex++ )
				listBlock.push_back( dwIndex );

			if ( listBlock.size() > 0 )
				pChannel->m_peerMgr.OnGetSegment( pCmd->m_nUserID, listBlock );
		}
		else
		{
			if ( pChannel->m_packetMgr.GetExistBlockList( listBlock, nStartPos, nEndPos ) )
			{
				if ( listBlock.size() > 0 )
				{
					pChannel->m_peerMgr.OnGetSegment( pCmd->m_nUserID, listBlock );
					CKLog::WriteLog( LOG_TYPE_DEBUG, 
						"GetSegment Block. channelid = %s, userid=%d, startpos=%d, endpos=%d\n", 
						CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0), pCmd->m_nUserID, nStartPos, nEndPos);
				}
				else
				{
					CKLog::WriteLog( LOG_TYPE_DEBUG, 
						"GetSegment Block no exists. channelid = %s, userid=%d, startpos=%d, endpos=%d\n", 
						CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0), pCmd->m_nUserID, nStartPos, nEndPos);
				}
			}
		}
	}
	else
	{
		CKLog::WriteLog( LOG_TYPE_DEBUG, "GetSegment channel is not exist. channelid = %s\n", 
			CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	}
}

void CChannelMgr::OnGetSegmentRet( P2P_COMMAND::CCmdGetSegmentRet* pCmd )
{
	CKAutoLock l(&m_KCritSec);

	CChannel* pChannel = FindChannelByID( CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	if ( pChannel )
	{
		if ( !pChannel->IsSource() )
		{
			if ( pCmd->m_nCheckSum == g_pNetwork->CalcCheckSum( pCmd->m_szBuffer, sizeof(pCmd->m_szBuffer) ) )
			{
				static int nLastUserID = 0;
				pChannel->m_packetMgr.PacketArrived( pCmd->m_nBlockNum, pCmd->m_szBuffer );
				pChannel->m_peerMgr.OnGetSegmentRet( pCmd->m_nUserID, pCmd->m_nBlockNum );

				if ( pCmd->m_nBlockNum % 128 == 0 || pCmd->m_nUserID != nLastUserID )
				{
					nLastUserID = pCmd->m_nUserID;
					CKLog::WriteLog( LOG_TYPE_DEBUG, "packet arrived blocknum = %u, userid=%u", pCmd->m_nBlockNum, pCmd->m_nUserID );
				}
			}
			else
			{
				CKLog::WriteLog( LOG_TYPE_DEBUG, "checksum error: checksum(%u) --> bufcalc(%u), userid=%u\n", 
					pCmd->m_nCheckSum, g_pNetwork->CalcCheckSum( pCmd->m_szBuffer, sizeof(pCmd->m_szBuffer) ), pCmd->m_nUserID );
			}
		}
	}
	else
	{
		CKLog::WriteLog( LOG_TYPE_DEBUG, "OnGetSegmentRet channel is not exist. channelid = %s\n", 
			CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	}
}

void CChannelMgr::OnStatUpload( P2P_COMMAND::CCmdStatUpload* pCmd )
{
}

void CChannelMgr::OnStatUploadRet( P2P_COMMAND::CCmdStatUploadRet* pCmd )
{
}

void CChannelMgr::OnCancelSegment( P2P_COMMAND::CCmdCancelSegment* pCmd )
{
	CKAutoLock l(&m_KCritSec);

	CChannel* pChannel = FindChannelByID( CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	if ( pChannel )
	{
		pChannel->m_peerMgr.OnCancelSegment( pCmd->m_nUserID, pCmd->m_nStartNum, pCmd->m_nEndNum );
	}
	else
	{
		printf("OnCancelSegment channel is not exist. channelid = %s\n", CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	}
}

void CChannelMgr::OnCancelSegmentRet( P2P_COMMAND::CCmdCancelSegmentRet* pCmd )
{
}

void CChannelMgr::OnHeartbeat( P2P_COMMAND::CCmdHeartbeat* pCmd )
{
}

void CChannelMgr::OnExit( P2P_COMMAND::CCmdExit* pCmd )
{
	CKAutoLock l(&m_KCritSec);

	CChannel* pChannel = FindChannelByID( CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	if ( pChannel )
	{
		pChannel->m_peerMgr.Remove( pCmd->m_nUserID );
	}
	else
	{
		printf("channel is not exist. channelid = %s\n", CMD5::HashToString( (MD5 *)pCmd->m_szChannelID ).GetBuffer(0) );
	}
}


