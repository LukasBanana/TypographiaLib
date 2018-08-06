/*
 * Font.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Font.h>
#include <Typo/MultiLineString.h>
#include <exception>
#include <cmath>
#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include "GlyphTree.h"


namespace Tg
{


static void Failed(FT_Error err, const std::string& msg)
{
    if (err)
        throw std::runtime_error(msg);
}

Font::Font(const FontDescription& desc, const FontGlyphSet& glyphSet) :
    desc_     { desc     },
    glyphSet_ { glyphSet }
{
}

Font::Font(const FontDescription& desc, FontGlyphSet&& glyphSet) :
    desc_     { desc                },
    glyphSet_ { std::move(glyphSet) }
{
}

Font::~Font()
{
}

template <typename T>
int TextWidthTmpl(const FontGlyphSet& glyphSet, const std::basic_string<T>& text, std::size_t offset, std::size_t len)
{
    /* Clamp text range */
    if (offset > text.size())
        offset = text.size();
    if (len > text.size() - offset)
        len = text.size() - offset;

    /* Sum glyph widths */
    int width = 0;

    for (std::size_t i = 0; i < len; ++i)
        width += glyphSet[text[offset + i]].advance;

    return width;
}

int Font::TextWidth(const std::string& text, std::size_t offset, std::size_t len) const
{
    return TextWidthTmpl(glyphSet_, text, offset, len);
}

int Font::TextWidth(const std::wstring& text, std::size_t offset, std::size_t len) const
{
    return TextWidthTmpl(glyphSet_, text, offset, len);
}


/* --- Global Operators --- */

#if 0
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
#endif


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

FontModel BuildFont(const FontDescription& desc, unsigned int border)
{
    return BuildFont(desc, FontGlyphRange { 32, 255 }, border);
}

UnpackedFontModel BuildUnpackedFont(const FontDescription& desc, unsigned int border)
{
    return BuildUnpackedFont(desc, FontGlyphRange { 32, 255 }, border);
}

static const FT_Pos g_metricSize = 64;

UnpackedFontModel BuildUnpackedFont(const FontDescription& desc, const FontGlyphRange& glyphRange, unsigned int border)
{
    UnpackedFontModel font;

    /* Store glyph set */
    font.glyphSet.SetGlyphRange(glyphRange);
    font.glyphSet.border = border;

    /* Initialize free type library */
    FT_Library ftLib;
    FT_Face face;

    auto err = FT_Init_FreeType(&ftLib);
    Failed(err, "failed to initialize FreeType library");

    /* Load font */
    if (desc.buffer)
        err = FT_New_Memory_Face(ftLib, reinterpret_cast<const FT_Byte*>(desc.buffer), static_cast<FT_Long>(desc.bufferSize), 0, &face);
    else
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
    font.glyphImages.reserve(glyphRange.GetSize());

    //#define TEST_STROKER
    #ifdef TEST_STROKER

    FT_Stroker stroker;
    err = FT_Stroker_New(ftLib, &stroker);
    Failed(err, "failed to create new stroker");

    FT_Stroker_Set(stroker, 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

    #endif

    for (auto chr = glyphRange.first; chr <= glyphRange.last; ++chr)
    {
        /* Load glyph image */
        auto glyphIndex = FT_Get_Char_Index(face, chr);

        err = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
        Failed(err, "failed to load glyph");

        #ifdef TEST_STROKER

        FT_Glyph ftglyph;
        err = FT_Get_Glyph(face->glyph, &ftglyph);
        Failed(err, "failed to get glyph");

        err = FT_Glyph_StrokeBorder(&ftglyph, stroker, 0, 1);
        Failed(err, "failed to set glyph stroker border");

        err = FT_Glyph_Stroke(&ftglyph, stroker, 1);
        Failed(err, "failed to stroke glyph");

        /* Draw current glyph */
        err = FT_Glyph_To_Bitmap(&ftglyph, FT_RENDER_MODE_NORMAL, nullptr, 1);
        Failed(err, "failed to render glyph");

        //FT_Done_Glyph(ftglyph);

        #else

        /* Draw current glyph */
        err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        Failed(err, "failed to render glyph");

        #endif

        /* Store glyph */
        const auto& metrics = face->glyph->metrics;
        auto& glyph = font.glyphSet[chr];

        glyph.width = metrics.width / g_metricSize;
        glyph.height = metrics.height / g_metricSize;

        if (isVertical)
        {
            glyph.xOffset = metrics.vertBearingX / g_metricSize;
            glyph.yOffset = metrics.vertBearingY / g_metricSize;
            glyph.advance = metrics.vertAdvance / g_metricSize;
        }
        else
        {
            glyph.xOffset = metrics.horiBearingX / g_metricSize;
            glyph.yOffset = metrics.horiBearingY / g_metricSize;
            glyph.advance = metrics.horiAdvance / g_metricSize;
        }

        /* Store glyph size */
        glyph.rect.right = glyph.width + border*2;
        glyph.rect.bottom = glyph.height + border*2;

        /* Store glyph image */
        #ifdef TEST_STROKER
        const auto& bitmap = reinterpret_cast<FT_BitmapGlyph>(ftglyph)->bitmap;
        glyph.rect.right = bitmap.width + border*2;
        glyph.rect.bottom = bitmap.rows + border*2;
        #else
        const auto& bitmap = face->glyph->bitmap;
        #endif
        Image image { Size { bitmap.width, bitmap.rows } };
        {
            std::copy(bitmap.buffer, bitmap.buffer + (bitmap.width*bitmap.rows), image.ImageBufferBegin());
        }
        font.glyphImages.emplace_back(std::move(image));
    }

    /* Release free type objects */
    FT_Done_Face(face);
    FT_Done_FreeType(ftLib);

    return font;
}

/*
This is the main function to build a font atlas image.
This function makes use of the FreeType library to render the font glyphs.
The function works is several phases:
 (1) Load font 'face' with FreeType 'ftLib'.
 (2) Render each font glyph and store its image in 'glyphImages'.
 (3) Approximate the font atlas size by 'sqrt(visualArea)'.
 (4) Build glyph tree to tightly pack the glyphs into a single image.
 (5) If a glyph does not fit into the glyph tree, double the smallest size (width or height) and go to phase 4.
 (6) Plot all glyph sub images into the final font atlas image.
*/
FontModel BuildFont(const FontDescription& desc, const FontGlyphRange& glyphRange, unsigned int border)
{
    auto fontUnpacked = BuildUnpackedFont(desc, glyphRange, border);

    FontModel font;
    font.glyphSet = std::move(fontUnpacked.glyphSet);

    unsigned int visualArea = 0;

    for (auto chr = glyphRange.first; chr <= glyphRange.last; ++chr)
    {
        /* Accumulate visual area to approximate glyph image size */
        const auto& glyph = font.glyphSet[chr];
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
        const auto& image = fontUnpacked.glyphImages[chr - glyphRange.first];

        font.image.PlotImage(
            glyph.rect.left + border,
            glyph.rect.top + border,
            image
        );
    }

    return font;
}

std::vector<FontGlyphGeometry> BuildFontGeometrySet(const FontModel& fontModel)
{
    std::vector<FontGlyphGeometry> geometries;

    const auto& glyphs = fontModel.glyphSet.GetGlyphs();
    geometries.reserve(glyphs.size());

    const auto& texSize = fontModel.image.GetSize();
    auto invTexWidth = 1.0f / texSize.width;
    auto invTexHeight = 1.0f / texSize.height;

    for (const auto& glyph : glyphs)
    {
        FontGlyphGeometry geom;

        /* Setup left-top vertex */
        geom.lt.x   = 0.0f;
        geom.lt.y   = 0.0f;
        geom.lt.tx  = invTexWidth * glyph.rect.left;
        geom.lt.ty  = invTexHeight * glyph.rect.top;

        /* Setup right-top vertex */
        geom.rt.x   = static_cast<float>(glyph.width);
        geom.rt.y   = 0.0f;
        geom.rt.tx  = invTexWidth * glyph.rect.right;
        geom.rt.ty  = invTexHeight * glyph.rect.top;

        /* Setup left-bottom vertex */
        geom.lb.x   = 0.0f;
        geom.lb.y   = static_cast<float>(glyph.height);
        geom.lb.tx  = invTexWidth * glyph.rect.left;
        geom.lb.ty  = invTexHeight * glyph.rect.bottom;

        /* Setup right-bottom vertex */
        geom.rb.x   = static_cast<float>(glyph.width);
        geom.rb.y   = static_cast<float>(glyph.height);
        geom.rb.tx  = invTexWidth * glyph.rect.right;
        geom.rb.ty  = invTexHeight * glyph.rect.bottom;

        geometries.push_back(geom);
    }

    return geometries;
}

static const auto staticGlpyhBorder = 2u;

//TODO: this is incomplete!!!
Image PlotTextImage(const FontModel& fontModel, const String& text)
{
    if (text.empty())
        return Image();

    /* Determine image size */
    const auto& glyphSet = fontModel.glyphSet;

    auto xPos = std::max(0, -glyphSet[text[0]].xOffset);
    auto width = static_cast<unsigned int>(xPos);

    int top = 0, bottom = 0, yOffsetMax = 0;

    for (auto chr : text)
    {
        const auto& glyph = fontModel.glyphSet[chr];

        width += glyph.advance;
        top = std::max(top, glyph.yOffset);
        bottom = std::max(bottom, glyph.height - glyph.yOffset);

        yOffsetMax = std::max(yOffsetMax, glyph.yOffset);
    }

    /* Plot each glyph into the image */
    Image image(Size(width + staticGlpyhBorder, top + bottom + staticGlpyhBorder));

    for (auto chr : text)
    {
        const auto& glyph = glyphSet[chr];

        image.PlotImage(
            xPos + glyph.xOffset,
            yOffsetMax - glyph.yOffset,
            fontModel.image,
            glyph.rect.left + glyphSet.border,
            glyph.rect.top + glyphSet.border,
            glyph.width + staticGlpyhBorder,
            glyph.height + staticGlpyhBorder,
            true
        );

        xPos += glyph.advance;
    }

    return image;
}

//TODO: this is incomplete!!!
Image PlotMultiLineTextImage(const FontModel& fontModel, const String& text, unsigned int maxWidth, unsigned int rowOffset)
{
    /* Setup multi-line text */
    MultiLineString mtText(fontModel.glyphSet, static_cast<int>(maxWidth), text);

    /* Determine image size */
    const auto& glyphSet = fontModel.glyphSet;

    Size size;

    int top = 0, bottom = 0, yOffsetMax = 0, xPos = 0;
    unsigned int width = 0, yPos = 0;

    for (const auto& line : mtText.GetLines())
    {
        if (line.text.empty())
            continue;

        xPos = std::max(0, -glyphSet[line.text[0]].xOffset);
        width = static_cast<unsigned int>(xPos);

        for (auto chr : line.text)
        {
            const auto& glyph = fontModel.glyphSet[chr];

            width += glyph.advance;
            top = std::max(top, glyph.yOffset);
            bottom = std::max(bottom, glyph.height - glyph.yOffset);

            yOffsetMax = std::max(yOffsetMax, glyph.yOffset);
        }

        size.width = std::max(size.width, width + staticGlpyhBorder);
        size.height = std::max(size.height, top + bottom + staticGlpyhBorder);
    }

    /* Plot each glyph into the image */
    size.height += rowOffset*mtText.GetLines().size();
    Image image(size);

    auto xPosStart = xPos;

    for (const auto& line : mtText.GetLines())
    {
        xPos = xPosStart;

        for (auto chr : line.text)
        {
            const auto& glyph = glyphSet[chr];

            image.PlotImage(
                xPos + glyph.xOffset,
                yPos + yOffsetMax - glyph.yOffset,
                fontModel.image,
                glyph.rect.left + glyphSet.border,
                glyph.rect.top + glyphSet.border,
                glyph.width + staticGlpyhBorder,
                glyph.height + staticGlpyhBorder,
                true
            );

            xPos += glyph.advance;
        }

        yPos += rowOffset;
    }

    return image;
}


} // /namespace Tg



// ================================================================================
