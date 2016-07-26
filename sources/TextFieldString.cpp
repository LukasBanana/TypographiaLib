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

/* --- Cursor operations --- */

void TextFieldString::SetCursorPosition(SizeType position)
{
    /* Clamp position to the range [0, length] */
    cursorPos_ = ClampedPos(position);

    /* If selection is disabled, also set selection start */
    if (!selectionEnabled)
        selStart_ = cursorPos_;
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

/* --- Selection operations --- */

void TextFieldString::SetSelection(SizeType start, SizeType end)
{
    /* Store (and later reset) current selection state */
    auto selEnabled = selectionEnabled;
    {
        /* Set selection start */
        selectionEnabled = false;
        SetCursorPosition(start);

        /* Set selection end */
        selectionEnabled = true;
        SetCursorPosition(end);
    }
    selectionEnabled = selEnabled;
}

void TextFieldString::GetSelection(SizeType& start, SizeType& end) const
{
    start = GetCursorPosition();
    end = selStart_;
    if (start > end)
        std::swap(start, end);
}

void TextFieldString::SelectAll()
{
    SetSelection(0, GetText().size());
}

void TextFieldString::Deselect()
{
    selectionEnabled = false;
    SetCursorPosition(GetCursorPosition());
}

bool TextFieldString::IsSelected() const
{
    return GetCursorPosition() != selStart_;
}

String TextFieldString::GetSelectionText() const
{
    SizeType start, end;
    GetSelection(start, end);
    return (start < end) ? text_.substr(start, end - start) : String();
}

/* --- String content --- */

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

bool TextFieldString::IsInsertionActive() const
{
    return insertionEnabled && !IsCursorEnd() && !IsSelected();
}

void TextFieldString::Insert(const Char& chr)
{
    if (IsValidChar(chr))
    {
        /* Replace selection by character */
        auto isSel = IsSelected();
        if (isSel)
            RemoveSelection();

        if (IsCursorEnd())
        {
            /* Push back the new character */
            text_ += chr;
        }
        else
        {
            /* Insert the new character (only use insertion if selection was not replaced) */
            if (insertionEnabled && !isSel)
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
