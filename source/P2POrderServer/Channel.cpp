#include "StdAfx.h"
#include ".\channel.h"

CChannel::CChannel(void) :
m_nUpUserID(0),
m_nUserID(0)
{
}

CChannel::~CChannel(void)
{
}

void CChannel::UserLogin( int nUserID )
{
	m_mapUserID.Insert( nUserID, NULL );
}

void CChannel::UserLogout( int nUserID )
{
	m_mapUserID.Erase( nUserID );
	map<int, int>::iterator it = m_mapSourceID.find( nUserID );
	if ( it != m_mapSourceID.end() )
	{
		m_mapSourceID.erase(it);
	}
}

void CChannel::GetPeerList( int nSelfID, list<int>& listPeers )
{
	CKAutoLock l( m_mapUserID.GetCritSec() );

	map<int, void *>::iterator it = m_mapUserID.GetMapPtr()->begin();
	while ( it != m_mapUserID.GetMapPtr()->end() )
	{
		if ( it->first != nSelfID )
		{
			listPeers.push_back( it->first );
		}
		it ++;
	}
}

void CChannel::GetPeerList( int nSelfID, map<int, unsigned char>& mapPeers )
{
	CKAutoLock l( m_mapUserID.GetCritSec() );

	map<int, void *>::iterator it = m_mapUserID.GetMapPtr()->begin();
	while ( it != m_mapUserID.GetMapPtr()->end() )
	{
		if ( it->first != nSelfID )
		{
			if ( m_mapSourceID.find(it->first) != m_mapSourceID.end())
				mapPeers[it->first] = '1';
			else
				mapPeers[it->first] = '0';
		}
		it ++;
	}
}

bool CChannel::IsInChannel( int nUserID )
{
	CKAutoLock l( m_mapUserID.GetCritSec() );
	return ( m_mapUserID.GetMapPtr()->find( nUserID ) != m_mapUserID.GetMapPtr()->end() );
}


