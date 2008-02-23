/*
	Copyright (c) 2004-2005 Cory Nelson

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "stdafx.h"
#include "resource.h"

using std::wstring;

static void Config_OnClose(HWND hwnd) {
	EndDialog(hwnd, IDCANCEL);
}

static void Config_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
	switch(id) {
		case IDC_LIMITUPLOAD: {
			BOOL state=(IsDlgButtonChecked(hwnd, IDC_LIMITUPLOAD)==BST_CHECKED)?TRUE:FALSE;
			EnableWindow(GetDlgItem(hwnd, IDC_UPLOADLIMIT), state);
			EnableWindow(GetDlgItem(hwnd, IDC_UPLOADLIMITSPIN), state);

			if(!state) {
				EnableWindow(GetDlgItem(hwnd, IDC_LIMITDOWNLOAD), TRUE);
				EnableWindow(GetDlgItem(hwnd, IDC_DOWNLOADLIMITSPIN), TRUE);
				SendDlgItemMessage(hwnd, IDC_DOWNLOADLIMIT, EM_SETREADONLY, FALSE, 0);
				break;
			}
		}
		case IDC_UPLOADLIMIT: {
			int i=(int)GetDlgItemInt(hwnd, IDC_UPLOADLIMIT, NULL, FALSE);
			if(i==0 || i>=12) {
				EnableWindow(GetDlgItem(hwnd, IDC_LIMITDOWNLOAD), TRUE);
				EnableWindow(GetDlgItem(hwnd, IDC_DOWNLOADLIMITSPIN), TRUE);
				SendDlgItemMessage(hwnd, IDC_DOWNLOADLIMIT, EM_SETREADONLY, FALSE, 0);
			}
			else {
				EnableWindow(GetDlgItem(hwnd, IDC_LIMITDOWNLOAD), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_DOWNLOADLIMITSPIN), FALSE);
				SendDlgItemMessage(hwnd, IDC_DOWNLOADLIMIT, EM_SETREADONLY, TRUE, 0);
				SendDlgItemMessage(hwnd, IDC_DOWNLOADLIMITSPIN, UDM_SETPOS, 0, MAKELONG(i*5, 0));
			}
		} break;
		case IDC_LIMITDOWNLOAD: {
			BOOL state=(IsDlgButtonChecked(hwnd, IDC_LIMITDOWNLOAD)==BST_CHECKED)?TRUE:FALSE;
			EnableWindow(GetDlgItem(hwnd, IDC_DOWNLOADLIMIT), state);
			EnableWindow(GetDlgItem(hwnd, IDC_DOWNLOADLIMITSPIN), state);
		} break;
		case IDC_FIRSTPORT:
			if(codeNotify==EN_KILLFOCUS) {
				int i=(int)GetDlgItemInt(hwnd, IDC_FIRSTPORT, NULL, TRUE);
				int limit=(int)GetDlgItemInt(hwnd, IDC_LASTPORT, NULL, TRUE);

				if(i>limit) SetDlgItemInt(hwnd, IDC_FIRSTPORT, limit, TRUE);
			}
			break;
		case IDC_LASTPORT:
			if(codeNotify==EN_KILLFOCUS) {
				int i=(int)GetDlgItemInt(hwnd, IDC_LASTPORT, NULL, TRUE);
				int limit=(int)GetDlgItemInt(hwnd, IDC_FIRSTPORT, NULL, TRUE);

				if(i<limit) SetDlgItemInt(hwnd, IDC_LASTPORT, limit, TRUE);
				else if(i>65535) SetDlgItemInt(hwnd, IDC_LASTPORT, 65536, FALSE);
			}
			break;
		case IDC_BROWSE: {
			wstring title=loadstring(IDS_DOWNPATH);

			wchar_t folder[MAX_PATH]={0};

			BROWSEINFO info={0};
			info.hwndOwner=hwnd;
			info.pszDisplayName=folder;
			info.lpszTitle=title.c_str();
			info.ulFlags=BIF_USENEWUI;

			ITEMIDLIST *id=SHBrowseForFolder(&info);
			if(id && SHGetPathFromIDList(id, folder))
				SetDlgItemText(hwnd, IDC_DOWNLOADPATH, folder);
		} break;
		case IDC_ASSOCIATE:
			Associate();
			break;
		case IDOK:
			if(IsDlgButtonChecked(hwnd, IDC_LIMITUPLOAD)==BST_CHECKED) {
				conf.uplimit=(int)GetDlgItemInt(hwnd, IDC_UPLOADLIMIT, NULL, TRUE);
				if(conf.uplimit==0) conf.uplimit=-1;
			}
			else conf.uplimit=-1;

			if(IsDlgButtonChecked(hwnd, IDC_LIMITDOWNLOAD)==BST_CHECKED) {
				conf.downlimit=(int)GetDlgItemInt(hwnd, IDC_DOWNLOADLIMIT, NULL, TRUE);
				if(conf.downlimit==0) conf.downlimit=-1;
			}
			else conf.downlimit=-1;

			conf.firstport=(int)GetDlgItemInt(hwnd, IDC_FIRSTPORT, NULL, TRUE);
			conf.lastport=(int)GetDlgItemInt(hwnd, IDC_LASTPORT, NULL, TRUE);
			if(conf.uplimit>-1 && conf.uplimit<12) conf.downlimit=conf.uplimit*5;

			char path[MAX_PATH];
			GetDlgItemTextA(hwnd, IDC_DOWNLOADPATH, path, MAX_PATH);

			conf.savepath=boost::filesystem::path(path);

			conf.Save();
			EndDialog(hwnd, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
	}
}

static BOOL Config_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
	SendDlgItemMessage(hwnd, IDC_UPLOADLIMIT, EM_SETLIMITTEXT, 4, 0);
	SendDlgItemMessage(hwnd, IDC_UPLOADLIMITSPIN, UDM_SETRANGE, 0, MAKELONG(9999, 0));

	SendDlgItemMessage(hwnd, IDC_DOWNLOADLIMIT, EM_SETLIMITTEXT, 4, 0);
	SendDlgItemMessage(hwnd, IDC_DOWNLOADLIMITSPIN, UDM_SETRANGE, 0, MAKELONG(9999, 0));

	SendDlgItemMessage(hwnd, IDC_FIRSTPORT, EM_SETLIMITTEXT, 5, 0);
	SendDlgItemMessage(hwnd, IDC_FIRSTPORTSPIN, UDM_SETRANGE32, 0, 65535);

	SendDlgItemMessage(hwnd, IDC_LASTPORT, EM_SETLIMITTEXT, 5, 0);
	SendDlgItemMessage(hwnd, IDC_LASTPORTSPIN, UDM_SETRANGE32, 0, 65535);

	if(conf.downlimit>0) {
		CheckDlgButton(hwnd, IDC_LIMITDOWNLOAD, BST_CHECKED);
		SetDlgItemInt(hwnd, IDC_DOWNLOADLIMIT, (UINT)conf.downlimit, TRUE);

		EnableWindow(GetDlgItem(hwnd, IDC_DOWNLOADLIMIT), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_DOWNLOADLIMITSPIN), TRUE);
	}
	else SendDlgItemMessage(hwnd, IDC_DOWNLOADLIMITSPIN, UDM_SETPOS, 0, 0);

	if(conf.uplimit>0) {
		CheckDlgButton(hwnd, IDC_LIMITUPLOAD, BST_CHECKED);
		SetDlgItemInt(hwnd, IDC_UPLOADLIMIT, (UINT)conf.uplimit, TRUE);

		EnableWindow(GetDlgItem(hwnd, IDC_UPLOADLIMIT), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_UPLOADLIMITSPIN), TRUE);
	}
	else SendDlgItemMessage(hwnd, IDC_UPLOADLIMITSPIN, UDM_SETPOS, 0, 0);

	SetDlgItemInt(hwnd, IDC_FIRSTPORT, (UINT)conf.firstport, TRUE);
	SetDlgItemInt(hwnd, IDC_LASTPORT, (UINT)conf.lastport, TRUE);

	SetDlgItemTextA(hwnd, IDC_DOWNLOADPATH, conf.savepath.native_directory_string().c_str());

	return TRUE;
}

static LRESULT Config_OnNotify(HWND hwnd, int idCtrl, LPNMHDR lpNmhdr) {
	if(lpNmhdr->idFrom==IDC_FIRSTPORTSPIN && lpNmhdr->code==UDN_DELTAPOS) {
		int limit=(int)GetDlgItemInt(hwnd, IDC_LASTPORT, NULL, TRUE);

		NMUPDOWN *nmud=(NMUPDOWN*)lpNmhdr;
		if((nmud->iPos+nmud->iDelta)>limit) nmud->iDelta=0;
	}
	else if(lpNmhdr->idFrom==IDC_LASTPORTSPIN && lpNmhdr->code==UDN_DELTAPOS) {
		int limit=(int)GetDlgItemInt(hwnd, IDC_FIRSTPORT, NULL, TRUE);

		NMUPDOWN *nmud=(NMUPDOWN*)lpNmhdr;
		if((nmud->iPos+nmud->iDelta)<limit) nmud->iDelta=0;
	}
	return FALSE;
}

INT_PTR CALLBACK Config_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		HANDLE_MSG(hwnd, WM_CLOSE, Config_OnClose);
		HANDLE_MSG(hwnd, WM_COMMAND, Config_OnCommand);
		HANDLE_MSG(hwnd, WM_INITDIALOG, Config_OnInitDialog);
		HANDLE_MSG(hwnd, WM_NOTIFY, Config_OnNotify);
		default: return FALSE;
	}
}
