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
template <typename T>
struct Point
{
    Point() = default;
    Point(const T& x, const T& y) :
        x( x ),
        y( y )
    {
    }

    T x = T(0);
    T y = T(0);
};


template <typename T>
inline bool operator == (const Point<T>& lhs, const Point<T>& rhs)
{
    return (lhs.x == rhs.x && lhs.y == rhs.y);
}

template <typename T>
inline bool operator != (const Point<T>& lhs, const Point<T>& rhs)
{
    return !(lhs == rhs);
}


} // /namespace Tg


#endif



// ================================================================================
