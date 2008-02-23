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

#ifndef __COMMON_H__
#define __COMMON_H__

#define WM_ARCTIC_ADDDEBUGMESSAGE (WM_APP+1)

struct AddDebugMessageData {
	LPWSTR time;
	LPWSTR message;
};

extern LRESULT CALLBACK Main_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern LRESULT CALLBACK Debug_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern INT_PTR CALLBACK About_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern INT_PTR CALLBACK Config_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

extern boost::filesystem::path getmodulepath();
extern libtorrent::entry bdecode(const boost::filesystem::path &file);
extern bool bencode(const boost::filesystem::path &file, const libtorrent::entry &e);
extern std::wstring loadstring(UINT id);
extern std::string loadstringa(UINT id);

extern std::wstring mbstowcs(const std::string &str);
extern std::string wcstombs(const std::wstring &str);

extern void Associate();

extern Configuration conf;
extern HWND main;
extern HWND about;
extern bool debugopen;


#endif
