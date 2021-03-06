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
    text_ { glyphSet, maxWidth, text }
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

/* --- Text position conversion --- */

TextFieldMultiLineString::SizeType TextFieldMultiLineString::GetTextIndex(const Point& position) const
{
    return text_.GetTextIndex(position.y, position.x);
}

Point TextFieldMultiLineString::GetTextPosition(SizeType index) const
{
    SizeType lineIndex = 0, positionInLine = 0;
    text_.GetTextPosition(index, lineIndex, positionInLine);
    return Point(positionInLine, lineIndex);
}

TextFieldMultiLineString::SizeType TextFieldMultiLineString::GetXPositionFromCoordinate(SizeType coordinateX, std::size_t lineIndex) const
{
    if (lineIndex < GetLines().size())
    {
        /* Iterate over line text to find suitable X coordinate by the text width */
        const auto& text = GetLineText(lineIndex);

        SizeType pos = 0;

        for (auto width = static_cast<long long>(coordinateX); pos < text.size(); ++pos)
        {
            /* Reduce width to zero, to find the suitable */
            auto prevWidth = width;
            width -= GetGlyphSet()[text[pos]].advance;

            if (width <= 0)
            {
                if (prevWidth > -width)
                    ++pos;
                break;
            }
        }

        return pos;
    }
    return 0;
}

TextFieldMultiLineString::SizeType TextFieldMultiLineString::GetXCoordinateFromPosition(SizeType positionX, std::size_t lineIndex) const
{
    if (lineIndex < GetLines().size())
    {
        /* Return text width of the specified line to the X position */
        return GetGlyphSet().TextWidth(GetLineText(lineIndex), 0, positionX);
    }
    return 0;
}

/* --- Cursor operations --- */

void TextFieldMultiLineString::SetCursorCoordinate(Point position)
{
    if (!GetLines().empty())
    {
        position.y = std::min(position.y, GetLines().size() - 1);
        position.x = std::min(position.x, GetLineText(position.y).size());
        SetCursorPosition(GetTextIndex(position));
    }
    else
        SetCursorPosition(0);
}

Point TextFieldMultiLineString::GetCursorCoordinate() const
{
    return GetTextPosition(GetCursorPosition());
}

bool TextFieldMultiLineString::IsCursorTop() const
{
    return (GetLines().empty() || GetCursorCoordinate().y == 0);
}

bool TextFieldMultiLineString::IsCursorBottom() const
{
    return (GetLines().empty() || GetCursorCoordinate().y + 1 == GetLines().size());
}

void TextFieldMultiLineString::MoveCursor(int direction)
{
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

void TextFieldMultiLineString::MoveCursorLine(int direction)
{
    /* Get number of lines and quit if moving cursor is not possible */
    auto count = GetLines().size();
    if (count < 2)
        return;

    if (direction < 0)
    {
        auto dir = static_cast<SizeType>(-direction);

        if (GetCursorCoordinate().y >= dir)
        {
            /* Move cursor up */
            RestoreCursorCoordX(GetCursorCoordinate().y - dir);
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
            /* Move cursor down */
            RestoreCursorCoordX(GetCursorCoordinate().y + dir);
        }
        else
        {
            /* Locate cursor to the bottom */
            MoveCursorBottom();
        }
    }
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
                MoveCursor(-1);
            else
                break;
        }
    }
    else
        SetCursorCoordinate(0, GetCursorCoordinate().y);

    StoreCursorCoordX();
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
                MoveCursor(1);
            else
                break;
        }
    }
    else
        SetCursorCoordinate(GetLineText().size(), GetCursorCoordinate().y);

    StoreCursorCoordX();
}

void TextFieldMultiLineString::MoveCursorTop()
{
    RestoreCursorCoordX(0);
}

void TextFieldMultiLineString::MoveCursorBottom()
{
    if (!GetLines().empty())
        RestoreCursorCoordX(GetLines().size() - 1);
}

void TextFieldMultiLineString::JumpUp()
{
    /* Move up to the first non-empty line, then move up to the last non-empty line */
    while (!IsCursorTop() && IsUpperLineEmpty())
        MoveCursorLine(-1);
    while (!IsCursorTop() && !IsUpperLineEmpty())
        MoveCursorLine(-1);
}

void TextFieldMultiLineString::JumpDown()
{
    /* Move down to the first non-empty line, then move down to the last non-empty line */
    while (!IsCursorBottom() && IsLowerLineEmpty())
        MoveCursorLine(1);
    while (!IsCursorBottom() && !IsLowerLineEmpty())
        MoveCursorLine(1);
}

/* --- Selection operations --- */

void TextFieldMultiLineString::SetSelectionCoordinate(const Point& start, const Point& end)
{
    SetSelection(GetTextIndex(start), GetTextIndex(end));
}

void TextFieldMultiLineString::GetSelectionCoordinate(Point& start, Point& end) const
{
    SizeType startPos = 0, endPos = 0;
    GetSelection(startPos, endPos);

    start = GetTextPosition(startPos);
    end = GetTextPosition(endPos);
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
        MoveCursor(-1);
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
        SizeType start, end;
        GetSelection(start, end);

        /* Locate cursor to the selection start */
        selectionEnabled = false;
        SetCursorPosition(start);

        /* Remove the selected amount of characters from the start position */
        auto erasePos = GetTextPosition(start);

        for (; start < end; ++start)
            text_.Remove(erasePos.y, erasePos.x);
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

const MultiLineString::TextLine& TextFieldMultiLineString::GetLine() const
{
    return GetLine(GetCursorCoordinate().y);
}

const MultiLineString::TextLine& TextFieldMultiLineString::GetLine(std::size_t lineIndex) const
{
    static const MultiLineString::TextLine dummyLine;
    if (lineIndex < GetLines().size())
        return GetLines()[lineIndex];
    return dummyLine;
}

const String& TextFieldMultiLineString::GetLineText() const
{
    return GetLine().text;
}

const String& TextFieldMultiLineString::GetLineText(std::size_t lineIndex) const
{
    return GetLine(lineIndex).text;
}


/*
 * ======= Private: =======
 */

void TextFieldMultiLineString::InsertChar(Char chr, bool wasSelected)
{
    /* Replace '\r' by '\n' */
    if (chr == '\r')
        chr = '\n';

    /* Insert the new character (only use insertion if selection was not replaced) */
    auto coord = GetCursorCoordinate();
    text_.Insert(coord.y, coord.x, chr, (insertionEnabled && !wasSelected));
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
    auto cursorCoord = GetCursorCoordinate();
    storedCursorCoordX_ = GetXCoordinateFromPosition(cursorCoord.x, cursorCoord.y);
}

void TextFieldMultiLineString::RestoreCursorCoordX(SizeType lineIndex)
{
    SetCursorCoordinate(GetXPositionFromCoordinate(storedCursorCoordX_, lineIndex), lineIndex);
}


} // /namespace Tg



// ================================================================================
