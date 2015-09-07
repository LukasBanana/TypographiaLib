/*
 * Font.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Font.h>
#include <exception>
#include <cmath>

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
}

Font::Font(const FontDescription& desc, FontGlyphSet&& glyphSet) :
    desc_       ( desc                ),
    glyphSet_   ( std::move(glyphSet) )
{
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

static unsigned int RoundPow2(unsigned int size)
{
    unsigned int result = 1;
    
    while (result < size)
        result <<= 1;
    
    if (result - size <= size - result/2)
        return result;
    
    return result/2;
}

static Size ApproximateFontAtlasSize(unsigned int visualArea)
{
    visualArea = static_cast<unsigned int>(std::ceil(std::sqrt(visualArea)));

    auto size = RoundPow2(visualArea);
    auto result = Size(size, size);

    if (result.width < visualArea)
        result.width *= 2;

    return result;
}

FontModel BuildFont(const FontDescription& desc, const FontGlyphRange& glyphRange, unsigned int border)
{
    static const FT_Pos metricSize = 64;

    FontModel font;

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

    /* Store flags */
    auto isVertical = (FT_HAS_VERTICAL(face) != 0);

    /* Setup pixel size */
    //err = FT_Set_Char_Size(face, 0, 15*64, 50, 50);
    //Failed(err, "failed to set character size");

    err = FT_Set_Pixel_Sizes(face, desc.width, desc.height);
    Failed(err, "failed to set pixel sizes");
    
    /* Reserve glyph container */
    font.glyphSet.SetGlyphRange(glyphRange);
    
    std::vector<Image> glyphImages;
    glyphImages.reserve(glyphRange.GetSize());

    unsigned int visualArea = 0;

    for (auto chr = glyphRange.first; chr <= glyphRange.last; ++chr)
    {
        /* Load glyph image */
        auto glyphIndex = FT_Get_Char_Index(face, chr);

        err = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
        Failed(err, "failed to load glyph");

        /* Draw current glyph */
        err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        Failed(err, "failed to render glyph");

        /* Store glyph */
        const auto& metrics = face->glyph->metrics;
        auto& glyph = font.glyphSet[chr];

        auto width = metrics.width / metricSize;
        auto height = metrics.height / metricSize;

        if (isVertical)
        {
            glyph.startOffsetX  = metrics.vertBearingX / metricSize;
            glyph.drawnSize     = height;
            glyph.whiteSpace    = metrics.vertAdvance / metricSize - glyph.drawnSize - glyph.startOffsetY;
        }
        else
        {
            glyph.startOffsetX  = metrics.horiBearingX / metricSize;
            glyph.drawnSize     = width;
            glyph.whiteSpace    = metrics.horiAdvance / metricSize - glyph.drawnSize - glyph.startOffsetX;
        }

        /* Store glyph size */
        glyph.rect.right = width + border*2;
        glyph.rect.bottom = height + border*2;

        /* Store glyph image */
        const auto& bitmap = face->glyph->bitmap;
        Image image(Size(bitmap.width, bitmap.rows));
        {
            std::copy(bitmap.buffer, bitmap.buffer + (bitmap.width*bitmap.rows), image.ImageBufferBegin());
        }
        glyphImages.emplace_back(std::move(image));

        /* Accumulate visual area to approximate glyph image size */
        visualArea += Size(glyph.rect.right, glyph.rect.bottom).Area();
    }

    /* Generate glyph tree */
    auto fontAtlasSize = ApproximateFontAtlasSize(visualArea);
    bool fillTreeFailed = false;

    GlyphTree glyphTree;

    do
    {
        /* Reset glyph tree */
        glyphTree.Reset(fontAtlasSize);
        fillTreeFailed = false;

        /* Insert all font glyphs into the tree */
        for (auto chr = glyphRange.first; chr <= glyphRange.last; ++chr)
        {
            /* Insert current glyph into tree */
            if (!glyphTree.Insert(font.glyphSet[chr]))
            {
                /* Increase font atlas size */
                if (fontAtlasSize.width < fontAtlasSize.height)
                    fontAtlasSize.width *= 2;
                else
                    fontAtlasSize.height *= 2;

                /* Break insertion and start with new tree */
                fillTreeFailed = true;
                break;
            }
        }
    }
    while (fillTreeFailed);

    /* Plot final font atlas */
    font.image.SetSize(fontAtlasSize);

    for (auto chr = glyphRange.first; chr <= glyphRange.last; ++chr)
    {
        const auto& glyph = font.glyphSet[chr];
        const auto& image = glyphImages[chr - glyphRange.first];

        font.image.PlotImage(
            glyph.rect.left + border,
            glyph.rect.top + border,
            image
        );
    }

    /* Release free type objects */
    FT_Done_Face(face);
    FT_Done_FreeType(ftLib);

    return font;
}


} // /namespace Tg



// ================================================================================
