/*
 * Font.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_FONT_H__
#define __TG_FONT_H__


#include <vector>
#include <iostream>
#include <string>


namespace Tg
{


struct FontFlags
{
    enum
    {
        Bold        = (1 << 0),
        Italic      = (1 << 1),
        Underlined  = (1 << 2),
        StrikeOut   = (1 << 3),
    };
};

struct FontGlyphRange
{
    wchar_t first = 0, last = 0;
};

struct FontDescription
{
    std::string     name;
    int             width       = 0;
    int             height      = 0;
    FontGlyphRange  glyphRange;
    int             flags       = 0;
};

//! Simple rectangle structure with members: 'left', 'right', 'top', and 'bottom'.
struct Rect
{
    Rect() = default;
    Rect(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom) :
        left    ( left   ),
        top     ( top    ),
        right   ( right  ),
        bottom  ( bottom )
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

    unsigned int left    = 0;
    unsigned int top     = 0;
    unsigned int right   = 0;
    unsigned int bottom  = 0;
};

struct FontGlyph
{
    inline int TotalWidth() const
    {
        return startOffset + drawnWidth + whiteSpace;
    }
    
    Rect    rect;
    
    int     startOffset = 0;
    int     drawnWidth  = 0;
    int     whiteSpace  = 0;
};

struct FontGlyphSet
{
    FontGlyphSet() = default;
    FontGlyphSet(const FontGlyphSet&) = default;
    
    FontGlyphSet(FontGlyphSet&& rhs) :
        glyphRange  ( rhs.glyphRange        ),
        glyphs      ( std::move(rhs.glyphs) )
    {
    }
    
    const FontGlyph& operator [] (char chr) const
    {
        return (*this)[static_cast<wchar_t>(chr)];
    }
    
    const FontGlyph& operator [] (wchar_t chr) const
    {
        static const FontGlyph dummy;
        return (chr < glyphRange.first || chr > glyphRange.last) ? dummy : glyphs[chr - glyphRange.first];
    }
    
    FontGlyphRange            glyphRange;
    std::vector<FontGlyph>    glyphs;
};

struct Image
{
    Image() = default;
    Image(const Image&) = default;
    
    Image(Image&& rhs) :
        width       ( rhs.width                  ),
        height      ( rhs.height                 ),
        imageBuffer ( std::move(rhs.imageBuffer) )
    {
    }
    
    unsigned int                width;
    unsigned int                height;
    std::vector<unsigned char>  imageBuffer; //!< Gray scaled image buffer with (width*height) elements.
};

struct FontModel
{
    FontModel() = default;
    FontModel(const FontModel&) = default;
    
    FontModel(FontModel&& rhs) :
        image   ( std::move(rhs.image)    ),
        glyphSet( std::move(rhs.glyphSet) )
    {
    }
    
    Image           image;
    FontGlyphSet    glyphSet;
};


//! Font base class.
class Font
{
    
    public:
        
        Font(const FontDescription& desc, const FontGlyphSet& glyphSet);
        virtual ~Font();
        
        int TextWidth(const std::string& text, std::size_t offset = 0, std::size_t len = std::string::npos) const;
        int TextWidth(const std::wstring& text, std::size_t offset = 0, std::size_t len = std::string::npos) const;
        
        const FontDescription& GetDesc() const
        {
            return desc_;
        }
        
        const FontGlyphSet& GetGlyphSet() const
        {
            return glyphSet_;
        }
        
    private:
        
        FontDescription desc_;
        FontGlyphSet    glyphSet_;
        
};


/* --- Global Operators --- */

std::ostream& operator << (std::ostream& stream, const Image& image);
std::ostream& operator << (std::ostream& stream, const FontGlyphSet& glyphSet);
std::ostream& operator << (std::ostream& stream, const FontModel& fontModel);

std::istream& operator >> (std::istream& stream, Image& image);
std::istream& operator >> (std::istream& stream, FontGlyphSet& glyphSet);
std::istream& operator >> (std::istream& stream, FontModel& fontModel);


/* --- Global Functions --- */

FontModel BuildFont(const FontDescription& desc);


} // /namespace Tg


#endif



// ================================================================================
