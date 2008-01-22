// MSSqlDB.cpp: implementation of the CMSSqlDB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MSSqlDB.h"

#include <string>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=450DFB6801F5
CMSSqlDB::CMSSqlDB()
{
}

//##ModelId=450DFB6801F6
CMSSqlDB::~CMSSqlDB()
{
}

//##ModelId=450DFB680247
BOOL CMSSqlDB::InitConfig()
{
	TCHAR FilePath[MAX_PATH]; 
    GetModuleFileName(NULL,FilePath,MAX_PATH); 
    (_tcsrchr(FilePath,'\\'))[1] = 0; 
	lstrcat(FilePath,_T("P2POrderClient.ini")); 

	SysConfig.FromType = GetPrivateProfileInt(
		"Config",
		"FromType",
		0,
		FilePath);

	if( SysConfig.FromType < 1)
		return false;

	GetPrivateProfileString(
		"Config",        // section name
		"FromStr",        // key name
		"",        // default string
		SysConfig.FromStr,  // destination buffer
		sizeof(SysConfig.FromStr),  // size of destination buffer
		FilePath        // initialization file name
		);

	if(strlen(SysConfig.FromStr) < 1)
		return false;

	GetPrivateProfileString(
		"DBConfig",        // section name
		"TableName",        // key name
		"",        // default string
		SysConfig.TableName,  // destination buffer
		sizeof(SysConfig.TableName),  // size of destination buffer
		FilePath        // initialization file name
		);

	if(strlen(SysConfig.TableName) < 1)
		return false;

	GetPrivateProfileString(
		"DBConfig",        // section name
		"FileNameField",        // key name
		"",        // default string
		SysConfig.FileNameField,  // destination buffer
		sizeof(SysConfig.FileNameField),  // size of destination buffer
		FilePath        // initialization file name
		);

	if(strlen(SysConfig.FileNameField) < 1)
		return false;

	GetPrivateProfileString(
		"DBConfig",        // section name
		"MediaAddrIDField",        // key name
		"",        // default string
		SysConfig.MediaAddrIDField,  // destination buffer
		sizeof(SysConfig.MediaAddrIDField),  // size of destination buffer
		FilePath        // initialization file name
		);

	if(strlen(SysConfig.MediaAddrIDField) < 1)
		return false;

	GetPrivateProfileString(
		"DBConfig",        // section name
		"FilePathField",        // key name
		"",        // default string
		SysConfig.FilePathField,  // destination buffer
		sizeof(SysConfig.FilePathField),  // size of destination buffer
		FilePath        // initialization file name
		);

	if(strlen(SysConfig.FilePathField) < 1)
		return false;

	GetPrivateProfileString(
		"DBConfig",        // section name
		"FileHashField",        // key name
		"",        // default string
		SysConfig.FileHashField,  // destination buffer
		sizeof(SysConfig.FileHashField),  // size of destination buffer
		FilePath        // initialization file name
		);

	if(strlen(SysConfig.FileHashField) < 1)
		return false;

	GetPrivateProfileString(
		"DBConfig",        // section name
		"FileSizeField",        // key name
		"",        // default string
		SysConfig.FileSizeField,  // destination buffer
		sizeof(SysConfig.FileSizeField),  // size of destination buffer
		FilePath        // initialization file name
		);

	if(strlen(SysConfig.FileSizeField) < 1)
		return false;

	GetPrivateProfileString(
		"DBConfig",        // section name
		"NodeNameField",        // key name
		"",        // default string
		SysConfig.NodeNameField,  // destination buffer
		sizeof(SysConfig.NodeNameField),  // size of destination buffer
		FilePath        // initialization file name
		);

	if(strlen(SysConfig.NodeNameField) < 1)
		return false;

	GetPrivateProfileString(
		"DBConfig",        // section name
		"ModifyDateField",        // key name
		"",        // default string
		SysConfig.ModifyDateField,  // destination buffer
		sizeof(SysConfig.ModifyDateField),  // size of destination buffer
		FilePath        // initialization file name
		);

	if(strlen(SysConfig.ModifyDateField) < 1)
		return false;

	GetPrivateProfileString(
		"DBConfig",        // section name
		"PublishDateField",        // key name
		"",        // default string
		SysConfig.PublishDateField,  // destination buffer
		sizeof(SysConfig.PublishDateField),  // size of destination buffer
		FilePath        // initialization file name
		);

	if(strlen(SysConfig.PublishDateField) < 1)
		return false;
	
	sprintf(m_pcConnection,
		"%s",
		SysConfig.FromStr);
	
	return true;

}

//##ModelId=450DFB680203
bool CMSSqlDB::CheckLocalFileIsExist(char *pPath)
{
	return false;

}

//##ModelId=450DFB6802DF
char * CMSSqlDB::ExecSP(CString sSql)
{
	try
	{
		char szSql[1024];
		memset(szSql, 0, sizeof(szSql));
		strncpy(szSql, sSql, 1024);

		m_pCommand->ActiveConnection = m_pLocalConnectPtr;

		VARIANT vtRoyalty;

		char szRet[1024];
		memset(szRet, 0, sizeof(szRet));
		vtRoyalty.pcVal = szRet;
		m_pCommand->Parameters->Append(
			m_pCommand->CreateParameter("Return", adChar, adParamReturnValue,sizeof(szRet),vtRoyalty));

		std::string m_szName;
		std::string m_szPasswd;

		m_szName.assign("5000");
		m_szPasswd.assign("xxx");
		
		vtRoyalty.pcVal = (char *)m_szName.c_str();
		m_pCommand->Parameters->Append(
			m_pCommand->CreateParameter("loginname", adChar, adParamInput,m_szName.size(),vtRoyalty));

		vtRoyalty.pcVal = (char *)m_szPasswd.c_str();
		m_pCommand->Parameters->Append(
			m_pCommand->CreateParameter("msgbody", adChar, adParamInput,m_szPasswd.size(),vtRoyalty));
			

		strcpy(szSql, "sp_receive");

		m_pCommand->CommandText = szSql;//"P_KS_ADDUSER";
		m_pCommand->CommandType = adCmdStoredProc;//adCmdText;//adCmdStoredProc;
		HRESULT _hr = m_pCommand->Execute(NULL,NULL,adCmdStoredProc);

		if (FAILED(_hr))
			return NULL;
		else
		{
			return m_pCommand->Parameters->GetItem("Return")->GetValue().pcVal;

		}
	}
	catch(_com_error e)///捕捉异常
	{
		CString errormessage;
		errormessage.Format("连接数据库失败!\r\n错误信息:%s",e.ErrorMessage());
		
		//AfxMessageBox(errormessage);///显示错误信息
	}

	return NULL;
}

bool CMSSqlDB::GetFileList( map<int, FileInfo>& mapOldFiles,  map<int, FileInfo>& mapNewFiles)
{
	if ( IsOpen())
	{
		try
		{
			char szSql[256];
			sprintf( szSql, "SELECT * FROM %s", SysConfig.TableName);

			HRESULT _hr = m_pRecordsetPtr->Open(
				szSql,
				_variant_t((IDispatch*)m_pLocalConnectPtr,true),
				adOpenStatic,
				adLockOptimistic,
				adCmdText
				);

			if (FAILED(_hr))
			{
				m_pRecordsetPtr->Close();
				return FALSE;
			}

			while( !m_pRecordsetPtr->adoEOF)
			{
				FileInfo fInfo;
				_variant_t vtReturn;

				vtReturn = m_pRecordsetPtr->GetCollect(_variant_t( SysConfig.MediaAddrIDField));
				if( VT_NULL != vtReturn.vt)
					fInfo.MediaAddrID = (unsigned int)vtReturn;

				vtReturn = m_pRecordsetPtr->GetCollect(_variant_t( SysConfig.FileNameField));
				if( VT_NULL != vtReturn.vt)
					fInfo.FileName = (LPCTSTR)(_bstr_t)vtReturn;

				vtReturn = m_pRecordsetPtr->GetCollect(_variant_t( SysConfig.FilePathField));
				if( VT_NULL != vtReturn.vt )
					fInfo.FilePath = (LPCTSTR)(_bstr_t)vtReturn;

				vtReturn = m_pRecordsetPtr->GetCollect(_variant_t( SysConfig.FileHashField));
				if( VT_NULL != vtReturn.vt )
					fInfo.FileHash = (LPCTSTR)(_bstr_t)vtReturn;

				vtReturn = m_pRecordsetPtr->GetCollect(_variant_t( SysConfig.NodeNameField));
				if( VT_NULL != vtReturn.vt)
					fInfo.NodeName = (LPCTSTR)(_bstr_t)vtReturn;

				vtReturn = m_pRecordsetPtr->GetCollect(_variant_t( SysConfig.FileSizeField));
				if( VT_NULL != vtReturn.vt )
					fInfo.FileSize = (LONGLONG)vtReturn;

				vtReturn = m_pRecordsetPtr->GetCollect(_variant_t( SysConfig.ModifyDateField));
				if( VT_NULL != vtReturn.vt )
					fInfo.ModifyDate = (unsigned int)vtReturn;

				if ( fInfo.FileHash.length() > 0 && fInfo.FileSize > 0 && fInfo.ModifyDate > 0)
					mapOldFiles[fInfo.MediaAddrID] = fInfo;
				else
					mapNewFiles[fInfo.MediaAddrID] = fInfo;

				m_pRecordsetPtr->MoveNext();
			}

			m_pRecordsetPtr->Close();

			return true;

		}
		catch(_com_error e)///捕捉异常
		{
			CString errormessage;
			errormessage.Format("操作数据库失败!\r\n错误信息:%s",e.ErrorMessage());
		}
	}

	return false;
}
