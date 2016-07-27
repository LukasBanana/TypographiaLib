/*
 * TextFieldMultiLineString.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/TextFieldMultiLineString.h>
#include <algorithm>

#include <iostream>//!!!


namespace Tg
{


void TextFieldMultiLineString::_TEST_()
{
    auto p = GetCursorCoordinate();
    std::cout << std::endl;
    std::cout << "index = " << GetCursorIndex() << ", x = " << p.x << ", y = " << p.y << std::endl;
    std::cout << "left = '" << CharLeft() << "', right = '" << CharRight() << '\'' << std::endl;
}

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

void TextFieldMultiLineString::SetCursorCoordinate(Point position)
{
    if (!GetLines().empty())
    {
        position.y = std::min(position.y, GetLines().size() - 1);
        position.x = std::min(position.x, GetLineText(position.y).size());
        SetCursorIndex(GetTextIndex(position.y, position.x));
    }
    else
        SetCursorIndex(0);
}

void TextFieldMultiLineString::SetCursorIndex(SizeType index)
{
    /* First clamp Y position to the range [0, lines), then clamp X position to the range [0, length] */
    cursorPos_ = ClampedPos(index);

    /* If selection is disabled, also set selection start */
    if (!selectionEnabled)
        selStart_ = cursorPos_;
}

TextFieldMultiLineString::Point TextFieldMultiLineString::GetCursorCoordinate() const
{
    Point pos;
    GetTextPosition(GetCursorIndex(), pos.y, pos.x);
    return pos;
}

bool TextFieldMultiLineString::IsCursorBegin() const
{
    return (GetCursorCoordinate().x == 0);
}

bool TextFieldMultiLineString::IsCursorEnd() const
{
    return (GetCursorCoordinate().x == GetLineText().size());
}

bool TextFieldMultiLineString::IsCursorTop() const
{
    return (GetLines().empty() || GetCursorCoordinate().y == 0);
}

bool TextFieldMultiLineString::IsCursorBottom() const
{
    return (GetLines().empty() || GetCursorCoordinate().y + 1 == GetLines().size());
}

void TextFieldMultiLineString::MoveCursorX(int direction)
{
    /* Get line size and quit if moving the cursor is not possible */
    auto size = GetLineText().size();

    if (direction < 0)
    {
        auto dir = static_cast<SizeType>(-direction);
        SetCursorIndex(GetCursorIndex() - std::min(dir, GetCursorIndex()));
    }
    else if (direction > 0)
    {
        auto dir = static_cast<SizeType>(direction);
        SetCursorIndex(std::min(GetText().size(), GetCursorIndex() + dir));
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

        if (GetCursorCoordinate().y >= dir)
        {
            /* Move cursor left */
            SetCursorCoordinate(GetCursorCoordinate().x, GetCursorCoordinate().y - dir);
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

        if (GetCursorCoordinate().y + dir <= count)
        {
            /* Move cursor right */
            SetCursorCoordinate(GetCursorCoordinate().x, GetCursorCoordinate().y + dir);
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
    //TODO -> move within complete line (terminated by an explicit new line character)
    SetCursorCoordinate(0, GetCursorCoordinate().y);
}

void TextFieldMultiLineString::MoveCursorEnd()
{
    //TODO -> move within complete line (terminated by an explicit new line character)
    SetCursorCoordinate(GetLineText().size(), GetCursorCoordinate().y);
}

void TextFieldMultiLineString::MoveCursorTop()
{
    SetCursorCoordinate(GetCursorCoordinate().x, 0);
}

void TextFieldMultiLineString::MoveCursorBottom()
{
    if (!GetLines().empty())
        SetCursorCoordinate(GetCursorCoordinate().x, GetLines().size() - 1);
}

void TextFieldMultiLineString::JumpLeft()
{
    /* Move left to first non-separator character, then move left to the last non-separator character */
    while ( ( !IsCursorTop() || !IsCursorBegin() ) && IsSeparator(CharLeft()) )
        MoveCursorX(-1);
    while ( ( !IsCursorTop() || !IsCursorBegin() ) && !IsSeparator(CharLeft()) )
        MoveCursorX(-1);
}

void TextFieldMultiLineString::JumpRight()
{
    /* Move right to first non-separator character, then move right to the last non-separator character */
    while ( ( !IsCursorBottom() || !IsCursorEnd() ) && IsSeparator(CharRight()) )
        MoveCursorX(1);
    while ( ( !IsCursorBottom() || !IsCursorEnd() ) && !IsSeparator(CharRight()) )
        MoveCursorX(1);
}

void TextFieldMultiLineString::JumpUp()
{
    /* Move up to the first non-empty line, then move up to the last non-empty line */
    while (!IsCursorTop() && IsUpperLineEmpty())
        MoveCursorY(-1);
    while (!IsCursorTop() && !IsUpperLineEmpty())
        MoveCursorY(-1);
}

void TextFieldMultiLineString::JumpDown()
{
    /* Move down to the first non-empty line, then move down to the last non-empty line */
    while (!IsCursorBottom() && IsLowerLineEmpty())
        MoveCursorY(1);
    while (!IsCursorBottom() && !IsLowerLineEmpty())
        MoveCursorY(1);
}

/* --- Selection operations --- */

void TextFieldMultiLineString::SetSelection(const Point& start, const Point& end)
{
    /* Store (and later reset) current selection state */
    auto selEnabled = selectionEnabled;
    {
        /* Set selection start */
        selectionEnabled = false;
        SetCursorCoordinate(start);

        /* Set selection end */
        selectionEnabled = true;
        SetCursorCoordinate(end);
    }
    selectionEnabled = selEnabled;
}

void TextFieldMultiLineString::GetSelection(Point& start, Point& end) const
{
    start = GetCursorCoordinate();
    GetTextPosition(selStart_, end.y, end.x);

    if (GetCursorIndex() > selStart_)
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
    SetCursorIndex(GetCursorIndex());
}

bool TextFieldMultiLineString::IsSelected() const
{
    return (GetCursorIndex() != selStart_);
}

String TextFieldMultiLineString::GetSelectionText() const
{
    String text;

    if (IsSelected())
    {
        Point start, end;
        GetSelection(start, end);

        auto startPos = GetTextIndex(start.y, start.x);
        auto endPos = GetTextIndex(end.y, end.x);

        return GetText().substr(startPos, endPos - startPos);
    }

    return text;
}

/* --- String content --- */

Char TextFieldMultiLineString::CharLeft() const
{
    return (GetCursorIndex() > 0 ? GetText()[GetCursorIndex() - 1] : Char(0));
}

Char TextFieldMultiLineString::CharRight() const
{
    return (GetCursorIndex() < GetText().size() ? GetText()[GetCursorIndex()] : Char(0));
}

void TextFieldMultiLineString::RemoveLeft()
{
    if (IsSelected())
    {
        /* First remove selection */
        RemoveSelection();
    }
    else if (!IsCursorTop() || !IsCursorBegin())
    {
        /* Move cursor left and then remove character */
        MoveCursorX(-1);
        auto selState = GetSelectionState();
        MultiLineString::Remove(GetCursorCoordinate().y, GetCursorCoordinate().x);
        SetSelectionState(selState);
    }
}

void TextFieldMultiLineString::RemoveRight()
{
    if (IsSelected())
    {
        /* First remove selection */
        RemoveSelection();
    }
    else if (!IsCursorBottom() || !IsCursorEnd())
    {
        /* Only remove character without moving the cursor */
        auto selState = GetSelectionState();
        MultiLineString::Remove(GetCursorCoordinate().y, GetCursorCoordinate().x);
        SetSelectionState(selState);
    }
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
    /* Remove characters from the start position */
    if (IsSelected())
    {
        /* Get selection range */
        Point start, end;
        GetSelection(start, end);

        /* Locate cursor to the selection start */
        selectionEnabled = false;
        SetCursorCoordinate(start);

        auto selState = GetSelectionState();

        /* Remove the selected amount of characters from the start position */
        auto startPos = GetTextIndex(start.y, start.x);
        auto endPos = GetTextIndex(end.y, end.x);

        for (; startPos < endPos; ++startPos)
            MultiLineString::Remove(start.y, start.x);

        SetSelectionState(selState);
    }
}

bool TextFieldMultiLineString::IsInsertionActive() const
{
    return insertionEnabled && !IsCursorEnd() && !IsSelected();
}

void TextFieldMultiLineString::Insert(const Char& chr)
{
    if (IsValidChar(chr))
    {
        /* Replace selection by character */
        auto isSel = IsSelected();
        if (isSel)
            RemoveSelection();

        /* Insert the new character (only use insertion if selection was not replaced) */
        auto selState = GetSelectionState();
        MultiLineString::Insert(GetCursorCoordinate().y, GetCursorCoordinate().x, chr, (insertionEnabled && !isSel));
        SetSelectionState(selState);

        /* Move cursor position */
        MoveCursorX(1);
    }
}

void TextFieldMultiLineString::Put(const Char& chr)
{
    if (chr == Char('\b'))
        RemoveLeft();
    else if (chr == Char(127))
        RemoveRight();
    else
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

void TextFieldMultiLineString::SetMaxWidth(int maxWidth)
{
    if (GetMaxWidth() != maxWidth)
    {
        auto selState = GetSelectionState();
        MultiLineString::SetMaxWidth(maxWidth);
        SetSelectionState(selState);
    }
}

const String& TextFieldMultiLineString::GetLineText() const
{
    return GetLineText(GetCursorCoordinate().y);
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
    return (unsigned(chr) >= 32 || chr == '\r' || chr == '\n');
}


/*
 * ======= Private: =======
 */

TextFieldMultiLineString::SizeType TextFieldMultiLineString::ClampedPos(SizeType pos) const
{
    return std::min(pos, GetText().size());
}

void TextFieldMultiLineString::UpdateCursorRange()
{
    cursorPos_ = ClampedPos(GetCursorIndex());
    selStart_ = ClampedPos(selStart_);
}

TextFieldMultiLineString::SelectionState TextFieldMultiLineString::GetSelectionState() const
{
    SelectionState state;

    Point start, end;
    GetSelection(start, end);

    state.startPos = GetTextIndex(start.y, start.x);
    state.endPos = GetTextIndex(end.y, end.x);

    return state;
}

void TextFieldMultiLineString::SetSelectionState(const SelectionState& state)
{
    Point start, end;
    
    GetTextPosition(state.startPos, start.y, start.x);
    GetTextPosition(state.endPos, end.y, end.x);

    SetSelection(start, end);
}

bool TextFieldMultiLineString::IsUpperLineEmpty() const
{
    return GetLines()[GetCursorCoordinate().y - 1].text.empty();
}

bool TextFieldMultiLineString::IsLowerLineEmpty() const
{
    return GetLines()[GetCursorCoordinate().y + 1].text.empty();
}


} // /namespace Tg



// ================================================================================
