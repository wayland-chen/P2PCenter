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

using std::pair;
using std::sort;
using std::vector;
using std::string;
using std::wstring;
using std::auto_ptr;
using std::exception;
using boost::filesystem::path;
using namespace boost::spirit;

#define IDC_LIST		201
#define IDC_STATUS	202

#define TIMER_UPDATE	1
#define TRAY_ARCTIC		1
#define WM_ARCTIC_TRAY (WM_APP+1)

static const UINT columns[]={
	IDS_NAME, IDS_SIZE, IDS_DOWNLOADED, IDS_UPLOADED,
	IDS_STATUS, IDS_PROGRESS, IDS_DOWNSPEED, IDS_UPSPEED,
	IDS_HEALTH, IDS_SEEDS, IDS_PEERS
};
static const size_t columncount=sizeof(columns)/sizeof(UINT);

struct Torrent {
	std::string file;
	libtorrent::torrent_handle handle;

	wstring cols[columncount];

	bool operator<(const Torrent &t) const {
		return wcsicmp(cols[0].c_str(), t.cols[0].c_str())<0;
	}
};

Configuration conf;

static libtorrent::session *session=NULL;
static vector<Torrent> torrents;
static bool allpaused=false;

HWND main=NULL;
HWND about=NULL;
static HWND debug=NULL;
bool debugopen=false;

static wstring paused;
static wstring queued;
static wstring checking;
static wstring connecting;
static wstring downloading;
static wstring seeding;
static wstring unknown;
static wstring bytes;
static wstring kibibytes;
static wstring mebibytes;
static wstring gibibytes;

static NOTIFYICONDATA nid={0};
static unsigned int traycreatedmessage=0;

static void Main_OnClose(HWND hwnd) {
	DestroyWindow(hwnd);
}

static wchar_t *strsize(double s) {
	double downloaded;
	const wchar_t *units;
	if(s>=1073741824) {
		downloaded=s/1073741824.0;
		units=gibibytes.c_str();
	}
	else if(s>=1048576) {
		downloaded=s/1048576.0;
		units=mebibytes.c_str();
	}
	else if(s>=1024) {
		downloaded=s/1024.0;
		units=kibibytes.c_str();
	}
	else {
		downloaded=s;
		units=bytes.c_str();
	}

	static wchar_t buf[64];
	StringCchPrintf(buf, 64, L"%.1f %s", downloaded, units);

	return buf;
}

static void AddTorrent(HWND hwnd, path file) {
	try {
		const path rfile=getmodulepath()/"resume"/file;

		libtorrent::entry metadata=bdecode(file);
		libtorrent::entry resumedata;
		

		if(boost::filesystem::exists(rfile)) {
			try {
				resumedata=bdecode(rfile);
			}
			catch(...) {
				wchar_t text[256], title[128];
				LoadString(GetModuleHandle(NULL), IDS_RESUMEERR, title, 128);
				LoadString(GetModuleHandle(NULL), IDS_RESUMEERRTEXT, text, 256);
				MessageBox(hwnd, text, title, MB_ICONERROR|MB_OK);

				boost::filesystem::remove(rfile);
			}
		}

		if(!boost::filesystem::exists(getmodulepath()/"torrents"))
			boost::filesystem::create_directory(getmodulepath()/"torrents");

		if(!boost::filesystem::exists(getmodulepath()/"torrents"/file.leaf()))
			boost::filesystem::copy_file(file, getmodulepath()/"torrents"/file.leaf());

		if(!boost::filesystem::exists(conf.savepath))
			boost::filesystem::create_directory(conf.savepath);

		vector<libtorrent::torrent_handle>::size_type i=torrents.size();

		Torrent t;
		t.file=file.leaf();
		t.handle=session->add_torrent(metadata, conf.savepath, resumedata);

		t.handle.set_max_uploads(conf.maxup);
		t.handle.set_max_connections(conf.maxcon);

		const libtorrent::torrent_info &info=t.handle.get_torrent_info();

		t.cols[0]=mbstowcs(info.name());
		t.cols[1]=strsize((double)info.total_size());

		torrents.push_back(t);
		sort(torrents.begin(), torrents.end());

		if(allpaused) t.handle.pause();

		HWND list=GetDlgItem(hwnd, IDC_LIST);
		ListView_SetItemCount(list, torrents.size());
	}
	catch(exception &ex) {
		wstring text=mbstowcs(ex.what());
		wstring caption=loadstring(IDS_EXCEPTION);

		MessageBox(hwnd, text.c_str(), caption.c_str(), MB_ICONERROR|MB_OK);
	}
}

static void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
	switch(id) {
		case ID_FILE_OPEN1: {
			wchar_t file[MAX_PATH]=L"";

			OPENFILENAME ofn={0};
			ofn.lStructSize=sizeof(OPENFILENAME);
			ofn.hwndOwner=hwnd;
			ofn.lpstrFilter=L"Torrent Files (*.torrent)\0*.torrent\0All Files (*.*)\0*.*\0";
			ofn.lpstrFile=file;
			ofn.nMaxFile=MAX_PATH;
			ofn.Flags=OFN_EXPLORER|OFN_HIDEREADONLY|OFN_FILEMUSTEXIST;
			ofn.lpstrDefExt=L"torrent";

			if(GetOpenFileName(&ofn)) AddTorrent(hwnd, wcstombs(file));
		} break;
		case ID_FILE_CONFIGURATION:
			if(DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CONFIG), hwnd, Config_DlgProc)==IDOK) {
				try {
					session->set_upload_rate_limit((conf.uplimit!=-1)?conf.uplimit*1024:-1);
					session->set_download_rate_limit((conf.downlimit!=-1)?conf.downlimit*1024:-1);

					session->listen_on(pair<int,int>(conf.firstport, conf.lastport));
					
					for(vector<Torrent>::size_type i=0; i<torrents.size(); i++)
						torrents[i].handle.move_storage(conf.savepath);
				}
				catch(exception &ex) {
					wstring text=mbstowcs(ex.what());
					wstring caption=loadstring(IDS_EXCEPTION);

					MessageBox(hwnd, text.c_str(), caption.c_str(), MB_ICONERROR|MB_OK);
				}
			}
			break;
		case ID_FILE_DEBUG:
			if(debugopen) {
				ShowWindow(debug, SW_HIDE);

				HMENU menu=GetMenu(hwnd);
				CheckMenuItem(menu, ID_FILE_DEBUG, MF_BYCOMMAND|MF_UNCHECKED);
				debugopen=false;
			}
			else {
				ShowWindow(debug, SW_SHOWNORMAL);
				SetForegroundWindow(debug);

				HMENU menu=GetMenu(hwnd);
				CheckMenuItem(menu, ID_FILE_DEBUG, MF_BYCOMMAND|MF_CHECKED);
				debugopen=true;
			}
			break;
		case ID_FILE_EXIT:
		case ID_TRAY_EXIT:
			DestroyWindow(hwnd);
			break;
		case ID_ABOUT_ABOUT:
		case ID_TRAY_ABOUT:
			if(!about) about=CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, About_DlgProc);
			else SetForegroundWindow(about);
			break;
		case ID_CONTEXT_OPEN: {
			HWND list=GetDlgItem(hwnd, IDC_LIST);
			int index=ListView_GetSelectionMark(list);

			if(index!=-1) {
				path p=torrents[index].handle.save_path()/torrents[index].handle.get_torrent_info().name();
				ShellExecuteA(NULL, "open", p.native_file_string().c_str(), NULL, NULL, SW_SHOW);
			}
		} break;
		case ID_CONTEXT_ANNOUNCE: {
			HWND list=GetDlgItem(hwnd, IDC_LIST);
			int index=ListView_GetSelectionMark(list);

			if(index!=-1) torrents[index].handle.force_reannounce();
		} break;
		case ID_CONTEXT_PAUSE: {
			HWND list=GetDlgItem(hwnd, IDC_LIST);
			int index=ListView_GetSelectionMark(list);

			if(index!=-1) torrents[index].handle.pause();
		} break;
		case ID_CONTEXT_RESUME: {
			HWND list=GetDlgItem(hwnd, IDC_LIST);
			int index=ListView_GetSelectionMark(list);

			if(index!=-1) torrents[index].handle.resume();
		} break;
		case ID_CONTEXT_REMOVE: {
			HWND list=GetDlgItem(hwnd, IDC_LIST);
			int index=ListView_GetSelectionMark(list);

			if(index!=-1) {
				wchar_t text[512];
				wstring caption=loadstring(IDS_REMOVECAPTION);

				wstring fmt=loadstring(IDS_REMOVE);
				string name=torrents[index].handle.get_torrent_info().name();
				StringCchPrintf(text, 512, fmt.c_str(), name.c_str());

				if(MessageBox(hwnd, text, caption.c_str(), MB_ICONQUESTION|MB_YESNO)==IDYES) {
					ListView_DeleteItem(list, index);

					session->remove_torrent(torrents[index].handle);

					{
						try {
							boost::filesystem::remove(getmodulepath()/"torrents"/torrents[index].file);
							boost::filesystem::remove(getmodulepath()/"resume"/torrents[index].file);
						}
						catch(...) {
							// whatever.
						}
					}

					torrents.erase(torrents.begin()+index);
				}
			}
		} break;
		case ID_TRAY_OPEN:
			ShowWindow(hwnd, SW_SHOWNORMAL);
			SetForegroundWindow(hwnd);
			break;
		case ID_TRAY_PAUSE:
			for(vector<Torrent>::size_type i=0; i<torrents.size(); i++)
				torrents[i].handle.pause();
			allpaused=true;
			break;
		case ID_TRAY_RESUME:
			for(vector<Torrent>::size_type i=0; i<torrents.size(); i++)
				torrents[i].handle.resume();
			allpaused=false;
			break;
	}
}

static BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) {
	////////////////////////////////////////////////////////////
	/// Setup tray icon

	nid.cbSize=sizeof(NOTIFYICONDATA);
	nid.hWnd=hwnd;
	nid.uID=TRAY_ARCTIC;
	nid.uCallbackMessage=WM_ARCTIC_TRAY;
	nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
	nid.hIcon=(HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ARCTIC), IMAGE_ICON, 16, 16, LR_SHARED);
	StringCchCopy(nid.szTip, 64, L"Arctic");

	Shell_NotifyIcon(NIM_ADD, &nid);
	traycreatedmessage=RegisterWindowMessage(L"TaskbarCreated");

	////////////////////////////////////////////////////////////
	/// Setup status bar

	HWND status=CreateWindow(STATUSCLASSNAME, NULL, WS_CHILD|WS_VISIBLE,
		0, 0, 0, 0,
		hwnd, (HMENU)IDC_STATUS, NULL, NULL);

	RECT statusrect;
	GetWindowRect(status, &statusrect);

	int parts[3];
	parts[0]=std::max(((statusrect.right-statusrect.left)-192), (LONG)0);
	parts[1]=parts[0]+96;
	parts[2]=parts[1]+96;

	SendMessage(status, SB_SETPARTS, 3, (LPARAM)parts);
	SendMessage(status, SB_SETTEXT, 0|SBT_NOBORDERS, (LPARAM)L"");
	SendMessage(status, SB_SETTEXT, 1|SBT_NOBORDERS, (LPARAM)L"");
	SendMessage(status, SB_SETTEXT, 2|SBT_NOBORDERS, (LPARAM)L"");
	
	////////////////////////////////////////////////////////////
	/// Setup ListView

	RECT rect;
	GetClientRect(hwnd, &rect);

	HWND list=CreateWindow(WC_LISTVIEW, NULL, WS_CHILD|WS_VISIBLE|LVS_NOSORTHEADER|LVS_OWNERDATA|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL,
		0, 0, rect.right-rect.left, (rect.bottom-rect.top)-(statusrect.bottom-statusrect.top),
		hwnd, (HMENU)IDC_LIST, NULL, NULL);
	ListView_SetExtendedListViewStyle(list, LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP);

	wstring buf;

	LVCOLUMN lvc={0};
	lvc.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvc.fmt=LVCFMT_LEFT;

	for(size_t i=0; i<columncount; i++) {
		lvc.iSubItem=(int)i;
		lvc.cx=conf.columns[i];
		buf=loadstring(columns[i]);
		lvc.pszText=(LPWSTR)buf.c_str();

		ListView_InsertColumn(list, i, &lvc);
	}
	
	////////////////////////////////////////////////////////////
	/// Create torrent session

	try {
		session=new libtorrent::session(libtorrent::fingerprint("AR", 1, 0, 0, 1), pair<int,int>(conf.firstport, conf.lastport));

		session->disable_extensions();
		session->enable_extension(libtorrent::peer_connection::extended_metadata_message);
		session->enable_extension(libtorrent::peer_connection::extended_peer_exchange_message);
		session->enable_extension(libtorrent::peer_connection::extended_listen_port_message);

		session->set_upload_rate_limit((conf.uplimit!=-1)?conf.uplimit*1024:-1);
		session->set_download_rate_limit((conf.downlimit!=-1)?conf.downlimit*1024:-1);
		session->set_max_connections(conf.maxcon);

#ifdef _DEBUG
		session->set_severity_level(libtorrent::alert::debug);
#else
		session->set_severity_level(libtorrent::alert::info);
#endif
	}
	catch(exception &ex) {
		wstring text=mbstowcs(ex.what());
		wstring caption=loadstring(IDS_EXCEPTION);

		MessageBox(hwnd, text.c_str(), caption.c_str(), MB_ICONERROR|MB_OK);
		return -1;
	}
	
	////////////////////////////////////////////////////////////
	/// Create debug window.

	main=hwnd;

	GetWindowRect(hwnd, &rect);
	
	wstring caption=loadstring(IDS_DEBUG);
	debug=CreateWindowEx(WS_EX_APPWINDOW, L"ArcticDebug", caption.c_str(), WS_OVERLAPPEDWINDOW,
		rect.left+32, rect.top+32, 512, 160,
		hwnd, NULL, NULL, NULL);
	if(!debug) DestroyWindow(hwnd);
	
	////////////////////////////////////////////////////////////
	/// Load internationalized strings and start timer.

	paused=loadstring(IDS_PAUSED);
	queued=loadstring(IDS_QUEUED);
	checking=loadstring(IDS_CHECKING);
	connecting=loadstring(IDS_CONNECTING);
	downloading=loadstring(IDS_DOWNLOADING);
	seeding=loadstring(IDS_SEEDING);
	unknown=loadstring(IDS_UNKNOWN);
	bytes=loadstring(IDS_BYTES);
	kibibytes=loadstring(IDS_KIBIBYTES);
	mebibytes=loadstring(IDS_MEBIBYTES);
	gibibytes=loadstring(IDS_GIBIBYTES);

	SetTimer(hwnd, TIMER_UPDATE, 1000, NULL);
	
	////////////////////////////////////////////////////////////
	/// Resume torrents from last session.

	path p=getmodulepath()/"torrents";

	WIN32_FIND_DATAA finddata={0};
	HANDLE find=FindFirstFileA((p/"*.torrent").native_file_string().c_str(), &finddata);
	if(find!=INVALID_HANDLE_VALUE) {
		do AddTorrent(hwnd, p/finddata.cFileName);
		while(FindNextFileA(find, &finddata));
		FindClose(find);
	}

	return TRUE;
}

static void Main_OnDestroy(HWND hwnd) {
	Shell_NotifyIcon(NIM_DELETE, &nid);

	path p=getmodulepath()/"resume";
	CreateDirectoryA(p.native_directory_string().c_str(), NULL);

	for(vector<Torrent>::size_type i=0; i<torrents.size(); i++) {
		torrents[i].handle.pause();
		libtorrent::entry e=torrents[i].handle.write_resume_data();

		bencode(p/torrents[i].file, e);
	}

	delete session;

	HWND list=GetDlgItem(hwnd, IDC_LIST);

	for(int i=0; i<columncount; i++)
		conf.columns[i]=ListView_GetColumnWidth(list, i);

	conf.Save();

	PostQuitMessage(0);
}

static void Main_OnDropFiles(HWND hwnd, HDROP hdrop) {
	char file[MAX_PATH];

	unsigned int amount=DragQueryFileA(hdrop, 0xFFFFFFFF, 0, 0);
	while(amount--) {
		DragQueryFileA(hdrop, amount, file, MAX_PATH);
		AddTorrent(hwnd, file);
	}
	DragFinish(hdrop);
}

static void Main_OnEndSession(HWND hwnd, BOOL fEnding) {
	if(fEnding) Main_OnDestroy(hwnd);
}

static BOOL Main_OnEraseBkgnd(HWND hwnd, HDC hdc) {
	return TRUE;
}

static void Main_OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo) {
	lpMinMaxInfo->ptMinTrackSize.x=512;
	lpMinMaxInfo->ptMinTrackSize.y=200;
}

static LRESULT Main_OnNotify(HWND hwnd, int idCtrl, LPNMHDR lpNmh) {
	if(lpNmh->idFrom==IDC_LIST) {
		if(lpNmh->code==LVN_GETDISPINFO) {
			NMLVDISPINFO *di=(NMLVDISPINFO*)lpNmh;

			if(di->item.iItem<(int)torrents.size() && di->item.iSubItem<(int)columncount)
				di->item.pszText=(LPTSTR)torrents[di->item.iItem].cols[di->item.iSubItem].c_str();
		}
		else if(lpNmh->code==NM_RCLICK && ListView_GetSelectedCount(lpNmh->hwndFrom)>0) {
			POINT pt;
			GetCursorPos(&pt);

			HMENU menu=LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_CONTEXTMENU));
			HMENU submenu=GetSubMenu(menu, 0);

			int index=ListView_GetSelectionMark(lpNmh->hwndFrom);

			if(torrents[index].handle.is_paused()) {
				EnableMenuItem(submenu, ID_CONTEXT_PAUSE, MF_BYCOMMAND|MF_GRAYED);
				EnableMenuItem(submenu, ID_CONTEXT_RESUME, MF_BYCOMMAND|MF_ENABLED);
			}
			else {
				EnableMenuItem(submenu, ID_CONTEXT_PAUSE, MF_BYCOMMAND|MF_ENABLED);
				EnableMenuItem(submenu, ID_CONTEXT_RESUME, MF_BYCOMMAND|MF_GRAYED);
			}

			SetForegroundWindow(hwnd);
			TrackPopupMenu(submenu, TPM_LEFTALIGN|TPM_TOPALIGN, pt.x, pt.y, 0, hwnd, NULL);
		}
	}
	return 0;
}

static void Main_OnSize(HWND hwnd, UINT state, int cx, int cy) {
	if(state==SIZE_MINIMIZED) ShowWindow(hwnd, SW_HIDE);
	else {
		HWND status=GetDlgItem(hwnd, IDC_STATUS);
		SendMessage(status, WM_SIZE, 0, 0);

		RECT statusrect;
		GetWindowRect(status, &statusrect);

		int parts[3];
		parts[0]=std::max(((statusrect.right-statusrect.left)-192), (LONG)0);
		parts[1]=parts[0]+96;
		parts[2]=parts[1]+96;

		SendMessage(status, SB_SETPARTS, 3, (LPARAM)parts);

		MoveWindow(GetDlgItem(hwnd, IDC_LIST), 0, 0, cx, cy-(statusrect.bottom-statusrect.top), TRUE);

		if(state==SIZE_RESTORED) {
			RECT rc;
			GetWindowRect(hwnd, &rc);

			if(rc.left>=0 && rc.top>=0 && rc.right>=0 && rc.bottom>=0)
				conf.position=rc;
		}
	}
}

static void Main_OnTimer(HWND hwnd, UINT id) {
	if(id==TIMER_UPDATE) {	
		////////////////////////////////////////////////////////////
		/// Update status bar

		wchar_t buf[32];
		HWND status=GetDlgItem(hwnd, IDC_STATUS);
		libtorrent::session_status s=session->status();

		StringCchPrintf(buf, 32, L"D: %s/s", strsize(s.download_rate));
		SendMessage(status, SB_SETTEXT, 1|SBT_NOBORDERS, (LPARAM)buf);

		StringCchPrintf(buf, 32, L"U: %s/s", strsize(s.upload_rate));
		SendMessage(status, SB_SETTEXT, 2|SBT_NOBORDERS, (LPARAM)buf);

		HWND list=GetDlgItem(hwnd, IDC_LIST);
		for(vector<Torrent>::size_type i=0; i<torrents.size(); i++) {
			try {
				////////////////////////////////////////////////////////////
				/// Update data in ListView

				libtorrent::torrent_status status=torrents[i].handle.status();

				torrents[i].cols[2]=strsize((double)status.total_done);
				torrents[i].cols[3]=strsize((double)status.total_upload);

				if(status.paused) torrents[i].cols[4]=paused;
				else switch(status.state) {
					case libtorrent::torrent_status::queued_for_checking:
						torrents[i].cols[4]=queued;
						break;
					case libtorrent::torrent_status::checking_files:
						torrents[i].cols[4]=checking;
						break;
					case libtorrent::torrent_status::connecting_to_tracker:
						torrents[i].cols[4]=connecting;
						break;
					case libtorrent::torrent_status::downloading:
					case libtorrent::torrent_status::downloading_metadata:
						torrents[i].cols[4]=downloading;
						break;
					case libtorrent::torrent_status::seeding:
						torrents[i].cols[4]=seeding;
						break;
					default:
						torrents[i].cols[4]=unknown;
						break;
				}

				StringCchPrintf(buf, 32, L"%.1f%%", (double)status.progress*100.0);
				torrents[i].cols[5]=buf;

				StringCchPrintf(buf, 32, L"%s/s", strsize(status.download_rate));
				torrents[i].cols[6]=buf;

				StringCchPrintf(buf, 32, L"%s/s", strsize(status.upload_rate));
				torrents[i].cols[7]=buf;

				StringCchPrintf(buf, 32, L"%d%%", (int)(status.distributed_copies*100.0f));
				torrents[i].cols[8]=buf;

				torrents[i].cols[9]=_itow(status.num_seeds, buf, 10);
				torrents[i].cols[10]=_itow(status.num_peers, buf, 10);

				InvalidateRect(list, NULL, FALSE);
	
				////////////////////////////////////////////////////////////
				/// Process alerts
				
				for(auto_ptr<libtorrent::alert> a=session->pop_alert(); a.get(); a=session->pop_alert()) {
					string timestamp=to_simple_string(a->timestamp().time_of_day());
					string message=a->msg();

					wstring timestamp_wcs=mbstowcs(timestamp);
					wstring message_wcs=mbstowcs(message);

					AddDebugMessageData data;
					data.time=(LPWSTR)timestamp_wcs.c_str();
					data.message=(LPWSTR)message_wcs.c_str();
					
					SendMessage(debug, WM_ARCTIC_ADDDEBUGMESSAGE, 0, (LPARAM)&data);
				}
			}
			catch(exception &ex) {
				wstring text=mbstowcs(ex.what());
				wstring caption=loadstring(IDS_EXCEPTION);

				MessageBox(hwnd, text.c_str(), caption.c_str(), MB_ICONERROR|MB_OK);
				continue;
			}
		}
	}
}

static void Main_OnDDEInitiate(HWND hwnd, HWND client, ATOM appatom, ATOM topicatom) {
	ATOM a=GlobalAddAtom(L"ArcticTorrent");
	ATOM t=GlobalAddAtom(L"system");

	if(appatom==a && topicatom==t)
		SendMessage(client, WM_DDE_ACK, (WPARAM)hwnd, MAKELPARAM(a,t));

	GlobalDeleteAtom(a);
	GlobalDeleteAtom(t);
}

static bool parse_ddeopen(vector<wstring> &res, const wstring &str) {
	return parse(str.begin(), str.end(),
		L"open(" >> (('"' >> (+~ch_p('"'))[push_back_a(res)] >> '"') % ',') >> ')'
	).full;
}

static void Main_OnDDEExecute(HWND hwnd, HWND client, HGLOBAL hCmdStr) {
	wstring command((const wchar_t*)GlobalLock(hCmdStr));
	GlobalUnlock(hCmdStr);

	vector<wstring> res;
	if(parse_ddeopen(res, command)) {
		for(vector<wstring>::size_type i=0; i<res.size(); i++)
			AddTorrent(hwnd, wcstombs(res[i]));
	}

	LPARAM ack=PackDDElParam(WM_DDE_ACK, 1, (UINT_PTR)hCmdStr);
	PostMessage(client, WM_DDE_ACK, (WPARAM)hwnd, ack);
}

static void Main_OnDDETerminate(HWND hwnd, HWND client) {
	PostMessage(client, WM_DDE_TERMINATE, (WPARAM)hwnd, 0);
}

static void Main_OnTray(HWND hwnd, UINT idTray, UINT msg) {
	if(idTray==TRAY_ARCTIC) {
		if(msg==WM_LBUTTONUP) {
			ShowWindow(hwnd, SW_SHOWNORMAL);
			SetForegroundWindow(hwnd);
		}
		else if(msg==WM_RBUTTONUP) {
			POINT pt;
			GetCursorPos(&pt);

			HMENU menu=LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_TRAYMENU));
			HMENU submenu=GetSubMenu(menu, 0);

			if(allpaused) {
				EnableMenuItem(submenu, ID_TRAY_PAUSE, MF_BYCOMMAND|MF_GRAYED);
				EnableMenuItem(submenu, ID_TRAY_RESUME, MF_BYCOMMAND|MF_ENABLED);
			}
			else {
				EnableMenuItem(submenu, ID_TRAY_PAUSE, MF_BYCOMMAND|MF_ENABLED);
				EnableMenuItem(submenu, ID_TRAY_RESUME, MF_BYCOMMAND|MF_GRAYED);
			}

			SetForegroundWindow(hwnd);
			TrackPopupMenu(submenu, TPM_LEFTALIGN|TPM_TOPALIGN, pt.x, pt.y, 0, hwnd, NULL);
		}
	}
}

LRESULT CALLBACK Main_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		HANDLE_MSG(hwnd, WM_CLOSE, Main_OnClose);
		HANDLE_MSG(hwnd, WM_COMMAND, Main_OnCommand);
		HANDLE_MSG(hwnd, WM_CREATE, Main_OnCreate);
		HANDLE_MSG(hwnd, WM_DESTROY, Main_OnDestroy);
		HANDLE_MSG(hwnd, WM_DROPFILES, Main_OnDropFiles);
		HANDLE_MSG(hwnd, WM_ERASEBKGND, Main_OnEraseBkgnd);
		HANDLE_MSG(hwnd, WM_GETMINMAXINFO, Main_OnGetMinMaxInfo);
		HANDLE_MSG(hwnd, WM_NOTIFY, Main_OnNotify);
		HANDLE_MSG(hwnd, WM_SIZE, Main_OnSize);
		HANDLE_MSG(hwnd, WM_TIMER, Main_OnTimer);
		case WM_DDE_INITIATE:
			Main_OnDDEInitiate(hwnd, (HWND)wParam, LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_DDE_EXECUTE:
			Main_OnDDEExecute(hwnd, (HWND)wParam, (HGLOBAL)lParam);
			return 0;
		case WM_DDE_TERMINATE:
			Main_OnDDETerminate(hwnd, (HWND)wParam);
			return 0;
		case WM_ARCTIC_TRAY:
			Main_OnTray(hwnd, (UINT)wParam, (UINT)lParam);
			return 0;
		default:
			if(msg==traycreatedmessage) {
				Shell_NotifyIcon(NIM_ADD, &nid);
				return 0;
			}
			else return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}
