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

void TextFieldMultiLineString::SetCursorPosition(const Point& position)
{
    /* First clamp Y position to the range [0, lines), then clamp X position to the range [0, length] */
    cursorPos_ = ClampedPos(position);

    /* If selection is disabled, also set selection start */
    if (!selectionEnabled)
        selStart_ = cursorPos_;
}

bool TextFieldMultiLineString::IsCursorBegin() const
{
    return (GetCursorPosition().x == 0);
}

bool TextFieldMultiLineString::IsCursorEnd() const
{
    return (GetCursorPosition().x == GetLineText().size());
}

bool TextFieldMultiLineString::IsCursorTop() const
{
    return (GetCursorPosition().y == 0);
}

bool TextFieldMultiLineString::IsCursorBottom() const
{
    return (GetLines().empty() || GetCursorPosition().y == GetLines().size() - 1);
}

void TextFieldMultiLineString::MoveCursorX(int direction)
{
    /* Get line size and quit if moving the cursor is not possible */
    auto size = GetLineText().size();

    if (direction < 0)
    {
        auto dir = static_cast<SizeType>(-direction);

        if (GetCursorPosition().x >= dir)
        {
            /* Move cursor left */
            SetCursorPosition(GetCursorPosition().x - dir, GetCursorPosition().y);
        }
        else if (GetCursorPosition().y > 0)
        {
            /* Locate cursor to the end and move on in the previous line */
            direction += static_cast<int>(GetCursorPosition().x);
            MoveCursorY(-1);
            MoveCursorEnd();
            MoveCursorX(direction + 1);
        }
        else
            MoveCursorBegin();
    }
    else if (direction > 0)
    {
        auto dir = static_cast<SizeType>(direction);

        if (GetCursorPosition().x + dir <= size)
        {
            /* Move cursor right */
            SetCursorPosition(GetCursorPosition().x + dir, GetCursorPosition().y);
        }
        else if (GetCursorPosition().y + 1 < GetLines().size())
        {
            /* Locate cursor to the beginning and move on in the next line */
            direction -= static_cast<int>(size - GetCursorPosition().x);
            MoveCursorBegin();
            MoveCursorY(1);
            MoveCursorX(direction - 1);
        }
        else
            MoveCursorEnd();
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

        if (GetCursorPosition().y >= dir)
        {
            /* Move cursor left */
            SetCursorPosition(GetCursorPosition().x, GetCursorPosition().y - dir);
        }
        else
        {
            /* Locate cursor to the top */
            MoveCursorTop();
        }
    }
    else if (direction > 0)
    {
        auto dir = static_cast<SizeType>(direction);

        if (GetCursorPosition().y + dir <= count)
        {
            /* Move cursor right */
            SetCursorPosition(GetCursorPosition().x, GetCursorPosition().y + dir);
        }
        else
        {
            /* Locate cursor to the bottom */
            MoveCursorBottom();
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
    SetCursorPosition(0, GetCursorPosition().y);
}

void TextFieldMultiLineString::MoveCursorEnd()
{
    SetCursorPosition(GetLineText().size(), GetCursorPosition().y);
}

void TextFieldMultiLineString::MoveCursorTop()
{
    SetCursorPosition(GetCursorPosition().x, 0);
}

void TextFieldMultiLineString::MoveCursorBottom()
{
    if (!GetLines().empty())
        SetCursorPosition(GetCursorPosition().x, GetLines().size() - 1);
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

void TextFieldMultiLineString::SetSelection(const Point& start, const Point& end)
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

void TextFieldMultiLineString::GetSelection(Point& start, Point& end) const
{
    start = GetCursorPosition();
    end = selStart_;

    if ( start.y > end.y || ( start.y == end.y && start.x > end.x ) )
        std::swap(start, end);
}

void TextFieldMultiLineString::SelectAll()
{
    if (!GetLines().empty())
    {
        SetSelection(
            Point(0, 0),
            Point(GetLines().back().text.size(), GetLines().size() - 1)
        );
    }
}

void TextFieldMultiLineString::Deselect()
{
    selectionEnabled = false;
    SetCursorPosition(GetCursorPosition());
}

bool TextFieldMultiLineString::IsSelected() const
{
    return (GetCursorPosition().x != selStart_.x || GetCursorPosition().y != selStart_.y);
}

String TextFieldMultiLineString::GetSelectionText() const
{
    /*SizeType start, end;
    GetSelection(start, end);
    return (start < end) ? text_.substr(start, end - start) : String();*/

    String text;


    return text;
}

/* --- String content --- */

Char TextFieldMultiLineString::CharLeft() const
{
    return !IsCursorBegin() ? GetLineText()[GetCursorPosition().x - 1] : Char(0);
}

Char TextFieldMultiLineString::CharRight() const
{
    return !IsCursorEnd() ? GetLineText()[GetCursorPosition().x] : Char(0);
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
    return GetLineText(GetCursorPosition().y);
}

const String& TextFieldMultiLineString::GetLineText(std::size_t lineIndex) const
{
    static const String dummyString;
    if (lineIndex < GetLines().size())
        return GetLines()[lineIndex].text;
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

TextFieldMultiLineString::Point TextFieldMultiLineString::ClampedPos(Point pos) const
{
    if (!GetLines().empty())
    {
        pos.y = (GetLines().empty() ? 0 : std::min(pos.y, GetLines().size() - 1));
        pos.x = std::min(pos.x, GetLines()[pos.y].text.size());
        return pos;
    }
    return Point();
}

void TextFieldMultiLineString::UpdateCursorRange()
{
    cursorPos_ = ClampedPos(GetCursorPosition());
    selStart_ = ClampedPos(selStart_);
}


} // /namespace Tg



// ================================================================================
