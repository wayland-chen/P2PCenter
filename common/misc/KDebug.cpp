/****************************************************************************
 *
 *  Copyright (C) 2001-2006 Kevin(fuwenke@gmail.com), Inc. All rights reserved.
 *
 *  This program is free software.  It may be distributed under the terms
 *  in the file LICENSE, found in the top level of the source distribution.
 *
 */

#include <stdarg.h>

#include "KDebug.h"

DEBUG_TYPE CKDebug::m_type = DEBUG_TYPE_DEBUG;

void CKDebug::SetDebugType( DEBUG_TYPE type )
{
	m_type = type;
}

void CKDebug::Debug( DEBUG_TYPE type, const char* pArrFmt, ... )
{
	if ( type >= m_type )
	{
		struct tm *local;
		time_t t =time(NULL);
		local=localtime(&t);
		va_list argList = {0};
		char szBuf[4096] = {0};
		sprintf( szBuf, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d ", 
			local->tm_year + 1900, local->tm_mon, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec );
#ifdef _UNIX
		fputs( szBuf, stderr );
#endif
#ifdef _WIN32
		::OutputDebugString( szBuf );
#endif
		va_start(argList, pArrFmt);
		vsprintf(&szBuf[0], pArrFmt, argList);
		va_end(argList);
		printf(szBuf);
		printf("\n");

#ifdef _UNIX
		fputs( szBuf, stderr );
		fputs( "\n", stderr );
#endif
#ifdef _WIN32
		::OutputDebugString( szBuf );
		::OutputDebugString( "\n" );
#endif
	}
}


