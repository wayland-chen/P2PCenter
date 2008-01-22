#pragma once

#include "P2PPacketMgr.h"
#include "P2PPeerMgr.h"
#include "io/KFile.h"
#include "CacheFile.h"

class CChannel
{
public:
	CChannel(void);
	virtual ~CChannel(void);

	void Loop();

	void AddPeer( PEERINFO& peerinfo );
	PEERINFO DeletePeer( const DWORD ulID );
	void RequestSegment( DWORD dwStartPos, DWORD dwLength );
	bool ReadSegment( DWORD dwStartPos, char* pBuffer, DWORD& dwLength );

	bool OpenSource( const char* szNodeName, const char* szFilePath, const char* szFileName, const char* szFileHash, const LONGLONG llFileSize, bool bSource );
	string GetID();
	void SetID( string sID );
	bool IsSource();
	LONGLONG Length();
	string ChannelName();

protected:

private:
	void SendPeerNeedBlock();
	void SendGetSegment();

public:
	CP2PPeerMgr     m_peerMgr;
	CP2PPacketMgr   m_packetMgr;

protected:

private:
	string			m_sID;
	char			m_szHashID[16];
	bool			m_bSource;  // 文件源
	LONGLONG		m_llFileSize; // 文件长度
	CKFile			m_file;
	CCacheFile		m_cacheFile;
	string			m_sChannelName;
};
