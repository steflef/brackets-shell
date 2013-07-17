/*************************************************************************
 *
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 *  Copyright 2013 Adobe Systems Incorporated
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Adobe Systems Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Adobe Systems Incorporated and its
 * suppliers and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Adobe Systems Incorporated.
 **************************************************************************/
#include "cef_window.h"

extern HINSTANCE gInstance;

static wchar_t gCefClientWindowPropName[] = L"CefClientWindowPtr";

struct HookData {
    HookData() 
    {
        this->Reset();
    }
    void Reset()
    {
        mOldHook = NULL;
        mWindow = NULL;        
    }

    HHOOK       mOldHook;
    cef_window* mWindow;
} gHookData;




cef_window::cef_window(void) :
    mWnd(NULL),
    mSuperWndProc(NULL)
{
}


cef_window::~cef_window(void)
{
}

static LRESULT CALLBACK _HookProc(int code, WPARAM wParam, LPARAM lParam)
{
    if (code != HCBT_CREATEWND)
        return CallNextHookEx(gHookData.mOldHook, code, wParam, lParam);
    
    LPCREATESTRUCT lpcs = ((LPCBT_CREATEWND)lParam)->lpcs;

    HHOOK nextHook = gHookData.mOldHook;

    if (lpcs->lpCreateParams && lpcs->lpCreateParams == (LPVOID)gHookData.mWindow) 
    {
        HWND hWnd = (HWND)wParam;
        gHookData.mWindow->SubclassWindow(hWnd);
        // Rest the hook data here since we've already hooked this window
        //  this allows for other windows to be created in the WM_CREATE handlers
        //  of subclassed windows
        gHookData.Reset();
    }

    return CallNextHookEx(nextHook, code, wParam, lParam);
}

static void _HookWindowCreate(cef_window* window)
{
    // can only hook one creation at a time
    if (gHookData.mOldHook || gHookData.mWindow) 
        return;

    gHookData.mOldHook = ::SetWindowsHookEx(WH_CBT, _HookProc, NULL, ::GetCurrentThreadId());
    gHookData.mWindow = window;
}


static void _UnHookWindowCreate()
{
    gHookData.Reset();
}

static LRESULT CALLBACK _WindowProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  cef_window* window = (cef_window*)::GetProp(hWnd, ::gCefClientWindowPropName);
  if (window) 
  {
      return window->WindowProc(message, wParam, lParam);
  } 
  else 
  {
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
}

bool cef_window::SubclassWindow(HWND hWnd) 
{
    if (::GetProp(hWnd, ::gCefClientWindowPropName) != NULL) 
        return false;
    mWnd = hWnd;
    mSuperWndProc = (WNDPROC)SetWindowLongPtr(GWLP_WNDPROC, (LONG_PTR)&_WindowProc);
    SetProp(::gCefClientWindowPropName, (HANDLE)this);
    return true;
}

static HWND _xxCreateWindow(LPCTSTR szClassname, LPCTSTR szWindowTitle, DWORD dwStyles, int x, int y, int width, int height, HWND hWndParent, HMENU hMenu, cef_window* window)
{
    ::_HookWindowCreate(window);

    HWND result = CreateWindow(szClassname, szWindowTitle, 
                               dwStyles, x, y, width, height, hWndParent, hMenu, gInstance, (LPVOID)window);


    ::_UnHookWindowCreate();
    return result;
}

BOOL cef_window::Create(LPCTSTR szClassname, LPCTSTR szWindowTitle, DWORD dwStyles, int x, int y, int width, int height, cef_window* parent/*=NULL*/, cef_menu* menu/*=NULL*/)
{
    HWND hWndParent = parent ? parent->mWnd : NULL;
    HMENU hMenu = /*menu ? menu->m_hMenu :*/ NULL;

    HWND hWndThis = _xxCreateWindow(szClassname, szWindowTitle, 
                               dwStyles, x, y, width, height, hWndParent, hMenu, this);


    return hWndThis && hWndThis == mWnd;
}

LRESULT cef_window::DefaultWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (mSuperWndProc)
    {
        return ::CallWindowProc(mSuperWndProc, mWnd, message, wParam, lParam);
    } 
    else 
    {
        return ::DefWindowProc(mWnd, message, wParam, lParam);
    }

}

BOOL cef_window::HandleNonClientDestroy()
{
    WNDPROC superWndProc = WNDPROC(GetWindowLongPtr(GWLP_WNDPROC));

    RemoveProp(::gCefClientWindowPropName);

    DefaultWindowProc(WM_NCDESTROY, 0, 0);
	
    if ((WNDPROC(GetWindowLongPtr(GWLP_WNDPROC)) == superWndProc) && (mSuperWndProc != NULL))
	    SetWindowLongPtr(GWLP_WNDPROC, reinterpret_cast<INT_PTR>(mSuperWndProc));
	
    mSuperWndProc = NULL;
    return TRUE;
}

void cef_window::PostNonClientDestory()
{
    mWnd = NULL;
}

LRESULT cef_window::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
	    case WM_NCDESTROY:
		    if (HandleNonClientDestroy())
			    return 0L;
		    break;
    }
	
    LRESULT lr = DefaultWindowProc(message, wParam, lParam);
	
    if (message == WM_NCDESTROY) 
    {
	    PostNonClientDestory();
    }

    return lr;
}

void cef_window::ScreenToNonClient(RECT& rect) 
{
    WINDOWINFO wi ;
    ::ZeroMemory ( &wi, sizeof ( wi ) ) ;
    wi.cbSize = sizeof ( wi ) ;
    GetWindowInfo ( &wi ) ;

    int height = ::RectHeight(rect);
    int width = ::RectWidth(rect);

    rect.top = rect.top - wi.rcWindow.top;
    rect.left = rect.left - wi.rcWindow.left;
    rect.bottom = rect.top + height;
    rect.right = rect.left + width;
}

// Computes the client rect relative to the Window Rect
//	Used to compute the clipping region, etc...
void cef_window::ComputeLogicalClientRect(RECT& rectClient)
{
    WINDOWINFO wi ;
    ::ZeroMemory ( &wi, sizeof ( wi ) ) ;
    wi.cbSize = sizeof ( wi ) ;
    GetWindowInfo ( &wi ) ;

    ::CopyRect(&rectClient, &wi.rcClient);

    int height = ::RectHeight(wi.rcClient);
    int width = ::RectWidth(wi.rcClient);

    rectClient.top = wi.rcClient.top - wi.rcWindow.top;
    rectClient.left = wi.rcClient.left - wi.rcWindow.left;
    rectClient.bottom = rectClient.top + height;
    rectClient.right = rectClient.left + width;
}
