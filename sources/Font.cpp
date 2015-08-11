/*
 * Font.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Font.h>


namespace Tg
{


Font::Font(const FontDescription& desc, const FontGlyphSet& glyphSet) :
    desc_       ( desc     ),
    glyphSet_   ( glyphSet )
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

FontModel BuildFont(const FontDescription& desc)
{
    FontModel font;

    //...

    return font;
}


} // /namespace Tg



// ================================================================================
