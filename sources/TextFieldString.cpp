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
    text_ { str }
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

/* --- Cursor operations --- */

void TextFieldString::MoveCursor(int direction)
{
    /* Get line size and quit if moving the cursor is not possible */
    auto size = GetText().size();
    if (size == 0)
        return;

    auto AdjustDir = [size](SizeType& p)
    {
        --p;
        p -= (p / size)*size;
    };

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

void TextFieldString::MoveCursorBegin()
{
    SetCursorPosition(0);
}

void TextFieldString::MoveCursorEnd()
{
    SetCursorPosition(GetText().size());
}

/* --- String content --- */

Char TextFieldString::CharLeft() const
{
    return (!IsCursorBegin() ? GetText()[GetCursorPosition() - 1] : Char(0));
}

Char TextFieldString::CharRight() const
{
    return (!IsCursorEnd() ? GetText()[GetCursorPosition()] : Char(0));
}

void TextFieldString::RemoveLeft()
{
    if (IsSelected())
    {
        /* First remove selection */
        RemoveSelection();
    }
    else if (!IsCursorBegin())
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
    if (IsSelected())
    {
        /* First remove selection */
        RemoveSelection();
    }
    else if (!IsCursorEnd())
    {
        /* Only remove character without moving the cursor */
        if ((GetCursorPosition() + 1) == text_.size())
            text_.pop_back();
        else
            text_.erase(Iter());
    }
}

void TextFieldString::RemoveSelection()
{
    /* Get selection range */
    SizeType start, end;
    GetSelection(start, end);

    /* Remove sub string */
    if (start < end)
        text_.erase(start, end - start);

    /* Locate cursor to the selection start */
    selectionEnabled = false;
    SetCursorPosition(start);
}

bool TextFieldString::IsValidChar(Char chr) const
{
    return (unsigned(chr) >= 32);
}

void TextFieldString::SetText(const String& text)
{
    text_ = text;
    UpdateCursorRange();
}

const String& TextFieldString::GetText() const
{
    return text_;
}


/*
 * ======= Private: =======
 */

void TextFieldString::InsertChar(Char chr, bool wasSelected)
{
    if (IsCursorEnd())
    {
        /* Push back the new character */
        text_ += chr;
    }
    else
    {
        /* Insert the new character (only use insertion if selection was not replaced) */
        if (insertionEnabled && !wasSelected)
            text_[GetCursorPosition()] = chr;
        else
            text_.insert(Iter(), chr);
    }
}

String::iterator TextFieldString::Iter()
{
    return (text_.begin() + GetCursorPosition());
}

String::const_iterator TextFieldString::Iter() const
{
    return (text_.begin() + GetCursorPosition());
}


} // /namespace Tg



// ================================================================================
