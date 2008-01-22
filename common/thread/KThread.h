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

// KThread.h: interface for the CKThread class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __THREAD_H__
#define __THREAD_H__

#include "KCritSec.h"
#include "../network/KSocket.h"
#include "KTimer.h"

#ifdef _WIN32
typedef HANDLE  threadobj_t;
typedef DWORD   waittimer_t;
#define INFTIM  INFINITE

#else
#include <pthread.h>
typedef pthread_t       threadobj_t;
typedef int             waittimer_t;
#ifndef INFTIM
#define INFTIM (-1) /* Linux uses -1 but doesn't define INFTIM */
#endif

extern pthread_key_t g_keyself;

#endif

class CKThread
{
public:
	CKThread()
	{
		m_hThread = NULL;
		m_evStop.Set();
	}

	virtual ~CKThread()
	{
	}

	bool StartThread(void)
	{
		bool bCreate = false;
		//如果线程已经创建，则不需要再创建
		if (!m_hThread)
		{ 
			//创建并启动线程函数
#ifdef  WIN32
			DWORD dwThreadID;
			m_hThread = CreateThread(
				NULL,			//the handle cannot be inherited. 
				0,				//the default Thread Stack Size is set
				InitThreadProc,	//线程函数
				this,			//线程函数的参数
				0,				//使线程函数创建完后立即启动
				&dwThreadID	//receives the thread identifier
				);

			bCreate = ( m_hThread != NULL );
#else
			if(0 == pthread_create(&m_hThread, 0, &CKThread::InitThreadProc, this))
				bCreate = true;
#endif
		}

		if ( m_hThread ) {
			m_evStop.Reset();
		}

		return	bCreate;
	}

	void WaitForStop(void)
	{
		if ( !m_hThread ) return;
#ifdef  WIN32
		// 返回线程句柄
		m_evStop.Wait( 0xffffffff );
		HANDLE hThread = (HANDLE)InterlockedExchange((LONG *)&m_hThread, 0);
		if (hThread) {
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
			m_hThread = NULL;
		}
#else
		pthread_join(m_hThread, 0);
#endif 
	}

#ifdef WIN32
	static DWORD WINAPI InitThreadProc(void *pObj)
	{
		CKThread* pth;
		DWORD dwRet = -1;

		pth = (CKThread*)pObj;
		if( pth->Init() )
		{
			dwRet = pth->ThreadProc();
		}

		pth->m_nRetVal = pth->Exit();

		return dwRet;
	}
#else
	static void* InitThreadProc(void *pObj) 
	{
		CKThread* pth;

		pth = (CKThread*)pObj;

		if( pth->Init() )
		{
			pth->ThreadProc();
		}

		pth->m_nRetVal = pth->Exit();

		return 0;
	}
#endif
	
	unsigned long ThreadProc(void)
	{
		ThreadProcMain();
		m_evStop.Set();
		return 0;
	}

	static CKThread* This( void );

protected:
	virtual void ThreadProcMain(void) = 0;

	virtual bool Init( void ){ return true; };
	virtual int  Exit( void ){ return 0; };

	CKEvent		m_evStop;

#ifdef  WIN32
	HANDLE		m_hThread;			// 线程句柄
#else
	pthread_t	m_hThread;  		// 线程句柄
#endif
	int         m_nRetVal;			// 退出返回值 Exit()
};

/*
* CKEventThread -- the scheduler
*/

class CKEventThread : public CKThread
{
public:
	CKEventThread( void );
	~CKEventThread( void );

protected:
	virtual bool Init( void );
	virtual int  Exit( void );

	virtual void ThreadProcMain(void);

private:
	friend class CKSocket;
	friend class CKTimer;
	bool AddStream( CKSocket* pSock );
	void DelStream( CKSocket* pSock );
	void SetStreamSelect( CKSocket* pSock, UINT nWhich );
	void AddTimer( CKTimer* pTimer );
	void DelTimer( CKTimer* pTimer );

	void Heapify( UINT32 now, UINT n );

protected:
	bool		m_bWantStop;

private:
	UINT        m_nSocks;
	UINT        m_nSockAlloc;
	CKSocket**   m_ppSocks;
	waitobj_t*  m_pWaitObjs;
	UINT        m_nTimers;
	UINT        m_nTimerAlloc;
	CKTimer**    m_ppTimers;
};

#endif	//#ifndef __THREAD_H__
