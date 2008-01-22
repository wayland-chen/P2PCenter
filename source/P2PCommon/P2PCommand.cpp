#include ".\p2pcommand.h"

namespace P2P_COMMAND
{
	//------------------------------------------------------------------------------------
		
	string CP2PCommandQuery::CommandString( unsigned int unCommand )
	{
		string sRet = "-- no name command --";

		switch( unCommand ) 
		{
		case P2P_CMD_HANDSHAKE:
			sRet = "P2P_CMD_HANDSHAKE";
			break;

		case P2P_CMD_HANDSHAKE_RET:
			sRet = "P2P_CMD_HANDSHAKE_RET";
			break;

		case P2P_CMD_GETBLOCK_AREA:
			sRet = "P2P_CMD_GETBLOCK_AREA";
			break;

		case P2P_CMD_GETBLOCK_AREA_RET:
			sRet = "P2P_CMD_GETBLOCK_AREA_RET";
			break;

		case P2P_CMD_GETSEGMENT:
			sRet = "P2P_CMD_GETSEGMENT";
			break;

		case P2P_CMD_GETSEGMENT_RET:
			sRet = "P2P_CMD_GETSEGMENT_RET";
			break;

		case P2P_CMD_STAT_UPLOAD:
			sRet = "P2P_CMD_STAT_UPLOAD";
			break;

		case P2P_CMD_STAT_UPLOAD_RET:
			sRet = "P2P_CMD_STAT_UPLOAD_RET";
			break;

		case P2P_CMD_HEARTBEAT:
			sRet = "P2P_CMD_HEARTBEAT";
			break;

		case P2P_CMD_EXIT:
			sRet = "P2P_CMD_EXIT";
			break;

		default:
			break;
		}

		return sRet;
	}

	//------------------------------------------------------------------------------------

	CP2PBaseCommand* CP2PCmdFactory::CreateHandshakeCmd( int nUserID, int nSessionID )
	{
		CCmdHandshake* pCmd = new CCmdHandshake();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
		}

		return pCmd;
	}

	//------------------------------------------------------------------------------------

	CP2PBaseCommand* CP2PCmdFactory::CreateHandshakeRetCmd( int nUserID, int nSessionID )
	{
		CCmdHandshakeRet* pCmd = new CCmdHandshakeRet();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
		}

		return pCmd;
	}

	//------------------------------------------------------------------------------------

	CP2PBaseCommand* CP2PCmdFactory::CreateGetBlockAreaCmd( int nUserID, int nSessionID, const char* pChannelID )
	{
		CCmdGetBlockArea* pCmd = new CCmdGetBlockArea();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			memcpy( pCmd->m_szChannelID, pChannelID, sizeof(pCmd->m_szChannelID) );
		}

		return pCmd;
	}

	//------------------------------------------------------------------------------------

	CP2PBaseCommand* CP2PCmdFactory::CreateGetBlockAreaRetCmd( int nUserID, int nSessionID, const char* pChannelID, map<DWORD, DWORD>& mapAreas )
		{
		CCmdGetBlockAreaRet* pCmd = new CCmdGetBlockAreaRet();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			memcpy( pCmd->m_szChannelID, pChannelID, sizeof(pCmd->m_szChannelID) );
			pCmd->m_mapAreas = mapAreas;
		}

		return pCmd;
	}

	//------------------------------------------------------------------------------------

	CP2PBaseCommand* CP2PCmdFactory::CreateGetSegmentCmd( int nUserID, int nSessionID, const char* pChannelID, int nStartPos, int nEndPos )
	{
		CCmdGetSegment* pCmd = new CCmdGetSegment();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			memcpy( pCmd->m_szChannelID, pChannelID, sizeof(pCmd->m_szChannelID) );
			pCmd->m_nStartPos = nStartPos;
			pCmd->m_nEndPos = nEndPos;
		}

		return pCmd;
	}

	//------------------------------------------------------------------------------------

	CP2PBaseCommand* CP2PCmdFactory::CreateGetSegmentRetCmd( int nUserID, int nSessionID, const char* pChannelID, int nBlockNum, int nCheckNum, const char* pBuffer )
	{
		CCmdGetSegmentRet* pCmd = new CCmdGetSegmentRet();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			memcpy( pCmd->m_szChannelID, pChannelID, sizeof(pCmd->m_szChannelID) );
			pCmd->m_nBlockNum = nBlockNum;
			pCmd->m_nCheckSum = nCheckNum;
			memcpy( pCmd->m_szBuffer, pBuffer, sizeof(pCmd->m_szBuffer) );
		}

		return pCmd;
	}

	//------------------------------------------------------------------------------------

	CP2PBaseCommand* CP2PCmdFactory::CreateStatUploadCmd( int nUserID, int nSessionID )
	{
		CCmdStatUpload* pCmd = new CCmdStatUpload();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
		}

		return pCmd;
	}

	//------------------------------------------------------------------------------------

	CP2PBaseCommand* CP2PCmdFactory::CreateStatUploadRetCmd( int nUserID, int nSessionID, int nUploadBytes )
	{
		CCmdStatUploadRet* pCmd = new CCmdStatUploadRet();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			pCmd->m_nUploadBytes = nUploadBytes;
		}

		return pCmd;
	}

	//------------------------------------------------------------------------------------

	CP2PBaseCommand* CP2PCmdFactory::CreateHeartbeat( int nUserID, int nSessionID, const char* pChannelID )
	{
		CCmdHeartbeat* pCmd = new CCmdHeartbeat();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
			memcpy( pCmd->m_szChannelID, pChannelID, sizeof(pCmd->m_szChannelID) );
		}

		return pCmd;
	}

	//------------------------------------------------------------------------------------

	CP2PBaseCommand* CP2PCmdFactory::CreateExitCmd( int nUserID, int nSessionID )
	{
		CCmdExit* pCmd = new CCmdExit();
		if ( pCmd )
		{
			pCmd->m_nUserID = nUserID;
			pCmd->m_nSessionID = nSessionID;
		}

		return pCmd;
	}

	//____________________________________________________________________________________

	CCmdHandshake::CCmdHandshake() : 
	CP2PBaseCommand( P2P_CMD_HANDSHAKE )
	{
	}

	CCmdHandshake::~CCmdHandshake(void)
	{
	}

	int CCmdHandshake::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdHandshake::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdHandshakeRet::CCmdHandshakeRet() : 
	CP2PBaseCommand( P2P_CMD_HANDSHAKE_RET )
	{
	}

	CCmdHandshakeRet::~CCmdHandshakeRet(void)
	{
	}

	int CCmdHandshakeRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdHandshakeRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdGetBlockArea::CCmdGetBlockArea() : 
	CP2PBaseCommand( P2P_CMD_GETBLOCK_AREA )
	{
	}

	CCmdGetBlockArea::~CCmdGetBlockArea(void)
	{
	}

	int CCmdGetBlockArea::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_buf_2( p, m_szChannelID, sizeof(m_szChannelID), nLen);
		return (int)(p - pBuffer);
	}

	int CCmdGetBlockArea::Parse( char* pBuffer, int nLen )
	{
		short len = 0;
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_buf( p, m_szChannelID, len );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdGetBlockAreaRet::CCmdGetBlockAreaRet() : 
	CP2PBaseCommand( P2P_CMD_GETBLOCK_AREA_RET )
	{
	}

	CCmdGetBlockAreaRet::~CCmdGetBlockAreaRet(void)
	{
	}

	int CCmdGetBlockAreaRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_buf_2( p, m_szChannelID, sizeof(m_szChannelID), nLen);
		// 为防止超长缓冲溢出，忽略超出部分
		while ( m_mapAreas.size() * 8 > UDP_PACKET_LENGTH)
		{
			map<DWORD, DWORD>::reverse_iterator rit = m_mapAreas.rbegin();
			m_mapAreas.erase( rit->first);
		}
		int nCount = (int)m_mapAreas.size();
		p = push_int32_2( p, nCount, nLen);
		map<DWORD, DWORD>::iterator it = m_mapAreas.begin();
		while ( it != m_mapAreas.end() )
		{
			p = push_int32_2( p, it->first, nLen);
			p = push_int32_2( p, it->second, nLen);
			it ++;
		}
		return (int)(p - pBuffer);
	}

	int CCmdGetBlockAreaRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		short len = 0;
		p = pop_buf( p, m_szChannelID, len );
		int nCount = 0;
		p = pop_int32( p, nCount );
		for( int i = 0; i < nCount; i++ )
		{
			int nStart = 0, nEnd = 0;
			p = pop_int32( p, nStart );
			p = pop_int32( p, nEnd );
			m_mapAreas.insert( map<DWORD, DWORD>::value_type( nStart, nEnd ) );
		}
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdGetSegment::CCmdGetSegment() : 
	CP2PBaseCommand( P2P_CMD_GETSEGMENT )
	{
	}

	CCmdGetSegment::~CCmdGetSegment(void)
	{
	}

	int CCmdGetSegment::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_buf_2( p, m_szChannelID, sizeof(m_szChannelID), nLen);
		p = push_int32_2( p, m_nStartPos, nLen);
		p = push_int32_2( p, m_nEndPos, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdGetSegment::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		short len = 0;
		p = pop_buf( p, m_szChannelID, len );
		p = pop_int32( p, m_nStartPos );
		p = pop_int32( p, m_nEndPos );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdGetSegmentRet::CCmdGetSegmentRet() : 
	CP2PBaseCommand( P2P_CMD_GETSEGMENT_RET )
	{
	}

	CCmdGetSegmentRet::~CCmdGetSegmentRet(void)
	{
	}

	int CCmdGetSegmentRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_buf_2( p, m_szChannelID, sizeof(m_szChannelID), nLen);
		p = push_int32_2( p, m_nBlockNum, nLen);
		p = push_int32_2( p, m_nCheckSum, nLen);
		p = push_buf_2( p, m_szBuffer, sizeof(m_szBuffer), nLen);
		return (int)(p - pBuffer);
	}

	int CCmdGetSegmentRet::Parse( char* pBuffer, int nLen )
	{
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		short len = 0;
		p = pop_buf( p, m_szChannelID, len );
		p = pop_int32( p, m_nBlockNum );
		p = pop_int32( p, m_nCheckSum );
		p = pop_buf( p, m_szBuffer, len );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdStatUpload::CCmdStatUpload() : 
	CP2PBaseCommand( P2P_CMD_STAT_UPLOAD )
	{
	}

	CCmdStatUpload::~CCmdStatUpload(void)
	{
	}

	int CCmdStatUpload::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_buf_2( p, m_szChannelID, sizeof(m_szChannelID), nLen);
		return (int)(p - pBuffer);
	}

	int CCmdStatUpload::Parse( char* pBuffer, int nLen )
	{
		short len = 0;
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_buf( p, m_szChannelID, len );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdStatUploadRet::CCmdStatUploadRet() : 
	CP2PBaseCommand( P2P_CMD_STAT_UPLOAD_RET )
	{
	}

	CCmdStatUploadRet::~CCmdStatUploadRet(void)
	{
	}

	int CCmdStatUploadRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_buf_2( p, m_szChannelID, sizeof(m_szChannelID), nLen);
		p = push_int32_2( p, m_nUploadBytes, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdStatUploadRet::Parse( char* pBuffer, int nLen )
	{
		short len = 0;
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_buf( p, m_szChannelID, len );
		p = pop_int32( p, m_nUploadBytes );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdCancelSegment::CCmdCancelSegment() : 
	CP2PBaseCommand( P2P_CMD_CANCELSEGMENT )
	{
	}

	CCmdCancelSegment::~CCmdCancelSegment(void)
	{
	}

	int CCmdCancelSegment::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_buf_2( p, m_szChannelID, sizeof(m_szChannelID), nLen);
		p = push_int32_2( p, m_nStartNum, nLen);
		p = push_int32_2( p, m_nEndNum, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdCancelSegment::Parse( char* pBuffer, int nLen )
	{
		short len = 0;
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_buf( p, m_szChannelID, len );
		p = pop_int32( p, m_nStartNum );
		p = pop_int32( p, m_nEndNum );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdCancelSegmentRet::CCmdCancelSegmentRet() : 
	CP2PBaseCommand( P2P_CMD_CANCELSEGMENT_RET )
	{
	}

	CCmdCancelSegmentRet::~CCmdCancelSegmentRet(void)
	{
	}

	int CCmdCancelSegmentRet::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_buf_2( p, m_szChannelID, sizeof(m_szChannelID), nLen);
		p = push_int32_2( p, m_nStartNum, nLen);
		p = push_int32_2( p, m_nEndNum, nLen);
		return (int)(p - pBuffer);
	}

	int CCmdCancelSegmentRet::Parse( char* pBuffer, int nLen )
	{
		short len = 0;
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_buf( p, m_szChannelID, len );
		p = pop_int32( p, m_nStartNum );
		p = pop_int32( p, m_nEndNum );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdHeartbeat::CCmdHeartbeat() : 
	CP2PBaseCommand( P2P_CMD_HEARTBEAT )
	{
	}

	CCmdHeartbeat::~CCmdHeartbeat(void)
	{
	}

	int CCmdHeartbeat::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_buf_2( p, m_szChannelID, sizeof(m_szChannelID), nLen);
		return (int)(p - pBuffer);
	}

	int CCmdHeartbeat::Parse( char* pBuffer, int nLen )
	{
		short len = 0;
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_buf( p, m_szChannelID, len );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------

	CCmdExit::CCmdExit() : 
	CP2PBaseCommand( P2P_CMD_EXIT )
	{
	}

	CCmdExit::~CCmdExit(void)
	{
	}

	int CCmdExit::Create( char* pBuffer, int& nLen )
	{
		int nRet = CBaseCommand::Create( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = push_int32_2( p, m_nUserID, nLen);
		p = push_int32_2( p, m_nSessionID, nLen);
		p = push_buf_2( p, m_szChannelID, sizeof(m_szChannelID), nLen);
		return (int)(p - pBuffer);
	}

	int CCmdExit::Parse( char* pBuffer, int nLen )
	{
		short len = 0;
		int nRet = CBaseCommand::Parse( pBuffer, nLen );
		char* p = pBuffer + nRet;
		p = pop_int32( p, m_nUserID );
		p = pop_int32( p, m_nSessionID );
		p = pop_buf( p, m_szChannelID, len );
		return (int)(p - pBuffer);
	}

	//------------------------------------------------------------------------------------
};

