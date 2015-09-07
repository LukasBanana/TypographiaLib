/*
 * Font.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_FONT_H__
#define __TG_FONT_H__


#include "Rect.h"
#include "FontGlyphSet.h"
#include "Image.h"

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

struct FontDescription
{
    std::string name;
    int         width   = 0;
    int         height  = 0;
    int         flags   = 0; //!< This can be a bitwise OR combination of the values of the 'FontFlags' enumeration.
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
        
        Font(const FontDescription& desc, const FontGlyphRange& glyphRange);
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

        //! Returns true if this has a vertical text layout. Otherwise it has a horizontal text layout.
        bool IsVertical() const
        {
            return isVertical_;
        }
        
    private:
        
        FontDescription desc_;
        FontGlyphSet    glyphSet_;
        bool            isVertical_ = false;
        
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
