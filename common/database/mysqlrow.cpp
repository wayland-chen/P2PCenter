#include "mysqllib0.h"
#include "mysqlrow.h"
#include "mysqlrecordset.h"

namespace DB_TOOLKIT
{

	_db_variant_t CMySQLRow::operator[] (int lIndex) throw(CDBBadParamException,CDBException)
	{
		_db_variant_t var;
		
		//fwk CMySQLRecordset* pRs = dynamic_cast<CMySQLRecordset*>(m_pRecordset);
		CMySQLRecordset* pRs = (CMySQLRecordset*)(m_pRecordset);
		if(NULL == pRs)
		{
			throw CDBException(UD_ERROR_INVALID_RECORDSET_OBJ,g_ErrorMsg[UD_ERROR_INVALID_RECORDSET_OBJ].m_szError);
		}
		
		if((lIndex<0)||(lIndex>=(int)FieldCount()))
		{
			throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER,g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_szError);
		}
		if(m_Datas[lIndex] != NULL)
		{
			var = m_Datas[lIndex];
			var.ChangeType(CMapGEDTToVarType::ToVarType(pRs->m_FieldInfos[lIndex].GetType()));
			return var;
		}
		throw CDBException(UD_ERROR_FIELD_VALUE_NULL,g_ErrorMsg[UD_ERROR_FIELD_VALUE_NULL_IDX].m_szError);
	}

	unsigned int CMySQLRow::FieldCount() throw(CDBException)
	{
		//fwk CMySQLRecordset* pRs = dynamic_cast<CMySQLRecordset*>(m_pRecordset);
		CMySQLRecordset* pRs = (CMySQLRecordset*)(m_pRecordset);
		if(NULL == pRs)
		{
			throw CDBException(UD_ERROR_INVALID_RECORDSET_OBJ,g_ErrorMsg[UD_ERROR_INVALID_RECORDSET_OBJ].m_szError);
		}
		return pRs->GetFieldCount();
	}

	bool CMySQLRow::IsValueNULL(int nIndex) throw(CDBBadParamException,CDBException)
	{
		//fwk CMySQLRecordset* pRs = dynamic_cast<CMySQLRecordset*>(m_pRecordset);
		CMySQLRecordset* pRs = (CMySQLRecordset*)(m_pRecordset);
		if(NULL == pRs)
		{
			throw CDBException(UD_ERROR_INVALID_RECORDSET_OBJ,g_ErrorMsg[UD_ERROR_INVALID_RECORDSET_OBJ].m_szError);
		}
		if((nIndex<0)||(nIndex>=pRs->GetFieldCount()))
		{
			throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER,g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_szError);
		}
		return m_Datas[nIndex] == NULL;
	}
		

	int CMySQLRow::IndexOf(const string& strFieldName) throw()
	{
		//fwk CMySQLRecordset* pRs = dynamic_cast<CMySQLRecordset*>(m_pRecordset);
		CMySQLRecordset* pRs = (CMySQLRecordset*)(m_pRecordset);
		if(NULL == pRs)
		{
			return -1;
		}
		
		int i=0;
		for(vector<CFieldInfo>::iterator idx=pRs->m_FieldInfos.begin();
		idx!=pRs->m_FieldInfos.end();idx++,i++)
		{
			if(pRs->m_FieldInfos[i].GetName() == strFieldName)
			{
				return i;
			}
		}
		return -1;
	}


	_db_variant_t CMySQLRow::operator[] (const string& strFieldName) throw(CDBBadParamException,CDBException)
	{
		int nIndex = IndexOf(strFieldName);
		if(nIndex != -1)
		{
			return operator [](nIndex);
		}
		throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER,g_ErrorMsg[UD_ERROR_INVALID_PARAMETER_IDX].m_szError);
	}
};

