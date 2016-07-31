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
    text_( glyphSet, maxWidth, text )
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

TextFieldMultiLineString& TextFieldMultiLineString::operator += (Char chr)
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
        SetCursorPosition(text_.GetTextIndex(position.y, position.x));
    }
    else
        SetCursorPosition(0);
}

void TextFieldMultiLineString::SetCursorPosition(SizeType position)
{
    /* Clmap position to the rnage [0, GetText().size()] */
    cursorPos_ = ClampedPos(position);

    /* If selection is disabled, also set selection start */
    if (!selectionEnabled)
        selStart_ = cursorPos_;
}

Point TextFieldMultiLineString::GetCursorCoordinate() const
{
    Point pos;
    text_.GetTextPosition(GetCursorPosition(), pos.y, pos.x);
    return pos;
}

bool TextFieldMultiLineString::IsCursorBegin() const
{
    return (GetCursorPosition() == 0);
}

bool TextFieldMultiLineString::IsCursorEnd() const
{
    return (GetCursorPosition() == GetText().size());
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
        SetCursorPosition(GetCursorPosition() - std::min(dir, GetCursorPosition()));
    }
    else if (direction > 0)
    {
        auto dir = static_cast<SizeType>(direction);
        SetCursorPosition(std::min(GetText().size(), GetCursorPosition() + dir));
    }

    StoreCursorCoordX();
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

    RestoreCursorCoordX();
}

void TextFieldMultiLineString::MoveCursor(int directionX, int directionY)
{
    /* First move cursor in Y direction to get the new line, then move cursor in X direction */
    MoveCursorY(directionY);
    MoveCursorX(directionX);
}

//!INCOMPLETE! (due to trunaced spaces at an implicit line break)
void TextFieldMultiLineString::MoveCursorBegin()
{
    if (wrapLines)
    {
        /* Move cursor left until the left sided character is a new-line character */
        while (!IsCursorBegin())
        {
            SetCursorCoordinate(0, GetCursorCoordinate().y);
            if (!text_.IsNewLine(CharLeft()))
                MoveCursorX(-1);
            else
                break;
        }
    }
    else
        SetCursorCoordinate(0, GetCursorCoordinate().y);
}

//!INCOMPLETE! (due to trunaced spaces at an implicit line break)
void TextFieldMultiLineString::MoveCursorEnd()
{
    if (wrapLines)
    {
        /* Move cursor right until the right sided character is a new-line character */
        while (!IsCursorEnd())
        {
            SetCursorCoordinate(GetLineText().size(), GetCursorCoordinate().y);
            if (!text_.IsNewLine(CharRight()))
                MoveCursorX(1);
            else
                break;
        }
    }
    else
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
    while (!IsCursorBegin() && IsSeparator(CharLeft()))
        MoveCursorX(-1);
    while (!IsCursorBegin() && !IsSeparator(CharLeft()))
        MoveCursorX(-1);
}

void TextFieldMultiLineString::JumpRight()
{
    /* Move right to first non-separator character, then move right to the last non-separator character */
    while (!IsCursorEnd() && IsSeparator(CharRight()))
        MoveCursorX(1);
    while (!IsCursorEnd() && !IsSeparator(CharRight()))
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
    text_.GetTextPosition(selStart_, end.y, end.x);

    if (GetCursorPosition() > selStart_)
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
    return (GetCursorPosition() != selStart_);
}

bool TextFieldMultiLineString::IsAllSelected() const
{
    auto start = GetCursorPosition();
    auto end = selStart_;
    if (start > end)
        std::swap(start, end);
    return (start == 0 && end == GetText().size());
}

String TextFieldMultiLineString::GetSelectionText() const
{
    String text;

    if (IsSelected())
    {
        Point start, end;
        GetSelection(start, end);

        auto startPos = text_.GetTextIndex(start.y, start.x);
        auto endPos = text_.GetTextIndex(end.y, end.x);

        text = GetText().substr(startPos, endPos - startPos);
    }

    return text;
}

/* --- String content --- */

Char TextFieldMultiLineString::CharLeft() const
{
    return (!IsCursorBegin() ? GetText()[GetCursorPosition() - 1] : Char(0));
}

Char TextFieldMultiLineString::CharRight() const
{
    return (!IsCursorEnd() ? GetText()[GetCursorPosition()] : Char(0));
}

void TextFieldMultiLineString::RemoveLeft()
{
    if (IsSelected())
    {
        /* First remove selection */
        RemoveSelection();
    }
    else if (!IsCursorBegin())
    {
        /* Move cursor left and then remove character */
        MoveCursorX(-1);
        auto cursorCoord = GetCursorCoordinate();
        text_.Remove(cursorCoord.y, cursorCoord.x);
    }
}

void TextFieldMultiLineString::RemoveRight()
{
    if (IsSelected())
    {
        /* First remove selection */
        RemoveSelection();
    }
    else if (!IsCursorEnd())
    {
        /* Only remove character without moving the cursor */
        auto cursorCoord = GetCursorCoordinate();
        text_.Remove(cursorCoord.y, cursorCoord.x);
    }
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

        /* Remove the selected amount of characters from the start position */
        auto startPos = text_.GetTextIndex(start.y, start.x);
        auto endPos = text_.GetTextIndex(end.y, end.x);

        for (; startPos < endPos; ++startPos)
            text_.Remove(start.y, start.x);
    }
}

void TextFieldMultiLineString::Insert(Char chr)
{
    if (IsValidChar(chr))
    {
        /* Replace selection by character */
        auto isSel = IsSelected();
        if (isSel)
            RemoveSelection();

        /* Replace '\r' by '\n' */
        if (chr == '\r')
            chr = '\n';

        /* Insert the new character (only use insertion if selection was not replaced) */
        text_.Insert(GetCursorCoordinate().y, GetCursorCoordinate().x, chr, (insertionEnabled && !isSel));

        /* Move cursor position */
        MoveCursorX(1);
    }
}

bool TextFieldMultiLineString::IsValidChar(Char chr) const
{
    return (unsigned(chr) >= 32 || chr == '\r' || chr == '\n');
}

void TextFieldMultiLineString::SetText(const String& text)
{
    text_.SetText(text);
    UpdateCursorRange();
}

const String& TextFieldMultiLineString::GetText() const
{
    return text_.GetText();
}

void TextFieldMultiLineString::SetMaxWidth(int maxWidth)
{
    if (GetMaxWidth() != maxWidth)
    {
        text_.SetMaxWidth(maxWidth);
        StoreCursorCoordX();
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
 * ======= Private: =======
 */

TextFieldMultiLineString::SizeType TextFieldMultiLineString::ClampedPos(SizeType pos) const
{
    return std::min(pos, GetText().size());
}

void TextFieldMultiLineString::UpdateCursorRange()
{
    cursorPos_ = ClampedPos(cursorPos_);
    selStart_ = ClampedPos(selStart_);
}

bool TextFieldMultiLineString::IsUpperLineEmpty() const
{
    return GetLines()[GetCursorCoordinate().y - 1].text.empty();
}

bool TextFieldMultiLineString::IsLowerLineEmpty() const
{
    return GetLines()[GetCursorCoordinate().y + 1].text.empty();
}

void TextFieldMultiLineString::StoreCursorCoordX()
{
    storedCursorCoordX_ = GetCursorCoordinate().x;
}

void TextFieldMultiLineString::RestoreCursorCoordX()
{
    SetCursorCoordinate(storedCursorCoordX_, GetCursorCoordinate().y);
}


} // /namespace Tg



// ================================================================================
