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

//CKCritSec.h
//
#ifndef __KCRITSEC_H__
#define __KCRITSEC_H__

#include "../define/kwin32types.h"

#ifdef  WIN32
//#include <windows.h>
#pragma warning(disable:4800)
#else
#include <pthread.h>
#include <semaphore.h>
#endif

class CKCritSec
{
public:
	CKCritSec() {
#ifdef  WIN32
		InitializeCriticalSection(&m_CritSec);         
#else
		pthread_mutexattr_t attr= {PTHREAD_MUTEX_RECURSIVE_NP};
		pthread_mutex_init(&m_CritSec, &attr);
#endif
	};

    ~CKCritSec() {
#ifdef  WIN32
		DeleteCriticalSection(&m_CritSec);         
#else
		pthread_mutex_destroy(&m_CritSec);
#endif
    };

    void Lock() {
#ifdef  WIN32
		EnterCriticalSection(&m_CritSec);
#else
		pthread_mutex_lock(&m_CritSec);
#endif
    };

    void Unlock() {
#ifdef  WIN32
		LeaveCriticalSection(&m_CritSec);
#else
		pthread_mutex_unlock(&m_CritSec);
#endif
    };

protected:
    #ifdef  WIN32
		CRITICAL_SECTION    m_CritSec;
	#else
		pthread_mutex_t     m_CritSec;
	#endif
};

/****************************************************************************
*
* CKMutex
*
****************************************************************************/

class CKMutex
{
public :
	CKMutex::CKMutex( void )
	{
#ifdef _UNIX
		// We could use PTHREAD_MUTEX_ERRORCHECK_NP for Linux debug builds
		pthread_mutex_init( &m_mutex, NULL );
#endif
#ifdef _WIN32
		m_mutex = ::CreateMutex( NULL, TRUE, NULL );
#endif
	}

	CKMutex::~CKMutex( void )
	{
#ifdef _UNIX
		pthread_mutex_destroy( &m_mutex );
#endif
#ifdef _WIN32
		::CloseHandle( m_mutex );
#endif
	}

	void CKMutex::Lock( void )
	{
#ifdef _UNIX
		pthread_mutex_lock( &m_mutex );
#endif
#ifdef _WIN32
		::WaitForSingleObject( m_mutex, INFINITE );
#endif
	}

	void CKMutex::Unlock( void )
	{
#ifdef _UNIX
		pthread_mutex_unlock( &m_mutex );
#endif
#ifdef _WIN32
		::ReleaseMutex( m_mutex );
#endif
	}

private:
#ifdef _UNIX
	pthread_mutex_t m_mutex;
#endif
#ifdef _WIN32
	HANDLE          m_mutex;
#endif
};

//typedef CKMutex CKCritSec;

/****************************************************************************
*
* CKSemaphore
*
****************************************************************************/

class CKSemaphore
{
public:
	CKSemaphore::CKSemaphore( UINT nCount )
	{
	#ifdef _UNIX
		// We could use PTHREAD_MUTEX_ERRORCHECK_NP for Linux debug builds
		pthread_mutex_init( &m_mutex, NULL );
		pthread_cond_init( &m_cond, NULL );
		m_count = nCount;
	#endif
	#ifdef _WIN32
		m_semaphore = ::CreateSemaphore( NULL, (LONG)nCount, MAXLONG, NULL );
	#endif
	}

	CKSemaphore::~CKSemaphore( void )
	{
	#ifdef _UNIX
		pthread_cond_destroy( &m_cond );
		pthread_mutex_destroy( &m_mutex );
	#endif
	#ifdef _WIN32
		::CloseHandle( m_semaphore );
	#endif
	}

	void CKSemaphore::Lock( void )
	{
	#ifdef _UNIX
		pthread_mutex_lock( &m_mutex );
		while( m_count == 0 )
		{
			pthread_cond_wait( &m_cond, &m_mutex );
		}
		m_count--;
		pthread_mutex_unlock( &m_mutex );
	#endif
	#ifdef _WIN32
		::WaitForSingleObject( m_semaphore, INFINITE );
	#endif
	}

	void CKSemaphore::Unlock( void )
	{
	#ifdef _UNIX
		pthread_mutex_lock( &m_mutex );
		m_count++;
		pthread_mutex_unlock( &m_mutex );
		pthread_cond_signal( &m_cond );
	#endif
	#ifdef _WIN32
		::ReleaseSemaphore( m_semaphore, 1, NULL );
	#endif
	}

private:
#ifdef _UNIX
	pthread_mutex_t m_mutex;
	pthread_cond_t  m_cond;
	UINT            m_count;
#endif
#ifdef _WIN32
	HANDLE          m_semaphore;
#endif
};



class CKAutoLock
{
public:
    CKAutoLock(CKCritSec * plock)
    {
        m_pLock = plock;
        m_pLock->Lock();
    };

    ~CKAutoLock() {
        m_pLock->Unlock();
    };

protected:
    CKCritSec * m_pLock;
};


class CKEvent
{
public:
    CKEvent(bool bManualReset = false)
	{
#ifdef  WIN32
		m_hEvent = CreateEvent(NULL, bManualReset, false, NULL);
#else
		sem_init(&m_hEvent, 0, 0);
#endif
	};

    virtual ~CKEvent()
	{
#ifdef  WIN32
		CloseHandle(m_hEvent);
#else
		sem_destroy(&m_hEvent);
#endif	
	};
public:
    bool Set(void) 
	{
		bool    bRet = false;
#ifdef  WIN32
		bRet = ::SetEvent(m_hEvent);
#else
		int nVal = sem_post(&m_hEvent);
		bRet = ++nVal;
#endif
		return bRet;
	};

    bool Wait(unsigned long ulTimeout)
	{
		if (ulTimeout == 0)
			ulTimeout = 0xffffffff;

		bool    bRet = false;
#ifdef  WIN32
		if(WAIT_OBJECT_0 == WaitForSingleObject(m_hEvent, ulTimeout))
			bRet = true;
#else
		sem_wait(&m_hEvent);		// ´Ë´¦ ulTimeout ´ýÐÞ¸Ä
		bRet = true;
#endif
		return bRet;
    };

    bool Reset(void) 
	{ 
		bool    bRet = false;
#ifdef  WIN32
		bRet = ::ResetEvent(m_hEvent);
#else
		int nVal =0;

		sem_getvalue(&m_hEvent, &nVal);
		while(nVal)
		{
			sem_wait(&m_hEvent);
			sem_getvalue(&m_hEvent, &nVal);
		}
		bRet = true;
#endif
		return bRet;
	};

protected:
#ifdef  WIN32
	HANDLE  m_hEvent;
#else
	sem_t   m_hEvent;
#endif

};

#endif
