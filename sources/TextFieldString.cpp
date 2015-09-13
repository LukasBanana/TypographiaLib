/*
 * TextFieldString.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/TextFieldString.h>
#include <algorithm>


namespace Tg
{


TextFieldString::TextFieldString(const String& str) :
    text_( str )
{
}

TextFieldString& TextFieldString::operator = (const String& str)
{
    SetText(str);
    return *this;
}

TextFieldString& TextFieldString::operator += (const String& str)
{
    for (const auto& chr : str)
        Insert(chr);
    return *this;
}

TextFieldString& TextFieldString::operator += (const Char& chr)
{
    Insert(chr);
    return *this;
}

bool TextFieldString::IsCursorBegin() const
{
    return cursorPos_ == 0;
}

bool TextFieldString::IsCursorEnd() const
{
    return cursorPos_ == text_.size();
}

void TextFieldString::MoveCursor(int direction)
{
    if (direction < 0)
    {
        auto dir = static_cast<SizeType>(-direction);
        if (cursorPos_ > dir)
            cursorPos_ -= dir;
        else
            cursorPos_ = 0;
    }
    else if (direction > 0)
    {
        auto dir = static_cast<SizeType>(direction);
        cursorPos_ += dir;

        if (cursorPos_ > text_.size())
            cursorPos_ = text_.size();
    }
}

void TextFieldString::MoveCursorBegin()
{
    cursorPos_ = 0;
}

void TextFieldString::MoveCursorEnd()
{
    cursorPos_ = text_.size();
}

void TextFieldString::JumpLeft()
{
    if (IsSeparator(CharLeft()))
    {
        while (!IsCursorBegin() && IsSeparator(CharLeft()))
            --cursorPos_;
    }
    while (!IsCursorBegin() && !IsSeparator(CharLeft()))
        --cursorPos_;
}

void TextFieldString::JumpRight()
{
    if (IsSeparator(CharRight()))
    {
        while (!IsCursorEnd() && IsSeparator(CharRight()))
            ++cursorPos_;
    }
    while (!IsCursorEnd() && !IsSeparator(CharRight()))
        ++cursorPos_;
}

Char TextFieldString::CharLeft() const
{
    return !IsCursorBegin() ? text_[cursorPos_ - 1] : Char(0);
}

Char TextFieldString::CharRight() const
{
    return !IsCursorEnd() ? text_[cursorPos_] : Char(0);
}

void TextFieldString::RemoveLeft()
{
    if (!IsCursorBegin())
    {
        /* Remove character and then move cursor left */
        --cursorPos_;
        if (IsCursorEnd())
            text_.pop_back();
        else
            text_.erase(Iter());
    }
}

void TextFieldString::RemoveRight()
{
    if (!IsCursorEnd())
    {
        /* Only remove character without moving the cursor */
        if ((cursorPos_ + 1) == text_.size())
            text_.pop_back();
        else
            text_.erase(Iter());
    }
}

void TextFieldString::RemoveSequenceLeft()
{
    /* Remove all characters before the cursor, until the next separator appears */
    while (!IsCursorBegin() && IsSeparator(CharLeft()))
        RemoveLeft();
    while (!IsCursorBegin() && !IsSeparator(CharLeft()))
        RemoveLeft();
}

void TextFieldString::RemoveSequenceRight()
{
    /* Remove all characters after the cursor, until the next separator appears */
    while (!IsCursorEnd() && IsSeparator(CharRight()))
        RemoveRight();
    while (!IsCursorEnd() && !IsSeparator(CharRight()))
        RemoveRight();
}

void TextFieldString::Insert(const Char& chr)
{
    if (IsValidChar(chr))
    {
        if (IsCursorEnd())
        {
            /* Push back the new character */
            text_ += chr;
        }
        else
        {
            /* Insert the new character */
            if (insertionEnabled)
                text_[cursorPos_] = chr;
            else
                text_.insert(Iter(), chr);
        }

        /* Move cursor position */
        ++cursorPos_;
    }
}

void TextFieldString::Put(const Char& chr)
{
    if (chr == Char('\b'))
        RemoveLeft();
    else if (chr == Char(127))
        RemoveRight();
    else if (unsigned(chr) >= 32)
        Insert(chr);
}

void TextFieldString::Put(const String& text)
{
    for (const auto& chr : text)
        Put(chr);
}

void TextFieldString::SetText(const String& text)
{
    text_ = text;
    ClampCursorPos();
}


/*
 * ======= Protected: =======
 */

bool TextFieldString::IsValidChar(const Char& chr) const
{
    return unsigned(chr) >= 32;
}


/*
 * ======= Private: =======
 */

String::iterator TextFieldString::Iter()
{
    return text_.begin() + cursorPos_;
}

String::const_iterator TextFieldString::Iter() const
{
    return text_.begin() + cursorPos_;
}

void TextFieldString::ClampCursorPos()
{
    cursorPos_ = std::max(0u, text_.size());
}


} // /namespace Tg



// ================================================================================
