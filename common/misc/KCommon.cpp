
#include "KCommon.h"

string CKCommon::GetModuleFolderName()
{
	char szFolderName[260];
	GetModuleFileName(NULL, szFolderName, sizeof(szFolderName));
	(_tcsrchr(szFolderName,'\\'))[1] = 0;
	return szFolderName;
}
