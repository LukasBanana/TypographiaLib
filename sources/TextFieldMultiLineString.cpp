/*
 * TextFieldMultiLineString.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/TextFieldMultiLineString.h>
#include <algorithm>


namespace Tg
{


TextFieldMultiLineString::TextFieldMultiLineString(const FontGlyphSet& glyphSet, int maxWidth, const String& text) :
    MultiLineString( glyphSet, maxWidth, text )
{
}

TextFieldMultiLineString& TextFieldMultiLineString::operator = (const String& str)
{
    SetText(str);
    return *this;
}

TextFieldMultiLineString& TextFieldMultiLineString::operator += (const String& str)
{
    for (const auto& chr : str)
        Insert(chr);
    return *this;
}

TextFieldMultiLineString& TextFieldMultiLineString::operator += (const Char& chr)
{
    Insert(chr);
    return *this;
}

/* --- Cursor operations --- */

void TextFieldMultiLineString::SetCursorPosition(SizeType positionX, SizeType positionY)
{
    /* First clamp Y position to the range [0, lines), then clamp X position to the range [0, length] */
    cursorPosY_ = ClampedPosY(positionY);
    cursorPosX_ = ClampedPosX(positionX);

    /* If selection is disabled, also set selection start */
    if (!selectionEnabled)
    {
        selStartX_ = cursorPosX_;
        selStartY_ = cursorPosY_;
    }
}

void TextFieldMultiLineString::GetCursorPosition(SizeType& positionX, SizeType& positionY) const
{
    positionX = GetCursorPositionX();
    positionY = GetCursorPositionY();
}

bool TextFieldMultiLineString::IsCursorBegin() const
{
    return (GetCursorPositionX() == 0);
}

bool TextFieldMultiLineString::IsCursorEnd() const
{
    return (GetCursorPositionX() == GetLineText().size());
}

bool TextFieldMultiLineString::IsCursorTop() const
{
    return (GetCursorPositionY() == 0);
}

bool TextFieldMultiLineString::IsCursorBottom() const
{
    return (GetLines().empty() || GetCursorPositionY() == GetLines().size() - 1);
}

template <typename T>
static void AdjustDir(T size, T& dir)
{
    --dir;
    dir -= (dir / size)*size;
};

void TextFieldMultiLineString::MoveCursorX(int direction)
{
    /* Get line size and quit if moving the cursor is not possible */
    auto size = GetLineText().size();
    if (size == 0)
        return;

    if (direction < 0)
    {
        auto dir = static_cast<SizeType>(-direction);

        if (GetCursorPositionX() >= dir)
        {
            /* Move cursor left */
            SetCursorPosition(GetCursorPositionX() - dir, GetCursorPositionY());
        }
        else
        {
            if (cursorLoopEnabled)
            {
                /* Locate cursor to the end and move on (subtract all repeated loops) */
                AdjustDir(size, dir);
                SetCursorPosition(size - dir, GetCursorPositionY());
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

        if (GetCursorPositionX() + dir <= size)
        {
            /* Move cursor right */
            SetCursorPosition(GetCursorPositionX() + dir, GetCursorPositionY());
        }
        else
        {
            if (cursorLoopEnabled)
            {
                /* Locate cursor to the beginning and move on (subtract all repeated loops) */
                AdjustDir(size, dir);
                SetCursorPosition(dir, GetCursorPositionY());
            }
            else
            {
                /* Locate cursor to the end */
                MoveCursorEnd();
            }
        }
    }
}

void TextFieldMultiLineString::MoveCursorY(int direction)
{
    /* Get number of lines and quit if moving cursor is not possible */
    auto count = GetLines().size();
    if (count <= 2)
        return;

    if (direction < 0)
    {
        auto dir = static_cast<SizeType>(-direction);

        if (GetCursorPositionY() >= dir)
        {
            /* Move cursor left */
            SetCursorPosition(GetCursorPositionX(), GetCursorPositionY() - dir);
        }
        else
        {
            if (cursorLoopEnabled)
            {
                /* Locate cursor to the bottom and move on (subtract all repeated loops) */
                AdjustDir(count, dir);
                SetCursorPosition(GetCursorPositionX(), count - dir - 1);
            }
            else
            {
                /* Locate cursor to the top */
                MoveCursorTop();
            }
        }
    }
    else if (direction > 0)
    {
        auto dir = static_cast<SizeType>(direction);

        if (GetCursorPositionY() + dir <= count)
        {
            /* Move cursor right */
            SetCursorPosition(GetCursorPositionX(), GetCursorPositionY() + dir);
        }
        else
        {
            if (cursorLoopEnabled)
            {
                /* Locate cursor to the top and move on (subtract all repeated loops) */
                AdjustDir(count, dir);
                SetCursorPosition(GetCursorPositionX(), dir);
            }
            else
            {
                /* Locate cursor to the bottom */
                MoveCursorBottom();
            }
        }
    }
}

void TextFieldMultiLineString::MoveCursor(int directionX, int directionY)
{
    /* First move cursor in Y direction to get the new line, then move cursor in X direction */
    MoveCursorY(directionY);
    MoveCursorX(directionX);
}

void TextFieldMultiLineString::MoveCursorBegin()
{
    SetCursorPosition(0, GetCursorPositionY());
}

void TextFieldMultiLineString::MoveCursorEnd()
{
    SetCursorPosition(GetLineText().size(), GetCursorPositionY());
}

void TextFieldMultiLineString::MoveCursorTop()
{
    SetCursorPosition(GetCursorPositionX(), 0);
}

void TextFieldMultiLineString::MoveCursorBottom()
{
    if (!GetLines().empty())
        SetCursorPosition(GetCursorPositionX(), GetLines().size() - 1);
}

void TextFieldMultiLineString::JumpLeft()
{
    if (IsSeparator(CharLeft()))
    {
        while (!IsCursorBegin() && IsSeparator(CharLeft()))
            MoveCursor(-1, 0);
    }
    while (!IsCursorBegin() && !IsSeparator(CharLeft()))
        MoveCursor(-1, 0);
}

void TextFieldMultiLineString::JumpRight()
{
    if (IsSeparator(CharRight()))
    {
        while (!IsCursorEnd() && IsSeparator(CharRight()))
            MoveCursor(1, 0);
    }
    while (!IsCursorEnd() && !IsSeparator(CharRight()))
        MoveCursor(1, 0);
}

/* --- Selection operations --- */

void TextFieldMultiLineString::SetSelection(SizeType start, SizeType end)
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

void TextFieldMultiLineString::GetSelection(SizeType& start, SizeType& end) const
{
    start = GetCursorPosition();
    end = selStart_;
    if (start > end)
        std::swap(start, end);
}

void TextFieldMultiLineString::SelectAll()
{
    SetSelection(0, GetText().size());
}

void TextFieldMultiLineString::Deselect()
{
    selectionEnabled = false;
    SetCursorPosition(GetCursorPosition());
}

bool TextFieldMultiLineString::IsSelected() const
{
    return GetCursorPosition() != selStart_;
}

String TextFieldMultiLineString::GetSelectionText() const
{
    SizeType start, end;
    GetSelection(start, end);
    return (start < end) ? text_.substr(start, end - start) : String();
}

/* --- String content --- */

Char TextFieldMultiLineString::CharLeft() const
{
    return !IsCursorBegin() ? GetLineText()[GetCursorPositionX() - 1] : Char(0);
}

Char TextFieldMultiLineString::CharRight() const
{
    return !IsCursorEnd() ? GetLineText()[GetCursorPositionX()] : Char(0);
}

void TextFieldMultiLineString::RemoveLeft()
{
#if 0
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
#endif
}

void TextFieldMultiLineString::RemoveRight()
{
#if 0
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
#endif
}

void TextFieldMultiLineString::RemoveSequenceLeft()
{
    /* Remove all characters before the cursor, until the next separator appears */
    while (!IsCursorBegin() && IsSeparator(CharLeft()))
        RemoveLeft();
    while (!IsCursorBegin() && !IsSeparator(CharLeft()))
        RemoveLeft();
}

void TextFieldMultiLineString::RemoveSequenceRight()
{
    /* Remove all characters after the cursor, until the next separator appears */
    while (!IsCursorEnd() && IsSeparator(CharRight()))
        RemoveRight();
    while (!IsCursorEnd() && !IsSeparator(CharRight()))
        RemoveRight();
}

void TextFieldMultiLineString::RemoveSelection()
{
#if 0
    /* Get selection range */
    SizeType start, end;
    GetSelection(start, end);

    /* Remove sub string */
    if (start < end)
        text_.erase(start, end - start);

    /* Locate cursor to the selection start */
    selectionEnabled = false;
    SetCursorPosition(start);
#endif
}

bool TextFieldMultiLineString::IsInsertionActive() const
{
    return insertionEnabled && !IsCursorEnd() && !IsSelected();
}

void TextFieldMultiLineString::Insert(const Char& chr)
{
#if 0
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
#endif
}

void TextFieldMultiLineString::Put(const Char& chr)
{
    if (chr == Char('\b'))
        RemoveLeft();
    else if (chr == Char(127))
        RemoveRight();
    else if (unsigned(chr) >= 32)
        Insert(chr);
}

void TextFieldMultiLineString::Put(const String& text)
{
    for (const auto& chr : text)
        Put(chr);
}

void TextFieldMultiLineString::SetText(const String& text)
{
    MultiLineString::SetText(text);
    UpdateCursorRange();
}

const String& TextFieldMultiLineString::GetLineText() const
{
    static const String dummyString;
    if (GetCursorPositionY() < GetLines().size())
        return GetLines()[GetCursorPositionY()].text;
    return dummyString;
}


/*
 * ======= Protected: =======
 */

bool TextFieldMultiLineString::IsValidChar(const Char& chr) const
{
    return unsigned(chr) >= 32;
}


/*
 * ======= Private: =======
 */

#if 0
String::iterator TextFieldMultiLineString::Iter()
{
    return text_.begin() + GetCursorPosition();
}

String::const_iterator TextFieldMultiLineString::Iter() const
{
    return text_.begin() + GetCursorPosition();
}
#endif

TextFieldMultiLineString::SizeType TextFieldMultiLineString::ClampedPosX(SizeType pos) const
{
    return std::min(pos, GetText().size());
}

TextFieldMultiLineString::SizeType TextFieldMultiLineString::ClampedPosY(SizeType pos) const
{
    return (GetLines().empty() ? 0 : std::min(pos, GetLines().size() - 1));
}

void TextFieldMultiLineString::UpdateCursorRange()
{
    cursorPosX_ = ClampedPosX(GetCursorPositionX());
    cursorPosY_ = ClampedPosY(GetCursorPositionY());
    selStartX_ = ClampedPosX(selStartX_);
    selStartY_ = ClampedPosY(selStartY_);
}


} // /namespace Tg



// ================================================================================
