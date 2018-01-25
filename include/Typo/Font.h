/*
 * Font.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_FONT_H
#define TG_FONT_H


#include "Rect.h"
#include "FontGlyphSet.h"
#include "Image.h"
#include "Char.h"

#include <vector>
#include <iostream>
#include <string>


namespace Tg
{


//! Font flags enumeration.
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

//! Font descriptions structure.
struct FontDescription
{
    FontDescription() = default;
    FontDescription(const FontDescription&) = default;
    FontDescription& operator = (const FontDescription&) = default;

    FontDescription(const std::string& name, int size, int flags = 0) :
        name   { name  },
        height { size  },
        flags  { flags }
    {
    }

    FontDescription(const char* buffer, std::size_t bufferSize, int fontSize, int flags = 0) :
        buffer     { buffer     },
        bufferSize { bufferSize },
        height     { fontSize   },
        flags      { flags      }
    {
    }

    std::string name;
    const char* buffer      = nullptr;  //!< Pointer to a byte buffer if the font has already been loaded into memory. If null, 'name' is used to load the font from file.
    std::size_t bufferSize  = 0;        //!< Size of the byte buffer (if set).
    int         width       = 0;
    int         height      = 0;
    int         flags       = 0;        //!< This can be a bitwise OR combination of the values of the 'FontFlags' enumeration.
};

//! Font model data structure.
struct FontModel
{
    FontModel() = default;
    FontModel(const FontModel&) = default;
    FontModel& operator = (const FontModel&) = default;
    
    inline FontModel(FontModel&& rhs) :
        image    { std::move(rhs.image)    },
        glyphSet { std::move(rhs.glyphSet) }
    {
    }
    
    inline FontModel& operator = (FontModel&& rhs)
    {
        image    = std::move(rhs.image);
        glyphSet = std::move(rhs.glyphSet);
        return *this;
    }
    
    Image           image;      //!< Font atlas image.
    FontGlyphSet    glyphSet;   //!< Font glyph set.
};

//! Font model data structure.
struct UnpackedFontModel
{
    UnpackedFontModel() = default;
    UnpackedFontModel(const UnpackedFontModel&) = default;
    UnpackedFontModel& operator = (const UnpackedFontModel&) = default;
    
    inline UnpackedFontModel(UnpackedFontModel&& rhs) :
        glyphImages { std::move(rhs.glyphImages) },
        glyphSet    { std::move(rhs.glyphSet)    }
    {
    }
    
    inline UnpackedFontModel& operator = (UnpackedFontModel&& rhs)
    {
        glyphImages = std::move(rhs.glyphImages);
        glyphSet    = std::move(rhs.glyphSet);
        return *this;
    }
    
    std::vector<Image>  glyphImages;    //!< Font glyph image list.
    FontGlyphSet        glyphSet;       //!< Font glyph set.
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
\brief Builds an unpacked font model (i.e. one image object for each font glyph) with the specified description and the glyph range [32, 255].
\param[in] desc Specifies the font description.
\param[in] border Specifies the border (in pixels) for each glyph in the final glyph image.
*/
UnpackedFontModel BuildUnpackedFont(const FontDescription& desc, unsigned int border = 1);

/**
\brief Builds an unpacked font model (i.e. one image object for each font glyph) with the specified description and glyph range.
\param[in] desc Specifies the font description.
\param[in] glyphRange Specifies the range of glyphs which are to be contained in the resulting font.
\param[in] border Specifies the border (in pixels) for each glyph in the final glyph image.
*/
UnpackedFontModel BuildUnpackedFont(const FontDescription& desc, const FontGlyphRange& glyphRange, unsigned int border = 1);

/**
\brief Builds a font model with the specified description and the glyph range [32, 255].
\param[in] desc Specifies the font description.
\param[in] border Specifies the border (in pixels) for each glyph in the final glyph image.
*/
FontModel BuildFont(const FontDescription& desc, unsigned int border = 1);

/**
\brief Builds a font model with the specified description and glyph range.
\param[in] desc Specifies the font description.
\param[in] glyphRange Specifies the range of glyphs which are to be contained in the resulting font.
\param[in] border Specifies the border (in pixels) for each glyph in the final glyph image.
*/
FontModel BuildFont(const FontDescription& desc, const FontGlyphRange& glyphRange, unsigned int border = 1);

/**
\brief Builds the geometry list for all font glyphs.
\remarks This can be used to generate a vertex buffer for the font.
*/
std::vector<FontGlyphGeometry> BuildFontGeometrySet(const FontModel& fontModel);

/**
\brief Plots a text with the specified font model into an image.
\param[in] fontModel Specifies the font model which is used for text rendering.
\param[in] text Specifies the text for the output.
\see BuildFont
*/
Image PlotTextImage(const FontModel& fontModel, const String& text);

/**
\brief Plots a multi-line text with the specified font model into an image.
\param[in] fontModel Specifies the font model which is used for text rendering.
\param[in] text Specifies the text for the output.
\param[in] maxWidth Specifies the maximal width for the text.
\see BuildFont
\see PlotTextImage
\see MultiLineString
*/
Image PlotMultiLineTextImage(const FontModel& fontModel, const String& text, unsigned int maxWidth, unsigned int rowOffset);


} // /namespace Tg


#endif



// ================================================================================
