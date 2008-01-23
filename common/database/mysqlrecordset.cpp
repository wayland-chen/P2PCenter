/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 *				PURPOSE		: implemention of class CMySQLRecordset
 */


#include "mysqllib0.h"
#include "mysqlrecordset.h"
#include "mysqlcommand.h"
#include "mysqlconnection.h"
#include "mysqlrow.h"
#include "mapdt.h"
#include <typeinfo>

namespace DB_TOOLKIT
{

	inline bool CMySQLRecordset::CheckConnObject() throw()
	{
		CMySQLConnection* pConn = (CMySQLConnection*)(m_pConnection);
		//fwkCMySQLConnection* pConn = dynamic_cast<CMySQLConnection*>(m_pConnection);
		
		if((NULL==pConn) || (NULL==pConn->m_pMySQL))
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_CONNECTION_OBJ_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_INVALID_CONNECTION_OBJ_IDX].m_nError;
			return false;
		}
		
		return true;
	}

	void CMySQLRecordset::InitFields() throw()
	{
		if(!CheckConnAndResultObj())
		{
			return;
		}
	    
		ClearFields();

		int nCount = mysql_field_count(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
		m_FieldInfos.reserve(nCount);
		CFieldInfo FieldInfo;
		MYSQL_FIELD* pField = NULL;
		for(int i=0;i<nCount;i++)
		{
			pField = mysql_fetch_field_direct(m_pResult,i);
			FieldInfo.m_strName = pField->name;
			FieldInfo.m_Type = CMapGEToMysqlDT::ToGEDT(pField->type);
			FieldInfo.m_nLength = pField->length;
			FieldInfo.m_lAttributes = pField->flags;
			m_FieldInfos.push_back(FieldInfo);
		}
	}

	int CMySQLRecordset::MapFieldIndex(const string& strFieldName) throw()
	{
		int nCount = m_FieldInfos.size();
		for(int i=0;i<nCount;i++)
		{
			if(m_FieldInfos[i].m_strName == strFieldName)
			{
				return i;
			}
		}
		m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_szError;
		m_lLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_nError;

		return -1;
	}

	void CMySQLRecordset::ClearQueryString() throw()
	{
		if(m_szQueryString)
		{
			delete m_szQueryString;
			m_szQueryString = NULL;
		}
		m_nQueryStringLen = 0;
	}

	void CMySQLRecordset::SetQueryString(const char* szQuery,int nLen) throw(CDBOutOfMemory)
	{
		if(nLen > m_nQueryStringLen)
		{
			ClearQueryString();
			if(NULL==(m_szQueryString=new char[nLen]))
			{
				m_lLastError = g_ErrorMsg[UD_ERROR_OUT_OF_MEMORY_IDX].m_nError;
				m_strLastError = g_ErrorMsg[UD_ERROR_OUT_OF_MEMORY_IDX].m_szError;
				throw CDBOutOfMemory(m_lLastError,m_strLastError);
			}

		}
		::memcpy(m_szQueryString,szQuery,nLen);
		m_nQueryStringLen = nLen;
	}

	void CMySQLRecordset::MoveFirst() throw(CDBException)
	{
		if(!CheckResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}

		m_nOffset = 0;
		mysql_data_seek(m_pResult,0);
	}

	void CMySQLRecordset::MoveLast() throw(CDBException)
	{
		if(!CheckResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}
	    
		m_nOffset = mysql_num_rows(m_pResult)-1;
		mysql_data_seek(m_pResult,m_nOffset);
	}

	void CMySQLRecordset::MoveNext() throw(CDBException)
	{
		if(!CheckResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}

		if(IsEOF())
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_RST_EOF_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_RST_EOF_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}
		++m_nOffset;
		mysql_data_seek(m_pResult,m_nOffset);
	}

	void CMySQLRecordset::MovePrevious() throw(CDBException)
	{
		if(!CheckResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}

		if(IsBOF())
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_RST_BOF_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_RST_BOF_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}
		--m_nOffset;
		mysql_data_seek(m_pResult,m_nOffset);
	}

	bool CMySQLRecordset::IsEOF() throw(CDBException)
	{
		if(!CheckResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}

		return (m_nOffset == (Int64)mysql_num_rows(m_pResult));
	}

	bool CMySQLRecordset::IsBOF() throw(CDBException)
	{
		if(!CheckResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}
	    
		return (m_nOffset == -1);
	}

	UInt64 CMySQLRecordset::GetRecordCount() throw(CDBException)
	{
		if(!CheckResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}

		return mysql_num_rows(m_pResult);
	    
	}   

	bool CMySQLRecordset::Store(ICommand* pCommand) throw(CDBBadQuery,CDBOutOfMemory,CDBBadParamException,CDBException)
	{
		if(!CheckConnObject())
		{
			throw CDBException(m_lLastError,m_strLastError);			
		}
		//fwkCMySQLCommand* pCmd = dynamic_cast<CMySQLCommand*>(pCommand);
		CMySQLCommand* pCmd = (CMySQLCommand*)(pCommand);
		if((NULL==pCommand) || !pCmd)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_COMMAND_OBJ_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_INVALID_COMMAND_OBJ_IDX].m_nError;
			throw CDBBadParamException(m_lLastError,m_strLastError);
		}
		
		ClearFields();
		if(m_pResult)
		{
			mysql_free_result(m_pResult);
			m_pResult = NULL;
		}
		m_pConnection->SetState(m_pConnection->GetState()|IConnection::CONNECTION_STATE_FETCHING);
		string str;
		switch(pCommand->GetCommandType())
		{
		case ICommand::CmdUnknown:
		case ICommand::CmdText:
			Store(pCommand->GetCommandText());
			break;
		case ICommand::CmdStoredProc:
			{
				str = "CALL ";
				str += pCommand->GetCommandText();
				str += "(";
				IParameterCollection* pParaColl = pCommand->GetParameters();
				for(int i=0;i<pParaColl->GetParamCount();i++)
				{
					if(((*pParaColl)[i].m_Direction == IParameter::Input) ||
						((*pParaColl)[i].m_Direction == IParameter::InputAndOutput))
					{
						str += (string)((*pParaColl)[i].m_Value);
						str += ",";
						continue;
					}
					str += "@";
					str += (*pParaColl)[i].m_strName;
					str += ",";
				}

				str.erase(str.find_last_of(','),1);
				str += ")";
				if(0 != mysql_real_query(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL
					,str.c_str(),str.length()))
				{
					m_lLastError = mysql_errno(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
					m_strLastError = mysql_error(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
					m_pConnection->SetState(m_pConnection->GetState() & ~(unsigned long)(IConnection::CONNECTION_STATE_FETCHING));
					throw CDBBadQuery(m_lLastError,m_strLastError);
				}
				
				str = "SELECT ";
				for(int ii=0;ii<pParaColl->GetParamCount();ii++)
				{
					if(((*pParaColl)[ii].m_Direction == IParameter::Output) ||
						((*pParaColl)[ii].m_Direction == IParameter::InputAndOutput))
					{
						str += "@";
						str += (string)((*pParaColl)[ii].m_strName);
						str += ",";
						continue;
					}
				}
				str.erase(str.find_last_of(','),1);

				Store(str);
				
				for(int j=0;j<pParaColl->GetParamCount();j++)
				{
					if(((*pParaColl)[j].m_Direction == IParameter::Output) ||
						((*pParaColl)[j].m_Direction == IParameter::InputAndOutput))
					{
						GetFieldValue("@"+(*pParaColl)[j].m_strName,(*pParaColl)[j].m_Value);
						(*pParaColl)[j].m_Value.ChangeType(
							CMapGEDTToVarType::ToVarType((*pParaColl)[j].m_Type));
						
					}
				}
			}
			break;
		case ICommand::CmdTableDirect:
			m_strLastError = g_ErrorMsg[UD_ERROR_NOT_IMPLEMENT_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_NOT_IMPLEMENT_IDX].m_nError;
			m_pConnection->SetState(m_pConnection->GetState() & ~(unsigned long)(IConnection::CONNECTION_STATE_FETCHING));
			throw CDBException(m_lLastError,m_strLastError);
		default:
			assert(false);
			break;
		}
		m_pConnection->SetState(m_pConnection->GetState() & ~(unsigned long)(IConnection::CONNECTION_STATE_FETCHING));

		return true;
	}

	bool CMySQLRecordset::Store(const string& strQuery) throw(CDBBadQuery,CDBOutOfMemory,CDBException)
	{
		if(!CheckConnObject())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}
	    
		ClearQueryString();
		ClearFields();
		
		if(m_pResult)
		{
			mysql_free_result(m_pResult);
			m_pResult = NULL;
		}
		m_pConnection->SetState(m_pConnection->GetState()|IConnection::CONNECTION_STATE_FETCHING);
		if(0 != mysql_real_query(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL
								,strQuery.c_str(),strQuery.length()))
		{
			m_lLastError = mysql_errno(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
			m_strLastError = mysql_error(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
			m_pConnection->SetState(m_pConnection->GetState() & ~(unsigned long)(IConnection::CONNECTION_STATE_FETCHING));
			throw CDBBadQuery(m_lLastError,m_strLastError);
		}
	    
		SetQueryString(strQuery.c_str(),strQuery.length());
		
		m_pResult = mysql_store_result(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
		if(NULL == m_pResult)
		{
			m_lLastError = mysql_errno(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
			m_strLastError = mysql_error(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
			m_pConnection->SetState(m_pConnection->GetState() & ~(unsigned long)(IConnection::CONNECTION_STATE_FETCHING));
			throw CDBBadQuery(m_lLastError,m_strLastError);
		}
		m_pConnection->SetState(m_pConnection->GetState() & ~(unsigned long)(IConnection::CONNECTION_STATE_FETCHING));
		InitFields();

		return true;
	}

	bool CMySQLRecordset::Execute(ICommand* pCommand) throw(CDBBadQuery,CDBOutOfMemory,CDBBadParamException,CDBException)
	{
		ClearFields();
		ClearQueryString();

		if(m_pResult)
		{
			mysql_free_result(m_pResult);
			m_pResult = NULL;
		}

		if(NULL == pCommand)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_szError;
			m_lLastError  = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_nError;
			throw CDBBadParamException(m_lLastError,m_strLastError);
		}
		if(!CheckConnObject())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}
		
		m_pConnection->SetState(m_pConnection->GetState()|IConnection::CONNECTION_STATE_EXECUTING);
		try
		{
			m_pConnection->Execute(pCommand);
			m_pConnection->SetState(m_pConnection->GetState() & ~(unsigned long)(IConnection::CONNECTION_STATE_EXECUTING));
		}
		catch(...)
		{
			m_strLastError = mysql_error(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
			m_lLastError = mysql_errno(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
			m_pConnection->SetState(m_pConnection->GetState() & ~(unsigned long)(IConnection::CONNECTION_STATE_EXECUTING));
			throw CDBBadQuery(m_lLastError,m_strLastError);
		}
		
		//m_strQuery = pCommand->GetCommandText();
		SetQueryString(pCommand->GetCommandText().c_str(),pCommand->GetCommandText().length());
		m_pConnection->SetState(m_pConnection->GetState() & ~(unsigned long)(IConnection::CONNECTION_STATE_EXECUTING));
		
		return true;
	}

	bool CMySQLRecordset::Execute(const string& strExec) throw(CDBBadQuery,CDBOutOfMemory,CDBException)
	{
 		return Execute(strExec.c_str(),strExec.length());
	}

	bool CMySQLRecordset::Execute(const char* szExec,unsigned long nLength) throw(CDBBadQuery,CDBOutOfMemory,CDBException)
	{
		ClearFields();
		//m_strQuery.erase();
		ClearQueryString();

		if(m_pResult)
		{
			mysql_free_result(m_pResult);
			m_pResult = NULL;
		}
		if(!CheckConnObject())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}
		
		m_pConnection->SetState(m_pConnection->GetState()|IConnection::CONNECTION_STATE_EXECUTING);
		try
		{
			m_pConnection->Execute(szExec,nLength);
		}
		catch(...)
		{
			m_strLastError = mysql_error(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
			m_lLastError = mysql_errno(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
			m_pConnection->SetState(m_pConnection->GetState() & ~(unsigned long)(IConnection::CONNECTION_STATE_EXECUTING));
			throw CDBBadQuery(m_lLastError,m_strLastError);
		}
		//m_strQuery = strExec;
		SetQueryString(szExec,nLength);
		m_pConnection->SetState(m_pConnection->GetState() & ~(unsigned long)(IConnection::CONNECTION_STATE_EXECUTING));
		
		return true;
	}


	int  CMySQLRecordset::GetFieldCount() throw(CDBException)
	{
		if(!CheckResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}
		return mysql_num_fields(m_pResult);
	}

	bool CMySQLRecordset::GetFieldValue(const string& strFieldName,_db_variant_t& vt) 
											throw(CDBBadParamException,CDBException)
	{
		int nIndex = MapFieldIndex(strFieldName);
		if(-1 == nIndex)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_nError;
			throw CDBBadParamException(m_lLastError,m_strLastError);
		}
		return GetFieldValue(nIndex,vt);
	}

	bool CMySQLRecordset::GetFieldValue(int nIndex,_db_variant_t& vt) 
											throw(CDBBadParamException,CDBException)
	{
		if(!CheckConnAndResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}
		if((nIndex>=(int)(m_FieldInfos.size())) || (nIndex<0))
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_nError;
			throw CDBBadParamException(m_lLastError,m_strLastError);
		}
		if(IsEOF())
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_RST_EOF_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_RST_EOF_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}
		if(IsBOF())
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_RST_BOF_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_RST_BOF_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}
	    
		MYSQL_ROW  row = mysql_fetch_row(m_pResult);
		mysql_data_seek(m_pResult,m_nOffset);
		if(NULL == row)
		{
			m_strLastError = mysql_error(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
			m_lLastError = mysql_errno(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL);
			throw CDBException(m_lLastError,m_strLastError);
		}
	    
		switch(m_FieldInfos[nIndex].m_Type)
		{
		case CFieldInfo::ftNULL:
			break;
		case CFieldInfo::ftByte:
			vt = (signed char)strtol(row[nIndex],NULL,10);
			break;
		case CFieldInfo::ftInt16:
		case CFieldInfo::ftUInt16:
			vt = (signed short)strtol(row[nIndex],NULL,10);
			break;
		case CFieldInfo::ftInt32:
		case CFieldInfo::ftUInt32:
			vt = (signed long)strtol(row[nIndex],NULL,10);
			break;
		case CFieldInfo::ftInt64:
		case CFieldInfo::ftUInt64:
			vt = (Int64)strtoll(row[nIndex],NULL,10);
			break;
		case CFieldInfo::ftDate:
			vt = StrToDate(row[nIndex]);
			break;
		case CFieldInfo::ftTime:
			vt = StrToTime(row[nIndex]);
			break;
		case CFieldInfo::ftDateTime:
			vt = StrToDateTime(row[nIndex]);
			break;
		case CFieldInfo::ftGUID:
			vt = StrToGUID(row[nIndex]);
			break;
		case CFieldInfo::ftSingle:
			vt = (float)strtod(row[nIndex],NULL);
			break;
		case CFieldInfo::ftDouble:
			vt = strtod(row[nIndex],NULL);
			break;
		case CFieldInfo::ftString:
			vt = (const char*)row[nIndex];
			break;
		case CFieldInfo::ftVarString:
			vt = (const char*)row[nIndex];
			break;
		case CFieldInfo::ftBlob:
		//	vt = (const char*)BlobToData(row[nIndex]);
			break;
		default:
			assert(false);
			break;
		}

		return true;
	}

	bool CMySQLRecordset::IsFieldNULL(int nIndex) throw(CDBBadParamException,CDBException)
	{
		if(!CheckConnAndResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}
		if(nIndex >= (int)(m_FieldInfos.size()) || nIndex<0)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_nError;
			throw CDBBadParamException(m_lLastError,m_strLastError);
		}
		return !IS_NOT_NULL(m_FieldInfos[nIndex].m_lAttributes);
	}

	bool CMySQLRecordset::IsFieldNULL(const string& strFieldName) throw(CDBBadParamException,CDBException)
	{
		int nIndex = MapFieldIndex(strFieldName);
		if(-1 == nIndex)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_nError;
			throw CDBBadParamException(m_lLastError,m_strLastError);
		}
		return IsFieldNULL(nIndex);
	}

	bool CMySQLRecordset::GetFieldInfo(int nIndex,CFieldInfo& FieldInfo) throw(CDBBadParamException,CDBException)
	{
		if(!CheckConnAndResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}
		if(nIndex >= (int)(m_FieldInfos.size()) || nIndex<0)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_nError;
			throw CDBBadParamException(m_lLastError,m_strLastError);
		}
		FieldInfo = m_FieldInfos[nIndex];

		return true;
	}

	bool CMySQLRecordset::GetFieldInfo(const string& strFieldName,CFieldInfo& FieldInfo) throw(CDBBadParamException,CDBException)
	{
		int nIndex = MapFieldIndex(strFieldName);
		if(-1 == nIndex)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_nError;
			throw CDBBadParamException(m_lLastError,m_strLastError);
		}
		return GetFieldInfo(nIndex,FieldInfo);
	}

	bool CMySQLRecordset::GetCurrentRow(IRow& row) throw(CDBBadParamException,CDBException)
	{
		if(!CheckResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}
		if(IsEOF())
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_RST_EOF_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_RST_EOF_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}
		if(IsBOF())
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_RST_BOF_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_RST_BOF_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}
		try
		{
			//fwkCMySQLRow& myrow = dynamic_cast<CMySQLRow&>(row);
			CMySQLRow& myrow = (CMySQLRow&)(row);
			MYSQL_ROW prow = mysql_fetch_row(m_pResult);
			myrow.m_pRecordset = this;
			//mysql_fetch_row会将当前记录的指针下移，而这不符合此方法的行为
			//所以重置指针
			mysql_data_seek(m_pResult,m_nOffset);
			//unsigned long nCount = GetFieldCount();
			myrow.Clear();
			myrow.m_Datas = prow;
	        
		}
		catch(std::bad_cast)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_ROW_OBJ_IDX].m_szError;
			m_lLastError   = g_ErrorMsg[UD_ERROR_INVALID_ROW_OBJ_IDX].m_nError;
			throw CDBBadParamException(m_lLastError,m_strLastError);
		}

		return true;
	}

	unsigned long CMySQLRecordset::GetBlobValue(int nIndex,void * pData,int nlength)
	{
		if(nIndex<0||pData == NULL||nlength<=0)
		{
			return 0;
		}
		if(!CheckResultObj())
		{
			throw CDBException(m_lLastError,m_strLastError);
		}
		if(IsEOF())
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_RST_EOF_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_RST_EOF_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}
		if(IsBOF())
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_RST_BOF_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_RST_BOF_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}
		try
		{
			
			MYSQL_ROW prow = mysql_fetch_row(m_pResult);
		
			mysql_data_seek(m_pResult,m_nOffset);
			memcpy(pData,prow[nIndex],nlength);	        
		}
		catch(std::bad_cast)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_ROW_OBJ_IDX].m_szError;
			m_lLastError   = g_ErrorMsg[UD_ERROR_INVALID_ROW_OBJ_IDX].m_nError;
			throw CDBBadParamException(m_lLastError,m_strLastError);
		}

		return nlength;
	}

	unsigned long CMySQLRecordset::Escape_String(char *to,const char *from, unsigned long length)
	{
		if(to == NULL||from == NULL||length == 0)
		{
			return 0;
		}

		if(m_pResult)
		{
			mysql_free_result(m_pResult);
			m_pResult = NULL;
		}

		return mysql_real_escape_string(static_cast<CMySQLConnection*>(m_pConnection)->m_pMySQL,to,from,length);
	}
};
