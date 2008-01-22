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

#ifndef _KAPP_H
#define _KAPP_H

#include "define/kwin32types.h"
#include "misc/KPtrList.h"
#include "thread/KThread.h"

class CKApp : public CKEventThread
{
public:
	CKApp( void );
	virtual ~CKApp( void );
	bool StartApp( void );
	void StopApp( void );

protected:
    virtual bool Init( void );
    virtual int  Exit( void );

    void         Daemonize( void );
};

extern CKApp* g_pApp;

#endif //ndef _KAPP_H
