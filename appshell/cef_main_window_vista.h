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
#include "cef_main_window.h"

class cef_main_window_vista : public cef_main_window
{
public:
    cef_main_window_vista();
    virtual ~cef_main_window_vista();

    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
    BOOL HandleCreate();
    BOOL HandlePaint();
    BOOL HandleActivate();

    virtual void GetCefBrowserRect(RECT& rect);
};