#pragma once

#include "network/KSocket.h"
#include "misc/KDataBuffer.h"

class CTransMonitor
{
public:
	CTransMonitor(void);
	virtual ~CTransMonitor(void);

	bool PushBuffer( const char* szData, int nLen);
	void SendData();
	void ClearData();

public:
	char m_cTransType;			// 传输方向，1 == 收，2 == 发
	char m_cMonitorType;		// 监控类型：1 == P2S TCP， 2 == P2S UDP, 3 == P2P TCP, 4 = P2P UDP
	char m_cTransWeight;		// 传输数据的量： 1 == 包里全部数据， 2 == 仅CMD ID,  目前忽略，发送全部包内容
	char m_cSendInterval;		// 监控发送最小间隔，精确到秒。
	int m_nDstIP;				// 目标UDP IP
	unsigned short m_usDstPort;	// 目标UDP PORT

private:
	CKUdpSocket m_udpSocket;
	CKPtrList<CKDataBuffer> m_listData; 
};
