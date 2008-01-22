#include "StdAfx.h"
#include ".\p2ppacket.h"

CP2PPacket::CP2PPacket(void) : 
m_dwLastRequest(0),
m_packetStatus(PACKET_STATUS_NONE),
m_pBuffer(NULL)
{
}

CP2PPacket::~CP2PPacket(void)
{
	if ( m_pBuffer )
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
}
