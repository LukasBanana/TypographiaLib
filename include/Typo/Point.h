/*
 * Point.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_POINT_H__
#define __TG_POINT_H__


namespace Tg
{


//! Simple point structure with members: 'x', and 'y'.
struct Point
{
    Point() = default;

    inline Point(std::size_t x, std::size_t y) :
        x( x ),
        y( y )
    {
    }

    std::size_t x = 0;
    std::size_t y = 0;
};


template <typename T>
inline bool operator == (const Point& lhs, const Point& rhs)
{
    return (lhs.x == rhs.x && lhs.y == rhs.y);
}

template <typename T>
inline bool operator != (const Point& lhs, const Point& rhs)
{
    return !(lhs == rhs);
}


} // /namespace Tg


#endif



// ================================================================================
