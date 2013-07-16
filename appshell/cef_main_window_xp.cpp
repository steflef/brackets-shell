#pragma once
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

// undefined windows constants for drawing
#ifndef DCX_USESTYLE
#define DCX_USESTYLE 0x00010000
#endif

cef_main_window_xp::cef_main_window_xp()
{

}
cef_main_window_xp::~cef_main_window_xp()
{

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
    HBRUSH br = ::CreateSolidBrush(RGB(59, 62, 64));
    FillRect(hdc, &rectFrame, br);
    DeleteObject(br);
}

void cef_main_window_xp::DoDrawSystemIcon(HDC hdc)
{
    // TODO: cache this icon
    HICON hSystemIcon = reinterpret_cast<HICON>(GetClassLongPtr(GCLP_HICONSM));
    if (hSystemIcon == 0)
        hSystemIcon = reinterpret_cast<HICON>(GetClassLongPtr(GCLP_HICON));

    // TODO: cache this data
    NONCLIENTMETRICS ncm;
    ::ZeroMemory(&ncm, sizeof(ncm));
    ncm.cbSize = sizeof (ncm);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

    ::DrawIconEx(hdc, ncm.iBorderWidth, ncm.iBorderWidth, hSystemIcon, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0, NULL, DI_NORMAL);

}

void cef_main_window_xp::DoDrawTitlebarText(HDC hdc)
{
    // TODO: cache this data
    NONCLIENTMETRICS ncm;
    ::ZeroMemory(&ncm, sizeof(ncm));
    ncm.cbSize = sizeof (ncm);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

    // TODO: cache this font
    HFONT hCaptionFont= ::CreateFontIndirect(&ncm.lfCaptionFont);
    HGDIOBJ hPreviousFont = ::SelectObject(hdc, hCaptionFont);        

    int oldBkMode = ::SetBkMode(hdc, TRANSPARENT);
    COLORREF oldRGB = ::SetTextColor(hdc, RGB(197,197,197));

    RECT wr;
    GetWindowRect(&wr);

    RECT textRect;
    textRect.top = 0;
    textRect.bottom = textRect.top + GetSystemMetrics (SM_CYCAPTION) +  ncm.iBorderWidth;
    textRect.left = (ncm.iBorderWidth * 2) + GetSystemMetrics(SM_CXSMICON);
    textRect.right = ::RectWidth(wr) - ncm.iBorderWidth;

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
    DoDrawSystemIcon(hdc);
    DoDrawTitlebarText(hdc);
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


LRESULT cef_main_window_xp::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
    case WM_NCPAINT:
        if (HandleNcPaint((HRGN)wParam)) 
            return 0L;
        break;
    }

    LRESULT lr = cef_main_window::WindowProc(message, wParam, lParam);
    
    // post default message processing
    switch (message)
    {
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