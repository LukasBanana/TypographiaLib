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
    FontGlyph()
    {
    }
    
    Rect    rect;
    int     xOffset = 0;
    int     yOffset = 0;
    int     width   = 0;
    int     height  = 0;
    int     advance = 0;
};

//! Font glyph basic vertex structure.
struct FontGlyphVertex
{
    float x     = 0.0f; //!< X coordinate.
    float y     = 0.0f; //!< Y coordinate.
    float tx    = 0.0f; //!< X texture coordinate.
    float ty    = 0.0f; //!< Y texture coordinate.
};

//! Font glyph geometry structure. Contains four vertices for a single font glyph (within a left-top origin system).
struct FontGlyphGeometry
{
    FontGlyphVertex lt; //!< Left-top vertex.
    FontGlyphVertex rt; //!< Right-top vertex.
    FontGlyphVertex rb; //!< Right-bottom vertex.
    FontGlyphVertex lb; //!< Left-bottom vertex.
};


class FontGlyphSet
{
    
    public:

        FontGlyphSet() = default;
        FontGlyphSet(const FontGlyphSet&) = default;

        FontGlyphSet(FontGlyphSet&& rhs);

        void SetGlyphRange(const FontGlyphRange& glyphRange);

        const FontGlyphRange& GetGlyphRange() const
        {
            return glyphRange_;
        }

        const std::vector<FontGlyph>& GetGlyphs() const
        {
            return glyphs_;
        }

        const FontGlyph& operator [] (char chr) const;
        const FontGlyph& operator [] (wchar_t chr) const;

        FontGlyph& operator [] (char chr);
        FontGlyph& operator [] (wchar_t chr);

        //! Specifies whether this glyph set has a vertical or a horizontal text layout. By default false.
        bool            isVertical  = false;

        //! Specifies the border for each glyph in the font atlas image.
        unsigned int    border      = 0;

    private:

        FontGlyphRange          glyphRange_;
        std::vector<FontGlyph>  glyphs_;

};


} // /namespace Tg


#endif



// ================================================================================
