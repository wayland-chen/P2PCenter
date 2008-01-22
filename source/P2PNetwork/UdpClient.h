#pragma once

class CUdpClient : public CKThread
{
public:
	CUdpClient(void);
	virtual ~CUdpClient(void);

	bool Connect( const char* szServer = "127.0.0.1", unsigned short usPort = 3344 );
	void Release();
	void Healthbeat();

	bool PushUdpCmd( CBaseCommand* pCmd );
	bool PushUdpPeerCmd( P2P_COMMAND::CP2PBaseCommand* pCmd );

	virtual void ThreadProcMain(void);

private:
	bool m_bWantStop;
	CKUdpSocket m_udpSocket;
	CKSockAddr  m_addrServer;
	CKPtrList<CBaseCommand> m_listUdpCmd; // to server
	CKPtrList<P2P_COMMAND::CP2PBaseCommand> m_listP2PCmd; // to peer
};
