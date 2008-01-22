#pragma once

#include "p2sdefine.h"
#include "P2SCommand.h"

class CClient
{
public:
	CClient(void);
	virtual ~CClient(void);

	bool IsCanWork();

	PEERINFO m_peerInfo;
	// 已收到过UDP打洞包
	bool m_bUdpAvail;
	// 已经收到连接包,对应信令 P2S_CMD_CONNECT
	bool m_bConnAvail;
	// 随机SESSION,不同于SOCKET Session
	int m_nSessionID;
	// socket ID
	unsigned long m_ulSocketID;
};
