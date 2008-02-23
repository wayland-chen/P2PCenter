#pragma once

#include "..\p2pnetwork\ip2pnetwork.h"

class CSocketClientMgr :
	public IP2PNetworkNotify
{
public:
	CSocketClientMgr(void);
	virtual ~CSocketClientMgr(void);

	virtual void OnConnect();
	virtual void OnDisConnect();

	bool Connect();
	void Release();

	bool IsConnected(){ return m_bConnected; };

private:
	bool m_bConnected;

public:
	IP2PNetwork* m_pP2PNetwork;
};
