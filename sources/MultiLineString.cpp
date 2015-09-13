/*
 * MultiLineString.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/MultiLineString.h>
#include <algorithm>


namespace Tg
{


MultiLineString::MultiLineString(const FontGlyphSet& glyphSet, int maxWidth, const String& text) :
    glyphSet_   ( &glyphSet ),
    maxWidth_   ( maxWidth  ),
    width_      ( 0         ),
    text_       ( text      )
{
    ResetLines();
}

MultiLineString& MultiLineString::operator = (const String& str)
{
    SetText(str);
    return *this;
}

MultiLineString& MultiLineString::operator += (const String& str)
{
    for (const auto& chr : str)
        PushBack(chr);
    return *this;
}

MultiLineString& MultiLineString::operator += (const Char& chr)
{
    PushBack(chr);
    return *this;
}

void MultiLineString::PushBack(const Char& chr)
{
    /* Update main string */
    text_ += chr;

    /* Get glyph set from font */
    if (IsNewLine(chr))
    {
        /* Append empty line */
        AppendLine();
    }
    else
    {
        /* Get width of new character */
        auto width = CharWidth(chr);

        if (lines_.empty())
        {
            /* Add first character without width-check */
            AppendLine(chr, width);
        }
        else
        {
            /* Try to append character to last line */
            auto& line = lines_.back();

            /* Check if character fits into last line */
            if (FitIntoLine(line.width + width))
            {
                /* Update line and widest width */
                line.width += width;
                line.text += chr;
                UpdateWidestWidth(line.width);
            }
            else
            {
                /*
                Reset lines, because separators may change
                the current (last) line and the new line
                */
                ResetLines();
            }
        }
    }
}

void MultiLineString::PopBack()
{
    if (lines_.empty() || text_.empty())
        return;

    /* Update main string */
    text_.pop_back();

    /* Remove last character from last line */
    auto& line = lines_.back();
    auto chr = line.text.back();

    line.text.pop_back();

    if (line.text.empty())
    {
        /* Remove last line if it's empty */
        lines_.pop_back();
    }
    else
    {
        /* Update width in current line */
        int width = CharWidth(chr);
        line.width -= width;
    }

    /* Always update widest width when characters are removed */
    UpdateWidestWidth();
}

void MultiLineString::SetGlyphSet(const FontGlyphSet& glyphSet)
{
    glyphSet_ = &glyphSet;
    ResetLines();
}

void MultiLineString::SetMaxWidth(int maxWidth)
{
    if (maxWidth_ != maxWidth)
    {
        maxWidth_ = maxWidth;
        ResetLines();
    }
}

void MultiLineString::SetText(const String& text)
{
    text_ = text;
    ResetLines();
}


/*
 * ======= Protected: =======
 */

int MultiLineString::CharWidth(const Char& chr) const
{
    return GetGlyphSet()[chr].advance;
}


/*
 * ======= Private: =======
 */

bool MultiLineString::IsNewLine(const Char& chr) const
{
    return chr == Char('\n') || chr == Char('\r');
}

bool MultiLineString::FitIntoLine(int width) const
{
    return width <= GetMaxWidth();
}

void MultiLineString::UpdateWidestWidth(int width)
{
    width_ = std::max(width_, width);
}

void MultiLineString::UpdateWidestWidth()
{
    width_ = 0;
    for (const auto& line : lines_)
        width_ = std::max(width_, line.width);
}

void MultiLineString::AppendLine(const String& text, int width)
{
    lines_.push_back({ text, width });
    UpdateWidestWidth(width);
}

void MultiLineString::AppendLine(const Char& chr, int width)
{
    AppendLine(String(1, chr), 0);
}

void MultiLineString::AppendLine()
{
    AppendLine(String(), 0);
}

void MultiLineString::ResetLines()
{
    /* Reset line strings */
    lines_.clear();
    width_ = 0;

    if (text_.empty())
        return;

    /* Get glyph set from font */
    int nextWidth = 0, width = 0, sepWidth = 0;
    Char chr = 0;

    /* Setup all wrapped lines */
    for (std::size_t pos = 0, sepLen = 0, len = 0, num = text_.size(); ( pos < num ); pos += len)
    {
        /* Find maximal length for current line */
        for (chr = 0, len = 0, sepLen = 0, nextWidth = 0, width = 0; ( FitIntoLine(nextWidth) ); ++len)
        {
            /* Store new line width */
            width = nextWidth;

            /* Check for separator of previous character */
            if (IsSeparator(chr))
            {
                sepLen = len;
                sepWidth = width;
            }

            if (pos + len >= num)
                break;

            /* Get current character from base string */
            chr = text_[pos + len];

            /* Break line for new-line character */
            if (IsNewLine(chr))
                break;

            /* Add width of current character */
            nextWidth += CharWidth(chr);
        }

        /* Clamp line size to a minimum of one character */
        if (len == 0 && nextWidth > 0 && pos < num)
        {
            ++len;
            width = nextWidth;
        }
        /* Jump back to the previous separated text (if break was not caused by a new-line or end-of-line) */
        else if (sepLen > 0 && !IsNewLine(chr) && pos + len < num)
        {
            len = sepLen;
            width = sepWidth;
        }

        /* Add new line to list */
        if (len > 0)
            AppendLine(text_.substr(pos, len), width);
        else
            AppendLine();

        /* Ignore new-line characters in output text */
        if (IsNewLine(chr))
            ++len;
    }
}


} // /namespace Tg



// ================================================================================
