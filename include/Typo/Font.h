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
        
        Font(const FontDescription& desc, const FontGlyphSet& glyphSet);
        Font(const FontDescription& desc, FontGlyphSet&& glyphSet);
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

/**
Builds a font model with the specified description and glyph range.
\param[in] desc Specifies the font description.
\param[in] glyphRange Specifies the range of glyphs which are to be contained in the resulting font.
\param[in] border Specifies the border (in pixels) for each glyph in the final glyph image.
*/
FontModel BuildFont(const FontDescription& desc, const FontGlyphRange& glyphRange, unsigned int border = 1);

/**
Plots a text with the specified font model into an image.
\param[in] fontModel Specifies the font model which is used for text rendering.
\param[in] text Specifies the text for the output.
\see BuildFont
*/
Image PlotTextImage(const FontModel& fontModel, const std::wstring& text);

/**
Plots a multi-line text with the specified font model into an image.
\param[in] fontModel Specifies the font model which is used for text rendering.
\param[in] text Specifies the text for the output.
\param[in] maxWidth Specifies the maximal width for the text.
\see BuildFont
\see PlotTextImage
\see MultiLineString
*/
Image PlotMultiLineTextImage(const FontModel& fontModel, const std::wstring& text, unsigned int maxWidth, unsigned int rowOffset);


} // /namespace Tg


#endif



// ================================================================================
