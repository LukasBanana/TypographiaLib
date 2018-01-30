/*
 * FontGlyphSet.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_FONT_GLYPH_SET_H
#define TG_FONT_GLYPH_SET_H


#include "Rect.h"

#include <vector>
#include <iostream>
#include <string>


namespace Tg
{


//! Font glyph range structure.
struct FontGlyphRange
{
    FontGlyphRange() = default;
    FontGlyphRange(const FontGlyphRange&) = default;
    inline FontGlyphRange(wchar_t first, wchar_t last) :
        first { first },
        last  { last  }
    {
    }

    FontGlyphRange& operator = (const FontGlyphRange&) = default;

    inline std::size_t GetSize() const
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
        // dummy (can not be defaulted for clang compiler!)
    }
    
    Rect    rect;           //!< Rectangular area of this font glyph within the font atlas.
    int     xOffset = 0;    //!< X coordinate offset of this font glyph to draw the glyph.
    int     yOffset = 0;    //!< Y coordinate offset of this font glyph to draw the glyph.
    int     width   = 0;    //!< Entire width of this font glyph.
    int     height  = 0;    //!< Entire height of this font glyph.
    int     advance = 0;    //!< Offset to draw the next font glyph (can be in X or Y direction).
};

//! Font glyph basic vertex structure.
struct FontGlyphVertex
{
    float x     = 0.0f; //!< X coordinate.
    float y     = 0.0f; //!< Y coordinate.
    float tx    = 0.0f; //!< X texture coordinate.
    float ty    = 0.0f; //!< Y texture coordinate.
};

//! Font glyph geometry structure. Contains four vertices for a single font glyph with triangle strip topology.
struct FontGlyphGeometry
{
    FontGlyphVertex lt; //!< Left-top vertex.
    FontGlyphVertex rt; //!< Right-top vertex.
    FontGlyphVertex lb; //!< Left-bottom vertex.
    FontGlyphVertex rb; //!< Right-bottom vertex.
};


//! Font glyph set class.
class FontGlyphSet
{
    
    public:

        FontGlyphSet() = default;
        FontGlyphSet(const FontGlyphSet&) = default;

        FontGlyphSet(FontGlyphSet&& rhs);

        FontGlyphSet& operator = (const FontGlyphSet&) = default;
        FontGlyphSet& operator = (FontGlyphSet&& rhs);

        //! Resizes the font glyph range.
        void SetGlyphRange(const FontGlyphRange& glyphRange);

        //! Returns the range of font glyphs.
        inline const FontGlyphRange& GetGlyphRange() const
        {
            return glyphRange_;
        }

        //! Returns the list of all font glyphs.
        inline const std::vector<FontGlyph>& GetGlyphs() const
        {
            return glyphs_;
        }

        //! Returns the font glyph for the specified UTF-8 character. If this character is not part of this glyph set, a dummy font glyph is returend.
        const FontGlyph& operator [] (char chr) const;
        //! Returns the font glyph for the specified UTF-16 character. If this character is not part of this glyph set, a dummy font glyph is returend.
        const FontGlyph& operator [] (wchar_t chr) const;

        //! Returns the font glyph for the specified UTF-8 character. If this character is not part of this glyph set, a dummy font glyph is returend.
        FontGlyph& operator [] (char chr);
        //! Returns the font glyph for the specified UTF-16 character. If this character is not part of this glyph set, a dummy font glyph is returend.
        FontGlyph& operator [] (wchar_t chr);

        //! Returns the width of the specified text.
        template <typename T>
        int TextWidth(const typename std::basic_string<T>& text) const
        {
            int width = 0;

            for (auto c : text)
                width += (*this)[c].advance;

            return width;
        }

        //! Returns the width of the specified sub text.
        template <typename T>
        int TextWidth(
            const typename std::basic_string<T>& text,
            typename std::basic_string<T>::size_type position,
            typename std::basic_string<T>::size_type count = std::basic_string<T>::npos) const
        {
            int width = 0;

            if (!text.empty() && position < text.size() && count > 0)
            {
                if (count == std::basic_string<T>::npos || count + position > text.size())
                    count = text.size() - position;

                for (auto end = position + count; position < end; ++position)
                    width += (*this)[text[position]].advance;
            }

            return width;
        }

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
