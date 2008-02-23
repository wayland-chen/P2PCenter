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

static void InitMainClass() {
	WNDCLASSEX wc={0};
	wc.cbSize=sizeof(WNDCLASSEX);
	wc.lpfnWndProc=Main_WndProc;
	wc.hInstance=GetModuleHandle(NULL);
	wc.hCursor=LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName=L"ArcticMain";
	wc.lpszMenuName=MAKEINTRESOURCE(IDR_MAINMENU);
	wc.hIcon=(HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ARCTIC), IMAGE_ICON, 32, 32, LR_SHARED);
	wc.hIconSm=(HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ARCTIC), IMAGE_ICON, 16, 16, LR_SHARED);
	RegisterClassEx(&wc);
}

static void InitDebugClass() {
	WNDCLASSEX wc={0};
	wc.cbSize=sizeof(WNDCLASSEX);
	wc.lpfnWndProc=Debug_WndProc;
	wc.hInstance=GetModuleHandle(NULL);
	wc.hCursor=LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName=L"ArcticDebug";
	wc.hIcon=(HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ARCTIC), IMAGE_ICON, 32, 32, LR_SHARED);
	wc.hIconSm=(HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ARCTIC), IMAGE_ICON, 16, 16, LR_SHARED);
	RegisterClassEx(&wc);
}

static bool IsAssociated() {
	HKEY key;
	RegCreateKeyEx(HKEY_CLASSES_ROOT, L".torrent", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &key, NULL);

	wchar_t app[128];
	unsigned long applen=sizeof(app);

	bool associated=(RegQueryValueEx(key, NULL, NULL, NULL, (LPBYTE)app, &applen)==ERROR_SUCCESS && !wcscmp(app, L"Arctic.torrent"));

	RegCloseKey(key);
	return associated;
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int nCmdShow) {
	HWND hwnd;
	{
		HANDLE mutex=CreateMutex(NULL, TRUE, L"Global\\ArcticTorrent");
		if(GetLastError()==ERROR_ALREADY_EXISTS) {
			hwnd=FindWindow(L"ArcticMain", L"Arctic");
			if(hwnd) {
				ShowWindow(hwnd, SW_SHOWNORMAL);
				SetForegroundWindow(hwnd);
			}
			return 0;
		}
	}

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	char* pclocale = setlocale(LC_CTYPE, ".ACP");
	InitCommonControls();
	InitMainClass();
	InitDebugClass();

	if(!conf.Load()) {
		if(!IsAssociated()) {
			wstring text=loadstring(IDS_ASSOCIATE);
			wstring caption=loadstring(IDS_ASSOCIATECAPTION);

			if(MessageBox(NULL, text.c_str(), caption.c_str(), MB_ICONQUESTION|MB_YESNO)==IDYES)
				Associate();
		}
		conf.Save();
	}

	{
		int x, y, w, h;
		
		const RECT &wpos=conf.position;
		if(wpos.top!=0 || wpos.left!=0 || wpos.bottom!=0 || wpos.right!=0) {
			x=wpos.left;
			y=wpos.top;
			w=wpos.right-wpos.left;
			h=wpos.bottom-wpos.top;
		}
		else {
			RECT rect;
			GetClientRect(GetDesktopWindow(), &rect);

			x=((rect.right-rect.left)/2)-380;
			y=((rect.bottom-rect.top)/2)-100;
			w=760;
			h=200;
		}

		hwnd=CreateWindowEx(WS_EX_ACCEPTFILES, L"ArcticMain", L"Arctic", WS_OVERLAPPEDWINDOW, x, y, w, h, NULL, NULL, NULL, NULL);
		if(!hwnd) return -1;
	}
	
	SendMessage(hwnd, DM_REPOSITION, 0, 0);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)>0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
