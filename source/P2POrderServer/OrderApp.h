#pragma once

#include "ClientMgr.h"
#include "BaseCommand.h"
#include "thread/KThread.h"

class COrderApp : 
	public ITcpServerNotify,
	public CKThread
{
public:
	COrderApp(void);
	virtual ~COrderApp(void);

	bool Connect( unsigned short usPort = /*8901*/ 8902 );
	void Release();

	// From ITcpServerNotify
	virtual void OnAccept(unsigned int nLinkNo);
	virtual void OnClosed(unsigned int nLinkNo);
	virtual void OnRecvData(const char* pData, int nLen, unsigned int nLinkNo);

	void OnRecvUdpData( char* pData, int nLen, unsigned long ulIP, unsigned short usPort );

	virtual void ThreadProcMain(void);

	void TransInputCommand( string sCmd);

private:
	CClientMgr m_clientMgr;
	ITcpServer* m_pTcpServer;
	bool m_bWantStop;
};
