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
#include "cef_main_window.h"

namespace Gdiplus
{
    class Image;
};

class cef_main_window_xp : public cef_main_window
{
public:
    cef_main_window_xp();
    virtual ~cef_main_window_xp();

    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
    BOOL HandleNcCreate();
    BOOL HandleCreate();
    BOOL HandleNcDestroy();
    BOOL HandleNcPaint(HRGN hUpdateRegion);

private:
    void UpdateNonClientArea();
    void InitDeviceContext(HDC hdc);
    void DoPaintNonClientArea(HDC hdc);
    void DoDrawFrame(HDC hdc);
    void DoDrawSystemMenuIcon(HDC hdc);
    void DoDrawTitlebarText(HDC hdc);
    void DoDrawSystemIcons(HDC hdc);

    void ComputeWindowIconRect(RECT& rect);
    void ComputeWindowCaptionRect(RECT& rect);
    void ComputeMinimizeButtonRect(RECT& rect);
    void ComputeMaximizeButtonRect(RECT& rect);
    void ComputeCloseButtonRect(RECT& rect);

    void LoadSysButtonImages();

    Gdiplus::Image* mSysCloseButton;
    Gdiplus::Image* mSysRestoreButton;
    Gdiplus::Image* mSysMinimizeButton;
    Gdiplus::Image* mSysMaximizeButton;

    NONCLIENTMETRICS mNcMetrics;
};