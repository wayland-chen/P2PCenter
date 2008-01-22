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

#ifndef _DBG_H
#define _DBG_H

#include <assert.h>

typedef enum DEBUG_TYPE
{
	DEBUG_TYPE_DEBUG = 0,
	DEBUG_TYPE_WARNING,
	DEBUG_TYPE_ERROR,
	DEBUG_TYPE_FORCE
};

class CKDebug
{
public:
	static void Debug( DEBUG_TYPE type, const char* pArrFmt, ... );
	static void SetDebugType( DEBUG_TYPE type );
	static DEBUG_TYPE m_type;
};

#include "../define/kwin32types.h"

#ifdef _DEBUG
#define DPRINTF_BUF_SZ 10240
inline void dbgout( const char* fmt, ... )
{
	va_list args;
	TCHAR buf[DPRINTF_BUF_SZ];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	OutputDebugString(buf);
}
#else
inline void dbgout( const char* fmt, ... ){}
#endif

inline void dump_alloc_heaps( void ) {}

#define assert_or_ret(cond) { assert(cond); if( !(cond) ) return; }
#define assert_or_retv(val,cond) { assert(cond); if( !(cond) ) return (val); }

#endif //ndef _DBG_H
