#include "StdAfx.h"
#include ".\socketclientmgr.h"

CSocketClientMgr::CSocketClientMgr(void) : 
m_pP2PNetwork(NULL),
m_bConnected(false)
{
}

CSocketClientMgr::~CSocketClientMgr(void)
{
	Release();
}

void CSocketClientMgr::OnConnect()
{
	m_bConnected = true;
}

void CSocketClientMgr::OnDisConnect()
{
	m_bConnected = false;
}

bool CSocketClientMgr::Connect()
{
	m_pP2PNetwork = CreateP2PNetwork();
	if ( !m_pP2PNetwork )
		return false;

	if ( !m_pP2PNetwork->InitModule( this ) )
		return false;

	return true;
}

void CSocketClientMgr::Release()
{
	if ( m_pP2PNetwork )
	{
		m_pP2PNetwork->ReleaseModule();
		m_pP2PNetwork = NULL;
	}
}


