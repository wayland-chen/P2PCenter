// AppOpt.cpp : CAppOpt 的实现
#include "stdafx.h"
#include "AppOpt.h"
#include ".\appopt.h"
#include <comutil.h>
#pragma comment(lib, "comsupp.lib")

// CAppOpt
CString GetModuleFolderName()
{
	char szFolderName[260];
	GetModuleFileName(_AtlBaseModule.m_hInstResource, szFolderName, sizeof(szFolderName));
	(_tcsrchr(szFolderName,'\\'))[1] = 0;
	return szFolderName;
}

#pragma comment(lib, "Version.lib")

char *getVersion(char *fileName)
{
	DWORD vSize;
	DWORD vLen,langD;
	BOOL retVal;    

	LPVOID version=NULL;
	LPVOID versionInfo=NULL;
	static char fileVersion[256];
	bool success = true;
	vSize = GetFileVersionInfoSize(fileName,&vLen);
	if (vSize) 
	{
		versionInfo = malloc(vSize+1);
		if (GetFileVersionInfo(fileName,vLen,vSize,versionInfo))
		{            
			sprintf(fileVersion,"\\VarFileInfo\\Translation");
			retVal = VerQueryValue(versionInfo,fileVersion,&version,(UINT *)&vLen);
			if (retVal && vLen==4) 
			{
				memcpy(&langD,version,4);            
				sprintf(fileVersion, "\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion",
					(langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24, 
					(langD & 0xff0000)>>16);            
			}
			else 
				sprintf(fileVersion, "\\StringFileInfo\\%04X04B0\\FileVersion",
				GetUserDefaultLangID());
			retVal = VerQueryValue(versionInfo,fileVersion,&version,(UINT *)&vLen);
			if (!retVal) success = false;
		}
		else 
			success = false;
	}
	else 
		success=false;    

	if (success) 
	{
		if (vLen<256)
			strcpy(fileVersion,(char *)version);
		else 
		{
			strncpy(fileVersion,(char *)version,250);
			fileVersion[250]=0;            
		}
		if (versionInfo) free(versionInfo);
		versionInfo = NULL;
		return fileVersion;
	}
	else 
	{
		if (versionInfo) free(versionInfo);
		versionInfo = NULL;
		return NULL;    
	}
}

STDMETHODIMP CAppOpt::StartApp(void)
{
	return S_OK;
	CRegKey reg;
	LPCTSTR lpszKey = "SOFTWARE\\P2PService_K";

	if ( reg.Open( HKEY_LOCAL_MACHINE, lpszKey ) == ERROR_SUCCESS )
	{
		TCHAR szAppPath[MAX_PATH];
		ULONG ulLen = sizeof( szAppPath );
		if ( reg.QueryStringValue( "AppPath", szAppPath, &ulLen ) == ERROR_SUCCESS )
		{
			reg.Close();
			ShellExecute( NULL, NULL, szAppPath, NULL, NULL, SW_SHOW );
			return S_OK;
		}
		reg.Close();
	}

	return E_FAIL;
}

STDMETHODIMP CAppOpt::StopApp(void)
{
	// TODO: 在此添加实现代码

	return S_OK;
}

STDMETHODIMP CAppOpt::get_ListenPort(ULONG* pVal)
{
	ULONG ulRet = 81;
	CRegKey reg;
	LPCTSTR lpszKey = "SOFTWARE\\P2PService_K";

	if ( reg.Open( HKEY_LOCAL_MACHINE, lpszKey ) == ERROR_SUCCESS )
	{
		if ( reg.QueryDWORDValue( "ListenPort", ulRet ) == ERROR_SUCCESS )
		{
			reg.Close();
			return S_OK;
		}
		reg.Close();
	}

	*pVal = ulRet;

	return S_OK;
}

STDMETHODIMP CAppOpt::get_ServiceVersion(BSTR* pVal)
{
	CString sServicePath = GetModuleFolderName() + "P2PService.exe";
	CString sVersion = getVersion( sServicePath.GetBuffer(0) );
	*pVal= _com_util::ConvertStringToBSTR(sVersion.GetBuffer(0)) ;
	return S_OK;
}


STDMETHODIMP CAppOpt::UpdateService(void)
{
	CString sParameter = "http://www.kan8888.com/p2p/update/update.asp?appname=p2pservice";
	CString sExePath = GetModuleFolderName() + "Updater.exe";
	ShellExecute(NULL, NULL, sExePath, sParameter, NULL, SW_SHOW);

	return S_OK;
}
