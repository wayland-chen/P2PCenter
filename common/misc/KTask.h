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

#ifndef __KTASK_H_
#define __KTASK_H_

// 未作特别说明的，单位均为 毫秒
class CKTask
{
public:
	CKTask()
	{
		m_dwCreateTime = GetTickCount();
	}

	CKTask(DWORD dwTimeOut, DWORD dwTimeOutMsg = 0)
	{
		m_dwCreateTime = GetTickCount();
		m_dwTimeOut = dwTimeOut;
		m_dwTimeOutMsg = dwTimeOutMsg;
	}

	void Init(DWORD dwTimeOut, DWORD dwTimeOutMsg = 0)
	{
		m_dwCreateTime = GetTickCount();
		m_dwTimeOut = dwTimeOut;
		m_dwTimeOutMsg = dwTimeOutMsg;
	}

	virtual ~CKTask(void)
	{
	}

	void Reset()
	{
		m_dwCreateTime = GetTickCount();
	}

	// nOffset 偏移量, 单位，毫秒
	bool IsTimeOut(int nOffset = 0)
	{
		if ( (GetTickCount() - m_dwCreateTime) > (m_dwTimeOut + nOffset) )
			return true;
		else
			return false;
	}

private:
	// 创建时间
	DWORD m_dwCreateTime;
	// 超时时间
	DWORD m_dwTimeOut;
	// 超时后保存的要发送的消息ID
	DWORD m_dwTimeOutMsg;
};

#endif //__KTASK_H_
