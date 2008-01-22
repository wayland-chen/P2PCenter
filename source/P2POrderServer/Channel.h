#pragma once

#include "misc/KPtrMap.h"
class CChannel
{
public:
	CChannel(void);
	virtual ~CChannel(void);

	void UserLogin( int nUserID );
	void UserLogout( int nUserID );

	void GetPeerList( int nSelfID, list<int>& listPeers );
	void GetPeerList( int nSelfID, map<int, unsigned char>& mapPeers );
	bool IsInChannel( int nUserID );

public:
	string m_sAgent;
	unsigned int m_nUpUserID;
	unsigned int m_nUserID;
	string m_sNodeName;
	string m_sChannelID;
	string m_sFilePath;
	string m_sFileName;
	string m_sFileSize;
	map<int, int> m_mapSourceID;

private:
	// second all is NULL
	CKPtrMap<int, void> m_mapUserID;
};
