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

    if (IsNewLine(chr))
        AppendLine();
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
                ResetLines();
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

void MultiLineString::Insert(SizeType lineIndex, SizeType positionInLine, const Char& chr, bool replace)
{
    /* Validate parameters and get selected line */
    if (lineIndex >= lines_.size())
        return;

    auto& line = lines_[lineIndex];

    if (positionInLine > line.text.size())
        return;

    if (positionInLine == line.text.size() || IsNewLine(chr))
        replace = false;

    /* Update main string */
    auto textPos = GetTextPosition(lineIndex, positionInLine);
    if (replace)
    {
        if (textPos < text_.size())
            text_[textPos] = chr;
        else
            return;
    }
    else
        text_.insert(textPos, 1, chr);

    /* Update selected line with new character */
    if (IsNewLine(chr))
        ResetLines();
    else
    {
        /* Get width of new character */
        auto width = CharWidth(chr);

        if (replace)
        {
            auto prevWidth = CharWidth(line.text[positionInLine]);

            /* Check if character fits into current line */
            if (FitIntoLine(line.width + width - prevWidth))
            {
                /* Replace character and update widest width */
                line.width += width - prevWidth;
                line.text[positionInLine] = chr;
                UpdateWidestWidth(line.width);
            }
            else
                ResetLines();
        }
        else
        {
            /* Check if character fits into current line */
            if (FitIntoLine(line.width + width))
            {
                /* Insert character and update widest width */
                line.width += width;
                line.text.insert(positionInLine, 1, chr);
                UpdateWidestWidth(line.width);
            }
            else
                ResetLines();
        }
    }
}

void MultiLineString::Remove(SizeType lineIndex, SizeType positionInLine)
{
    /* Validate parameters and get selected line */
    if (lineIndex >= lines_.size())
        return;

    auto& line = lines_[lineIndex];

    if (positionInLine > line.text.size())
        return;





    /* Always update widest width when characters are removed */
    UpdateWidestWidth();
}

MultiLineString::SizeType MultiLineString::GetTextPosition(SizeType lineIndex, SizeType positionInLine) const
{
    if (lineIndex >= lines_.size() || positionInLine > lines_[lineIndex].text.size())
        return String::npos;

    SizeType pos = 0;

    for (SizeType i = 0; i < lineIndex; ++i)
    {
        auto len = lines_[i].text.size();
        pos += len;
        if (pos < text_.size() && IsNewLine(text_[pos]))
            ++pos;
    }

    pos += positionInLine;

    return pos;
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
