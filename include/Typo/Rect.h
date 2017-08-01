/*
 * Rect.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_RECT_H
#define TG_RECT_H


#include "Size.h"


namespace Tg
{


//! Simple rectangle structure with members: 'left', 'right', 'top', and 'bottom'.
struct Rect
{
    Rect() = default;
    Rect(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom) :
        left   { left   },
        top    { top    },
        right  { right  },
        bottom { bottom }
    {
    }

    unsigned int Width() const
    {
        return right - left;
    }

    unsigned int Height() const
    {
        return bottom - top;
    }

    Size GetSize() const
    {
        return Size(Width(), Height());
    }

    void SetSize(const Size& size)
    {
        right = left + size.width;
        bottom = top + size.height;
    }

    unsigned int left    = 0;
    unsigned int top     = 0;
    unsigned int right   = 0;
    unsigned int bottom  = 0;
};


} // /namespace Tg


#endif



// ================================================================================
