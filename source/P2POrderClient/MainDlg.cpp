#include "stdafx.h"
#include ".\maindlg.h"

HWND g_hMain = NULL;
extern string GetModuleFolderName();

LRESULT CMainDlg::OnBnClickedBtnHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ShowWindow( SW_HIDE);

	return 0;
}

LRESULT CMainDlg::OnBnClickedBtnClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog( 0);
	return 0;
}

void CMainDlg::ShowState( char* pArrFmt, ... )
{
	va_list argList = {0};
	char szBuf[4096] = {0};
	va_start(argList, pArrFmt);
	vsprintf(szBuf, pArrFmt, argList);
	va_end(argList);
	::SetWindowText( ::GetDlgItem( m_hWnd, IDC_STC_STATE), szBuf);
}

void CMainDlg::ShowLog( char* pArrFmt, ... )
{
	if ( m_lstLog.GetItemCount() > 10000)
		m_lstLog.DeleteAllItems();

	struct tm *local;
	time_t t =time(NULL);
	local=localtime(&t);

	va_list argList = {0};
	char szBuf[4096] = {0};
	sprintf( szBuf, "%.2d:%.2d:%.2d ", 
		local->tm_hour, local->tm_min, local->tm_sec );

	va_start(argList, pArrFmt);
	vsprintf(&szBuf[strlen(szBuf)], pArrFmt, argList);
	va_end(argList);

	m_lstLog.InsertItem( 0, szBuf);
}

bool CMainDlg::InitModule(void)
{
	g_hMain = m_hWnd;

	ShowState("Current Version: 08.01.10.001");

	InitP2PSocket();

	if ( !m_app.Connect() )
	{
		ShowState("m_app connect failed");
		return false;
	}

	return true;
}

void CMainDlg::UninitModule(void)
{
	m_app.Release();
	UninitP2PStocket();
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	UninitModule();

	return 0;
}

LRESULT CMainDlg::OnTimer(UINT uMsg, WPARAM nID, LPARAM lParam, BOOL& bHandled)
{
	if ( TIMER_UPDATER == nID)
	{
		KillTimer( TIMER_UPDATER);
		CheckUpdater();
		SetTimer( TIMER_UPDATER, 30 * 60 * 1000);
	}

	return 0;
}


LRESULT CMainDlg::OnShowLog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CString* pStr = (CString *)wParam;
	ShowLog( pStr->GetBuffer(0));
	delete pStr;
	return 0;
}

void CMainDlg::CheckUpdater()
{
	CString sParameter = "http://p2p.kan8888.com/update/update.asp?appname=P2PIEClient";
	string sExePath = GetModuleFolderName() + "Updater.exe";
	ShellExecute(m_hWnd, NULL, sExePath.c_str(), sParameter, NULL, SW_SHOW);
}

LRESULT CMainDlg::OnNMRclickLstLog(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码

	return 0;
}

LRESULT CMainDlg::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return 0;
}

