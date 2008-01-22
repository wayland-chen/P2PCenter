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

#ifndef _KMACRO_H
#define _KMACRO_H

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x )  \
	if ( NULL != x )       \
	{                      \
	x->Release( );     \
	x = NULL;          \
	}
#endif

#ifndef SAFE_PTR_RELEASE
#define SAFE_PTR_RELEASE(p) \
	if (p) \
	p.Release();\
	p = NULL;
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE( x )	\
	if( NULL != x )		\
	{						\
	delete x;			\
	x = NULL;			\
	}
#endif

#ifndef SAFE_FREE
#define SAFE_FREE( x )	\
	if( NULL != x )		\
{						\
	free(x);			\
	x = NULL;			\
}
#endif

#ifndef SAFE_ARRAYDELETE
#define SAFE_ARRAYDELETE( x )   \
	if ( NULL != x )            \
	{                           \
	delete[] x;             \
	x = NULL;               \
	}
#endif

#ifndef SAFE_CLOSEHANDLE
#define SAFE_CLOSEHANDLE( x )   \
	if ( NULL != x )            \
{                           \
	CloseHandle( x );       \
	x = NULL;               \
}
#endif

#ifndef SAFE_SYSFREE
#define SAFE_SYSFREE( x )   \
	if ( NULL != x )            \
{                           \
	SysFreeString( x );       \
	x = NULL;               \
}
#endif

#ifndef BREAK_IF_FAILED
#define BREAK_IF_FAILED(hr) if(FAILED(hr)) break;
#endif

#ifndef SAFE_ADDREF
#define SAFE_ADDREF( x )    \
	if ( x )                \
{                       \
	x->AddRef();        \
}
#endif

#endif




