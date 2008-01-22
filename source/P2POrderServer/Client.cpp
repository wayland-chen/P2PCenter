#include "StdAfx.h"
#include ".\client.h"

CClient::CClient(void):
m_bUdpAvail(false),
m_bConnAvail(false),
m_nSessionID(0),
m_ulSocketID(0)
{
}

CClient::~CClient(void)
{
}

bool CClient::IsCanWork()
{
	return m_bConnAvail && m_bUdpAvail;
}
