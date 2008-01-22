// AppOpt.h : CAppOpt 的声明
#pragma once
#include "resource.h"       // 主符号
#include <atlctl.h>

// IAppOpt
[
	object,
	uuid(759FA1AD-9B89-4E2D-925F-BA1290B76D6A),
	dual,
	helpstring("IAppOpt 接口"),
	pointer_default(unique)
]
__interface IAppOpt : public IDispatch
{
	[id(1), helpstring("方法StartApp")] HRESULT StartApp(void);
	[id(2), helpstring("方法StopApp")] HRESULT StopApp(void);
	[propget, id(3), helpstring("属性 ListenPort")] HRESULT ListenPort([out, retval] ULONG* pVal);
	[propget, id(4), helpstring("属性 ServiceVersion")] HRESULT ServiceVersion([out, retval] BSTR* pVal);
	[id(5), helpstring("方法UpdateService")] HRESULT UpdateService(void);
};


// CAppOpt
[
	coclass,
	threading("apartment"),
	vi_progid("P2PWebAX.AppOpt"),
	progid("P2PWebAX.AppOpt.1"),
	version(1.0),
	uuid("E7506F35-82CD-4DCF-8F77-923A87A2D91A"),
	helpstring("AppOpt Class"),
	support_error_info(IAppOpt),
	registration_script("control.rgs")
]
class ATL_NO_VTABLE CAppOpt : 
	public IAppOpt,
	public IPersistStreamInitImpl<CAppOpt>,
	public IOleControlImpl<CAppOpt>,
	public IOleObjectImpl<CAppOpt>,
	public IOleInPlaceActiveObjectImpl<CAppOpt>,
	public IViewObjectExImpl<CAppOpt>,
	public IOleInPlaceObjectWindowlessImpl<CAppOpt>,
	public IPersistStorageImpl<CAppOpt>,
	public ISpecifyPropertyPagesImpl<CAppOpt>,
	public IQuickActivateImpl<CAppOpt>,
	public IDataObjectImpl<CAppOpt>,
	public IProvideClassInfo2Impl<&__uuidof(CAppOpt), NULL>,
	public CComControl<CAppOpt>
{
public:

	CAppOpt()
	{
	}

	BEGIN_CATEGORY_MAP(CAppOpt)
		IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
		IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
	END_CATEGORY_MAP()

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_ACTSLIKELABEL |
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)


BEGIN_PROP_MAP(CAppOpt)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// 示例项
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(CAppOpt)
	CHAIN_MSG_MAP(CComControl<CAppOpt>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// 处理程序原型: 
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IAppOpt
public:
		HRESULT OnDraw(ATL_DRAWINFO& di)
		{
		RECT& rc = *(RECT*)di.prcBounds;
		// 将剪辑区域设置为 di.prcBounds 指定的矩形
		HRGN hRgnOld = NULL;
		if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
			hRgnOld = NULL;
		bool bSelectOldRgn = false;

		HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

		if (hRgnNew != NULL)
		{
			bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
		}

		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("ATL 7.0 : AppOpt");
		TextOut(di.hdcDraw, 
			(rc.left + rc.right) / 2, 
			(rc.top + rc.bottom) / 2, 
			pszText, 
			lstrlen(pszText));

		if (bSelectOldRgn)
			SelectClipRgn(di.hdcDraw, hRgnOld);

		return S_OK;
	}


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}
	STDMETHOD(StartApp)(void);
	STDMETHOD(StopApp)(void);
	STDMETHOD(get_ListenPort)(ULONG* pVal);
	STDMETHOD(get_ServiceVersion)(BSTR* pVal);
	STDMETHOD(UpdateService)(void);
};

