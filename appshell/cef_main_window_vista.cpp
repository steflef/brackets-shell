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
#include "cef_main_window_vista.h"

#include <dwmapi.h>
// TODO: Load this dynamically
#pragma comment(lib, "dwmapi")

#include <uxtheme.h>
#include <tmschema.h>
#include <vsstyle.h>
// TODO: Load this dynamically
#pragma comment(lib, "uxtheme")

#include <CommonControls.h>

/* 
 * Starting with Windows Vista, custom frame drawing has to be handled
 * by extending the client area into the non-client area.  This is managed
 * by the Desktop Window Manager so we can't just handle WM_NCPAINT messages
 * to draw the frame directly.  This is documented in this article
 * http://msdn.microsoft.com/en-us/library/windows/desktop/bb688195(v=vs.85).aspx
 */

cef_main_window_vista::cef_main_window_vista()
{
    ::InitCommonControls();
}

cef_main_window_vista::~cef_main_window_vista()
{

}

// TODO: Pass state in
void cef_main_window_vista::DoDrawWindowCloseButton(HDC hdc)
{
    HTHEME hTheme = ::OpenThemeData(mWnd, L"CompositedWindow::Window");
    if (hTheme)
    {
        // TODO: Move this to a ComputeCloseButtonRect
        RECT rectClient;
        GetClientRect(&rectClient);
        
        RECT closeButtonRect;
        closeButtonRect.top = 0;
        closeButtonRect.bottom = closeButtonRect.top + 20;
        closeButtonRect.right = ::RectWidth(rectClient) - 10;
        closeButtonRect.left = closeButtonRect.right - 20;


        
        DTBGOPTS options = {0};
        options.dwSize = sizeof(options);
        options.dwFlags = DTBG_OMITBORDER;

        ::DrawThemeBackgroundEx(hTheme, hdc, WP_CLOSEBUTTON, CBS_NORMAL, &closeButtonRect, &options); 
        //::DrawThemeBackground(hTheme, hdc, WP_MAXBUTTON, MAXBS_NORMAL, &closeButtonRect, NULL);
/*        HBITMAP hCloseButtonBitmap = NULL;
        ::GetThemeBitmap(hTheme, WP_CLOSEBUTTON, CBS_NORMAL, TMT_DIBDATA, GBF_VALIDBITS, &hCloseButtonBitmap);
   //     RECT closeButtonBitmapRect;
  //      ::GetThemeBackgroundContentRect(hTheme, hdc, WP_CLOSEBUTTON, CBS_NORMAL, &closeButtonRect, &closeButtonBitmapRect);
        HIMAGELIST himl = ::ImageList_Create(::RectWidth(closeButtonBitmapRect),
                                             ::RectHeight(closeButtonBitmapRect),
                                             ILC_COLOR|ILC_COLORDDB,
                                             1,
                                             0);


        ::ImageList_Add(himl, hCloseButtonBitmap, NULL);
        ::DrawThemeIcon(hTheme, hdc, WP_CLOSEBUTTON, CBS_NORMAL, &closeButtonRect, himl, 0);

        HDC bmDC = ::CreateCompatibleDC(hdc);
        HBITMAP bmOld = (HBITMAP)::SelectObject(bmDC, hCloseButtonBitmap);
        BITMAP bm;
        ::GetObject(hCloseButtonBitmap, sizeof(bm), &bm);
        BitBlt(hdc, closeButtonRect.left, closeButtonRect.top, bm.bmWidth, bm.bmHeight, bmDC, 0, 0, SRCCOPY);
        ::SelectObject(bmDC, bmOld);
        ::DeleteDC(bmDC); */
        //::CloseThemeData(hTheme);
    }        
    
}


void cef_main_window_vista::DoDrawTitlebarText(HDC hdc)
{
    HTHEME hTheme = ::OpenThemeData(NULL, L"CompositedWindow::Window");
    if (hTheme)
    {
        LOGFONT lgFont;    
        HFONT hPreviousFont = NULL;
        if (SUCCEEDED(::GetThemeSysFont(hTheme, TMT_CAPTIONFONT, &lgFont)))
        {
            // TODO: Cache this font
            HFONT hFont = ::CreateFontIndirect(&lgFont);
            hPreviousFont = (HFONT) SelectObject(hdc, hFont);

            int oldBkMode = ::SetBkMode(hdc, TRANSPARENT);
            COLORREF oldRGB = ::SetTextColor(hdc, RGB(197,197,197));

            // TODO: Move this to a ComputeTextRect
            RECT rectClient;
            GetClientRect(&rectClient);

            RECT textRect;
            textRect.top = 0;
            textRect.bottom = textRect.top + GetSystemMetrics (SM_CYCAPTION) +  10;
            textRect.left  = GetSystemMetrics(SM_CXSMICON) + 20; 
            textRect.right  = ::RectWidth(rectClient) - 10;

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
        ::CloseThemeData(hTheme);
    }
}


// TODO: move to base class and add ComputeSystemMenuIconRect
void cef_main_window_vista::DoDrawSystemMenuIcon(HDC hdc)
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
    // TODO: Buffer this paint
    RECT rcClient;

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(&ps);

    GetClientRect(&rcClient);

    HBRUSH br = ::CreateSolidBrush(RGB(59, 62, 64));
    FillRect(hdc, &rcClient, br);
    DeleteObject(br);

    DoDrawSystemMenuIcon(hdc);
    DoDrawTitlebarText(hdc);
    DoDrawWindowCloseButton(hdc);

    EndPaint(&ps);
    return TRUE;
}

// TODO: Rename this to ComputeCefBrowserRect
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
    RECT rectClient;
    GetWindowRect(&rectClient);

    // Inform the application of the frame change.
    SetWindowPos(NULL, 
                 rectClient.left, 
                 rectClient.top,
                 ::RectWidth(rectClient), 
                 ::RectHeight(rectClient),
                 SWP_FRAMECHANGED);    

    return cef_main_window::HandleCreate();
}

BOOL cef_main_window_vista::HandleActivate()
{
    MARGINS margins = {0}; 

    if (SUCCEEDED(DwmExtendFrameIntoClientArea(mWnd, &margins)))
    {
        // TODO: set an internal drawing flag
    }

    return TRUE;
}

void cef_main_window_vista::UpdateCaptionBar()
{
    // TODO: ComputeTextRect
    RECT rectClient;
    GetClientRect(&rectClient);

    RECT textRect;
    textRect.top = 0;
    textRect.bottom = textRect.top + GetSystemMetrics (SM_CYCAPTION) +  10;
    textRect.left = (10 * 2) + GetSystemMetrics(SM_CXSMICON);
    textRect.right = ::RectWidth(rectClient) - 10;

    InvalidateRect(&textRect);
}

int cef_main_window_vista::HandleNonClientHitTest(LPPOINT ptHit)
{
    RECT rectClient;
    GetClientRect(&rectClient);

    if (::PtInRect(&rectClient, *ptHit)) {
        return HTCLIENT;
    }

    RECT textRect;
    textRect.top = 0;
    textRect.bottom = textRect.top + GetSystemMetrics (SM_CYCAPTION) +  10;
    textRect.left  = GetSystemMetrics(SM_CXSMICON) + 20; 
    textRect.right  = ::RectWidth(rectClient) - 10;


    if (::PtInRect(&textRect, *ptHit)) {
        return HTCAPTION;
    }
    else {
        return HTBORDER;
    }
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
    case WM_NCHITTEST:
        {
            POINT pt;
            POINTSTOPOINT(pt, lParam);
            return HandleNonClientHitTest(&pt);
        }
    }

    LRESULT lr = cef_main_window::WindowProc(message, wParam, lParam);
    return lr;
}