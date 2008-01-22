#pragma once

#include "P2PApp.h"
#include "ChannelMgr.h"

class CP2PNetwork :
	public CP2PApp,
	public IP2PNetwork
{
public:
	CP2PNetwork(void);
	virtual ~CP2PNetwork(void);

	virtual bool InitModule( IP2PNetworkNotify* pNotify );
	virtual void ReleaseModule();
	virtual DWORD OpenSource( const char* szUrl, bool bSource );
	//virtual DWORD OpenSourceFile( const char* szFilePath, const char* szFileName, const char* szFileHash, const LONGLONG llFileSize );
	virtual void CloseSource( DWORD dwChannelID );
	virtual void RequestSegment( DWORD dwChannelID, DWORD dwStartPos, DWORD dwLength );
	virtual bool ReadSegment( DWORD dwChannelID, DWORD dwStartPos, char* pBuffer, DWORD& dwLength );
	virtual LONGLONG GetChannelSize( DWORD dwChannelID );
	virtual bool GetAllChannelID( list<DWORD>& listChannels);
	virtual bool GetChannelMonitorInfo( DWORD dwChannelID, stMonitorInfo& monInfo);
	virtual bool GetChannelMonitorInfo( const char* szChannelHash, stMonitorInfo& monInfo);

	virtual void OnConnected(void);
	virtual void OnDisconnected(void);

	bool PushUdpPeerCmd( P2P_COMMAND::CP2PBaseCommand* pCmd );
	void OnRecvUdpData( char* pData, int nLen, unsigned long ulIP, unsigned short usPort );
	void RequestP2PTask();

protected:
	virtual void OnThreadIdle();
	virtual void TransCommand( CBaseCommand* pCmd );

private:
	IP2PNetworkNotify*	m_pNotify;
	CUdpClient			m_udpClient;
	CChannelMgr			m_channelMgr;
};
