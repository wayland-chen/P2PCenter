/*
 *				FILENAME	: mapdt.cpp
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 *				PURPOSE		: A map between general filed type and mysql field type
 *							  A map between general field type and db_variant_t type
 */


#include "mapdt.h"

namespace DB_TOOLKIT
{

	CMapGEToMysqlDT::MapStruct CMapGEToMysqlDT::s_MapTable[CMapGEToMysqlDT::ENTRY_NUM] = 
	{
		{ FIELD_TYPE_TINY,      CFieldInfo::ftByte  },
		{ FIELD_TYPE_SHORT,     CFieldInfo::ftInt16 },
		{ FIELD_TYPE_LONG,      CFieldInfo::ftInt32 },
		{ FIELD_TYPE_LONGLONG,  CFieldInfo::ftInt64 },
		{ FIELD_TYPE_FLOAT,     CFieldInfo::ftSingle },
		{ FIELD_TYPE_DOUBLE,    CFieldInfo::ftDouble },
		{ FIELD_TYPE_DATE,      CFieldInfo::ftDate },
		{ FIELD_TYPE_TIME,      CFieldInfo::ftTime },
		{ FIELD_TYPE_DATETIME,  CFieldInfo::ftDateTime },
		{ FIELD_TYPE_TIMESTAMP, CFieldInfo::ftDateTime },
		{ FIELD_TYPE_BLOB,      CFieldInfo::ftBlob },
		{ FIELD_TYPE_VAR_STRING,CFieldInfo::ftVarString },
		{ FIELD_TYPE_STRING,    CFieldInfo::ftString },
		{ FIELD_TYPE_SET,       CFieldInfo::ftSet },
		{ FIELD_TYPE_ENUM,      CFieldInfo::ftEnum },
		{ FIELD_TYPE_NULL,      CFieldInfo::ftNULL }
	};


	CMapGEDTToVarType::MapStruct CMapGEDTToVarType::s_MapTable[CMapGEDTToVarType::ENTRY_NUM] = 
	{
		{ VT_DBI1,      CFieldInfo::ftByte  },
		{ VT_DBI2,      CFieldInfo::ftInt16 },
		{ VT_DBI4,      CFieldInfo::ftInt32 },
		{ VT_DBI8,      CFieldInfo::ftInt64 },
		{ VT_DBSINGLE,  CFieldInfo::ftSingle },
		{ VT_DBDOUBLE,  CFieldInfo::ftDouble },
		{ VT_DBDATE,    CFieldInfo::ftDate },
		{ VT_DBTIME,    CFieldInfo::ftTime },
		{ VT_DBDATETIME,CFieldInfo::ftDateTime },
		{ VT_DBBLOB,    CFieldInfo::ftBlob },
		{ VT_DBBSTR,    CFieldInfo::ftString },
		{ VT_DBBSTR,    CFieldInfo::ftVarString },
		{ VT_DBBLOB,    CFieldInfo::ftBlob },
		{ VT_DBBOOLEAN, CFieldInfo::ftBoolean },
		{ VT_DBNULL,    CFieldInfo::ftNULL }
	};
};
