#include "StdAfx.h"
#include ".\clientapp.h"
extern HWND g_hMain;

CClientApp::CClientApp(void)
: m_pP2PNetwork(NULL)
, m_bWantStop(false)
{
	CKLog::SetDebugType( LOG_TYPE_WARNING);
}

CClientApp::~CClientApp(void)
{
}

void CClientApp::OnConnect()
{
}

void CClientApp::OnDisConnect()
{
}

bool CClientApp::Connect()
{
	m_pP2PNetwork = CreateP2PNetwork();
	if ( !m_pP2PNetwork )
	{
		WriteLog(LOG_TYPE_ERROR, "Create P2P Network Failed.");
		return false;
	}

	return StartThread();
}

void CClientApp::Release()
{
	m_bWantStop = true;
	m_evThreadWait.Set();
	WaitForStop();

	if ( m_pP2PNetwork )
	{
		m_pP2PNetwork->ReleaseModule();
		m_pP2PNetwork = NULL;
	}
}

bool CClientApp::EnumAllFiles(string sFolder)
{
	if ( sFolder.length() == 0 )
		return false;

	bool bRet = false;
	CFindFile fFind;
	string sFindStr = sFolder + "\\*.*";
	BOOL bFind = fFind.FindFile( sFindStr.c_str() );
	while ( bFind )
	{
		bRet = true;

		if ( fFind.IsDirectory() )
		{
			if ( !fFind.IsDots() )
			{
				if ( !EnumAllFiles( fFind.GetFilePath().GetBuffer(0) ) )
				{
					bRet = false;
					break;
				}
			}
		}
		else
		{
			CMediaFile* pMediaFile = new CMediaFile();
			if ( pMediaFile )
			{
				if ( pMediaFile->InitFile( fFind.GetFilePath().GetBuffer(0), fFind.GetFileName().GetBuffer(0) ) )
				{
					pMediaFile->m_sNodeName = "Kevin_Test_Node_Name";
					m_MediaFileMgr.Insert( pMediaFile->m_sFileHash, pMediaFile );
					// 查找到立即发布
					PublishFiles();
				}
				else
				{
					delete pMediaFile;
					CKLog::WriteLog( LOG_TYPE_DEBUG, "%s InitFile Failed.", fFind.GetFilePath() );
					WriteLog( LOG_TYPE_DEBUG, "%s InitFile Failed.", fFind.GetFilePath() );
				}
			}
		}

		bFind = fFind.FindNextFile();
	}
	fFind.Close();

	return bRet;
}

DWORD CClientApp::PublishFiles()
{
	if ( !m_pP2PNetwork ) return 0;
	CMediaFile* pMediaFile = m_MediaFileMgr.Pop();
	if ( !pMediaFile ) return 0;

	char szUrl[1024];
	sprintf( szUrl, "p2p://www.videobbs.com:8902/%s?agnet=agentname&upuserid=0001&userid=10002&nodename=%s&filehash=%s&filesize=%I64u&filepath=%s&filename=%s&modifydate=%u", 
		pMediaFile->m_sFileName.c_str(), pMediaFile->m_sNodeName.c_str(), pMediaFile->m_sFileHash.c_str(), 
		pMediaFile->m_llFileSize, pMediaFile->m_sFilePath.c_str(), pMediaFile->m_sFileName.c_str(), pMediaFile->m_nModifyDate );

	DWORD dwChannelID = m_pP2PNetwork->OpenSource( szUrl, true );
	if ( dwChannelID > 0)
	{
		m_AlreadyPublishs[pMediaFile->m_sFileHash] = dwChannelID;
		char szText[4096];
		sprintf( szText, "http://127.0.0.1:81/%s?filesize=%I64u&filename=%s&filehash=%s&filepath=%s&agnet=&upuserid=1&userid=2&nodename=%s", 
			pMediaFile->m_sFileName.c_str(), pMediaFile->m_llFileSize, 
			pMediaFile->m_sFileName.c_str(), pMediaFile->m_sFileHash.c_str(),
			pMediaFile->m_sFilePath.c_str(), pMediaFile->m_sNodeName.c_str() );
		WriteLog( LOG_TYPE_FORCE, szText );
	}

	delete pMediaFile;
	pMediaFile = NULL;

	return dwChannelID;
}

bool CClientApp::PublishFilesFromDB( CMSSqlDB& db)
{
	WriteLog( LOG_TYPE_DEBUG, "PublishFilesFromDB");

	bool bRet = false;

	do
	{
		if ( !db.Open())
		{
			WriteLog( LOG_TYPE_FORCE, "数据库打开失败");
			break;
		}

		char szRet[256];
		db.Exec( "EXEC sp_p2p_insert", szRet, sizeof(szRet));

		map<int, CMSSqlDB::FileInfo> mapOldFiles, mapNewFiles;
		if ( !db.GetFileList( mapOldFiles, mapNewFiles))
		{
			break;
		}

		CKLog::WriteLog(LOG_TYPE_FORCE, "oldfiles: %d, newfiles: %d, published: %d", 
			mapOldFiles.size(), mapNewFiles.size(), m_AlreadyPublishs.size());

		WriteLog(LOG_TYPE_FORCE, "oldfiles: %d, newfiles: %d, published: %d", 
			mapOldFiles.size(), mapNewFiles.size(), m_AlreadyPublishs.size());

		//关闭所有不存在旧列表里的已发布文件		
		map<string, int> OldFiles;
		//copy one
		map<int, CMSSqlDB::FileInfo>::iterator it = mapOldFiles.begin();
		while ( it != mapOldFiles.end())
		{
			OldFiles[it->second.FileHash] = it->first;
			it++;
		}
		map<string, DWORD>::iterator itpub = m_AlreadyPublishs.begin();
		while ( itpub != m_AlreadyPublishs.end())
		{
			if ( OldFiles.find( itpub->first) == OldFiles.end())
			{
				if ( m_pP2PNetwork)
					m_pP2PNetwork->CloseSource( itpub->second);
				CKLog::WriteLog( LOG_TYPE_FORCE, "检测到修改,关闭发布: %s", itpub->first.c_str());
				WriteLog( LOG_TYPE_FORCE, "检测到修改,关闭发布: %s", itpub->first.c_str());
				itpub = m_AlreadyPublishs.erase( itpub);
				continue;
			}
			itpub++;
		}
		
		// 先发布曾经发布过的文件
		it = mapOldFiles.begin();
		while ( it != mapOldFiles.end())
		{
			//如果已成功发布，忽略
			if ( m_AlreadyPublishs.find( it->second.FileHash) != m_AlreadyPublishs.end())
			{
				it ++;
				continue;
			}

			// 重新比较一下 FileSize 和 ModifyDate,如不同，放入 mapNewFiles里重新计算发布
			if ( it->second.FileSize != CMediaFile::GetFileSize( it->second.FilePath) ||
				it->second.ModifyDate != CMediaFile::GetModifyDate( it->second.FilePath))
			{
				CKLog::WriteLog( LOG_TYPE_FORCE, "%s, 重新比较size, date不相同", it->second.FilePath.c_str());
				WriteLog( LOG_TYPE_FORCE, "%s, 重新比较size, date不相同", it->second.FilePath.c_str());
				mapNewFiles[it->first] = it->second;
				it = mapOldFiles.erase( it);
				continue;
			}

			CMediaFile* pMediaFile = new CMediaFile();
			if ( pMediaFile )
			{
				pMediaFile->m_llFileSize = it->second.FileSize;
				pMediaFile->m_sFileHash = it->second.FileHash;
				pMediaFile->m_sFileName = it->second.FileName;
				pMediaFile->m_sFilePath = it->second.FilePath;
				pMediaFile->m_sNodeName = it->second.NodeName;
				pMediaFile->m_nModifyDate = it->second.ModifyDate;

				m_MediaFileMgr.Insert( pMediaFile->m_sFileHash, pMediaFile );

				CKLog::WriteLog( LOG_TYPE_DEBUG, "%s, Count: %u\n\t%s?filehash=%s&filesize=%I64u", 
					pMediaFile->m_sFilePath.c_str(), m_MediaFileMgr.GetCount(), 
					pMediaFile->m_sFileName.c_str(), pMediaFile->m_sFileHash.c_str(), pMediaFile->m_llFileSize );

				WriteLog( LOG_TYPE_DEBUG, "%s, Count: %u\n\t%s?filehash=%s&filesize=%I64u", 
					pMediaFile->m_sFilePath.c_str(), m_MediaFileMgr.GetCount(), 
					pMediaFile->m_sFileName.c_str(), pMediaFile->m_sFileHash.c_str(), pMediaFile->m_llFileSize );

				PublishFiles();
			}

			it ++;
		}

		// 再计算并发布新的文件
		it = mapNewFiles.begin();
		while ( it != mapNewFiles.end())
		{
			//如果已成功发布，先关闭
			map<string, DWORD>::iterator it2 = m_AlreadyPublishs.find( it->second.FileHash);
			if ( it2 != m_AlreadyPublishs.end())
			{
				if ( m_pP2PNetwork)
					m_pP2PNetwork->CloseSource( it2->second);
				m_AlreadyPublishs.erase( it2);
				CKLog::WriteLog( LOG_TYPE_FORCE, "新文件已发布，先关闭, %s", it->second.FileName.c_str());
				WriteLog( LOG_TYPE_FORCE, "新文件已发布，先关闭, %s", it->second.FileName.c_str());
			}

			CMediaFile* pMediaFile = new CMediaFile();
			if ( pMediaFile )
			{
				if ( pMediaFile->InitFileForDB( it->second.FilePath, it->second.FileName ) )
				{
					m_MediaFileMgr.Insert( pMediaFile->m_sFileHash, pMediaFile );
					CKLog::WriteLog( LOG_TYPE_DEBUG, "%s, Count: %u\n\t%s?filehash=%s&filesize=%I64u", 
						it->second.FilePath.c_str(), m_MediaFileMgr.GetCount(), 
						pMediaFile->m_sFileName.c_str(), pMediaFile->m_sFileHash.c_str(), pMediaFile->m_llFileSize );

					WriteLog( LOG_TYPE_DEBUG, "%s, Count: %u\n\t%s?filehash=%s&filesize=%I64u", 
						it->second.FilePath.c_str(), m_MediaFileMgr.GetCount(), 
						pMediaFile->m_sFileName.c_str(), pMediaFile->m_sFileHash.c_str(), pMediaFile->m_llFileSize );

					char szText[4096];
					sprintf( szText, "<a href=http://127.0.0.1:80/%s?agnet=agentname&upuserid=0001&userid=10002&nodename=%s&filehash=%s&filesize=%I64u&filepath=%s&filename=%s>%s</a>\r\n<br>\r\n", 
						pMediaFile->m_sFileName.c_str(), pMediaFile->m_sNodeName.c_str(), pMediaFile->m_sFileHash.c_str(), 
						pMediaFile->m_llFileSize, pMediaFile->m_sFilePath.c_str(), pMediaFile->m_sFileName.c_str(), pMediaFile->m_sFileName.c_str() );

					// 存档
					char szRet[256];
					sprintf( szText, 
						"Update %s Set %s = '%s',%s = %I64u, %s = %u Where %s = %u", 
						db.SysConfig.TableName,
						db.SysConfig.FileHashField, pMediaFile->m_sFileHash.c_str(), 
						db.SysConfig.FileSizeField, pMediaFile->m_llFileSize, 
						db.SysConfig.ModifyDateField, pMediaFile->m_nModifyDate,
						db.SysConfig.MediaAddrIDField, it->first);

					if ( !db.Exec( szText, szRet, sizeof(szRet)))
					{
						CKLog::WriteLog( LOG_TYPE_FORCE, "Update P2PPublish Failed, %s.", szText );
						WriteLog( LOG_TYPE_FORCE, "Update P2PPublish Failed, %s.", szText );
					}

					// 立即发布
					PublishFiles();
				}
				else
				{
					WriteLog( LOG_TYPE_FORCE, "InitFile Failed, %s.", it->second.FilePath.c_str() );
					CKLog::WriteLog( LOG_TYPE_FORCE, "InitFile Failed, %s.", it->second.FilePath.c_str() );
					delete pMediaFile;
				}
			}

			it ++;
		}

		bRet = true;
	}while(false);

	db.Close();

	return bRet;
}

void CClientApp::WriteLog( LOG_TYPE type, char* pArrFmt, ... )
{
	va_list argList = {0};
	char szBuf[4096] = {0};
	va_start(argList, pArrFmt);
	vsprintf(szBuf, pArrFmt, argList);
	va_end(argList);
	CString* pStr = new CString();
	*pStr = szBuf;
	CKLog::WriteLog( LOG_TYPE_FORCE, pStr->GetBuffer(0));
	PostMessage( g_hMain, WM_SHOW_LOG, (WPARAM)pStr, NULL);
}

void CClientApp::ThreadProcMain(void)
{
	WriteLog(LOG_TYPE_FORCE, "P2P Network connect to server...");

	if ( !m_pP2PNetwork->InitModule( this ) )
	{
		WriteLog(LOG_TYPE_ERROR, "P2P Network Init failed");
		return;
	}

	CMSSqlDB db;

	if ( !db.InitConfig())
	{
		WriteLog(LOG_TYPE_ERROR, "db config read failed");
		return;
	}

	switch( db.SysConfig.FromType) 
	{
	case 1:
		{
			string sPublishFolder = db.SysConfig.FromStr;
			if ( sPublishFolder.length() > 0)
			{
				if ( !EnumAllFiles( sPublishFolder ) )
				{
					WriteLog(LOG_TYPE_ERROR, "Enum file failed.");
					return;
				}
			}
			return;
		}
		break;

	case 2:
		{
			while ( !m_bWantStop)
			{
				if ( !PublishFilesFromDB( db))
					WriteLog(LOG_TYPE_ERROR, "database operation failed..");

				m_evThreadWait.Wait(60 * 1000);
			}
		}
		break;

	default:
		return;
		break;
	}
}



