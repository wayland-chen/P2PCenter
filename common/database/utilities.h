/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __UTILITIES_H__
#define     __UTILITIES_H__

#include "datatype.h"
#include "dbvariant0.h"

namespace DB_TOOLKIT
{

	extern DBDATE  StrToDate(const char* szDate);
	extern DBTIME  StrToTime(const char* szTime);
	extern DBDATETIME StrToDateTime(const char* szDateTime);
	extern DBDATE BlobToDate(DBBLOB blob);
	extern DBTIME BlobToTime(DBBLOB blob);
	extern DBDATETIME BlobToDateTime(DBBLOB blob);
	extern DBGUID     StrToGUID(const char* szGUID);
};

#endif
