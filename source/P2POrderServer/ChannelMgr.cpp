#include "StdAfx.h"
#include ".\channelmgr.h"
#include "io/KFile.h"
#include "misc/InternetUrl.h"

CChannelMgr::CChannelMgr(void)
{
}

CChannelMgr::~CChannelMgr(void)
{
}

string GetModuleFolderName()
{
	char szFolderName[260];
	GetModuleFileName(NULL, szFolderName, sizeof(szFolderName));
	(_tcsrchr(szFolderName,'\\'))[1] = 0;
	return szFolderName;
}

int replace_all(std::string& str,  
				const std::string& pattern,  
				const std::string& newpat) 
{ 
	int count = 0; 
	const size_t nsize = newpat.size(); 
	const size_t psize = pattern.size(); 

	for(size_t pos = str.find(pattern, 0);  
		pos != -1; 
		pos = str.find(pattern,pos + nsize)) 
	{ 
		str.replace(pos, psize, newpat); 
		count++; 
	} 

	return count; 
}

bool CChannelMgr::SaveHtmlFile( string sNodeName )
{
	string sFolder = GetModuleFolderName() + "html\\";
	CreateDirectory( sFolder.c_str(), NULL );

	string sFilePath = sFolder + "Kevin_Test_Node_Name" + ".htm";
	CKFile::DeleteFile( CKString(sFilePath.c_str()) );
	CKFile file;
	if ( !file.Create( CKString(sFilePath.c_str()) ) ) return false;

	// 按文件名排序
	map<string, CChannel *> mapChannels;

	map<string, CChannel *>::iterator it = m_map.begin();
	while ( it != m_map.end() )
	{
		CChannel* pChannel = it->second;
		//if ( pChannel->m_sNodeName == sNodeName )
		{
			string sFileName = pChannel->m_sFileName;

			int index = 1;
			while ( mapChannels.find( sFileName) != mapChannels.end())
			{
				char szNewFile[512];
				sprintf( szNewFile, "%s%d", sFileName.c_str(), index++);
				sFileName = szNewFile;
			}
			
			mapChannels[sFileName] = pChannel;
		}

		it ++;
	}

	it = mapChannels.begin();
	while ( it != mapChannels.end() )
	{
		CChannel* pChannel = it->second;

		string sFileName = pChannel->m_sFileName;
		replace_all( sFileName, " ", "_");
		char szPar[2048];
		sprintf( szPar, "agnet=agentname&upuserid=0001&userid=10002&nodename=%s&filehash=%s&filesize=%s&filename=%s", 
			pChannel->m_sNodeName.c_str(), pChannel->m_sChannelID.c_str(), 
			pChannel->m_sFileSize.c_str(), sFileName.c_str() );
		char szText[4096];
		sprintf( szText, "<a href=WebCtrl.asp?%s>%s</a> <a href=Download.asp?%s>下载</a> \r\n<br>\r\n", 
			szPar, sFileName.c_str(), szPar );
		if ( file.IsOpen() )
		{
			file.Write( szText, strlen(szText) );
		}

		it ++;
	}

	file.Close();

	SaveJSFile( mapChannels);

	return true;
}

bool CChannelMgr::SaveJSFile( map<string, CChannel *> Channels)
{
	string sFolder = GetModuleFolderName() + "html\\";
	CreateDirectory( sFolder.c_str(), NULL );

	string sFilePath = sFolder + "Channels.js";
	CKFile::DeleteFile( CKString(sFilePath.c_str()) );
	CKFile file;
	if ( !file.Create( CKString(sFilePath.c_str()) ) ) return false;

	string sJSHeader = "\
function CChannel()\n\
{\n\
	this.agent = \"\";\n\
	this.upuserid = -1;\n\
	this.userid = -1;\n\
	this.nodename = \"\";\n\
	this.filehash = \"\";\n\
	this.filesize = 0;\n\
	this.filename = \"\";\n\
}\n\
var cls = new Array();\n\n";

	file.Write( sJSHeader.c_str(), sJSHeader.length());

	map<string, CChannel *>::iterator it = Channels.begin();

	unsigned int nCount = 0;
	char szText[4096];
	while ( it != Channels.end() )
	{
		CChannel* pChannel = it->second;

		string sFileName = pChannel->m_sFileName;
		replace_all( sFileName, " ", "_");

		sprintf( szText, "cls[%u]=new CChannel();\n"\
			"cls[%u].agent=\"%s\";\n"\
			"cls[%u].upuserid=%u;\n"\
			"cls[%u].userid=%u;\n"\
			"cls[%u].nodename=\"%s\";\n"\
			"cls[%u].filehash=\"%s\";\n"\
			"cls[%u].filesize=%s;\n"\
			"cls[%u].filename=\"%s\";\n\n", 
			nCount,
			nCount, pChannel->m_sAgent.c_str(),
			nCount, pChannel->m_nUpUserID,
			nCount, pChannel->m_nUserID,
			nCount, pChannel->m_sNodeName.c_str(),
			nCount, pChannel->m_sChannelID.c_str(),
			nCount, pChannel->m_sFileSize.c_str(),
			nCount, pChannel->m_sFileName.c_str());

		file.Write( szText, strlen(szText));

		it ++;
		nCount ++;
	}

	file.Close();

	return true;
}
