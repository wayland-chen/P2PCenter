/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __INTERFACE_RECORDSET_H__
#define     __INTERFACE_RECORDSET_H__

#include "dbinterface0.h"
#include "row.h"
#include "iconnection.h"
#include "icommand.h"

namespace DB_TOOLKIT
{

	class   IRecordset
	{
		friend class IConnection;
	public:
		IRecordset()
		{ 
			m_pConnection = NULL; 
			m_lLastError = DB_NOERROR;
			m_szQueryString = NULL;
			m_nQueryStringLen = 0;
		}
	    
		IRecordset(IConnection* pConn)
		{
			m_pConnection = pConn;
			m_lLastError = DB_NOERROR;
			m_szQueryString = NULL;
			m_nQueryStringLen = 0;
		}

		virtual ~IRecordset()
		{
			if(m_szQueryString)
			{
				delete m_szQueryString;
				m_szQueryString = NULL;
			}
			m_nQueryStringLen = 0;
			m_pConnection = NULL;
			m_lLastError = DB_NOERROR;
		}

	public:
		virtual void MoveFirst() = 0;
		virtual void MoveNext() = 0;
		virtual void MoveLast() = 0;
		virtual void MovePrevious() = 0;
		virtual bool IsEOF() = 0;
		virtual bool IsBOF() = 0;
		virtual UInt64 GetRecordCount() = 0;

		virtual bool Execute(ICommand* pCommand) = 0;
		virtual bool Execute(const string& strExec) = 0;
		virtual bool Execute(const char* szExec,unsigned long nLength) = 0;
	   

		virtual bool Store(ICommand* pCommand) = 0;
		virtual bool Store(const string& strQuery) = 0;
	    
	    
		/*const string&  GetQueryString() const throw() 
		{ 
			return m_strQuery; 
		}*/
		void GetQueryString(char* szBuf,int nSize) throw()
		{
			::memcpy(szBuf,m_szQueryString,m_nQueryStringLen>nSize?nSize:m_nQueryStringLen);
		}
	    
		void SetActiveConnection(IConnection* pConnection) throw()
		{
			m_pConnection = pConnection;
			m_lLastError = DB_NOERROR;
			m_strLastError.erase();
		}

		virtual bool GetFieldValue(int nIndex,_db_variant_t& vt) = 0;
		virtual bool GetFieldValue(const string& strFieldName,_db_variant_t& vt) = 0;
		virtual int  GetFieldCount() = 0;
		virtual bool GetFieldInfo(int nIndex,CFieldInfo& FieldInfo) = 0;
		virtual bool GetFieldInfo(const string& strFieldName,CFieldInfo& FieldInfo) = 0;
		virtual bool GetCurrentRow(IRow& row) = 0;
		virtual bool Close() = 0;
		virtual bool IsFieldNULL(int nIndex) = 0;
		virtual bool IsFieldNULL(const string& strFieldName) = 0;

	private:
		IRecordset(const IRecordset&);
		const IRecordset& operator = (const IRecordset&);

	protected:
		IConnection*    m_pConnection;
		//string          m_strQuery;
		char*			m_szQueryString;
		int				m_nQueryStringLen;
		string          m_strLastError;
		unsigned long   m_lLastError;
	private:
	};
};

#endif
