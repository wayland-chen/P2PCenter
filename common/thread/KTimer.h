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

#ifndef _KTIMER_H
#define _KTIMER_H

#include "../define/kwin32types.h"
#include "../misc/KPtrList.h"

class CKTimerResponse
{
public:
    virtual void OnTimer( void ) = 0;
};

class CKTimer
{
    friend class CKEventThread;

public:
    typedef enum { Disabled, SingleShot, Repeating } Mode;

    CKTimer( CKTimerResponse* pResponse );
    virtual ~CKTimer( void );

    void    SetResponse( CKTimerResponse* pResponse );
    Mode    GetMode( void );
    UINT32  GetTimeout( void );
    void    SetRelative( UINT32 msec );
    void    SetAbsolute( UINT32 t );
    void    SetRepeating( UINT32 msec );
    void    Disable( void );

    static UINT32 CurrentTime( void );

protected:
    void            Set( UINT32 t );
    CKTimerResponse* GetResponse( void );

private:
    CKTimerResponse* m_pResponse;
    Mode            m_mode;
    UINT32          m_next;
    UINT32          m_interval;
};

typedef CKPtrList<CKTimer> CKTimerList;

#endif //ndef _KTIMER_H
