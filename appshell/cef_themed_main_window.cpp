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
    int oldBkMode   = ::SetBkMode(hdc, TRANSPARENT);
    COLORREF oldRGB = ::SetTextColor(hdc, RGB(197,197,197));

    RECT rectText;
    ::CopyRect(&rectText, &rectBar);
    
    rectText.top += 1;
    rectText.left += 6;

    for (i = 0; i < items; i++) {
        ::GetMenuString(mbi.hMenu, i, szMenuString, _countof(szMenuString), MF_BYPOSITION);

        RECT rectTemp;
        SetRectEmpty(&rectTemp);

        // calc the size of this menu item so we can move the start over for the next guy
        DrawText(hdc, szMenuString, wcslen(szMenuString), &rectTemp, DT_LEFT|DT_SINGLELINE|DT_CALCRECT);
        DrawText(hdc, szMenuString, wcslen(szMenuString), &rectText, DT_LEFT|DT_SINGLELINE|DT_HIDEPREFIX);
                
        rectText.left += (rectTemp.right + 14);
    }

    ::SelectObject(hdc, oldFont);            
    ::SetBkMode(hdc, oldBkMode);
    ::SetTextColor(hdc, oldRGB);

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


void cef_themed_main_window::DoPaintNonClientArea(HDC hdc)
{
    cef_main_window_xp::DoPaintNonClientArea(hdc);
    DoDrawMenuBar(hdc);
}

LRESULT cef_themed_main_window::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    return cef_main_window_xp::WindowProc(message, wParam, lParam);
}
