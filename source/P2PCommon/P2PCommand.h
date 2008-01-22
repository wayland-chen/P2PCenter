/********************************************************************
	created:	2008/01/11
	created:	11:1:2008   15:49
	author:		Kevin
	
	purpose:	

	email:		kan8888.com@163.com
	msn:		fuwenke@gmail.com
	QQ:		799383417
	site:		opensource.kan8888.com
*********************************************************************/

#pragma once

#include "basecommand.h"
#include <WTypes.h>

namespace P2P_COMMAND
{
	// Peer to peer 的 m_nSessionID 用来区分是否非法回复消息,只有应答方式消息才不意义
	//------------------------------------------------------------------------
	class CP2PCommandQuery
	{
	public:
		static string CommandString( unsigned int unCommand );
	};
	//------------------------------------------------------------------------
	class CP2PBaseCommand : public CBaseCommand
	{
	public:
		CP2PBaseCommand( unsigned int nCommandID ) : CBaseCommand( nCommandID ){};
		virtual ~CP2PBaseCommand(void){};

		DWORD m_dwIP;
		unsigned short m_usPort;
	};
	//------------------------------------------------------------------------
	class CP2PCmdFactory
	{
	public:
		static CP2PBaseCommand* CreateHandshakeCmd( int nUserID, int nSessionID );
		static CP2PBaseCommand* CreateHandshakeRetCmd( int nUserID, int nSessionID );
		static CP2PBaseCommand* CreateGetBlockAreaCmd( int nUserID, int nSessionID, const char* pChannelID );
		static CP2PBaseCommand* CreateGetBlockAreaRetCmd( int nUserID, int nSessionID, const char* pChannelID, map<DWORD, DWORD>& mapAreas );
		static CP2PBaseCommand* CreateGetSegmentCmd( int nUserID, int nSessionID, const char* pChannelID, int nStartPos, int nEndPos );
		static CP2PBaseCommand* CreateGetSegmentRetCmd( int nUserID, int nSessionID, const char* pChannelID, int nBlockNum, int nCheckNum, const char* pBuffer );
		static CP2PBaseCommand* CreateStatUploadCmd( int nUserID, int nSessionID );
		static CP2PBaseCommand* CreateStatUploadRetCmd( int nUserID, int nSessionID, int nUploadBytes );
		static CP2PBaseCommand* CreateExitCmd( int nUserID, int nSessionID );
		static CP2PBaseCommand* CreateHeartbeat( int nUserID, int nSessionID, const char* pChannelID );
	};
	//------------------------------------------------------------------------
	class CCmdHandshake : public CP2PBaseCommand
	{
	public:
		CCmdHandshake();
		virtual ~CCmdHandshake(void);

		virtual int Create( char* pBuffer, int& nLen ); 
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
	};

	class CCmdHandshakeRet : public CP2PBaseCommand
	{
	public:
		CCmdHandshakeRet();
		virtual ~CCmdHandshakeRet(void);

		virtual int Create( char* pBuffer, int& nLen ); 
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
	};
	//------------------------------------------------------------------------
	class CCmdGetBlockArea : public CP2PBaseCommand
	{
	public:
		CCmdGetBlockArea();
		virtual ~CCmdGetBlockArea(void);

		virtual int Create( char* pBuffer, int& nLen );
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
		char m_szChannelID[16];	// 频道ID HASH码
	};

	class CCmdGetBlockAreaRet : public CP2PBaseCommand
	{
	public:
		CCmdGetBlockAreaRet();
		virtual ~CCmdGetBlockAreaRet(void);

		virtual int Create( char* pBuffer, int& nLen ); 
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
		char m_szChannelID[16];	// 频道ID HASH码
		map<DWORD, DWORD> m_mapAreas;	// 闭区间，first 为起始位，second为终止位
	};
	//------------------------------------------------------------------------
	// 一个请求，返回多个结果
	class CCmdGetSegment : public CP2PBaseCommand
	{
	public:
		CCmdGetSegment();
		virtual ~CCmdGetSegment(void);

		virtual int Create( char* pBuffer, int& nLen ); 
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
		char m_szChannelID[16];	// 频道ID HASH码
		int m_nStartPos;		// 起始位
		int m_nEndPos;			// 终止位
	};

	class CCmdGetSegmentRet : public CP2PBaseCommand
	{
	public:
		CCmdGetSegmentRet();
		virtual ~CCmdGetSegmentRet(void);

		virtual int Create( char* pBuffer, int& nLen ); 
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
		char m_szChannelID[16];	// 频道ID HASH码
		int m_nBlockNum;		// 块号
		int m_nCheckSum;		// 缓冲区校验码,算法为四字节简单异或
		char m_szBuffer[UDP_PACKET_LENGTH];
	};
	//------------------------------------------------------------------------
	class CCmdStatUpload : public CP2PBaseCommand
	{
	public:
		CCmdStatUpload();
		virtual ~CCmdStatUpload(void);

		virtual int Create( char* pBuffer, int& nLen ); 
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
		char m_szChannelID[16];	// 频道ID HASH码
	};

	class CCmdStatUploadRet : public CP2PBaseCommand
	{
	public:
		CCmdStatUploadRet();
		virtual ~CCmdStatUploadRet(void);

		virtual int Create( char* pBuffer, int& nLen ); 
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
		char m_szChannelID[16];	// 频道ID HASH码
		int m_nUploadBytes;	// 上传每秒字节数
	};
	//------------------------------------------------------------------------
	class CCmdHeartbeat : public CP2PBaseCommand
	{
	public:
		CCmdHeartbeat();
		virtual ~CCmdHeartbeat(void);

		virtual int Create( char* pBuffer, int& nLen ); 
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
		char m_szChannelID[16];	// 频道ID HASH码
	};
	//------------------------------------------------------------------------
	class CCmdCancelSegment : public CP2PBaseCommand
	{
	public:
		CCmdCancelSegment();
		virtual ~CCmdCancelSegment(void);

		virtual int Create( char* pBuffer, int& nLen ); 
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
		char m_szChannelID[16];	// 频道ID HASH码
		int m_nStartNum;		// 起始号
		int m_nEndNum;			// 结束号 闭区间
	};

	class CCmdCancelSegmentRet : public CP2PBaseCommand
	{
	public:
		CCmdCancelSegmentRet();
		virtual ~CCmdCancelSegmentRet(void);

		virtual int Create( char* pBuffer, int& nLen ); 
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
		char m_szChannelID[16];	// 频道ID HASH码
		int m_nStartNum;		// 起始号 实际
		int m_nEndNum;			// 结束号 闭区间
	};
	//------------------------------------------------------------------------
	class CCmdExit : public CP2PBaseCommand
	{
	public:
		CCmdExit();
		virtual ~CCmdExit(void);

		virtual int Create( char* pBuffer, int& nLen ); 
		virtual int Parse( char* pBuffer, int nLen );

	public:
		int m_nUserID;
		int m_nSessionID;
		char m_szChannelID[16];	// 频道ID HASH码
	};
	//------------------------------------------------------------------------
};

