// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"       // main symbols
#include <atlapp.h>
#include <atlbase.h>
#include <atlmisc.h>
#include <atlhost.h>
#include "define/kmacro.h"

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

	CAxWindow m_wndView;
	CComPtr<IWebBrowser2> m_spWebProwser2;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if ( m_spWebProwser2)
			m_spWebProwser2.Release();

		return 0;
	}

	CString GetModuleFolderName()
	{
		char szFolderName[260];
		GetModuleFileName(NULL, szFolderName, sizeof(szFolderName));
		(_tcsrchr(szFolderName,'\\'))[1] = 0;
		return szFolderName;
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		MoveWindow( 0, 0, 800, 500, TRUE);
		CenterWindow();

		// set icons
		HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);

		AtlAxWinInit();

		CComPtr<IAxWinHostWindow>           spHost;
		HRESULT                             hr;

		m_wndView.Create(m_hWnd, rcDefault, NULL, 
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

		do {
			if (NULL == m_wndView.m_hWnd)
				break;

			hr = m_wndView.QueryHost(&spHost);
			BREAK_IF_FAILED(hr);

			hr = spHost->CreateControl(CComBSTR(_T("{8856F961-340A-11D0-A96B-00C04FD705A2}")), m_wndView, 0);
			BREAK_IF_FAILED(hr);

			hr = m_wndView.QueryControl(&m_spWebProwser2); 
			BREAK_IF_FAILED(hr);	

			FillWinClient();

			CString sConfigFile = GetModuleFolderName() + "P2PIEClient.ini";
			char szUrl[256];
			GetPrivateProfileString( "Main", "DefaultUrl", "http://www.kan8888.com", szUrl, sizeof(szUrl), sConfigFile);
			OpenURL( szUrl);

		} while(false);

		return TRUE;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		FillWinClient();
		return 1;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		CloseDialog(0);
		return 0;
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		CloseDialog(0);
		return 0;
	}

	void CloseDialog(int nVal)
	{
		::PostQuitMessage(nVal);
	}

	void FillWinClient()
	{
		if ( IsWindow() && m_wndView.IsWindow() )
		{
			RECT rcClient;
			GetClientRect(&rcClient);
			m_wndView.MoveWindow(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);	
		}
	}

	void OpenURL(CString sURL)
	{
		if ( IsWindow() && m_wndView.IsWindow() && m_spWebProwser2 )
		{
			m_spWebProwser2->Navigate2(&CComVariant(_T(sURL)),NULL,NULL,NULL,NULL);
		}
	}
};
