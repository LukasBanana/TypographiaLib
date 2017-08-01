/*
 * Size.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_SIZE_H
#define TG_SIZE_H


namespace Tg
{


//! Simple size structure with members: 'width', and 'height'.
struct Size
{
    Size() = default;
    Size(unsigned int width, unsigned int height) :
        width  { width  },
        height { height }
    {
    }

    //! Returns the area of this size (width*height).
    unsigned int Area() const
    {
        return width*height;
    }

    unsigned int width  = 0;
    unsigned int height = 0;
};


} // /namespace Tg


#endif



// ================================================================================
