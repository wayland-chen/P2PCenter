/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __DB_VARIANT_1_H__
#define     __DB_VARIANT_1_H__

#include "dbvariant0.h"
#include "datatype.h"

namespace DB_TOOLKIT
{

	typedef struct _DBVARIANT
	{
		DBVARTYPE    vt;
		union
		{
			char                    cValue;
			short                   sValue;
			long                    lValue;
			Int64                   llValue;
			float                   fValue;
			double                  dValue;
			DBDATE                  dateValue;
			DBTIME                  timeValue;
			DBDATETIME              dtValue;
			DBGUID                  guidValue;
			bool                    bValue;
			DBBSTR                  bstrValue;
			DBBLOB                  blobValue;
		};
	}DBVARIANT;
};

#endif
