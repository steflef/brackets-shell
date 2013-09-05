#pragma once
/*
 * Copyright (c) 2013 Adobe Systems Incorporated. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <windows.h>
#include <winuser.h>
#include <Shellapi.h>

class cef_window;
class cef_menu;

// Helpers
static __inline int RectWidth(const RECT &r) { return r.right - r.left; }
static __inline int RectHeight(const RECT &r) { return r.bottom - r.top; }

class cef_window
{
public:
    cef_window(void);
    virtual ~cef_window(void);

    BOOL Create(LPCTSTR szClass, LPCTSTR szWindowTitle, DWORD dwStyles, int x, int y, int width, int height, cef_window* parent = NULL, cef_menu* menu = NULL);
    bool SubclassWindow (HWND hWnd);

    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT DefaultWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    HWND GetSafeWnd() 
    { return (this != NULL) ? mWnd : NULL; }

    HMENU GetMenu() 
    { return ::GetMenu(mWnd); }

    BOOL UpdateWindow()
    { return ::UpdateWindow(mWnd); }

    BOOL GetWindowPlacement(LPWINDOWPLACEMENT wp)
    { return ::GetWindowPlacement(mWnd, wp); }

    BOOL SetWindowPlacement(LPWINDOWPLACEMENT wp)
    { return ::SetWindowPlacement(mWnd, wp); }

    BOOL GetWindowRect(LPRECT r)
    { return ::GetWindowRect(mWnd, r); }

    BOOL GetClientRect(LPRECT r)
    { return ::GetClientRect(mWnd, r); }

    HDC BeginPaint(PAINTSTRUCT* ps)
    { return ::BeginPaint(mWnd, ps); }

    BOOL EndPaint(PAINTSTRUCT* ps) 
    { return ::EndPaint(mWnd, ps); }

    BOOL DestroyWindow()
    { return ::DestroyWindow(mWnd); }

    BOOL SetProp(LPCWSTR lpString, HANDLE hData)
    { return ::SetProp(mWnd, lpString, hData); }

    HANDLE GetProp(LPCWSTR lpString)
    { return ::GetProp(mWnd, lpString); }

    HANDLE RemoveProp(LPCWSTR lpString)
    { return ::RemoveProp(mWnd, lpString); }

    LONG GetWindowLongPtr(int nIndex) 
    { return ::GetWindowLongPtr(mWnd, nIndex); }

    LONG SetWindowLongPtr(int nIndex, LONG dwNewLong) 
    { return ::SetWindowLongPtr(mWnd, nIndex, dwNewLong); }

    LONG GetClassLongPtr(int nIndex)
    { return ::GetClassLongPtr(mWnd, nIndex); }

    BOOL GetWindowInfo (PWINDOWINFO pwi) 
    { return ::GetWindowInfo (mWnd, pwi); }

    void DragAcceptFiles(BOOL fAccept)
    { return ::DragAcceptFiles(mWnd, fAccept); }

    BOOL ShowWindow(int nCmdShow)
    { return ::ShowWindow(mWnd, nCmdShow); }

    HDC GetDCEx(HRGN hrgnClip, DWORD dwFlags)
    { return ::GetDCEx(mWnd, hrgnClip, dwFlags); }

    HDC GetWindowDC()
    { return ::GetWindowDC(mWnd); }

    HDC GetDC()
    { return ::GetDC(mWnd); }

    int ReleaseDC(HDC dc)
    { return ::ReleaseDC(mWnd, dc); }

    BOOL SetWindowPos(cef_window* insertAfter, int x, int y, int cx, int cy, UINT uFlags) 
    { return ::SetWindowPos(mWnd, insertAfter->GetSafeWnd(), x, y, cx, cy, uFlags); }

    int GetWindowText(LPWSTR lpString, int nMaxCount) 
    { return ::GetWindowTextW(mWnd, lpString, nMaxCount); }

    int GetWindowTextLength() 
    { return ::GetWindowTextLengthW(mWnd); }

    BOOL InvalidateRect(LPRECT lpRect, BOOL bErase = FALSE)
    { return ::InvalidateRect(mWnd, lpRect, bErase); }

    BOOL IsZoomed() 
    { return ::IsZoomed(mWnd); }

    BOOL IsIconic() 
    { return ::IsIconic(mWnd); }

    void SetStyle(DWORD dwStyle) 
    { SetWindowLong(GWL_STYLE, dwStyle); }

    DWORD GetStyle() 
    { return GetWindowLong(GWL_STYLE); }

    void RemoveStyle(DWORD dwStyle) 
    { SetStyle(GetStyle() & ~dwStyle); }
    
    void AddStyle(DWORD dwStyle) 
    { SetStyle(GetStyle() & dwStyle); }

    void SetStyleEx(DWORD dwExStyle) 
    { SetWindowLong(GWL_EXSTYLE, dwExStyle); }

    DWORD GetStyleEx() 
    { return GetWindowLong(GWL_EXSTYLE); }

    void RemoveStyleEx(DWORD dwExStyle)
    { SetStyleEx(GetStyleEx() & ~dwExStyle); }

    void AddStyleEx(DWORD dwExStyle) 
    { SetStyleEx(GetStyleEx() & dwExStyle); }

protected:
    HWND mWnd;
    WNDPROC mSuperWndProc;

    BOOL HandleNcDestroy();
    virtual void PostNcDestory();

    void ComputeLogicalClientRect(RECT& rectClient);
    void ScreenToNonClient(RECT& rect);
};

