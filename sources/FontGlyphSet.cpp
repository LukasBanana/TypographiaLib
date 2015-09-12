/*
 * FontGlyphSet.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/FontGlyphSet.h>


namespace Tg
{


FontGlyphSet::FontGlyphSet(FontGlyphSet&& rhs) :
    glyphRange_ ( rhs.glyphRange_        ),
    glyphs_     ( std::move(rhs.glyphs_) ),
    isVertical  ( rhs.isVertical         )
{
}

void FontGlyphSet::SetGlyphRange(const FontGlyphRange& glyphRange)
{
    glyphRange_ = glyphRange;
    glyphs_.resize(glyphRange_.GetSize());
}

const FontGlyphRange& FontGlyphSet::GetGlyphRange() const
{
    return glyphRange_;
}

const FontGlyph& FontGlyphSet::operator [] (char chr) const
{
    return (*this)[static_cast<wchar_t>(unsigned char(chr))];
}

const FontGlyph& FontGlyphSet::operator [] (wchar_t chr) const
{
    static const FontGlyph dummy;
    return (chr < glyphRange_.first || chr > glyphRange_.last) ? dummy : glyphs_[chr - glyphRange_.first];
}

FontGlyph& FontGlyphSet::operator [] (char chr)
{
    return (*this)[static_cast<wchar_t>(unsigned char(chr))];
}

FontGlyph& FontGlyphSet::operator [] (wchar_t chr)
{
    static FontGlyph dummy;
    return (chr < glyphRange_.first || chr > glyphRange_.last) ? dummy : glyphs_[chr - glyphRange_.first];
}


} // /namespace Tg



// ================================================================================
