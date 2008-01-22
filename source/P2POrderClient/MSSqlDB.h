// MSSqlDB.h: interface for the CMSSqlDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSSQLDB_H__97AE25FC_37EF_4B58_955B_083A0EC91AAF__INCLUDED_)
#define AFX_MSSQLDB_H__97AE25FC_37EF_4B58_955B_083A0EC91AAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "database/mssql/kmssqlbase.h"

#include <map>
#include <string>
using namespace std;

//##ModelId=450DFB6801F4
class CMSSqlDB : public CKMSSqlBase
{
public:
	//##ModelId=450DFB68038A
	struct ConfigInfo
	{
		int  FromType;
		char FromStr[256];

		char TableName[256];
		char MediaAddrIDField[50];
		char FileNameField[50];
		char FilePathField[50];
		char FileHashField[50];
		char FileSizeField[50];
		char NodeNameField[50];
		char ModifyDateField[50];
		char PublishDateField[50];
	}; 

	struct FileInfo 
	{
		unsigned int	MediaAddrID;
		string			FileName;
		string			FilePath;
		string			FileHash;
		string			NodeName;
		LONGLONG		FileSize;
		unsigned int	ModifyDate;

		FileInfo()
		{
			MediaAddrID = 0;
			FileSize = 0;
			ModifyDate = 0;
		}
	};
	
public:
	CMSSqlDB();
	virtual ~CMSSqlDB();
	
	static bool CheckLocalFileIsExist(char *pPath); 
	BOOL InitConfig();
	ConfigInfo SysConfig;
	char* ExecSP(CString sSql);

	// old files 为曾经计算过HASH并正确的文件，new Files为新加入未计算过的文件
	bool GetFileList( map<int, FileInfo>& mapOldFiles,  map<int, FileInfo>& mapNewFiles);
};

#endif // !defined(AFX_MSSQLDB_H__97AE25FC_37EF_4B58_955B_083A0EC91AAF__INCLUDED_)
