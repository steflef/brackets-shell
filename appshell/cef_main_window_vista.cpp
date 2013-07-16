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
#include "cef_main_window_vista.h"

#include <dwmapi.h>
// TODO: Load this dynamically
#pragma comment(lib, "dwmapi")

#include <uxtheme.h>
#include <tmschema.h>
// TODO: Load this dynamically
#pragma comment(lib, "uxtheme")


cef_main_window_vista::cef_main_window_vista()
{

}
cef_main_window_vista::~cef_main_window_vista()
{

}

void cef_main_window_vista::DoDrawTitlebarText(HDC hdc)
{
    HTHEME hTheme = OpenThemeData(NULL, L"CompositedWindow::Window");
    if (hTheme)
    {
        LOGFONT lgFont;    
        HFONT hPreviousFont = NULL;
        if (SUCCEEDED(::GetThemeSysFont(hTheme, TMT_CAPTIONFONT, &lgFont)))
        {
            HFONT hFont = ::CreateFontIndirect(&lgFont);
            hPreviousFont = (HFONT) SelectObject(hdc, hFont);

            int oldBkMode = ::SetBkMode(hdc, TRANSPARENT);
            COLORREF oldRGB = ::SetTextColor(hdc, RGB(197,197,197));

            RECT wr;
            GetClientRect(&wr);

            RECT textRect;
            textRect.top = 0;
            textRect.bottom = textRect.top + GetSystemMetrics (SM_CYCAPTION) +  10;
            textRect.left = (10 * 2) + GetSystemMetrics(SM_CXSMICON);
            textRect.right = ::RectWidth(wr) - 10;

            int textLength = GetWindowTextLength() + 1;
            LPWSTR szCaption = new wchar_t [textLength + 1];
            ::ZeroMemory(szCaption, textLength + 1);
            int cchCaption = GetWindowText(szCaption, textLength);

            DrawText(hdc, szCaption, cchCaption, &textRect, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS|DT_NOPREFIX);

            delete []szCaption;
            ::SetTextColor(hdc, oldRGB);
            ::SetBkMode(hdc, oldBkMode);
            ::SelectObject(hdc, hPreviousFont);
        }        
        CloseThemeData(hTheme);
    }
}

void cef_main_window_vista::DoDrawSystemIcon(HDC hdc)
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

BOOL cef_main_window_vista::HandlePaint() 
{
    RECT rcClient;

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(&ps);

    GetClientRect(&rcClient);

    HBRUSH br = ::CreateSolidBrush(RGB(59, 62, 64));
    FillRect(hdc, &rcClient, br);
    DeleteObject(br);

    DoDrawSystemIcon(hdc);
    DoDrawTitlebarText(hdc);

    EndPaint(&ps);
    return TRUE;
}


void cef_main_window_vista::GetCefBrowserRect(RECT& rect)
{
    GetClientRect(&rect);
    // TODO: Compute these values (make room for menu bar)
    rect.top += 50;  
    rect.left += 10;
    rect.right -= 10;
    rect.bottom -= 10;
}


BOOL cef_main_window_vista::HandleCreate() 
{
    RECT rcClient;
    GetWindowRect(&rcClient);

    // Inform the application of the frame change.
    SetWindowPos(NULL, 
                 rcClient.left, rcClient.top,
                 ::RectWidth(rcClient), 
                 ::RectHeight(rcClient),
                 SWP_FRAMECHANGED);    

    return cef_main_window::HandleCreate();
}

BOOL cef_main_window_vista::HandleActivate()
{
    MARGINS margins;
    ::ZeroMemory(&margins, sizeof(margins));

    if (SUCCEEDED(DwmExtendFrameIntoClientArea(mWnd, &margins)))
    {
        // TODO: set an internal drawing flag
    }

    return TRUE;
}

void cef_main_window_vista::UpdateCaptionBar()
{
    RECT wr;
    GetClientRect(&wr);

    RECT textRect;
    textRect.top = 0;
    textRect.bottom = textRect.top + GetSystemMetrics (SM_CYCAPTION) +  10;
    textRect.left = (10 * 2) + GetSystemMetrics(SM_CXSMICON);
    textRect.right = ::RectWidth(wr) - 10;

    InvalidateRect(&textRect);
}

LRESULT cef_main_window_vista::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_CREATE:
		if (HandleCreate())
			return 0L;
		break;
    case WM_NCCALCSIZE:
        if (wParam == TRUE)
            return 0L;
        break;
    case WM_ACTIVATE:
        if (HandleActivate())
            return 0L;
        break;
    case WM_PAINT:
        if (HandlePaint()) 
            return 0L;
        break;
    case WM_SETTEXT:
        UpdateCaptionBar();
        break;
    }

    LRESULT lr = cef_main_window::WindowProc(message, wParam, lParam);
    return lr;
}