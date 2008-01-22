#include ".\basecommand.h"

//------------------------------------------------------------------------------------
CBaseCommand::CBaseCommand( unsigned int nCommandID ) : 
m_nCommandID(nCommandID),
m_ulSocketID(0),
m_nVersion(PROTOCOL_VERSION)
{
}

CBaseCommand::~CBaseCommand(void)
{
}

unsigned int CBaseCommand::getID( const char* pBuffer )
{
	return *(unsigned int *)pBuffer;
}

int CBaseCommand::Create( char* pBuffer, int& nLen )
{
	int nRet = 0;

	*(unsigned int*)pBuffer = m_nCommandID;
	nRet += sizeof(m_nCommandID);
	*(unsigned int*)(pBuffer + nRet) = m_nVersion;
	nRet += sizeof(m_nCommandID);

	nLen += nRet;
	return nRet;
}

int CBaseCommand::Parse( char* pBuffer, int nLen )
{
	int nRet = 0;

	m_nCommandID = *(unsigned int*)pBuffer;
	nRet += sizeof(m_nCommandID);
	m_nVersion = *(unsigned int*)(pBuffer + nRet);
	nRet += sizeof(m_nVersion);

	nLen -= nRet;
	return nRet;
}

unsigned int CBaseCommand::ID()
{
	return m_nCommandID;
}
//------------------------------------------------------------------------------------

