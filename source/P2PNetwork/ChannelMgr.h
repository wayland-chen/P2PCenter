#pragma once

#include "Channel.h"

class CChannelMgr :
	public CKPtrMap<DWORD, CChannel>
{
public:
	CChannelMgr(void);
	virtual ~CChannelMgr(void);

	void TransUdpCommand( P2P_COMMAND::CP2PBaseCommand* pCmd );

	void OnHandshake( P2P_COMMAND::CCmdHandshake* pCmd );
	void OnHandshakeRet( P2P_COMMAND::CCmdHandshakeRet* pCmd );
	void OnGetBlockArea( P2P_COMMAND::CCmdGetBlockArea* pCmd );
	void OnGetBlockAreaRet( P2P_COMMAND::CCmdGetBlockAreaRet* pCmd );
	void OnGetSegment( P2P_COMMAND::CCmdGetSegment* pCmd );
	void OnGetSegmentRet( P2P_COMMAND::CCmdGetSegmentRet* pCmd );
	void OnStatUpload( P2P_COMMAND::CCmdStatUpload* pCmd );
	void OnStatUploadRet( P2P_COMMAND::CCmdStatUploadRet* pCmd );
	void OnCancelSegment( P2P_COMMAND::CCmdCancelSegment* pCmd );
	void OnCancelSegmentRet( P2P_COMMAND::CCmdCancelSegmentRet* pCmd );
	void OnHeartbeat( P2P_COMMAND::CCmdHeartbeat* pCmd );
	void OnExit( P2P_COMMAND::CCmdExit* pCmd );

	void Loop();
	void RemoveSource( const char* szUrl );
	
	void RequestSegment( DWORD dwChannelID, DWORD dwStartPos, DWORD dwLength );
	bool ReadSegment( DWORD dwChannelID, DWORD dwStartPos, char* pBuffer, DWORD& dwLength );
	LONGLONG GetChannelSize( DWORD dwChannelID );
	bool GetAllChannelID( list<DWORD>& listChannels);
	bool GetChannelMonitorInfo( DWORD dwChannelID, stMonitorInfo& monInfo);
	bool GetChannelMonitorInfo( const char* szChannelHash, stMonitorInfo& monInfo);

	CChannel* FindChannelByID( string sID );
	CChannel* RemoveChannelByID( string sID );
};
