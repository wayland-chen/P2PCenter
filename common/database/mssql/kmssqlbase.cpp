#include "kmssqlbase.h"
#include <stdio.h>

static int g_nComInitCount = 0;

CKMSSqlBase::CKMSSqlBase()
{
	strcpy( m_pcConnection, "\"driver={SQL Server};server=serverip;uid=dbuser;pwd=dbpassword;database=dbname\"");

	if ( 0 == g_nComInitCount)
	{
		++g_nComInitCount;
		::CoInitialize( NULL);
	}

	m_pLocalConnectPtr.CreateInstance(__uuidof(Connection));
	m_pRecordsetPtr.CreateInstance(__uuidof(Recordset));
	m_pCommand.CreateInstance(__uuidof(Command));
}

CKMSSqlBase::~CKMSSqlBase()
{
	if ( IsOpen())
		Close();

	m_pLocalConnectPtr.Release();
	m_pRecordsetPtr.Release();
	m_pCommand.Release();

	g_nComInitCount--;
	if ( 0 == g_nComInitCount)
		::CoUninitialize();
}

bool CKMSSqlBase::ReadConnStrFromIni(const char* pFileName, const char* pSection, const char* pKeyName)
{
	char szFileName[260];
	char szConnStr[256];
	GetModuleFileName(NULL, szFileName, sizeof(szFileName));
	(strrchr(szFileName,'\\'))[1] = 0;
	strcat( szFileName, pFileName);
	if ( 0 == GetPrivateProfileString( pSection, pKeyName, "", szConnStr, sizeof(szConnStr), szFileName))
	{
		WritePrivateProfileString( pSection, pKeyName, m_pcConnection, szFileName);
		return false;
	}
	else
	{
		strcpy( m_pcConnection, szConnStr);
		return true;
	}
}

void CKMSSqlBase::SetConnString( const char* pConnStr)
{
	if ( pConnStr)
		strcpy( m_pcConnection, pConnStr);
}

BOOL CKMSSqlBase::Open( const char* pConnStr)
{
	try
	{
		if ( pConnStr)
			strcpy( m_pcConnection, pConnStr);

		m_pLocalConnectPtr->Open(m_pcConnection,"","",adConnectUnspecified);
		return TRUE;	
	}
	catch(_com_error e)
	{
	}

	return FALSE;
}

void CKMSSqlBase::Close()
{
	try
	{
		if(m_pLocalConnectPtr->State)
			m_pLocalConnectPtr->Close();
	}
	catch(_com_error e)
	{
	}
}

BOOL CKMSSqlBase::IsOpen()
{
	try
	{
		if(m_pLocalConnectPtr->State)
			return TRUE;	
		else
			return FALSE;
	}
	catch(_com_error e)
	{
		return FALSE;	
	}

}

BOOL CKMSSqlBase::TestConnect()
{
	try
	{
		m_pLocalConnectPtr->Open(m_pcConnection,"","",adConnectUnspecified);
		m_pLocalConnectPtr->Close();
		return TRUE;	
	}
	catch(_com_error e)
	{
		return FALSE;	
	}
}

BOOL CKMSSqlBase::Exec(char* pSql, char *pRet, int iRetLen)
{
	try
	{
		HRESULT _hr = m_pRecordsetPtr->Open(
			pSql,
			_variant_t((IDispatch*)m_pLocalConnectPtr,true),
			adOpenStatic,
			adLockOptimistic,
			adCmdText
			);

		if (FAILED(_hr))
		{
			m_pRecordsetPtr->Close();
			strncpy(pRet, "SQL执行失败", iRetLen);
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	catch(_com_error e)
	{
		sprintf(pRet, "连接数据库失败!\r\n错误信息:%s",e.ErrorMessage());
		return FALSE;
	}
}

