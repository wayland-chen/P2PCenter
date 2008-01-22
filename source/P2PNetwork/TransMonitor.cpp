#include "StdAfx.h"
#include ".\transmonitor.h"

CTransMonitor::CTransMonitor(void)
{
}

CTransMonitor::~CTransMonitor(void)
{
}

bool CTransMonitor::PushBuffer( const char* szData, int nLen)
{
	CKDataBuffer* pBuf = new CKDataBuffer();
	if ( pBuf)
	{
		if ( pBuf->StoreData( (unsigned char *)szData, nLen) )
		{
			m_listData.Push( pBuf);
			return true;
		}
	}
	return false;
}

void CTransMonitor::SendData()
{
	if ( !m_udpSocket.IsOpen())
	{
		if ( !m_udpSocket.Bind( CKSockAddr( "0.0.0.0", 0)))
			return;
	}

	char szBuffer[UDP_MAX_BUFFER_LENGTH];
	int nLen = 0;

	CKDataBuffer* pBuf = m_listData.Pop();
	while ( pBuf)
	{
		// 要优化为打成一个总包发送
		if ( (nLen + pBuf->GetLen()) >= UDP_MAX_BUFFER_LENGTH)
		{
			if ( nLen > 0)
				m_udpSocket.SendTo( CKSockAddr(*(in_addr*)m_nDstIP, m_usDstPort), szBuffer, nLen);

			nLen = 0;
		}

		memcpy( szBuffer + nLen, pBuf->GetData(), min( pBuf->GetLen(), UDP_MAX_BUFFER_LENGTH));
		nLen += min( pBuf->GetLen(), UDP_MAX_BUFFER_LENGTH);

		delete pBuf;
		pBuf = m_listData.Pop();
	}

	if ( nLen > 0)
		m_udpSocket.SendTo( CKSockAddr(*(in_addr*)m_nDstIP, m_usDstPort), szBuffer, nLen);
}

void CTransMonitor::ClearData()
{
	m_listData.RemoveAll();
}

