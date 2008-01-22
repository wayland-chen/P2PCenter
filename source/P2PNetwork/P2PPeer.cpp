#include "StdAfx.h"
#include ".\p2ppeer.h"

CP2PPeer::CP2PPeer(void) : 
m_connStatus(CONNNECT_STATUS_NONE),
m_dwUploadBytes(0),
m_dwRecvBytes(0),
m_dwLastHeartbeatTime(GetTickCount()),
m_dwReceiveBlocks(0),
m_dwRequestBlocks(0),
m_dwGetBlocks(0),
m_dwSendBlocks(0)
{
}

CP2PPeer::~CP2PPeer(void)
{
}

bool CP2PPeer::IsHaveBlock( int nBlockNum )
{
	if ( m_peerInfo.ucSource == PEER_TYPE_K8_SOURCE )
		return true;

	map<DWORD, DWORD>::iterator it = m_mapBlockAreas.begin();
	while ( it != m_mapBlockAreas.end() )
	{
		if ( it->first <= nBlockNum && nBlockNum <= it->second )
			return true;
		it ++;
	}

	return false;
}

void CP2PPeer::CalcRequestBlock( DWORD dwBlock )
{
	if ( dwBlock != 1)
		m_dwRequestBlocks += dwBlock;
	else
		m_dwRequestBlocks ++;
}

void CP2PPeer::CalcRecvBlock( DWORD dwBlock )
{
	m_dwReceiveBlocks ++;
	
	ST_TRANS_STAT ts;
	m_listReceives.push_back(ts);

	list<ST_TRANS_STAT>::iterator it = m_listReceives.begin();
	while ( it != m_listReceives.end())
	{
		if ( (*it).dwTime + SPEED_STAT_INTERVAL * 1000 < GetTickCount())
			it = m_listReceives.erase(it);
		else
			break;
	}

	m_dwRecvBytes = ( m_listReceives.size() * UDP_PACKET_LENGTH);
}

double CP2PPeer::HealthRate()
{
	double dbRet = m_dwReceiveBlocks + 1;
	dbRet /= (m_dwRequestBlocks + 1);
	dbRet *= 100;

	return dbRet;
}

void CP2PPeer::CalcGetBlock( DWORD dwBlock )
{ 
	m_dwGetBlocks ++; 
};

void CP2PPeer::CalcSendBlock( DWORD dwBlock )
{ 
	m_dwSendBlocks ++; 
	
	ST_TRANS_STAT ts;
	m_listUploads.push_back(ts);

	list<ST_TRANS_STAT>::iterator it = m_listUploads.begin();
	while ( it != m_listUploads.end())
	{
		if ( (*it).dwTime + SPEED_STAT_INTERVAL * 1000 < GetTickCount())
			it = m_listUploads.erase(it);
		else
			break;
	}

	m_dwUploadBytes = ( m_listUploads.size() * UDP_PACKET_LENGTH);
};

double CP2PPeer::ReturnRate()
{
	double dbRet = m_dwSendBlocks + 1;
	dbRet /= (m_dwGetBlocks + 1);
	dbRet *= 100;

	return dbRet;
}

