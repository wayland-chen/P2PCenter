#pragma once

#include "TransMonitor.h"
#include "misc/KPtrList.h"

class CTransMonitorMgr : public CKPtrList<CTransMonitor>
{
public:
	CTransMonitorMgr(void);
	virtual ~CTransMonitorMgr(void);

	void SendP2STcpRecvMonitor( const char* pData,int nLen );
	void SendP2STcpSendMonitor( const char* pData,int nLen );
	void SendP2SUdpRecvMonitor( const char* pData,int nLen );
	void SendP2SUdpSendMonitor( const char* pData,int nLen );

	void SendP2PTcpRecvMonitor( const char* pData,int nLen );
	void SendP2PTcpSendMonitor( const char* pData,int nLen );
	void SendP2PUdpRecvMonitor( const char* pData,int nLen );
	void SendP2PUdpSendMonitor( const char* pData,int nLen );
};
