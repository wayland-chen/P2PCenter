#include "StdAfx.h"
#include ".\p2ppacketmgr.h"

CP2PPacketMgr::CP2PPacketMgr(void)
{
}

CP2PPacketMgr::~CP2PPacketMgr(void)
{
}

bool CP2PPacketMgr::ReadSegment( DWORD dwStartPos, char* pBuffer, DWORD& dwLength )
{
	bool bRet = false;
	// 块索引
	DWORD dwBlockIndex = dwStartPos / UDP_PACKET_LENGTH;
	// 从当前块读取的位置
	DWORD dwCurBlockPos = dwStartPos - UDP_PACKET_LENGTH * dwBlockIndex;

	CKAutoLock l(&m_KCritSec);
	DWORD dwAlreadyRead = 0;
	while ( dwAlreadyRead < dwLength )
	{
		map<DWORD, CP2PPacket*>::iterator it = m_map.find( dwBlockIndex );
		if ( m_map.end() == it )
			break;

		CP2PPacket* pPacket = it->second;
		if ( pPacket->m_packetStatus != PACKET_STATUS_ARRIVED )
			break;

		DWORD dwWantRead = min( UDP_PACKET_LENGTH - dwCurBlockPos, dwLength - dwAlreadyRead );
		memcpy( pBuffer + dwAlreadyRead, pPacket->m_pBuffer + dwCurBlockPos, dwWantRead );
		bRet = true;

		dwAlreadyRead += dwWantRead;
		dwCurBlockPos = 0;
		dwBlockIndex ++;
	}

	dwLength = dwAlreadyRead;

	return bRet;
}

void CP2PPacketMgr::RemoveBlock( DWORD dwStartBlock, DWORD dwEndBlock )
{
	CKAutoLock l(&m_KCritSec);
	map<DWORD, CP2PPacket*>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		CP2PPacket* pPacket = it->second;
		if ( it->first < dwStartBlock || it->first > dwEndBlock )
		{
			it = m_map.erase(it);
			delete pPacket;
		}
		else
			it ++;
	}
}

bool CP2PPacketMgr::PacketArrived( DWORD dwBlockIndex, char* pBuffer )
{
	CKAutoLock l(&m_KCritSec);
	map<DWORD, CP2PPacket*>::iterator it = m_map.find( dwBlockIndex );
	if ( it != m_map.end() )
	{
		if ( PACKET_STATUS_REQUESTED == it->second->m_packetStatus )
		{
			it->second->m_pBuffer = new char[UDP_BUFFER_LENGTH];
			memcpy( it->second->m_pBuffer, pBuffer, UDP_BUFFER_LENGTH);
			it->second->m_packetStatus = PACKET_STATUS_ARRIVED;
			return true;
		}
		else
		{
			CKLog::WriteLog( LOG_TYPE_WARNING, "新块到，但状态非请求中，状态为： %d, 块号：%d", 
				it->second->m_packetStatus, dwBlockIndex);
		}
	}

	return false;
}

void CP2PPacketMgr::InsertShouldBlock( DWORD dwStartBlock, DWORD dwBlockNum )
{
	CKAutoLock l(&m_KCritSec);

	for ( DWORD dwIndex = dwStartBlock; dwIndex < dwStartBlock + dwBlockNum; dwIndex++)
	{
		map<DWORD, CP2PPacket*>::iterator it = m_map.find( dwIndex );
		if ( m_map.end() == it )
		{
			CP2PPacket* pPacket = new CP2PPacket();
			m_map.insert( map<DWORD, CP2PPacket*>::value_type(dwIndex, pPacket) );
		}
	}
}

void CP2PPacketMgr::ClearShouldBlock()
{
	CKAutoLock l(&m_KCritSec);

	map<DWORD, CP2PPacket*>::iterator it = m_map.begin();
	while ( m_map.end() != it )
	{
		CP2PPacket* pPacket = it->second;
		if ( pPacket->m_packetStatus != PACKET_STATUS_ARRIVED )
		{
			it = m_map.erase( it );
			delete pPacket;
		}
		else
			it ++;
	}
}

void CP2PPacketMgr::UpdateTimeOutStatus( DWORD dwTimeUsec )
{
	static DWORD dwTime = GetTickCount();

	CKAutoLock l(&m_KCritSec);
	if ( dwTime + 1000 < GetTickCount() )	// 一秒一次
	{
		map<DWORD, CP2PPacket*>::iterator it = m_map.begin();
		while ( it != m_map.end() )
		{
			if ( PACKET_STATUS_REQUESTED == it->second->m_packetStatus )
				if ( (it->second->m_dwLastRequest + dwTimeUsec) < GetTickCount() )
					it->second->m_packetStatus = PACKET_STATUS_NONE;

			it ++;
		}
		dwTime = GetTickCount();
	}
}

// 调用此函式后立即发起请求
bool CP2PPacketMgr::GetShouldBlock( DWORD& dwStart, DWORD& dwEnd )
{
	DWORD dwRet = 0xFFFFFFFF;

	CKAutoLock l(&m_KCritSec);
	map<DWORD, CP2PPacket*>::iterator it = m_map.begin();
	WORD wCount = 0;
	dwStart = dwEnd = 0xFFFFFFFF;

	while ( it != m_map.end() )
	{
		if ( PACKET_STATUS_NONE == it->second->m_packetStatus )
		{
			//it->second->m_dwLastRequest = GetTickCount();
			//it->second->m_packetStatus = PACKET_STATUS_REQUESTED;
			if ( 0xFFFFFFFF == dwStart )
			{
				dwStart = it->first;
				dwEnd = it->first;
				++wCount;
			}
			else
			{
				// 是否连号,连号一次请求P2P_TIMEREQUEST_MAX_NUM个
				if ( it->first == (dwEnd + 1) )
				{
					dwEnd = it->first;
					++wCount;
				}
				else
					break;
			}
		}

		if ( wCount >= P2P_TIMEREQUEST_MAX_NUM * 8 )
			return true;

		it ++;
	}

	return (wCount > 0);
}

void CP2PPacketMgr::UpdatePacketStatus( const map<DWORD, DWORD>& mapRequests, PACKET_STATUS status)
{
	CKAutoLock l(&m_KCritSec);

	static DWORD dwUpdates = 0;

	map<DWORD, DWORD>::const_iterator it = mapRequests.begin();
	while ( it != mapRequests.end() )
	{
		for ( DWORD dw = it->first; dw <= it->second; dw ++)
		{
			CP2PPacket* pPacket = FindNoLock( dw);
			if ( pPacket)
			{
				pPacket->m_dwLastRequest = GetTickCount();
				pPacket->m_packetStatus = status;
				dwUpdates ++;
			}
		}
		it ++;
	}

	//CKLog::WriteLog( LOG_TYPE_DEBUG, "已更新状态数： %d", dwUpdates);
}

bool CP2PPacketMgr::GetExistBlockAreas( map<DWORD, DWORD>& mapAreas )
{
	// is source, add from file read interface.

	DWORD dwStartPos = 0xFFFFFFFF, dwEndPos = 0xFFFFFFFF;
	//if ( PACKET_STATUS_ARRIVED == )

	CKAutoLock l(&m_KCritSec);

	map<DWORD, CP2PPacket*>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		if ( PACKET_STATUS_ARRIVED == it->second->m_packetStatus )
		{
			if ( 0xFFFFFFFF == dwStartPos ) // 第一次
			{
				dwStartPos = it->first;
				dwEndPos = it->first;
			}
			else
			{
				if ( it->first != (dwEndPos + 1) )
				{
					mapAreas.insert( map<DWORD, DWORD>::value_type( dwStartPos, dwEndPos ) );
					dwStartPos = it->first;
					dwEndPos = it->first;
				}
				else
				{
					dwEndPos = it->first;
				}
			}
		}

		it ++;
	}

	if ( dwStartPos != 0xFFFFFFFF && dwEndPos != 0xFFFFFFFF )
	{
		mapAreas.insert( map<DWORD, DWORD>::value_type( dwStartPos, dwEndPos ) );
	}

	return true;
}

bool CP2PPacketMgr::GetExistBlockList( list<DWORD>& listBlock, DWORD dwStartBlock, DWORD dwEndBlock )
{
	CKAutoLock l(&m_KCritSec);

	map<DWORD, CP2PPacket*>::iterator it = m_map.find( dwStartBlock );

	while ( it != m_map.end() )
	{
		if ( it->first > dwEndBlock )
			break;

		if ( PACKET_STATUS_ARRIVED == it->second->m_packetStatus )
		{
			listBlock.push_back( it->first );
		}

		it ++;
	}

	return true;
}

