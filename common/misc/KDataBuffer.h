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
// KDataBuffer.h: interface for the UDPPacket class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __KDATABUFFER_H__
#define __KDATABUFFER_H__

class CKDataBuffer  
{
public:
	CKDataBuffer()
{
	m_szData = NULL;
	m_nLen = 0;
}

virtual ~CKDataBuffer()
{
	if ( m_szData )
	{
		delete []m_szData;
		m_szData = NULL;
	}
}

bool StoreData(unsigned char* szData, unsigned int nLen, bool bStr = false)
{
	if ( m_szData )
	{
		delete []m_szData;
		m_szData = NULL;
	}

	if (!szData || 0 == nLen )
		return true;

	m_nLen = 0;

	if ( bStr )
		m_szData = new unsigned char[ nLen + 1 ];
	else
		m_szData = new unsigned char[ nLen ];

	if ( !m_szData )
		return false;

	memcpy( m_szData, szData, nLen);
	m_nLen = nLen;

	if ( bStr )
	{
		m_szData[nLen] = 0;
	}

	return true;
}

unsigned char* GetData(void)
{ 
	return m_szData; 
}

unsigned char* GetBuffer(void)
{ 
	return m_szData; 
}

unsigned int GetLen(void)
{ 
	return m_nLen; 
}

unsigned int GetSize(void)
{ 
	return m_nLen; 
}

protected:
	unsigned char*	m_szData;
	unsigned int	m_nLen;
};

#endif 
