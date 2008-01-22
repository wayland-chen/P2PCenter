#include "StdAfx.h"
#include ".\transmonitormgr.h"

CTransMonitorMgr::CTransMonitorMgr(void)
{
}

CTransMonitorMgr::~CTransMonitorMgr(void)
{

}


char m_cTransType;			// 传输方向，1 == 收，2 == 发
char m_cMonitorType;		// 监控类型：1 == P2S TCP， 2 == P2S UDP, 3 == P2P TCP, 4 = P2P UDP

void CTransMonitorMgr::SendP2STcpRecvMonitor( const char* pData,int nLen )
{
	CKAutoLock l( &m_lock);
	list<void *>::iterator it = m_listT.begin();
	while( it != m_listT.end())
	{
		CTransMonitor* pMonitor = (CTransMonitor *)(*it);
		if ( 1 == pMonitor->m_cTransType && 1 == pMonitor->m_cMonitorType)
			pMonitor->PushBuffer( pData, nLen);

		it ++;
	}
}

void CTransMonitorMgr::SendP2STcpSendMonitor( const char* pData,int nLen )
{
	CKAutoLock l( &m_lock);
	list<void *>::iterator it = m_listT.begin();
	while( it != m_listT.end())
	{
		CTransMonitor* pMonitor = (CTransMonitor *)(*it);
		if ( 2 == pMonitor->m_cTransType && 1 == pMonitor->m_cMonitorType)
			pMonitor->PushBuffer( pData, nLen);

		it ++;
	}
}

void CTransMonitorMgr::SendP2SUdpRecvMonitor( const char* pData,int nLen )
{
	CKAutoLock l( &m_lock);
	list<void *>::iterator it = m_listT.begin();
	while( it != m_listT.end())
	{
		CTransMonitor* pMonitor = (CTransMonitor *)(*it);
		if ( 1 == pMonitor->m_cTransType && 2 == pMonitor->m_cMonitorType)
			pMonitor->PushBuffer( pData, nLen);

		it ++;
	}
}

void CTransMonitorMgr::SendP2SUdpSendMonitor( const char* pData,int nLen )
{
	CKAutoLock l( &m_lock);
	list<void *>::iterator it = m_listT.begin();
	while( it != m_listT.end())
	{
		CTransMonitor* pMonitor = (CTransMonitor *)(*it);
		if ( 2 == pMonitor->m_cTransType && 2 == pMonitor->m_cMonitorType)
			pMonitor->PushBuffer( pData, nLen);

		it ++;
	}
}

void CTransMonitorMgr::SendP2PTcpRecvMonitor( const char* pData,int nLen )
{
	CKAutoLock l( &m_lock);
	list<void *>::iterator it = m_listT.begin();
	while( it != m_listT.end())
	{
		CTransMonitor* pMonitor = (CTransMonitor *)(*it);
		if ( 1 == pMonitor->m_cTransType && 3 == pMonitor->m_cMonitorType)
			pMonitor->PushBuffer( pData, nLen);

		it ++;
	}
}

void CTransMonitorMgr::SendP2PTcpSendMonitor( const char* pData,int nLen )
{
	CKAutoLock l( &m_lock);
	list<void *>::iterator it = m_listT.begin();
	while( it != m_listT.end())
	{
		CTransMonitor* pMonitor = (CTransMonitor *)(*it);
		if ( 2 == pMonitor->m_cTransType && 3 == pMonitor->m_cMonitorType)
			pMonitor->PushBuffer( pData, nLen);

		it ++;
	}
}

void CTransMonitorMgr::SendP2PUdpRecvMonitor( const char* pData,int nLen )
{
	CKAutoLock l( &m_lock);
	list<void *>::iterator it = m_listT.begin();
	while( it != m_listT.end())
	{
		CTransMonitor* pMonitor = (CTransMonitor *)(*it);
		if ( 1 == pMonitor->m_cTransType && 4 == pMonitor->m_cMonitorType)
			pMonitor->PushBuffer( pData, nLen);

		it ++;
	}
}

void CTransMonitorMgr::SendP2PUdpSendMonitor( const char* pData,int nLen )
{
	CKAutoLock l( &m_lock);
	list<void *>::iterator it = m_listT.begin();
	while( it != m_listT.end())
	{
		CTransMonitor* pMonitor = (CTransMonitor *)(*it);
		if ( 2 == pMonitor->m_cTransType && 4 == pMonitor->m_cMonitorType)
			pMonitor->PushBuffer( pData, nLen);

		it ++;
	}
}

