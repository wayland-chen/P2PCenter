string Decode( const string& str, bool bQuery )
{
	int index;
	string strDecoded = str;
	// special processing or query strings....
	if ( bQuery )
	{
		// change all '+' to ' '....
		while( (index=strDecoded.Find('+')) != -1 )
			strDecoded = strDecoded.Left(index) + ' ' + strDecoded.Mid(index+1);
	}

	// first see if there are any %s to decode....
	if ( strDecoded.Find( '%' ) != -1 )
	{
		// iterate through the string, changing %dd to special char....
		for( index=0; index < strDecoded.GetLength(); index++ )
		{
			char ch = strDecoded.GetAt( index );
			if ( ch == '%' )
			{
				if ( strDecoded.GetAt( index+1 ) == '%' )
				{
					// wanna keep one percent sign....
					strDecoded = strDecoded.Left(index) + strDecoded.Mid(index+1);
				}
				else
				{
					// assume we have a hex value....
					char ch1 = strDecoded.GetAt(index+1);
					char ch2 = strDecoded.GetAt(index+2);
					ch1 = ch1 >= 'A' ? (ch1&0xdf)-'A' : ch1-'0';
					ch2 = ch2 >= 'A' ? (ch2&0xdf)-'A' : ch2-'0';
					// replace the escape sequence with the char....
					strDecoded = strDecoded.Left(index)
						+ (char)(ch1*16 + ch2)
						+ strDecoded.Mid( index+3 );
				}
			}
		}
	}
	return strDecoded;
}

CgiHandler::CgiHandler()
{
#ifdef IMPL_CGI
	m_pThread = NULL;
	m_pCancel = NULL;
#endif // IMPL_CGI
}

CgiHandler::~CgiHandler ()
{
#ifdef IMPL_CGI
	if ( m_pCancel )
	{
		if ( m_pThread )
		{
			DWORD dwCode;
			// signal a cancel if still running....
			if ( ::GetExitCodeThread( m_pThread->m_hThread, &dwCode )
				&& dwCode == STILL_ACTIVE )
			{
				// signal a cancel....
				m_pCancel->SetEvent();
				// wait for the thread to die....
				WaitForSingleObject( m_pThread->m_hThread, INFINITE );
			}
			// kill the object...
			delete m_pThread;
		}
		delete m_pCancel;
	}
#endif // IMPL_CGI

}





bool CgiHandler::StartSvrApp( void )
{
#ifdef IMPL_CGI
	if ( m_pRequest->m_dwExecute != CRequest::APP_ISAPI )
		return CGIStart();
	else
	{
		StuffError( IDS_STATUS_NOTIMPL );
		return FALSE;
	}
#else //  IMPL_CGI
	StuffError( IDS_STATUS_NOTIMPL );
	return FALSE;
#endif // IMPL_CGI
}


int CgiHandler::StuffString( const string& strData )
{
	int nLen = strData.GetLength()*sizeof(TCHAR);
	// make sure there's enough room....
	if ( m_cbOut + nLen > m_buf.GetSize() )
	{
		int nChunks = nLen/1024 + 1;
		m_buf.SetSize( m_cbOut + nChunks*1024 );
	}
	// copy the data....
	MoveMemory( m_buf.GetData() + m_cbOut, (LPCSTR)strData, nLen );
	m_cbOut += nLen;
	// return amount of space left....
	return (m_buf.GetSize() - m_cbOut);
}

int CgiHandler::StuffString( UINT uId )
{
	string str;
	str.LoadString( uId );
	return StuffString( str );
}

int CgiHandler::StuffStatus( const string& strStatus )
{
	string strVer = "HTTP/1.0 ";
	StuffString( strVer );
	StuffString( strStatus );
	StuffString( CRLF );

	// stuff the server name....
	string strServer;
	if ( strServer.LoadString( IDS_SERVER_NAME ) && !strServer.IsEmpty() )
		StuffHeader( "Server", strServer );

	// stuff the date....
	return StuffHeader( "Date", GetHttpDate() );
}

int CgiHandler::StuffStatus( UINT uStatus )
{
	string strStatus;
	strStatus.LoadString( uStatus );
	// save the status for this request....
	m_pRequest->m_uStatus = uStatus;
	// stuff the HTTP status line....
	return StuffStatus( strStatus );
}

int CgiHandler::StuffError( UINT uMsg )
{
	StuffStatus( uMsg );
	return StuffString( CRLF );
}

int CgiHandler::StuffHeader( string strName, string strValue )
{
	StuffString( strName );
	StuffString( ": " );
	StuffString( strValue );
	return StuffString( CRLF );
}

int CgiHandler::StuffHeader( string strName, int nValue )
{
	string strValue;
	StuffString( strName );
	StuffString( ": " );
	strValue.Format( "%d", nValue );
	StuffString( strValue );
	return StuffString( CRLF );
}

bool CgiHandler::StuffHeading( void )
{
	bool bContinue = FALSE;
	if ( m_pRequest->m_dwAttr & FILE_ATTRIBUTE_HIDDEN )
	{
		// never show hidden files....
		StuffError( IDS_STATUS_FORBIDDEN );
	}
	else if ( m_pRequest->m_dwAttr & FILE_ATTRIBUTE_DIRECTORY )
	{
		if ( m_pDoc->m_bAllowListing )
		{
			// create a directory listing....
			StuffStatus( IDS_STATUS_OK );
			StuffString( CRLF );
			bContinue = TRUE;
		}
		else
			StuffError( IDS_STATUS_FORBIDDEN );
	}
#ifdef IMPL_CGI
	else if ( m_hFile != INVALID_HANDLE_VALUE )
	{
		// cgi's output file will be opened already....
		string strStatus, strHeaders;
		// loop until we find a blank line....
		DWORD dwRead = 0;
		CByteArray baFile;
		baFile.SetSize( 1024 );
		// read next chunk....
		bool bRead = ReadFile( m_hFile, baFile.GetData(),
			baFile.GetSize(), &dwRead, NULL );
		while ( dwRead > 0 )
		{
			int index = 0;
			while( GetLine( baFile, dwRead, index ) == TRUE )
			{
				bool bSave = TRUE;
				// stuff any non-empty lines.....
				if ( m_strLine.IsEmpty() )
				{
					// we found our empty line;
					// back up to where we left off....
					DWORD dwPos = SetFilePointer( m_hFile,
						index - dwRead,
						NULL, FILE_CURRENT );

					// and we're off....
					bContinue = TRUE;
					break;
				}
				else
				{
					int nPos = m_strLine.Find( ':' );
					if ( nPos != -1 )
					{
						string strName = m_strLine.Left( nPos );
						strName.TrimLeft();
						strName.TrimRight();
						string strVal  = m_strLine.Mid( nPos+1 );
						strVal.TrimLeft();
						strVal.TrimRight();
						if ( strName.CompareNoCase("Status") == 0 )
						{
							strStatus = strVal;
							bSave = FALSE;
						}
						else if ( strName.CompareNoCase("Location") == 0 )
						{
							strStatus.LoadString( IDS_STATUS_MOVEDTEMP );
						}
					}
				}

				// save the header (if we want to)....
				if ( bSave )
					strHeaders += m_strLine + CRLF;

				m_strLine.Empty();
			}
			// read next chunk if we're not done....
			if ( bContinue )
				break;
			else
				ReadFile( m_hFile, baFile.GetData(),
					baFile.GetSize(), &dwRead, NULL );
		}
		if ( strStatus.IsEmpty() )
			StuffStatus( IDS_STATUS_OK );
		else
			StuffStatus( strStatus );

		// stuff the headers....
		StuffString( strHeaders );
		// stuff the blank line....
		StuffString( CRLF );
	}
#endif // IMPL_CGI
	else
	{
		// open the file....
		m_hFile = CreateFile( m_pRequest->m_strFullPath,
			GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL );
		if ( m_hFile != INVALID_HANDLE_VALUE )
		{
			if ( m_reqStatus != REQ_SIMPLE )
			{
				CTime timeIfMod;
				string strIfMod = m_pRequest->GetHeaderValue( "If-Modified-Since" );
				if ( strIfMod.GetLength() > 0 &&
					FromHttpTime( strIfMod, timeIfMod ) &&
					!IfModSince( timeIfMod ) )
				{
					// eh, it hasn't been modified....
					StuffStatus( IDS_STATUS_NOTMODIFIED );
					// don't need it anymore....
					CloseHandle( m_hFile );
					m_hFile = INVALID_HANDLE_VALUE;
				}
				else
				{
					// send it off....
					StuffStatus( IDS_STATUS_OK );
					// any other header info....
					StuffFileType();
					StuffHeader( "Content-length", GetFileSize( m_hFile, NULL ) );
					// get the last modified time....
					FILETIME ft;
					if ( GetFileTime( m_hFile, NULL, NULL, &ft ) )
					{
						StuffHeader( "Last-Modified", GetHttpDate( &ft ) );
					}
					bContinue = TRUE;
				}
				// blank line....
				StuffString( CRLF );
			}
			else
				bContinue = TRUE;
		}
		else
		{
			// couldn't open; try again later....
			StuffError( IDS_STATUS_SVCUNAVAIL );
		}
	}
	return bContinue;
}

void CgiHandler::StartTargetStuff( void )
{
	if ( m_hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwRead = 0;
		// read the first chunk....
		ReadFile( m_hFile, m_buf.GetData() + m_cbOut,
			m_buf.GetSize()-m_cbOut, &dwRead, NULL );
		if ( dwRead > 0 )
			m_cbOut += dwRead;
		else
		{
			// nothing read.... close the file....
			CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	else if ( m_pRequest->m_dwAttr & FILE_ATTRIBUTE_DIRECTORY )
		StuffListing();
	else
		StuffString( CRLF );
}

void CgiHandler::StuffListing( void )
{
	bool bRoot = FALSE;
	bool bIcons = m_pDoc->m_bListIcon;
	string strIcon;
	string strLine = string("http://")
		+ m_pDoc->m_strServer
		+ m_pRequest->m_strURL;
	string strDir = m_pRequest->m_strURL;
	string strMask = m_pRequest->m_strFullPath;

	// make sure URL ends in a slash....
	if ( strDir.GetAt( strDir.GetLength()-1 ) != '/' )
		strDir += '/';
	// is this the server's root folder?
	else if ( strDir.Compare( "/" ) == 0 )
		bRoot = TRUE;

	// create the file search mask....
	AddFile( strMask, IDS_DIRMASK );
	StuffString( IDS_CONTENTS_PRE );
	StuffString( strLine );
	StuffString( IDS_CONTENTS_POST );
	if ( bRoot )
		StuffString( IDS_CONTENTS_DESC );

	if ( bIcons )
		strIcon.LoadString( IDS_ICON_BLANK );
	strLine.Format( IDS_CONTENTS_HEADING, strIcon );
	StuffString( strLine );

	int nFiles = 0;

	WIN32_FIND_DATA fd;
	// find the first file that matches the mask....
	HANDLE fh = FindFirstFile( strMask, &fd );
	if ( fh != INVALID_HANDLE_VALUE )
	{
		// create a line for the found file....
		nFiles += StuffListingFile( &fd, strDir, bIcons );
		// loop through all other files....
		while ( FindNextFile( fh, &fd ) )
			nFiles += StuffListingFile( &fd, strDir, bIcons );
	}

	if ( nFiles == 0 )
		StuffString( IDS_CONTENTS_EMPTY );

	StuffString( IDS_CONTENTS_FOOTER );
	// only add the parent link if there is one....
	if ( !bRoot )
	{
		if ( bIcons )
			strIcon.LoadString( IDS_ICON_PARENT );
		strLine.Format( IDS_CONTENTS_PARENT, strIcon );
		StuffString( strLine );
	}
	// add the note and end it....
	StuffString( IDS_CONTENTS_NOTE );
	StuffString( CRLF );
}

int CgiHandler::StuffListingFile( WIN32_FIND_DATA* pfd, const string& strDir, bool bIcons )
{
	int nFile = 0;
	// don't include '.', '..' or hidden files....
	if ( lstrcmp( pfd->cFileName, "." ) && lstrcmp( pfd->cFileName, ".." )
		&& (pfd->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0 )
	{
		string strSize, strIcon = "";
		string strLine, strFile = pfd->cFileName;
		CTime timeFile( pfd->ftLastWriteTime );
		bool bFolder = ((pfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
		if ( bIcons && bFolder )
			strIcon.LoadString( IDS_ICON_FOLDER );
		else if ( bIcons )
			strIcon.LoadString( IDS_ICON_FILE );

		// create the link string....
		string strLink = strDir + strFile;
		// make sure spaces are replaced with '%20'...
		int index;
		while ( (index=strLink.Find(' ')) != -1 )
			strLink = strLink.Left(index) + "%20" + strLink.Mid( index+1 );

		// format the size string....
		if ( bFolder )
			strSize = "  Folder";
		else if ( pfd->nFileSizeHigh > 0 )
			strSize = "   &gt; 4GB"; // yeah, right.
		else if ( pfd->nFileSizeLow < 1024 )
			strSize = "    &lt; 1K";
		else
			strSize.Format( "%7dK", pfd->nFileSizeLow/1024 );

		strLine.Format( IDS_CONTENTS_FORMAT,
			timeFile.Format( IDS_FILETIMEFMT ),
			strSize, strLink, strIcon, strFile );
		StuffString( strLine );
		nFile = 1;
	}
	return nFile;
}




#ifdef IMPL_CGI
bool CgiHandler::CGIStart( void )
{
	bool bOk = FALSE;
	// get the temp path...
	string strTempPath;
	GetTempPath( MAX_PATH, strTempPath.GetBuffer(MAX_PATH) );
	strTempPath.ReleaseBuffer();
	// create a temporary file for the output....
	string strTempName;
	GetTempFileName( strTempPath, "CGI", 0, strTempName.GetBuffer(MAX_PATH) );
	strTempName.ReleaseBuffer();
	m_hFile = CreateFile( strTempName, GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE, &g_sa,
		CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE, NULL );
	if ( m_hFile != INVALID_HANDLE_VALUE )
	{
		// create the cancel event....
		m_pCancel = new CEvent;
		if ( m_pCancel )
		{
			// make sure the event is reset....
			m_pCancel->ResetEvent();
			// create the CGI thread suspended....
			m_pThread = AfxBeginThread( (AFX_THREADPROC)CGIThread,
				(LPVOID)this, THREAD_PRIORITY_NORMAL, 0,
				CREATE_SUSPENDED, NULL );
			if ( m_pThread )
			{
				// don't self-destruct (we must delete)....
				m_pThread->m_bAutoDelete = FALSE;
				// resume...
				m_pThread->ResumeThread();
				bOk = TRUE;
			}
		}
	}

	if ( bOk == FALSE )
	{
		StuffError( IDS_STATUS_SVRERROR );
		if( m_hFile != INVALID_HANDLE_VALUE )
		{ // JIC....
			CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	return bOk;
}

void AddEnvVar( string& strEnv, string strName, string strVal )
{
	// add the name=val pair to the env in alphabetical order....
	strEnv += strName + '=' + strVal + '\a';
}

UINT CGIThread( LPVOID pvParam )
{
	CgiHandler* pReqSock = (CgiHandler*)pvParam;
	CRequest* pRequest = pReqSock->m_pRequest;
	bool bOk = FALSE;
	DWORD dwErr;
	HANDLE hWritePipe, hReadPipe;
	// create a pipe we'll use for STDIN....
	if ( CreatePipe( &hReadPipe, &hWritePipe, &g_sa, 0 ) )
	{
		// get the command line together....
		string strCmdLine = pRequest->m_strFullPath
			+ ' '
			+ Decode( pRequest->m_strArgs, TRUE );
		// get the directory....
		string strDir = pRequest->m_strFullPath;
		int index = strDir.ReverseFind( SEPCHAR );
		// assume we found it....
		strDir = strDir.Left( index+1 );

		// create an environment for the CGI process....
		DWORD dwCreateFlags = 0;
#ifdef UNICODE
		dwCreateFlags = CREATE_UNICODE_ENVIRONMENT;
#endif // UNICODE
		CEnvironment cEnv;

		string strValue;
		strValue.LoadString( IDS_SERVER_NAME );
		cEnv.Add( "SERVER_SOFTWARE", strValue );
		cEnv.Add( "SERVER_NAME", pReqSock->m_pDoc->m_strServer );
		cEnv.Add( "GATEWAY_INTERFACE", "CGI/1.1" );
		cEnv.Add( "SERVER_PROTOCOL", "HTTP/1.0" );
		strValue.Format( "%d", pReqSock->m_pDoc->m_uPort );
		cEnv.Add( "SERVER_PORT", strValue );

		cEnv.Add( "REQUEST_METHOD", pRequest->m_strMethod );
		cEnv.Add( "SCRIPT_NAME", pRequest->m_strURL );
		cEnv.Add( "QUERY_STRING", pRequest->m_strArgs );
		cEnv.Add( "REMOTE_ADDR", pRequest->m_strHost );
		if ( pRequest->m_cbBody > 0 )
		{
			cEnv.Add( "CONTENT_LENGTH", pRequest->GetHeaderValue("Content-Length") );
			cEnv.Add( "CONTENT_TYPE", pRequest->GetHeaderValue("Content-Type") );
		}
		if ( !pRequest->m_strPathInfo.IsEmpty() )
		{
			cEnv.Add( "PATH_INFO", pRequest->m_strPathInfo );
			cEnv.Add( "PATH_TRANSLATED", pRequest->m_strPathTranslated );
		}

		// all the passed headers prefixed with "HTTP_"....
		POSITION pos = pRequest->m_mapHeaders.GetStartPosition();
		while ( pos != NULL )
		{
			// get the name/value pair....
			string strName, strValue;
			pRequest->m_mapHeaders.GetNextAssoc( pos, strName, strValue );
			HeaderToEnvVar( strName );
			// set the environment variable....
			cEnv.Add( strName, strValue );
		}

		// create the process....
		LPVOID pEnv = (LPVOID)cEnv.GetBlock();
		PROCESS_INFORMATION pi;
		STARTUPINFO si = {0};
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.wShowWindow = SW_HIDE;
		si.hStdInput = hReadPipe;
		si.hStdOutput = pReqSock->m_hFile;
		si.hStdError = pReqSock->m_hFile;
		bOk = CreateProcess( NULL, strCmdLine.GetBuffer(1),
			NULL, NULL, TRUE,
			dwCreateFlags, pEnv,
			strDir, &si, &pi );
		strCmdLine.ReleaseBuffer();
		// if created....
		if ( bOk )
		{
			// release our hold on the thread....
			CloseHandle( pi.hThread );
			// send the body of the post to the stdin....
			if ( pRequest->m_cbBody > 0 )
			{
				DWORD dwWritten = 0;
				WriteFile( hWritePipe, pRequest->m_baBody.GetData(),
					pRequest->m_cbBody, &dwWritten, NULL );
			}
			// wait for either cancel or process done....
			HANDLE aHandles[2];
			aHandles[0] = pi.hProcess;
			aHandles[1] = pReqSock->m_pCancel->m_hObject;
			if ( WaitForMultipleObjects( 2, aHandles, FALSE, INFINITE ) == WAIT_OBJECT_0 )
			{
				// process finished; notify main thread....
				AfxGetApp()->m_pMainWnd->PostMessage( WSM_CGIDONE, 0, (LPARAM)pReqSock );
			}
			else
			{
				// canceled or some other error....
				bOk = FALSE;
			}
			// close our hold on it....
			CloseHandle( pi.hProcess );
		}
		else
			dwErr = GetLastError();

		// close the stdin pipe....
		CloseHandle( hWritePipe );
		CloseHandle( hReadPipe );
		delete pEnv;
	}
	if ( bOk == FALSE && pReqSock->m_hFile != INVALID_HANDLE_VALUE )
	{ // JIC....
		CloseHandle( pReqSock->m_hFile );
		pReqSock->m_hFile = INVALID_HANDLE_VALUE;
	}

	return (bOk?0:1);
}

void CgiHandler::CGIDone( void )
{
	if ( !m_bKilled )
	{
		// flush the temp file's buffers....
		bool bSucceed = FlushFileBuffers( m_hFile );
		// go to start of file....
		DWORD dwPos = SetFilePointer( m_hFile, 0, NULL, FILE_BEGIN );
		// output the header....
		StuffHeading();
		if ( m_pRequest->m_strMethod.Compare( "HEAD" ) )
			StartTargetStuff();
		else
		{
			CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		}
		AsyncSelect( FD_WRITE | FD_CLOSE );
	}
	else
	{
		CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

void HeaderToEnvVar( string& strVar )
{
	int index;
	// make upper case, change '-' to '_', and prefix....
	strVar.MakeUpper();
	while( (index = strVar.Find('-')) != -1 )
		strVar = strVar.Left(index) + '_' + strVar.Mid(index+1);
	strVar = "HTTP_" + strVar;
}

CEnvironment::CEnvironment( void )
{
	m_nSize = 2;
}

CEnvironment::~CEnvironment( void )
{
}

bool CEnvironment::Add( string name, string value )
{
	bool bOk = TRUE;
	// create the entry pair string....
	string strPair = name + __TEXT('=') + value;
	m_nSize += strPair.GetLength() + 1;
	POSITION pos = m_list.GetHeadPosition();

	// find the first item bigger than this string....
	while( pos != NULL )
	{
		if ( m_list.GetAt(pos).CompareNoCase(strPair) > 0 )
		{
			m_list.InsertBefore( pos, strPair );
			break;
		}
		m_list.GetNext( pos );
	}
	if ( pos == NULL )
		m_list.AddTail( strPair );

	return bOk;
}

LPVOID CEnvironment::GetBlock( void )
{
	// allocate a block....
	PTCHAR pBlock = new TCHAR[m_nSize];
	if ( pBlock )
	{
		// iterate through the list....
		PTCHAR pPos = pBlock;
		POSITION pos = m_list.GetHeadPosition();
		while( pos != NULL )
		{
			string& str = m_list.GetNext( pos );
			// copy the string....
			lstrcpy( pPos, str );
			pPos += str.GetLength() + 1;
		}
		// NULL for the whole list....
		*pPos = __TEXT('\0');
	}
	return pBlock;
}
#endif // IMPL_CGI
