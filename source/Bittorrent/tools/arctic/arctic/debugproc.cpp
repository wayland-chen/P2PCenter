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

#define IDC_LIST		201

static void Debug_OnClose(HWND hwnd) {
	ShowWindow(hwnd, SW_HIDE);

	HMENU menu=GetMenu(main);
	CheckMenuItem(menu, ID_FILE_DEBUG, MF_BYCOMMAND|MF_UNCHECKED);
	debugopen=false;
}

static BOOL Debug_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) {
	RECT rect;
	GetClientRect(hwnd, &rect);

	HWND list=CreateWindow(WC_LISTVIEW, NULL, WS_CHILD|WS_VISIBLE|LVS_NOSORTHEADER|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL,
		0, 0, rect.right-rect.left, rect.bottom-rect.top,
		hwnd, (HMENU)IDC_LIST, NULL, NULL);
	ListView_SetExtendedListViewStyle(list, LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP);

	wstring buf;

	LVCOLUMN lvc={0};
	lvc.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvc.fmt=LVCFMT_LEFT;

	lvc.iSubItem=0;
	lvc.cx=64;
	buf=loadstring(IDS_TIME);
	lvc.pszText=(LPWSTR)buf.c_str();
	ListView_InsertColumn(list, 0, &lvc);

	lvc.iSubItem=1;
	lvc.cx=384;
	buf=loadstring(IDS_MESSAGE);
	lvc.pszText=(LPWSTR)buf.c_str();
	ListView_InsertColumn(list, 1, &lvc);

	return TRUE;
}

static void Debug_OnSize(HWND hwnd, UINT state, int cx, int cy) {
	MoveWindow(GetDlgItem(hwnd, IDC_LIST), 0, 0, cx, cy, TRUE);
}

static void Debug_OnAddDebugMessage(HWND hwnd, AddDebugMessageData *data) {
	HWND list=GetDlgItem(hwnd, IDC_LIST);

	if(ListView_GetItemCount(list)==20)
		ListView_DeleteItem(list, 19);

	LVITEM lvi={0};
	lvi.mask=LVIF_TEXT;

	lvi.iSubItem=0;
	lvi.pszText=data->time;
	ListView_InsertItem(list, &lvi);

	lvi.iSubItem=1;
	lvi.pszText=data->message;
	ListView_SetItem(list, &lvi);
}

LRESULT CALLBACK Debug_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		HANDLE_MSG(hwnd, WM_CLOSE, Debug_OnClose);
		HANDLE_MSG(hwnd, WM_CREATE, Debug_OnCreate);
		HANDLE_MSG(hwnd, WM_SIZE, Debug_OnSize);
		case WM_ARCTIC_ADDDEBUGMESSAGE: Debug_OnAddDebugMessage(hwnd, (AddDebugMessageData*)lParam); return 0;
		default: return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}
