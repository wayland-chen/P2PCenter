#pragma once

#include "P2PPeerK8General.h"
#include "P2PPeerK8Source.h"
#include "P2PPeerHttp.h"
#include "P2PPeerFtp.h"
#include "P2PPeerBittorrent.h"
#include "P2PPeerEdonkey.h"

class CP2PPeerMgr :
	public CKPtrMap<DWORD, CP2PPeer>
{
public:
	CP2PPeerMgr(void);
	virtual ~CP2PPeerMgr(void);

	void QueryExistBlocks( const char* pChannelID );
	bool GetNeedSendCmd( list<P2P_COMMAND::CCmdGetSegmentRet *>& listCmd );
	//mapRequests 闭区间，要更新已请求状态列表 
	void SendGetSegment( const char* pChannelID, DWORD dwStartBlock, DWORD dwEndBlock, map<DWORD, DWORD>& mapRequests);
	void SendHeartbeat( const char* pChannelID );

	void OnPeerHandshake( int nUserID, unsigned long ulIP, unsigned short usPort );
	void OnPeerHandshakeRet( int nUserID, unsigned long ulIP, unsigned short usPort );
	void OnGetBlockAreasRet( int nUserID, const map<DWORD, DWORD>& mapAreas );
	void OnGetSegment( int nUserID, const list<DWORD>& listBlock );
	void OnGetSegmentRet( int nUserID, DWORD dwBlock );
	void OnCancelSegment( int nUserID, DWORD nStartBlock, DWORD nEndBlock );

	bool GetMonitorInfo( stMonitorInfo& monInfo);

private:
	bool GetSortBySpeed( map<DWORD, CP2PPeer*>& mapPeers);
};
