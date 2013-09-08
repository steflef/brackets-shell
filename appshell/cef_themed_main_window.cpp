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
#include <stdlib.h>
#include "cef_themed_main_window.h"


cef_themed_main_window::cef_themed_main_window()
{

}
cef_themed_main_window::~cef_themed_main_window()
{

}

void cef_themed_main_window::DoDrawMenuBar(HDC hdc)
{
    RECT rectBar;

    MENUBARINFO mbi = {0};
    mbi.cbSize = sizeof(mbi);

    if (!GetMenuBarInfo(OBJID_MENU, 0, &mbi))
        return;

    int items = ::GetMenuItemCount(mbi.hMenu);

    ::CopyRect(&rectBar, &mbi.rcBar);
    ScreenToNonClient(&rectBar);

    
    int i;
    wchar_t szMenuString[256] = L"";

    HFONT hMenuFont = ::CreateFontIndirect(&mNcMetrics.lfMenuFont);
    HGDIOBJ oldFont = ::SelectObject(hdc, hMenuFont);            

    HBRUSH hbrHighlight = ::CreateSolidBrush(RGB(247, 247, 247));

    RECT rectText;
    ::CopyRect(&rectText, &rectBar);
    
    rectText.top += 1;

    for (i = 0; i < items; i++) {
        ::GetMenuString(mbi.hMenu, i, szMenuString, _countof(szMenuString), MF_BYPOSITION);

        COLORREF rgbMenuText = RGB(197, 197, 197);

        RECT rectTemp;
        SetRectEmpty(&rectTemp);

        // calc the size of this menu item so we can move the start over for the next guy
        ::DrawText(hdc, szMenuString, wcslen(szMenuString), &rectTemp, DT_LEFT|DT_SINGLELINE|DT_CALCRECT);

        MENUITEMINFO mmi = {0};
        mmi.cbSize = sizeof (mmi);
        mmi.fMask = MIIM_STATE;
        ::GetMenuItemInfo (mbi.hMenu, i, TRUE, &mmi);

        if (mmi.fState & MFS_HILITE) {
            RECT rectButton;
            ::CopyRect(&rectButton, &rectText);
            rectButton.right = rectButton.left + rectTemp.right + 12;

            ::FillRect(hdc, &rectButton, hbrHighlight);

            rgbMenuText = RGB(30, 30, 30);

        } else if (mmi.fState & MFS_GRAYED) {
            rgbMenuText = RGB(130, 130, 130);
        }

        rectText.left += 6;

        COLORREF oldRGB = ::SetTextColor(hdc, rgbMenuText);
        int oldBkMode   = ::SetBkMode(hdc, TRANSPARENT);
        ::DrawText(hdc, szMenuString, wcslen(szMenuString), &rectText, DT_LEFT|DT_SINGLELINE|DT_HIDEPREFIX);
        ::SetTextColor(hdc, oldRGB);
        ::SetBkMode(hdc, oldBkMode);
                
        rectText.left += (rectTemp.right + 8);
    }

    ::SelectObject(hdc, oldFont);            
    ::DeleteObject(hbrHighlight);
    ::DeleteObject(hMenuFont);
}

void cef_themed_main_window::ComputeMenuBarRect(RECT& rect)
{
    MENUBARINFO mbi = {0};
    mbi.cbSize = sizeof(mbi);
    
    GetMenuBarInfo(OBJID_MENU, 0, &mbi);

    ::CopyRect(&rect, &mbi.rcBar);
    ScreenToNonClient(&rect);
}


void cef_themed_main_window::UpdateMenuBar()
{
    HDC hdc = GetWindowDC();

	RECT rectWindow ;
	ComputeLogicalWindowRect ( rectWindow ) ;
    ::ExcludeClipRect (hdc, rectWindow.left, rectWindow.top, rectWindow.right, rectWindow.bottom);

    RECT rectMenuBar;
    ComputeMenuBarRect(rectMenuBar);
    HRGN hrgnUpdate = ::CreateRectRgnIndirect(&rectMenuBar);

    if (::SelectClipRgn(hdc, hrgnUpdate) != NULLREGION) {
        DoDrawFrame(hdc);           
        DoDrawMenuBar(hdc);
    }

    ::DeleteObject(hrgnUpdate);

    ReleaseDC(hdc);
}



void cef_themed_main_window::DoPaintNonClientArea(HDC hdc)
{
    cef_main_window_xp::DoPaintNonClientArea(hdc);
    DoDrawMenuBar(hdc);
}

int cef_themed_main_window::HandleNcHitTest(LPPOINT ptHit)
{
    int hit = cef_main_window_xp::HandleNcHitTest(ptHit);

    if (hit == HTMENU) {
        UpdateMenuBar();
    }

    return hit;
}


LRESULT cef_themed_main_window::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = cef_main_window_xp::WindowProc(message, wParam, lParam);

    switch (message) 
    {
    case WM_NCHITTEST:
        {
            POINT pt;
            POINTSTOPOINT(pt, lParam);
            return HandleNcHitTest(&pt);
        }      
    case WM_MENUSELECT:
        UpdateMenuBar();
        break;
    }

    return lr;
}
