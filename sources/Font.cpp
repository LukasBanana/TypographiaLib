/*
 * Font.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Font.h>
#include <exception>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "GlyphTree.h"


namespace Tg
{


static void Failed(FT_Error err, const std::string& msg)
{
    if (err)
        throw std::runtime_error(msg);
}

Font::Font(const FontDescription& desc, const FontGlyphSet& glyphSet) :
    desc_       ( desc     ),
    glyphSet_   ( glyphSet )
{
    FT_Library ftLib;
    FT_Face face;

    /* Initialize free type library */
    auto err = FT_Init_FreeType(&ftLib);
    Failed(err, "failed to initialize FreeType library");

    /* Load font */
    err = FT_New_Face(ftLib, desc.name.c_str(), 0, &face);
    if (err == FT_Err_Unknown_File_Format)
        Failed(err, "unknown font file format");
    else
        Failed(err, "failed to load font file");

    /* Setup pixel size */
    err = FT_Set_Char_Size(face, 0, 15*64, 700, 700);
    Failed(err, "failed to set character size");

    err = FT_Set_Pixel_Sizes(face, desc.width, desc.height);
    Failed(err, "failed to set pixel sizes");

    for (FT_ULong chr = glyphSet.glyphRange.first; chr <= glyphSet.glyphRange.last; ++chr)
    {
        /* Load glyph image */
        auto glyphIndex = FT_Get_Char_Index(face, chr);

        err = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
        Failed(err, "failed to load glyph");

        /* Draw current glyph */
        err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        Failed(err, "failed to render glyph");
    }

    /* Release free type objects */
    FT_Done_Face(face);
    FT_Done_FreeType(ftLib);
}

Font::~Font()
{
}

int Font::TextWidth(const std::string& text, std::size_t offset, std::size_t len) const
{
    return 0;//!!!
}

int Font::TextWidth(const std::wstring& text, std::size_t offset, std::size_t len) const
{
    return 0;//!!!
}


/* --- Global Operators --- */

std::ostream& operator << (std::ostream& stream, const Image& image)
{
    //...
    return stream;
}

std::ostream& operator << (std::ostream& stream, const FontGlyphSet& glyphSet)
{
    //...
    return stream;
}

std::ostream& operator << (std::ostream& stream, const FontModel& fontModel)
{
    //...
    return stream;
}

std::istream& operator >> (std::istream& stream, Image& image)
{
    //...
    return stream;
}

std::istream& operator >> (std::istream& stream, FontGlyphSet& glyphSet)
{
    //...
    return stream;
}

std::istream& operator >> (std::istream& stream, FontModel& fontModel)
{
    //...
    return stream;
}


/* --- Global Functions --- */

FontModel BuildFont(const FontDescription& desc)
{
    FontModel font;

    //...

    return font;
}


} // /namespace Tg



// ================================================================================
