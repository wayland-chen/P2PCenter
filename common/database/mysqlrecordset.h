/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __MYSQL_RECORDSET_H__
#define     __MYSQL_RECORDSET_H__

#include "mysqllib0.h"

namespace DB_TOOLKIT
{

	class CMySQLRow;
	class CMySQLConnection;
 
	class CMySQLRecordset : public IRecordset
	{ 
		friend class CMySQLRow;
	public:
		CMySQLRecordset() throw()
		{
			m_pResult = NULL;
			m_nOffset = 0;
		}
		CMySQLRecordset(IConnection* pConnection) throw()
			:IRecordset(pConnection)
		{
			m_pResult = NULL;
			m_nOffset = (Int64)NULL;
		}
		~CMySQLRecordset() throw()
		{
			if(m_pResult)
			{
				mysql_free_result(m_pResult);
			}
		}
	public:
		virtual void MoveFirst() throw(CDBException);
		virtual void MoveNext() throw(CDBException);
		virtual void MovePrevious() throw(CDBException);
		virtual void MoveLast() throw(CDBException);

		virtual bool IsEOF() throw(CDBException);
		virtual bool IsBOF() throw(CDBException);
		virtual UInt64 GetRecordCount() throw(CDBException);

		virtual bool Store(const string& strQuery) throw(CDBBadQuery,CDBOutOfMemory,CDBException);
		virtual bool Store(ICommand* pCommand) throw(CDBBadQuery,CDBOutOfMemory,CDBBadParamException,CDBException);
		virtual bool Execute(const string& strExec) throw(CDBBadQuery,CDBOutOfMemory,CDBException);
		virtual bool Execute(ICommand* pCommand) throw(CDBBadQuery,CDBOutOfMemory,CDBBadParamException,CDBException);
		virtual bool Execute(const char* szExec,unsigned long nLength) throw(CDBBadQuery,CDBOutOfMemory,CDBException);
	    

		virtual bool GetFieldValue(int nIndex,_db_variant_t& vt) throw(CDBBadParamException,CDBException);
		virtual bool GetFieldValue(const string& strFieldName,_db_variant_t& vt) throw(CDBBadParamException,CDBException);
		virtual int  GetFieldCount() throw(CDBException);
		virtual bool GetFieldInfo(int nIndex,CFieldInfo& FieldInfo) throw(CDBBadParamException,CDBException);
		virtual bool GetFieldInfo(const string& strFieldName,CFieldInfo& FieldInfo) throw(CDBBadParamException,CDBException);
		virtual bool Close() throw()
		{
			if(m_pResult)
			{
				mysql_free_result(m_pResult);
				m_pResult = NULL;
			}
			return true;
		};
		virtual bool GetCurrentRow(IRow& row) throw(CDBBadParamException,CDBException);
		virtual bool IsFieldNULL(int nIndex) throw(CDBBadParamException,CDBException);
		virtual bool IsFieldNULL(const string& strFieldName) throw(CDBBadParamException,CDBException);

		int     MapFieldIndex(const string& strFieldName) throw();

			//¸ñÊ½»¯×Ö·û´®
		virtual unsigned long Escape_String(char *to,const char *from,
					       unsigned long length);
		virtual unsigned long GetBlobValue(int nIndex,void * pData,int nlength);
		

	private:
		bool CheckConnAndResultObj() throw()
		{
		return CheckConnObject() && CheckResultObj();
	        
		}

		bool CheckConnObject() throw();
	    
		bool CheckResultObj() throw()
		{
			if(NULL == m_pResult)
			{
				m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_RECORDSET_OBJ_IDX].m_szError;
				m_lLastError = g_ErrorMsg[UD_ERROR_INVALID_RECORDSET_OBJ_IDX].m_nError;
				return false;
			}
			return true;
		}
		void InitFields() throw();
		void ClearFields() throw()
		{
			m_FieldInfos.clear();
		}

		void ClearQueryString() throw();
		void SetQueryString(const char* szQuery,int nLen) throw(CDBOutOfMemory);


	private:
		CMySQLRecordset(const CMySQLRecordset&);
		const CMySQLRecordset& operator = (const CMySQLRecordset&);

	private:
		MYSQL_RES*  m_pResult;
		Int64       m_nOffset;
		vector<CFieldInfo>  m_FieldInfos;
	};
};

#endif

