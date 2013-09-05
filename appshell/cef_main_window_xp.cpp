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
#include "cef_main_window_xp.h"
#include "resource.h"
#include <minmax.h>
#include <objidl.h>
#include <GdiPlus.h>
#include <Uxtheme.h>
#include <Shlwapi.h>

#pragma comment(lib, "gdiplus")

extern HINSTANCE gInstance;

// undefined windows constants for drawing
#ifndef DCX_USESTYLE
#define DCX_USESTYLE 0x00010000
#endif

static ULONG_PTR gdiplusToken = NULL;

static void RECT2Rect(Gdiplus::Rect& dest, const RECT& src) {
    dest.X = src.left;
    dest.Y = src.top;   
    dest.Width = ::RectWidth(src);
    dest.Height = ::RectHeight(src);
}

namespace ResouceImage
{
    Gdiplus::Image* FromResource(LPCWSTR lpResourceName)
    {
        HRSRC hResource = ::FindResource(gInstance, lpResourceName, L"PNG");
        if (!hResource) 
            return NULL;

        HGLOBAL hData = ::LoadResource(gInstance, hResource);
        if (!hData)
            return NULL;

        void* data = ::LockResource(hData);
        ULONG size = ::SizeofResource(gInstance, hResource);

        IStream* pStream = ::SHCreateMemStream((BYTE*)data, (UINT)size);

        if (!pStream) 
            return NULL;

        Gdiplus::Image* image = Gdiplus::Image::FromStream(pStream);
        pStream->Release();
        return image;
    }

}

cef_main_window_xp::cef_main_window_xp() :
    mSysCloseButton(0),
    mSysRestoreButton(0),
    mSysMinimizeButton(0),
    mSysMaximizeButton(0),
    mWindowIcon(0)
{
    ::ZeroMemory(&mNcMetrics, sizeof(mNcMetrics));
}

cef_main_window_xp::~cef_main_window_xp()
{

}

void cef_main_window_xp::LoadSysButtonImages()
{
    mNcMetrics.cbSize = sizeof (mNcMetrics);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &mNcMetrics, 0);


    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    if (mSysCloseButton == NULL) {
        mSysCloseButton = ResouceImage::FromResource(MAKEINTRESOURCE(IDB_CLOSE_BUTTON));
    }

    if (mSysMaximizeButton == NULL) {
        mSysMaximizeButton = ResouceImage::FromResource(MAKEINTRESOURCE(IDB_MAX_BUTTON));
    }

    if (mSysMinimizeButton == NULL) {
        mSysMinimizeButton = ResouceImage::FromResource(MAKEINTRESOURCE(IDB_MIN_BUTTON));
    }

    if (mSysRestoreButton == NULL) {
        mSysRestoreButton = ResouceImage::FromResource(MAKEINTRESOURCE(IDB_RESTORE_BUTTON));
    }

    ::SetWindowTheme(mWnd, L"", L"");
}

BOOL cef_main_window_xp::HandleNcCreate()
{
    LoadSysButtonImages();
    return FALSE;
}

// TODO This can go if we don't need it
BOOL cef_main_window_xp::HandleCreate()
{
    return cef_main_window::HandleCreate();
}


BOOL cef_main_window_xp::HandleNcDestroy()
{
    BOOL result = cef_main_window::HandleNcDestroy();

    delete mSysCloseButton;
    delete mSysRestoreButton;
    delete mSysMinimizeButton;
    delete mSysMaximizeButton;

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return result;
}

void cef_main_window_xp::ComputeWindowIconRect(RECT& rect)
{
    int top = mNcMetrics.iBorderWidth;
    int left = mNcMetrics.iBorderWidth;


    if (IsZoomed()) {
        top = 12;
        left = 12;
    }

    ::SetRectEmpty(&rect);
    rect.top =  top;
    rect.left = left;
    rect.bottom = rect.top + ::GetSystemMetrics(SM_CYSMICON);
    rect.right = rect.left + ::GetSystemMetrics(SM_CXSMICON);
}

void cef_main_window_xp::ComputeWindowCaptionRect(RECT& rect)
{
    RECT wr;
    GetWindowRect(&wr);

    int top = mNcMetrics.iBorderWidth;
    int left = mNcMetrics.iBorderWidth;

    if (IsZoomed()) {
        top = 8;
        left = 8;
    }

    rect.top = top;
    rect.bottom = rect.top + ::GetSystemMetrics (SM_CYCAPTION);

    RECT ir;
    ComputeWindowIconRect(ir);

    RECT mr;
    ComputeMinimizeButtonRect(mr);

    rect.left = ir.right + 1;
    rect.right = mr.left - 1;
}

void cef_main_window_xp::ComputeMinimizeButtonRect(RECT& rect)
{
    ComputeMaximizeButtonRect(rect);

    rect.left -= (mSysMinimizeButton->GetWidth() + 1);
    rect.right = rect.left + mSysMinimizeButton->GetWidth();
    rect.bottom = rect.top + mSysMinimizeButton->GetHeight();
}

void cef_main_window_xp::ComputeMaximizeButtonRect(RECT& rect)
{
    ComputeCloseButtonRect(rect);

    rect.left -= (mSysMaximizeButton->GetWidth() + 1);
    rect.right = rect.left + mSysMaximizeButton->GetWidth();
    rect.bottom = rect.top + mSysMaximizeButton->GetHeight();
}

void cef_main_window_xp::ComputeCloseButtonRect(RECT& rect)
{
    int top = 1;
    int right =  mNcMetrics.iBorderWidth;


    if (IsZoomed()) {
        top =  8;
        right = 8;
    }

    RECT wr;
    GetWindowRect(&wr);

    rect.left = ::RectWidth(wr) - right - mSysCloseButton->GetWidth();
    rect.top = top;
    rect.right = rect.left + mSysCloseButton->GetWidth();
    rect.bottom = rect.top + mSysCloseButton->GetHeight();
}

void cef_main_window_xp::DoDrawFrame(HDC hdc)
{
    RECT rectWindow;
    GetWindowRect(&rectWindow);

    RECT rectFrame;

    ::SetRectEmpty(&rectFrame);

    rectFrame.bottom = ::RectHeight(rectWindow);
    rectFrame.right = ::RectWidth(rectWindow);

    // TODO: cache this brush and use const color
                            
    HBRUSH br = ::CreateSolidBrush(RGB(60, 63, 65));
    FillRect(hdc, &rectFrame, br);
    DeleteObject(br);
}

void cef_main_window_xp::DoDrawSystemMenuIcon(HDC hdc)
{
    if (mWindowIcon == 0) {
        mWindowIcon = reinterpret_cast<HICON>(GetClassLongPtr(GCLP_HICONSM));
        if (mWindowIcon == 0)
            mWindowIcon = reinterpret_cast<HICON>(GetClassLongPtr(GCLP_HICON));
    }

    RECT rectIcon;
    ComputeWindowIconRect(rectIcon);

    ::DrawIconEx(hdc, rectIcon.top, rectIcon.left, mWindowIcon, ::RectWidth(rectIcon), ::RectHeight(rectIcon), 0, NULL, DI_NORMAL);
}

void cef_main_window_xp::DoDrawTitlebarText(HDC hdc)
{
    // TODO: cache this font
    HFONT hCaptionFont= ::CreateFontIndirect(&mNcMetrics.lfCaptionFont);
    HGDIOBJ hPreviousFont = ::SelectObject(hdc, hCaptionFont);        

    int oldBkMode = ::SetBkMode(hdc, TRANSPARENT);
    COLORREF oldRGB = ::SetTextColor(hdc, RGB(197,197,197));

    RECT textRect;
    ComputeWindowCaptionRect(textRect);

    int textLength = GetWindowTextLength() + 1;
    LPWSTR szCaption = new wchar_t [textLength + 1];
    ::ZeroMemory(szCaption, textLength + 1);
    int cchCaption = GetWindowText(szCaption, textLength);

    DrawText(hdc, szCaption, cchCaption, &textRect, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS|DT_NOPREFIX);

    delete []szCaption;
    ::SetTextColor(hdc, oldRGB);
    ::SetBkMode(hdc, oldBkMode);
    ::SelectObject(hdc, hPreviousFont);

    // TODO: Once we start caching the font we will need to move this to DestroyWindow()
    ::DeleteObject(hCaptionFont);
}

void cef_main_window_xp::DoDrawSystemIcons(HDC hdc)
{
    RECT rcButton;
    Gdiplus::Rect rect;
    Gdiplus::Graphics grpx(hdc);
   
    ComputeCloseButtonRect(rcButton);
    ::RECT2Rect(rect, rcButton);

    grpx.DrawImage(mSysCloseButton, rect);

    ComputeMaximizeButtonRect(rcButton);
    ::RECT2Rect(rect, rcButton);

    if (IsZoomed()) {
        grpx.DrawImage(mSysRestoreButton, rect);
    } else {
        grpx.DrawImage(mSysMaximizeButton, rect);
    }

    ComputeMinimizeButtonRect(rcButton);
    ::RECT2Rect(rect, rcButton);

    grpx.DrawImage(mSysMinimizeButton, rect);
}


void cef_main_window_xp::InitDeviceContext(HDC hdc)
{
    RECT rectClipClient;
    SetRectEmpty(&rectClipClient);
    ComputeLogicalClientRect(rectClipClient);

    // exclude the client area to reduce flicker
    ::ExcludeClipRect(hdc, rectClipClient.left, rectClipClient.top, rectClipClient.right, rectClipClient.bottom);
}

void cef_main_window_xp::DoPaintNonClientArea(HDC hdc)
{
    // TODO: buffer this drawing to reduce flicker
    InitDeviceContext(hdc);
    DoDrawFrame(hdc);
    DoDrawSystemMenuIcon(hdc);
    DoDrawTitlebarText(hdc);
    DoDrawSystemIcons(hdc);
}

void cef_main_window_xp::UpdateNonClientArea()
{
    HDC hdc = GetWindowDC();
    DoPaintNonClientArea(hdc);
    ReleaseDC(hdc);
}

BOOL cef_main_window_xp::HandleNcPaint(HRGN hUpdateRegion)
{
    HDC hdc = GetDCEx(hUpdateRegion, DCX_WINDOW|DCX_INTERSECTRGN|DCX_USESTYLE);
    DoPaintNonClientArea(hdc);
    ReleaseDC(hdc);

    return TRUE;
}

BOOL cef_main_window_xp::HandleSysCommand(UINT uType)
{
    return TRUE;
}


LRESULT cef_main_window_xp::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
    case WM_NCCREATE:
        if (HandleNcCreate())
            return 0L;
        break;
    case WM_CREATE:
        if (HandleCreate())
            return 0L;
        break;
    case WM_NCPAINT:
        if (HandleNcPaint((HRGN)wParam)) 
            return 0L;
        break;
    case WM_NCDESTROY:
        if (HandleNcDestroy())
            return 0L;
        break;
    }

    LRESULT lr = cef_main_window::WindowProc(message, wParam, lParam);
    
    // post default message processing
    switch (message)
    {
    case WM_SYSCOMMAND:
        HandleSysCommand((UINT)(wParam & 0xFFF0));
        break;
    case WM_WINDOWPOSCHANGING:
    case WM_WINDOWPOSCHANGED:
    case WM_MOVE:
    case WM_SIZE:
    case WM_SIZING:
    case WM_EXITSIZEMOVE:
    case WM_NCACTIVATE:
    case WM_ACTIVATE:
    case WM_SETTEXT:
        UpdateNonClientArea();
        break;

    }

    return lr;
}