#ifndef     __INTERFACE_ROW_H__
#define     __INTERFACE_ROW_H__

#include "dbvariant_t.h"

namespace DB_TOOLKIT
{

	class IRecordset;

	class IRow
	{

	public:
		IRow()
		{
			m_pRecordset = NULL;
		}
	    
		IRow(IRecordset* pRecordset)
		{
			m_pRecordset = pRecordset;
		}

		virtual ~IRow()
		{
		}

		virtual _db_variant_t operator[] (int lIndex) = 0;
		virtual _db_variant_t GetAt(int lIndex) = 0;
		virtual _db_variant_t operator[] (const string& strFieldName) = 0;
		virtual _db_variant_t GetAt(const string& strFieldName) = 0;
		virtual unsigned int FieldCount() = 0;
		
	private:
		IRow(const IRow&);
		const IRow& operator = (const IRow&);

	protected:
		IRecordset*     m_pRecordset;
	};
};    


#endif

