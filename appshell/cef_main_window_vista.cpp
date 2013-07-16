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

cef_main_window_vista::cef_main_window_vista()
{

}
cef_main_window_vista::~cef_main_window_vista()
{

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
    EndPaint(&ps);
    return TRUE;
}


void cef_main_window_vista::GetCefBrowserRect(RECT& rect)
{
    GetClientRect(&rect);
    // TODO: Compute these values (make room for menu bar)
    rect.top += 40;  
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
    }

    LRESULT lr = cef_main_window::WindowProc(message, wParam, lParam);
    return lr;
}