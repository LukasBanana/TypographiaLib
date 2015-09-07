/*
 * FontGlyphSet.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_FONT_GLYPH_SET_H__
#define __TG_FONT_GLYPH_SET_H__


#include "Rect.h"

#include <vector>
#include <iostream>
#include <string>


namespace Tg
{


struct FontGlyphRange
{
    FontGlyphRange() = default;
    FontGlyphRange(wchar_t first, wchar_t last) :
        first   ( first ),
        last    ( last  )
    {
    }

    std::size_t GetSize() const
    {
        return std::size_t(last >= first ? (last - first + 1) : 0u);
    }

    wchar_t first = 0, last = 0;
};

//! Font glyph data structure.
struct FontGlyph
{
    int TotalWidth() const
    {
        return startOffsetX + drawnSize + whiteSpace;
    }
    
    Rect    rect;
    int     startOffsetX    = 0;
    int     startOffsetY    = 0;
    int     drawnSize       = 0;
    int     whiteSpace      = 0;
};

class FontGlyphSet
{
    
    public:

        FontGlyphSet() = default;
        FontGlyphSet(const FontGlyphSet&) = default;

        FontGlyphSet(FontGlyphSet&& rhs);

        void SetGlyphRange(const FontGlyphRange& glyphRange);
        const FontGlyphRange& GetGlyphRange() const;

        const FontGlyph& operator [] (char chr) const;
        const FontGlyph& operator [] (wchar_t chr) const;

        FontGlyph& operator [] (char chr);
        FontGlyph& operator [] (wchar_t chr);

        //! Specifies whether this glyph set has a vertical or a horizontal text layout. By default false.
        bool isVertical = false;

    private:

        FontGlyphRange          glyphRange_;
        std::vector<FontGlyph>  glyphs_;

};


} // /namespace Tg


#endif



// ================================================================================
