#pragma once

#include "thread/KThread.h"
#include "p2sdefine.h"
#include "network/KSocket.h"
#include ".\orderapp.h"

class CUdpServer : public CKThread
{
public:
	CUdpServer(void);
	virtual ~CUdpServer(void);

	void SetApp( COrderApp* pApp );

	bool Connect( unsigned short usPort = 3344 );
	void Release();

	virtual void ThreadProcMain(void);

private:
	bool m_bWantStop;
	CKEvent m_evUdpSend;
	CKUdpSocket m_udpSocket;
	COrderApp* m_pApp;
};
