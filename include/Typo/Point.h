/*
 * Point.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_POINT_H
#define TG_POINT_H


namespace Tg
{


//! Simple point structure with members: 'x', and 'y'.
struct Point
{
    Point() = default;

    inline Point(std::size_t x, std::size_t y) :
        x { x },
        y { y }
    {
    }

    std::size_t x = 0;
    std::size_t y = 0;
};


inline bool operator == (const Point& lhs, const Point& rhs)
{
    return (lhs.x == rhs.x && lhs.y == rhs.y);
}

inline bool operator != (const Point& lhs, const Point& rhs)
{
    return !(lhs == rhs);
}


} // /namespace Tg


#endif



// ================================================================================
