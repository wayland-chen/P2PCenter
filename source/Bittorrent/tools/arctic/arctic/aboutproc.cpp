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

static void About_OnClose(HWND hwnd) {
	DestroyWindow(hwnd);
}

static void About_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
	switch(id) {
		case IDC_HOMEPAGE:
			ShellExecute(NULL, NULL, L"http://dev.int64.org/arctic.html", NULL, NULL, SW_SHOW);
			break;
		case IDOK:
			DestroyWindow(hwnd);
			break;
	}
}

static void About_OnDestroy(HWND hwnd) {
	about=NULL;
}

static BOOL About_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
	HRSRC textres=FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_ABOUTTEXT), L"TEXT");
	HGLOBAL text=LoadResource(GetModuleHandle(NULL), textres);

	const std::string s((LPCSTR)LockResource(text), SizeofResource(GetModuleHandle(NULL), textres));
	
	UnlockResource(text);
	FreeResource(text);

	SetWindowTextA(GetDlgItem(hwnd, IDC_LICENSE), s.c_str());

	return TRUE;
}

INT_PTR CALLBACK About_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		HANDLE_MSG(hwnd, WM_CLOSE, About_OnClose);
		HANDLE_MSG(hwnd, WM_COMMAND, About_OnCommand);
		HANDLE_MSG(hwnd, WM_DESTROY, About_OnDestroy);
		HANDLE_MSG(hwnd, WM_INITDIALOG, About_OnInitDialog);
		default: return FALSE;
	}
}
