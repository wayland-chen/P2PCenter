#pragma once

#include "misc/KPtrMap.h"
#include "misc/KPtrList.h"
#include "MSSQLStore.h"
#include "Client.h"
#include "ChannelMgr.h"

// this class lock after m_channelMgr'lock
class CClientMgr : public CKPtrMap<DWORD, CClient>
{
public:
	CClientMgr(void);
	virtual ~CClientMgr(void);

	void TransCommand( CBaseCommand* pCmd );
	void TransUdpCommand( CBaseCommand* pCmd, unsigned long ulIP, unsigned short usPort );
	void LinkClose( unsigned long ulSockID );

	void OnConnect( P2S_COMMAND::CCmdConnect* pCmd );
	void OnGetGroups( P2S_COMMAND::CCmdGetGroups* pCmd );
	void OnGetNodes( P2S_COMMAND::CCmdGetNodes* pCmd );
	void OnLoginChannel( P2S_COMMAND::CCmdLoginChannel* pCmd );
	void OnLogoutChannel( P2S_COMMAND::CCmdLogoutChannel* pCmd );
	void OnGetPeers( P2S_COMMAND::CCmdGetPeers* pCmd );
	void OnReqShootTo( P2S_COMMAND::CCmdReqShootTo* pCmd );
	void OnClose( P2S_COMMAND::CCmdClose* pCmd );

	void OnUdpShootHole( P2S_COMMAND::CCmdShootHole* pCmd, unsigned long ulIP, unsigned short usPort);
	void OnUdpHeartbeat( P2S_COMMAND::CCmdUdpHeartbeat* pCmd, unsigned long ulIP, unsigned short usPort);

	bool PushTcpResponseCmd( CBaseCommand* pCmd );

	void MonitorUser( map<int, int>& mapUsers);
	void CancelMonitor( map<int, int>& mapUsers);

	void UpdateChannelTick();

private:
	CClient* FindClientByUserID( unsigned long nUserID );
	// ????????????????锁
	void NotifyLoginChannel( CChannel* pChannel, int nUserID );		// 调用前lock
	void NotifyLogoutChannel( CChannel* pChannel, int nUserID );	// 调用前lock

	//sql str check
	string CheckSqlStr( string sStr);

public:
	CKPtrList<CBaseCommand> m_listSendCmd;
	CKEvent m_evSendCmd;

private:
	CChannelMgr m_channelMgr;
	CMSSQLStore m_db;
};
