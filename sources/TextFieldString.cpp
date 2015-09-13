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

void TextFieldString::SetCursorPosition(SizeType position)
{
    /* Clamp position to the range [0, size] */
    position = ClampedPos(position);

    /* Always set cursor position */
    cursorPos_ = position;

    /* If selection is enabled, also set selection start */
    if (selectionEnabled)
        selStart_ = position;
}

bool TextFieldString::IsCursorBegin() const
{
    return GetCursorPosition() == 0;
}

bool TextFieldString::IsCursorEnd() const
{
    return GetCursorPosition() == text_.size();
}

void TextFieldString::MoveCursor(int direction)
{
    auto size = GetText().size();

    auto AdjustDir = [size](SizeType& p)
    {
        --p;
        p -= (p / size)*size;
    };

    if (size > 0)
    {
        if (direction < 0)
        {
            auto dir = static_cast<SizeType>(-direction);

            if (GetCursorPosition() >= dir)
            {
                /* Move cursor left */
                SetCursorPosition(GetCursorPosition() - dir);
            }
            else
            {
                if (cursorLoopEnabled)
                {
                    /* Locate cursor to the end and move on (subtract all repeated loops) */
                    AdjustDir(dir);
                    SetCursorPosition(size - dir);
                }
                else
                {
                    /* Locate cursor to the beginning */
                    MoveCursorBegin();
                }
            }
        }
        else if (direction > 0)
        {
            auto dir = static_cast<SizeType>(direction);

            if (GetCursorPosition() + dir <= size)
            {
                /* Move cursor right */
                SetCursorPosition(GetCursorPosition() + dir);
            }
            else
            {
                if (cursorLoopEnabled)
                {
                    /* Locate cursor to the beginning and move on (subtract all repeated loops) */
                    AdjustDir(dir);
                    SetCursorPosition(dir);
                }
                else
                {
                    /* Locate cursor to the end */
                    MoveCursorEnd();
                }
            }
        }
    }
}

void TextFieldString::MoveCursorBegin()
{
    SetCursorPosition(0);
}

void TextFieldString::MoveCursorEnd()
{
    SetCursorPosition(GetText().size());
}

void TextFieldString::JumpLeft()
{
    if (IsSeparator(CharLeft()))
    {
        while (!IsCursorBegin() && IsSeparator(CharLeft()))
            MoveCursor(-1);
    }
    while (!IsCursorBegin() && !IsSeparator(CharLeft()))
        MoveCursor(-1);
}

void TextFieldString::JumpRight()
{
    if (IsSeparator(CharRight()))
    {
        while (!IsCursorEnd() && IsSeparator(CharRight()))
            MoveCursor(1);
    }
    while (!IsCursorEnd() && !IsSeparator(CharRight()))
        MoveCursor(1);
}

Char TextFieldString::CharLeft() const
{
    return !IsCursorBegin() ? GetText()[GetCursorPosition() - 1] : Char(0);
}

Char TextFieldString::CharRight() const
{
    return !IsCursorEnd() ? GetText()[GetCursorPosition()] : Char(0);
}

void TextFieldString::RemoveLeft()
{
    if (!IsCursorBegin())
    {
        /* Remove character and then move cursor left */
        MoveCursor(-1);
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
        if ((GetCursorPosition() + 1) == text_.size())
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
                text_[GetCursorPosition()] = chr;
            else
                text_.insert(Iter(), chr);
        }

        /* Move cursor position */
        MoveCursor(1);
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
    UpdateCursorRange();
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
    return text_.begin() + GetCursorPosition();
}

String::const_iterator TextFieldString::Iter() const
{
    return text_.begin() + GetCursorPosition();
}

TextFieldString::SizeType TextFieldString::ClampedPos(SizeType pos) const
{
    return std::min(pos, GetText().size());
}

void TextFieldString::UpdateCursorRange()
{
    cursorPos_ = ClampedPos(GetCursorPosition());
    selStart_ = ClampedPos(selStart_);
}


} // /namespace Tg



// ================================================================================
