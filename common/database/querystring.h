/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __INTERFACE_QUERY_H__
#define     __INTERFACE_QUERY_H__

#include "datatype.h"
#include "dbvariant_t.h"
#include <string>
using namespace std;

namespace DB_TOOLKIT
{

	class CQueryString
	{
	public:
		const string& Preview()
		{
			return m_strQuery;
		}
		void Clear()
		{
			m_strQuery.erase();
		}
		CQueryString& operator << (const char* szValue)
		{
			m_strQuery.append(szValue); return *this;
		}
		CQueryString& operator << (const string& strValue)
		{
			m_strQuery.append(strValue); return *this;
		}
		CQueryString& operator << (signed char cValue)
		{
			char s[6];sprintf(s,"%c",cValue);m_strQuery.append(s);return *this;
		}
		CQueryString& operator << (unsigned char cValue)
		{
			char s[6];sprintf(s,"%c",cValue);m_strQuery.append(s);return *this;
		}
		CQueryString& operator << (unsigned short sValue)
		{
			char s[40];sprintf(s,"%hu",sValue); m_strQuery.append(s);return *this;
		}
		CQueryString& operator << (signed short sValue)
		{
			char s[40];sprintf(s,"%hd",sValue);m_strQuery.append(s);return *this;
		}
		CQueryString& operator << (unsigned int nValue)
		{
			char s[40];sprintf(s,"%u",nValue);m_strQuery.append(s);return *this;
		}
		CQueryString& operator << (signed int nValue)
		{
			char s[40];sprintf(s,"%d",nValue);m_strQuery.append(s);return *this;
		}
		CQueryString& operator << (unsigned long lValue)
		{
			char s[64];sprintf(s,"%lu",lValue);m_strQuery.append(s);return *this;
		}
		CQueryString& operator << (signed long lValue)
		{
			char s[64];sprintf(s,"%ld",lValue);m_strQuery.append(s);return *this;
		}
		CQueryString& operator << (UInt64 nValue)
		{
			char s[128];
	#ifdef OS_IS_MSWINDOWS
			sprintf(s,"%I64u",nValue);m_strQuery.append(s);
	#else
			sprintf(s,"%llu",nValue);m_strQuery.append(s);
	#endif
			return *this;
		}
		CQueryString& operator << (Int64 nValue)
		{
			char s[128];
	#ifdef OS_IS_MSWINDOWS
			sprintf(s,"%I64d",nValue);m_strQuery.append(s);
	#else
			sprintf(s,"%lld",nValue);m_strQuery.append(s);
	#endif
			return *this;
		}
		CQueryString& operator<< (float fValue)
		{
			char s[128];
			sprintf(s,"%f",fValue);
			m_strQuery.append(s);
			return *this;
		}
		CQueryString& operator<< (double dValue)
		{
			char s[128];
			sprintf(s,"%f",dValue);
			m_strQuery.append(s);
			return *this;
		}
		CQueryString& operator << (DBDATE date)
		{
			char s[128];
			sprintf(s,"%d-%02d-%02d",date.year,date.month,date.day);
			m_strQuery.append(s);
			return *this;
		}
		CQueryString& operator << (DBTIME time)
		{
			char s[128];
			sprintf(s,"%02d:%02d:%02d",time.hour,time.minute,time.second);
			m_strQuery.append(s);
			return *this;
		}
		CQueryString& operator << (DBDATETIME dt)
		{
			char s[128];
			sprintf(s,"%d-%02d-%02d %02d:%02d:%02d",dt.year,dt.month,dt.day,dt.hour,dt.minute,dt.second);
			m_strQuery.append(s);
			return *this;
		}
		CQueryString& operator << (DBGUID guid)
		{
			char s[256];
			sprintf(s,"%08lX-%04X-%04X-%04X-%02X%02X%02X%02X%02X%02X",
				guid.Data1,guid.Data2,guid.Data3,guid.Data4,
				guid.Data[0],guid.Data[1],guid.Data[2],
				guid.Data[3],guid.Data[4],guid.Data[5]);
			m_strQuery.append(s);
			return *this;
		}
		CQueryString& operator << (const _db_variant_t& var)
		{
			switch(var.vt)
			{
			case VT_DBI1:
				*this<<var.cValue;
				break;
			case VT_DBUI1:
				*this<<(unsigned char)(var.cValue);
				break;
			case VT_DBI2:
				*this<<var.sValue;
				break;
			case VT_DBUI2:
				*this<<(unsigned short)(var.sValue);
				break;
			case VT_DBI4:
				*this<<var.lValue;
				break;
			case VT_DBUI4:
				*this<<(unsigned long)(var.lValue);
				break;
			case VT_DBI8:
				*this<<var.llValue;
				break;
			case VT_DBUI8:
				*this<<(UInt64)(var.llValue);
				break;
			case VT_DBSINGLE:
				*this<<var.fValue;
				break;
			case VT_DBDOUBLE:
				*this<<var.dValue;
				break;
			case VT_DBDATE:
				*this<<var.dateValue;
				break;
			case VT_DBTIME:
				*this<<var.timeValue;
				break;
			case VT_DBDATETIME:
				*this<<var.dtValue;
				break;
			case VT_DBGUID:
				*this<<var.guidValue;
				break;
			case VT_DBBSTR:
				*this<<BSTR_TO_CCHAR(var.bstrValue);
				break;
			case VT_DBBLOB:
				*this<<(const char*)(BLOB_TO_REALDATA(var.blobValue));
				break;
			default:
				assert(false);
				break;
			}
			return *this;
		}
	protected:
		string          m_strQuery;
	};
};

#endif
