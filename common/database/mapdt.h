/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __DATE_TYPE_MAP_H__
#define     __DATE_TYPE_MAP_H__

#include "mysqllib0.h"
#include "dbvariant_t.h"
#include "fieldinfo.h"


namespace DB_TOOLKIT
{

	class CMapGEToMysqlDT
	{
	public:
		static enum_field_types ToMysqlDT(CFieldInfo::FieldType type)
		{
			for(int i=0;i<ENTRY_NUM;i++)
			{
				if(type == s_MapTable[i].GEType)
				{
					return s_MapTable[i].mysqlType;
				}
			}

			return FIELD_TYPE_NULL;
		}

		static CFieldInfo::FieldType ToGEDT(enum_field_types type)
		{
			for(int i=0;i<ENTRY_NUM;i++)
			{
				if(type == s_MapTable[i].mysqlType)
				{
					return s_MapTable[i].GEType;
				}
			}
	        
			return CFieldInfo::ftNULL;
		}

	private:
		struct  MapStruct
		{
			enum_field_types        mysqlType;
			CFieldInfo::FieldType   GEType;
		};
		enum { ENTRY_NUM = 16 };
		static MapStruct s_MapTable[ENTRY_NUM];
	};



	class CMapGEDTToVarType
	{
	public:
		static DBVARTYPE ToVarType(CFieldInfo::FieldType type)
		{        
			for(int i=0;i<ENTRY_NUM;i++)
			{
				if(type == s_MapTable[i].GEType)
				{
					return s_MapTable[i].VarType;
				}
			}
			return VT_DBEMPTY;        
		}

		static CFieldInfo::FieldType ToGEDT(DBVARTYPE type)
		{
			for(int i=0;i<ENTRY_NUM;i++)
			{
				if(type == s_MapTable[i].VarType)
				{
					return s_MapTable[i].GEType;
				}
			}
	        
			return CFieldInfo::ftNULL;
		}

	private:
		enum { ENTRY_NUM = 15 };
		struct MapStruct
		{
			DBVARTYPE					VarType;
			CFieldInfo::FieldType		GEType;
		};
		static MapStruct s_MapTable[ENTRY_NUM];
	};
};

#endif
