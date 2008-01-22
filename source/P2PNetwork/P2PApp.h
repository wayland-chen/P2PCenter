#pragma once

#include "UdpClient.h"
#include "TransMonitorMgr.h"
#include "Config.h"

class CP2PApp : 
	public CKThread,
	public ITcpClientNotify
{
public:
	CP2PApp(void);
	virtual ~CP2PApp(void);
	
	bool Connect( const char* szServerIP, unsigned short usPort = 8901 );
	void Release();

	virtual void OnReceivedData( const char* pData,int nLen);
	virtual void OnConnected(void);
	virtual void OnConnectFailed(void);
	virtual void OnDisconnected(void);

	virtual void ThreadProcMain(void);

	virtual void TransCommand( CBaseCommand* pCmd );

	bool PushRequestCmd( CBaseCommand* pCmd );

	int UserID();
	int SessionID();

	int GetRandSession();
	int CalcCheckSum( const char* pBuffer, int nLen );

protected:
	virtual void OnThreadIdle() = 0;

private:

public:
	int				m_nUserID;
	int				m_nSessionID;
	CConfig			m_Config;
	CTransMonitorMgr m_transMonitorMgr;

protected:
	bool			m_bAuthSucceed; // 是否认证成功 CONNECT
	bool			m_bWantStop;
	ITcpClient*     m_pClient;
	CKEvent			m_evConnect;
	CKEvent			m_evAuth;
	CKPtrList<CBaseCommand> m_listTcpCmd;
	CKEvent			m_evTcpCmd;
	bool			m_bConnected;

	int				m_nLocalIP;
	short			m_sLocalUdpPort;
	short			m_sLocalTcpPort;

};


