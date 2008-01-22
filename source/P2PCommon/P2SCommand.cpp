#include ".\p2scommand.h"
#include <assert.h>

namespace P2S_COMMAND
{	
	//------------------------------------------------------------------------------------
	string CP2SCommandQuery::CommandString( unsigned int unCommand )
	{
		string sRet = "-- no name command --";

		switch( unCommand ) 
		{
		case P2S_CMD_SHOOTHOLE:
			sRet = "UDP_P2S_CMD_SHOOTHOLE";
			break;

		case P2S_CMD_SHOOTHOLE_RET:
			sRet = "UDP_P2S_CMD_SHOOTHOLE_RET";
			break;

		case P2S_CMD_UDP_HEARTBEAT:
			sRet = "UDP_P2S_CMD_UDP_HEARTBEAT";
			break;

		case P2S_CMD_CONNECT:
			sRet = "P2S_CMD_CONNECT";
			break;

		case P2S_CMD_CONNECT_RET:
			sRet = "P2S_CMD_CONNECT_RET";
			break;

		case P2S_CMD_GETGROUPS:
			sRet = "P2S_CMD_GETGROUPS";
			break;

		case P2S_CMD_GETGROUPS_RET:
			sRet = "P2S_CMD_GETGROUPS_RET";
			break;

		case P2S_CMD_GETNODES:
			sRet = "P2S_CMD_GETNODES";
			break;

		case P2S_CMD_GETNODES_RET:
			sRet = "P2S_CMD_GETNODES_RET";
			break;

		case P2S_CMD_LOGIN_CHANNEL:
			sRet = "P2S_CMD_LOGIN_CHANNEL";
			break;

		case P2S_CMD_LOGIN_CHANNEL_RET:
			sRet = "P2S_CMD_LOGIN_CHANNEL_RET";
			break;

		case P2S_CMD_LOGOUT_CHANNEL:
			sRet = "P2S_CMD_LOGOUT_CHANNEL";
			break;

		case P2S_CMD_LOGOUT_CHANNEL_RET:
			sRet = "P2S_CMD_LOGOUT_CHANNEL_RET";
			break;

		case P2S_CMD_GETPEERS:
			sRet = "P2S_CMD_GETPEERS";
			break;

		case P2S_CMD_GETPEERS_RET:
			sRet = "P2S_CMD_GETPEERS_RET";
			break;

		case P2S_CMD_PEERLOGIN:
			sRet = "P2S_CMD_PEERLOGIN";
			break;

		case P2S_CMD_PEERLOGOUT:
			sRet = "P2S_CMD_PEERLOGOUT";
			break;

		case P2S_CMD_REQSHOOTTO:
			sRet = "P2S_CMD_REQSHOOTTO";
			break;

		case P2S_CMD_REQSHOOTTO_RET:
			sRet = "P2S_CMD_REQSHOOTTO_RET";
			break;

		case P2S_CMD_SHOOTARRIVED:
			sRet = "P2S_CMD_SHOOTARRIVED";
			break;

		case P2S_CMD_TCP_HEARTBEAT:
			sRet = "P2S_CMD_TCP_HEARTBEAT";
			break;

		case P2S_CMD_CLOSE:
			sRet = "P2S_CMD_CLOSE";
			break;

		case P2S_CMD_CLOSE_RET:
			sRet = "P2S_CMD_CLOSE_RET";
			break;

		default:
			break;
		}

		return sRet;
	}
	//------------------------------------------------------------------------------------
	CBaseCommand* CCommandFactory::CreateConnectCmd( string sUsername, 
		string sPassword, int nLocalIP, short sLocalUdpPort, short sLocalTcpPort)
	{
		CCmdConnect* pCmd = new CCmdConnect();
		if ( pCmd )
		{
			pCmd->m_sUsername = sUsername;
			pCmd->m_sPassword = sPassword;
			pCmd->m_nLocalIP = nLocalIP;
			pCmd->m_sLocalUdpPort = sLocalUdpPort;
			pCmd->m_sLocalTcpPort = sLocalTcpPort;
		}
		
		return pCmd;
	}
	//------------------------------------------------------------------------------------
	CBaseCommand* CCommandFactory::CreateGetPeersCmd( int nUserID, int nSessionID, string sChannelID, bool bSource )
	{
		CCmdGetPeers* pCmd = new CCmdGetPeers();
		if ( pCmd )
		{
			pCmd->m_sChannelID = sChannelID;
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			bSource ? pCmd->m_cSource = '1' : pCmd->m_cSource = '0';
		}

		return pCmd;
	}
	//------------------------------------------------------------------------------------
	CBaseCommand* CCommandFactory::CreateLoginChannelCmd( int nUserID, int nSessionID, string sNodeName,
		string sChannelID, bool bSource, string sFilePath, string sFileName, string sFileSize )
	{
		CCmdLoginChannel* pCmd = new CCmdLoginChannel();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			pCmd->m_sNodeName = sNodeName;
			pCmd->m_sChannelID = sChannelID;
			bSource ? pCmd->m_cSource = '1' : pCmd->m_cSource = '0';
			pCmd->m_sFilePath = sFilePath;
			pCmd->m_sFileName = sFileName;
			pCmd->m_sFileSize = sFileSize;
		}

		return pCmd;
	}
	//------------------------------------------------------------------------------------
	CBaseCommand* CCommandFactory::CreateLoginChannelRetCmd( int nUserID, int nSessionID, string sNodeName,
		string sChannelID, bool bSource, string sFilePath, string sFileName, string sFileSize )
	{
		CCmdLoginChannelRet* pCmd = new CCmdLoginChannelRet();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			pCmd->m_sNodeName = sNodeName;
			pCmd->m_sChannelID = sChannelID;
			bSource ? pCmd->m_cSource = '1' : pCmd->m_cSource = '0';
			pCmd->m_sFilePath = sFilePath;
			pCmd->m_sFileName = sFileName;
			pCmd->m_sFileSize = sFileSize;
		}

		return pCmd;
	}
	//------------------------------------------------------------------------------------
	CBaseCommand* CCommandFactory::CreateLogoutChannelCmd( int nUserID, int nSessionID, string sChannelID )
	{
		CCmdLogoutChannel* pCmd = new CCmdLogoutChannel();
		if ( pCmd )
		{
			pCmd->m_sChannelID = sChannelID;
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
		}

		return pCmd;
	}
	//------------------------------------------------------------------------------------
	CBaseCommand* CCommandFactory::CreateLogoutChannelRetCmd( int nUserID, int nSessionID, string sChannelID )
	{
		CCmdLogoutChannelRet* pCmd = new CCmdLogoutChannelRet();
		if ( pCmd )
		{
			pCmd->m_sChannelID = sChannelID;
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
		}

		return pCmd;
	}
	//------------------------------------------------------------------------------------
	CBaseCommand* CCommandFactory::CreateUdpHeartbeatCmd( int nUserID, int nSessionID, string sAuthStr )
	{
		CCmdUdpHeartbeat* pCmd = new CCmdUdpHeartbeat();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
		}

		return pCmd;
	}
	CBaseCommand* CCommandFactory::CreateTcpHeartbeatCmd( int nUserID, int nSessionID, string sAuthStr )
	{
		CCmdTcpHeartbeat* pCmd = new CCmdTcpHeartbeat();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
		}

		return pCmd;
	}
	//------------------------------------------------------------------------------------
	CBaseCommand* CCommandFactory::CreateReqShootToCmd( int nUserID, int nSessionID, int nTargetUserID )
	{
		CCmdReqShootTo* pCmd = new CCmdReqShootTo();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			pCmd->m_nTargetUserID = nTargetUserID;
		}

		return pCmd;
	}
	//------------------------------------------------------------------------------------
	CBaseCommand* CCommandFactory::CreateReqShootToRetCmd( int nUserID, int nSessionID, int nTargetUserID )
	{
		CCmdReqShootToRet* pCmd = new CCmdReqShootToRet();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			pCmd->m_nTargetUserID = nTargetUserID;
		}

		return pCmd;
	}
	//------------------------------------------------------------------------------------
	CBaseCommand* CCommandFactory::CreateReqShootArritedCmd( int nUserID, int nSessionID, const PEERINFO& targetPeerinfo)
	{
		CCmdShootArrived* pCmd = new CCmdShootArrived();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			pCmd->m_peerinfoSource = targetPeerinfo;
		}

		return pCmd;
	}
	//------------------------------------------------------------------------------------
	CCmdShootHole::CCmdShootHole() : 
	CBaseCommand(P2S_CMD_SHOOTHOLE)
	{
	}

	CCmdShootHole::~CCmdShootHole(void)
	{
	}

	int CCmdShootHole::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdShootHole::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdShootHoleRet::CCmdShootHoleRet() : 
	CBaseCommand(P2S_CMD_SHOOTHOLE_RET)
	{
	}

	CCmdShootHoleRet::~CCmdShootHoleRet(void)
	{
	}

	int CCmdShootHoleRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdShootHoleRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdConnect::CCmdConnect() : 
	CBaseCommand(P2S_CMD_CONNECT)
	{
	}

	CCmdConnect::~CCmdConnect(void)
	{
	}

	int CCmdConnect::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_str_2( p, m_sUsername, nLen );
		p = push_str_2( p, m_sPassword, nLen );
		p = push_int32_2( p, m_nLocalIP, nLen);
		p = push_int16_2( p, m_sLocalUdpPort, nLen);
		p = push_int16_2( p, m_sLocalTcpPort, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdConnect::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_str( p, m_sUsername );
		p = pop_str( p, m_sPassword );
		p = pop_int32( p, m_nLocalIP );
		p = pop_int16( p, m_sLocalUdpPort );
		p = pop_int16( p, m_sLocalTcpPort );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdConnectRet::CCmdConnectRet() : 
	CBaseCommand(P2S_CMD_CONNECT_RET),
	m_nResult(0)
	{
	}

	CCmdConnectRet::~CCmdConnectRet(void)
	{
	}

	int CCmdConnectRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nResult, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdConnectRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nResult );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdGetGroups::CCmdGetGroups() : 
	CBaseCommand(P2S_CMD_GETGROUPS)
	{
	}

	CCmdGetGroups::~CCmdGetGroups(void)
	{
	}

	int CCmdGetGroups::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdGetGroups::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdGetGroupsRet::CCmdGetGroupsRet() : 
	CBaseCommand(P2S_CMD_GETGROUPS_RET)
	{
	}

	CCmdGetGroupsRet::~CCmdGetGroupsRet(void)
	{
	}

	int CCmdGetGroupsRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdGetGroupsRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdGetNodes::CCmdGetNodes() : 
	CBaseCommand(P2S_CMD_GETNODES)
	{
	}

	CCmdGetNodes::~CCmdGetNodes(void)
	{
	}

	int CCmdGetNodes::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdGetNodes::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdGetNodesRet::CCmdGetNodesRet() : 
	CBaseCommand(P2S_CMD_GETGROUPS_RET)
	{
	}

	CCmdGetNodesRet::~CCmdGetNodesRet(void)
	{
	}

	int CCmdGetNodesRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdGetNodesRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdLoginChannel::CCmdLoginChannel() : 
	CBaseCommand(P2S_CMD_LOGIN_CHANNEL)
	{
	}

	CCmdLoginChannel::~CCmdLoginChannel(void)
	{
	}

	int CCmdLoginChannel::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_str_2( p, m_sNodeName, nLen );
		p = push_str_2( p, m_sChannelID, nLen );
		p = push_int8_2( p, m_cSource, nLen );
		p = push_str_2( p, m_sFilePath, nLen );
		p = push_str_2( p, m_sFileName, nLen );
		p = push_str_2( p, m_sFileSize, nLen );
		return (int)(p - pBuffer);
	}

	int CCmdLoginChannel::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_str( p, m_sNodeName );
		p = pop_str( p, m_sChannelID );
		p = pop_int8( p, m_cSource );
		p = pop_str( p, m_sFilePath );
		p = pop_str( p, m_sFileName );
		p = pop_str( p, m_sFileSize );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdLoginChannelRet::CCmdLoginChannelRet() : 
	CBaseCommand(P2S_CMD_LOGIN_CHANNEL_RET)
	{
	}

	CCmdLoginChannelRet::~CCmdLoginChannelRet(void)
	{
	}

	int CCmdLoginChannelRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_str_2( p, m_sChannelID, nLen );
		return (int)(p - pBuffer);
	}

	int CCmdLoginChannelRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_str( p, m_sChannelID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdLogoutChannel::CCmdLogoutChannel() : 
	CBaseCommand(P2S_CMD_LOGOUT_CHANNEL)
	{
	}

	CCmdLogoutChannel::~CCmdLogoutChannel(void)
	{
	}

	int CCmdLogoutChannel::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_str_2( p, m_sChannelID, nLen );
		return (int)(p - pBuffer);
	}

	int CCmdLogoutChannel::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_str( p, m_sChannelID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdLogoutChannelRet::CCmdLogoutChannelRet() : 
	CBaseCommand(P2S_CMD_LOGOUT_CHANNEL_RET)
	{
	}

	CCmdLogoutChannelRet::~CCmdLogoutChannelRet(void)
	{
	}

	int CCmdLogoutChannelRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_str_2( p, m_sChannelID, nLen );
		return (int)(p - pBuffer);
	}

	int CCmdLogoutChannelRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_str( p, m_sChannelID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdGetPeers::CCmdGetPeers() : 
	CBaseCommand(P2S_CMD_GETPEERS)
	{
	}

	CCmdGetPeers::~CCmdGetPeers(void)
	{
	}

	int CCmdGetPeers::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_str_2( p, m_sChannelID, nLen );
		p = push_int8_2( p, m_cSource, nLen );
		return (int)(p - pBuffer);
	}

	int CCmdGetPeers::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_str( p, m_sChannelID );
		p = pop_int8( p, m_cSource);
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdGetPeersRet::CCmdGetPeersRet() : 
	CBaseCommand(P2S_CMD_GETPEERS_RET)
	{
	}

	CCmdGetPeersRet::~CCmdGetPeersRet(void)
	{
	}

	int CCmdGetPeersRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_str_2( p, m_sChannelID, nLen);
		m_nPeerCount = (int)m_listPeers.size();
		p = push_int32_2( p, m_nPeerCount, nLen );
		list<PEERINFO>::iterator it = m_listPeers.begin();
		while ( it != m_listPeers.end() )
		{
			PEERINFO& peerinfo = *it;
			int len = sizeof(peerinfo);
			p = push_buf_2( p, (const char *)&peerinfo, len, nLen );
			it++;
		}
		return (int)(p - pBuffer);
	}

	int CCmdGetPeersRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_str( p, m_sChannelID );
		p = pop_int32( p, m_nPeerCount );
		PEERINFO peerinfo;
		short slen = sizeof(peerinfo);
		for ( int i = 0; i < m_nPeerCount; i++ )
		{
			p = pop_buf( p, (char *)&peerinfo, slen );
			m_listPeers.push_back(peerinfo);
		}
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdPeerLogin::CCmdPeerLogin() : 
	CBaseCommand(P2S_CMD_PEERLOGIN)
	{
	}

	CCmdPeerLogin::~CCmdPeerLogin(void)
	{
	}

	int CCmdPeerLogin::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_str_2( p, m_sChannelID, nLen);
		m_nPeerCount = (int)m_listPeers.size();
		p = push_int32_2( p, m_nPeerCount, nLen );
		list<PEERINFO>::iterator it = m_listPeers.begin();
		while ( it != m_listPeers.end() )
		{
			PEERINFO& peerinfo = *it;
			int len = sizeof(peerinfo);
			p = push_buf_2( p, (const char *)&peerinfo, len, nLen );
			it++;
		}
		return (int)(p - pBuffer);
	}

	int CCmdPeerLogin::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_str( p, m_sChannelID );
		p = pop_int32( p, m_nPeerCount );
		PEERINFO peerinfo;
		short slen = sizeof(peerinfo);
		for ( int i = 0; i < m_nPeerCount; i++ )
		{
			p = pop_buf( p, (char *)&peerinfo, slen );
			m_listPeers.push_back(peerinfo);
		}
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdPeerLogout::CCmdPeerLogout() : 
	CBaseCommand(P2S_CMD_PEERLOGOUT)
	{
	}

	CCmdPeerLogout::~CCmdPeerLogout(void)
	{
	}

	int CCmdPeerLogout::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_str_2( p, m_sChannelID, nLen);
		m_nPeerCount = (int)m_listPeers.size();
		p = push_int32_2( p, m_nPeerCount, nLen );
		list<PEERINFO>::iterator it = m_listPeers.begin();
		while ( it != m_listPeers.end() )
		{
			PEERINFO& peerinfo = *it;
			int len = sizeof(peerinfo);
			p = push_buf_2( p, (const char *)&peerinfo, len, nLen );
			it++;
		}
		return (int)(p - pBuffer);
	}

	int CCmdPeerLogout::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_str( p, m_sChannelID );
		p = pop_int32( p, m_nPeerCount );
		PEERINFO peerinfo;
		short slen = sizeof(peerinfo);
		for ( int i = 0; i < m_nPeerCount; i++ )
		{
			p = pop_buf( p, (char *)&peerinfo, slen );
			m_listPeers.push_back(peerinfo);
		}
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdReqShootTo::CCmdReqShootTo() : 
	CBaseCommand(P2S_CMD_REQSHOOTTO)
	{
	}

	CCmdReqShootTo::~CCmdReqShootTo(void)
	{
	}

	int CCmdReqShootTo::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nTargetUserID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdReqShootTo::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nTargetUserID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdReqShootToRet::CCmdReqShootToRet() : 
	CBaseCommand(P2S_CMD_REQSHOOTTO_RET)
	{
	}

	CCmdReqShootToRet::~CCmdReqShootToRet(void)
	{
	}

	int CCmdReqShootToRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nTargetUserID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdReqShootToRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nTargetUserID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdShootArrived::CCmdShootArrived() : 
	CBaseCommand(P2S_CMD_SHOOTARRIVED)
	{
	}

	CCmdShootArrived::~CCmdShootArrived(void)
	{
	}

	int CCmdShootArrived::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_buf_2( p, (const char *)&m_peerinfoSource, sizeof(PEERINFO), nLen );
		return (int)(p - pBuffer);
	}

	int CCmdShootArrived::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		short len = 0;
		p = pop_buf( p, (char *)&m_peerinfoSource, len );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdTcpHeartbeat::CCmdTcpHeartbeat() : 
	CBaseCommand(P2S_CMD_TCP_HEARTBEAT)
	{
	}

	CCmdTcpHeartbeat::~CCmdTcpHeartbeat(void)
	{
	}

	int CCmdTcpHeartbeat::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdTcpHeartbeat::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdClose::CCmdClose() : 
	CBaseCommand(P2S_CMD_CLOSE)
	{
	}

	CCmdClose::~CCmdClose(void)
	{
	}

	int CCmdClose::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdClose::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdCloseRet::CCmdCloseRet() : 
	CBaseCommand(P2S_CMD_CLOSE_RET)
	{
	}

	CCmdCloseRet::~CCmdCloseRet(void)
	{
	}

	int CCmdCloseRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdCloseRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdUdpHeartbeat::CCmdUdpHeartbeat() : 
	CBaseCommand(P2S_CMD_UDP_HEARTBEAT)
	{
	}

	CCmdUdpHeartbeat::~CCmdUdpHeartbeat(void)
	{
	}

	int CCmdUdpHeartbeat::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdUdpHeartbeat::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------


	//------------------------------------------------------------------------------------

	CCmdRoomCreate::CCmdRoomCreate() : 
	CBaseCommand(P2S_CMD_ROOM_CREATE)
	{
	}

	CCmdRoomCreate::~CCmdRoomCreate(void)
	{
	}

	int CCmdRoomCreate::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int8_2( p, m_ucMaxPersons, nLen);
		p = push_int32_2( p, m_nKind, nLen);
		p = push_str_2( p, m_sName, nLen );
		p = push_str_2( p, m_sPassword, nLen );
		return (int)(p - pBuffer);
	}

	int CCmdRoomCreate::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int8( p, (char &)m_ucMaxPersons);
		p = pop_int32( p, m_nKind );
		p = pop_str( p, m_sName );
		p = pop_str( p, m_sPassword );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomCreateRet::CCmdRoomCreateRet() : 
	CBaseCommand(P2S_CMD_ROOM_CREATE_RET)
	{
	}

	CCmdRoomCreateRet::~CCmdRoomCreateRet(void)
	{
	}

	int CCmdRoomCreateRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int8_2( p, m_ucMaxPersons, nLen);
		p = push_int32_2( p, m_nKind, nLen);
		p = push_str_2( p, m_sName, nLen );
		p = push_int32_2( p, m_nID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomCreateRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int8( p, (char &)m_ucMaxPersons);
		p = pop_int32( p, m_nKind );
		p = pop_str( p, m_sName );
		p = pop_int32( p, m_nID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomLogin::CCmdRoomLogin() : 
	CBaseCommand(P2S_CMD_ROOM_LOGIN)
	{
	}

	CCmdRoomLogin::~CCmdRoomLogin(void)
	{
	}

	int CCmdRoomLogin::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomLogin::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomLoginRet::CCmdRoomLoginRet() : 
	CBaseCommand(P2S_CMD_ROOM_LOGIN_RET)
	{
	}

	CCmdRoomLoginRet::~CCmdRoomLoginRet(void)
	{
	}

	int CCmdRoomLoginRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nMasterID, nLen);
		p = push_int32_2( p, m_nRet, nLen);
		p = push_int8_2( p, m_ucIndex, nLen);
		p = push_int8_2( p, m_ucMaxPersons, nLen);
		p = push_int8_2( p, m_ucCurPersons, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomLoginRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID);
		p = pop_int32( p, m_nSessionID);
		p = pop_int32( p, m_nID);
		p = pop_int32( p, m_nMasterID);
		p = pop_int32( p, m_nRet);
		p = pop_int8( p, (char &)m_ucIndex);
		p = pop_int8( p, (char &)m_ucMaxPersons);
		p = pop_int8( p, (char &)m_ucCurPersons);
		return (int)(p - pBuffer); 
	}

	//------------------------------------------------------------------------------------

	CCmdRoomLoginNtf::CCmdRoomLoginNtf() : 
	CBaseCommand(P2S_CMD_ROOM_LOGIN_NTF)
	{
	}

	CCmdRoomLoginNtf::~CCmdRoomLoginNtf(void)
	{
	}

	int CCmdRoomLoginNtf::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nMasterID, nLen);
		p = push_int8_2( p, m_ucMaxPersons, nLen);
		m_ucCurPersons = (unsigned char)m_mapPersons.size();
		p = push_int8_2( p, m_ucCurPersons, nLen);
		map<unsigned char, PEERINFO>::iterator it = m_mapPersons.begin();
		while ( it != m_mapPersons.end() )
		{
			p = push_int8_2( p, it->first, nLen);
			p = push_buf_2( p, (char *)(&it->second), sizeof(PEERINFO), nLen);
			it ++;
		}
		return (int)(p - pBuffer);
	}

	int CCmdRoomLoginNtf::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID);
		p = pop_int32( p, m_nSessionID);
		p = pop_int32( p, m_nID);
		p = pop_int32( p, m_nMasterID);
		p = pop_int8( p, (char &)m_ucMaxPersons);
		p = pop_int8( p, (char &)m_ucCurPersons);
		unsigned char ucIndex;
		PEERINFO peerInfo;
		for ( unsigned char uc = 0; uc < m_ucCurPersons; uc ++)
		{
			short len = 0;
			p = pop_int8( p, (char &)ucIndex);
			p = pop_buf( p, (char *)(&peerInfo), len);
			m_mapPersons[ucIndex] = peerInfo;
		}
		return (int)(p - pBuffer); 
	}

	//------------------------------------------------------------------------------------

	CCmdRoomLogout::CCmdRoomLogout() : 
	CBaseCommand(P2S_CMD_ROOM_LOGOUT)
	{
	}

	CCmdRoomLogout::~CCmdRoomLogout(void)
	{
	}

	int CCmdRoomLogout::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomLogout::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomLogoutRet::CCmdRoomLogoutRet() : 
	CBaseCommand(P2S_CMD_ROOM_LOGOUT_RET)
	{
	}

	CCmdRoomLogoutRet::~CCmdRoomLogoutRet(void)
	{
	}

	int CCmdRoomLogoutRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nRet, nLen);
		p = push_int8_2( p, m_ucIndex, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomLogoutRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID);
		p = pop_int32( p, m_nSessionID);
		p = pop_int32( p, m_nID);
		p = pop_int32( p, m_nRet);
		p = pop_int8( p, (char &)m_ucIndex);
		return (int)(p - pBuffer); 
	}

	//------------------------------------------------------------------------------------

	CCmdRoomLogoutNtf::CCmdRoomLogoutNtf() : 
	CBaseCommand(P2S_CMD_ROOM_LOGOUT_NTF)
	{
	}

	CCmdRoomLogoutNtf::~CCmdRoomLogoutNtf(void)
	{
	}

	int CCmdRoomLogoutNtf::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nPeerID, nLen);
		p = push_int8_2( p, m_ucIndex, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomLogoutNtf::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID);
		p = pop_int32( p, m_nSessionID);
		p = pop_int32( p, m_nID);
		p = pop_int32( p, m_nPeerID);
		p = pop_int8( p, (char &)m_ucIndex);
		return (int)(p - pBuffer); 
	}

	//------------------------------------------------------------------------------------

	CCmdRoomDelete::CCmdRoomDelete() : 
	CBaseCommand(P2S_CMD_ROOM_DELETE)
	{
	}

	CCmdRoomDelete::~CCmdRoomDelete(void)
	{
	}

	int CCmdRoomDelete::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomDelete::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomDeleteRet::CCmdRoomDeleteRet() : 
	CBaseCommand(P2S_CMD_ROOM_DELETE_RET)
	{
	}

	CCmdRoomDeleteRet::~CCmdRoomDeleteRet(void)
	{
	}

	int CCmdRoomDeleteRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nRet, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomDeleteRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID);
		p = pop_int32( p, m_nSessionID);
		p = pop_int32( p, m_nID);
		p = pop_int32( p, m_nRet);
		return (int)(p - pBuffer); 
	}

	//------------------------------------------------------------------------------------

	CCmdRoomModify::CCmdRoomModify() : 
	CBaseCommand(P2S_CMD_ROOM_MODIFY)
	{
	}

	CCmdRoomModify::~CCmdRoomModify(void)
	{
	}

	int CCmdRoomModify::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nOldKind, nLen);
		p = push_int32_2( p, m_nNewKind, nLen);
		p = push_str_2( p, m_sName, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomModify::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		p = pop_int32( p, m_nOldKind );
		p = pop_int32( p, m_nNewKind );
		p = pop_str( p, m_sName );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomModifyRet::CCmdRoomModifyRet() : 
	CBaseCommand(P2S_CMD_ROOM_MODIFY_RET)
	{
	}

	CCmdRoomModifyRet::~CCmdRoomModifyRet(void)
	{
	}

	int CCmdRoomModifyRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nOldKind, nLen);
		p = push_int32_2( p, m_nNewKind, nLen);
		p = push_str_2( p, m_sName, nLen);
		p = push_int32_2( p, m_nRet, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomModifyRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		p = pop_int32( p, m_nOldKind );
		p = pop_int32( p, m_nNewKind );
		p = pop_str( p, m_sName );
		p = pop_int32( p, m_nRet );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomPostData::CCmdRoomPostData() : 
	CBaseCommand(P2S_CMD_ROOM_POST_DATA)
	{
	}

	CCmdRoomPostData::~CCmdRoomPostData(void)
	{
	}

	int m_nUserID;
	int m_nSessionID;
	int m_nID;
	int m_nDataNum; // ÐòºÅ
	unsigned short m_usDataLen;
	char m_szData[ROOM_PACKET_MAX_LEN];

	int CCmdRoomPostData::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nDataNum, nLen);
		p = push_int16_2( p, m_usDataLen, nLen);
		p = push_buf_2( p, m_szData, m_usDataLen, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomPostData::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		p = pop_int32( p, m_nDataNum );
		p = pop_int16( p, (short &)m_usDataLen );
		short len = 0;
		p = pop_buf( p, m_szData, len);
		assert( m_usDataLen == len );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomPostDataRet::CCmdRoomPostDataRet() : 
	CBaseCommand(P2S_CMD_ROOM_POST_DATA_RET)
	{
	}

	CCmdRoomPostDataRet::~CCmdRoomPostDataRet(void)
	{
	}

	int CCmdRoomPostDataRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nDataNum, nLen);
		p = push_int16_2( p, m_usDataLen, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomPostDataRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		p = pop_int32( p, m_nDataNum );
		p = pop_int16( p, (short &)m_usDataLen );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomPostDataBrd::CCmdRoomPostDataBrd() : 
	CBaseCommand(P2S_CMD_ROOM_POST_DATA_BRD)
	{
	}

	CCmdRoomPostDataBrd::~CCmdRoomPostDataBrd(void)
	{
	}

	int CCmdRoomPostDataBrd::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nDataNum, nLen);
		m_ucBroadcastNum = m_listBroadcastIndex.size();
		p = push_int8_2( p, m_ucBroadcastNum, nLen);
		list<unsigned char>::iterator it = m_listBroadcastIndex.begin();
		while ( it != m_listBroadcastIndex.end() )
		{
			p = push_int8_2( p, *it, nLen);
			it ++;
		}
		p = push_int16_2( p, m_usDataLen, nLen);
		p = push_buf_2( p, m_szData, m_usDataLen, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomPostDataBrd::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		p = pop_int32( p, m_nDataNum );
		p = pop_int8( p, (char &)m_ucBroadcastNum );
		for( unsigned char uc = 0; uc < m_ucBroadcastNum; uc ++)
		{
			unsigned char ucIndex;
			p = pop_int8( p, (char &)ucIndex);
			m_listBroadcastIndex.push_back( ucIndex);
		}
		p = pop_int16( p, (short &)m_usDataLen );
		short len = 0;
		p = pop_buf( p, m_szData, len);
		assert( len == m_usDataLen);
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomGetData::CCmdRoomGetData() : 
	CBaseCommand(P2S_CMD_ROOM_GET_DATA)
	{
	}

	CCmdRoomGetData::~CCmdRoomGetData(void)
	{
	}

	int CCmdRoomGetData::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nDataNum, nLen);
		p = push_int32_2( p, m_nPriority, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomGetData::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		p = pop_int32( p, m_nDataNum );
		p = pop_int32( p, m_nPriority );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomGetDataRet::CCmdRoomGetDataRet() : 
	CBaseCommand(P2S_CMD_ROOM_GET_DATA_RET)
	{
	}

	CCmdRoomGetDataRet::~CCmdRoomGetDataRet(void)
	{
	}

	int CCmdRoomGetDataRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nDataNum, nLen);
		p = push_int16_2( p, m_usDataLen, nLen);
		p = push_buf_2( p, m_szData, m_usDataLen, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomGetDataRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		p = pop_int32( p, m_nDataNum );
		p = pop_int16( p, (short &)m_usDataLen );
		short len = 0;
		p = pop_buf( p, m_szData, len);
		assert( m_usDataLen == len );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomClose::CCmdRoomClose() : 
	CBaseCommand(P2S_CMD_ROOM_CLOSE)
	{
	}

	CCmdRoomClose::~CCmdRoomClose(void)
	{
	}

	int CCmdRoomClose::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomClose::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdRoomCloseRet::CCmdRoomCloseRet() : 
	CBaseCommand(P2S_CMD_ROOM_CLOSE_RET)
	{
	}

	CCmdRoomCloseRet::~CCmdRoomCloseRet(void)
	{
	}

	int CCmdRoomCloseRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int32_2( p, m_nRet, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdRoomCloseRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		p = pop_int32( p, m_nRet);
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdMonitorTransdata::CCmdMonitorTransdata() : 
	CBaseCommand(P2S_CMD_MONITOR_TRANSDATA)
	{
	}

	CCmdMonitorTransdata::~CCmdMonitorTransdata(void)
	{
	}

	int CCmdMonitorTransdata::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_int32_2( p, m_nID, nLen);
		p = push_int8_2( p, m_cRemoveAllFlag, nLen);
		p = push_int8_2( p, m_cTransType, nLen);
		p = push_int8_2( p, m_cMonitorType, nLen);
		p = push_int8_2( p, m_cTransWeight, nLen);
		p = push_int8_2( p, m_cSendInterval, nLen);
		p = push_int32_2( p, m_nDstIP, nLen);
		p = push_int16_2( p, m_usDstPort, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdMonitorTransdata::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_int32( p, m_nID );
		p = pop_int8( p, m_cRemoveAllFlag );
		p = pop_int8( p, m_cTransType );
		p = pop_int8( p, m_cMonitorType );
		p = pop_int8( p, m_cTransWeight );
		p = pop_int8( p, m_cSendInterval );
		p = pop_int32( p, m_nDstIP );
		p = pop_int16( p, (short &)m_usDstPort );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

}
