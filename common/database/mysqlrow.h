#ifndef     __MYSQL_ROW_H__
#define     __MYSQL_ROW_H__

#include "mysqllib0.h"
#include "row.h"
#include "mapdt.h"

namespace DB_TOOLKIT
{

	class CMySQLRecordset;

	class CMySQLRow : public IRow
	{
		friend class CMySQLRecordset;
	public:
		CMySQLRow()
		{
			m_Datas = NULL;
		}
		virtual ~CMySQLRow()
		{
		}
		virtual _db_variant_t operator[] (int lIndex) throw (CDBBadParamException,CDBException);
		virtual _db_variant_t operator[] (const string& strFieldName) throw(CDBBadParamException,CDBException);

		virtual _db_variant_t GetAt(int nIndex) throw(CDBBadParamException,CDBException)
		{ return operator [](nIndex); }
	    
		virtual _db_variant_t GetAt(const string& strFieldName) throw(CDBBadParamException,CDBException)
		{ return operator [](strFieldName); }
	    
		virtual unsigned int FieldCount() throw(CDBException);
		
		void Clear() throw()
		{ m_Datas = NULL; }
		
		bool IsValueNULL(int nIndex) throw(CDBBadParamException,CDBException);
		
	protected:
		int IndexOf(const string& strFieldName) throw();
	   
		MYSQL_ROW		     m_Datas;
		
	private:
		CMySQLRow(const CMySQLRow&);
		const CMySQLRow& operator = (const CMySQLRow&);
	};
};

#endif
