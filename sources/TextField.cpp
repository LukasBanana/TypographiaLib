/*
 * TextField.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/TextField.h>
#include <algorithm>


namespace Tg
{


TextField::~TextField()
{
}

/* --- Cursor operations --- */

void TextField::SetCursorPosition(SizeType position)
{
    /* Clamp position to the range [0, GetText().size()] */
    cursorPos_ = ClampedPos(position);

    /* If selection is disabled, also set selection start */
    if (!selectionEnabled)
        selStart_ = cursorPos_;
}

bool TextField::IsCursorBegin() const
{
    return (GetCursorPosition() == 0);
}

bool TextField::IsCursorEnd() const
{
    return (GetCursorPosition() == GetText().size());
}

void TextField::JumpLeft()
{
    /* Move left to first non-separator character, then move left to the last non-separator character */
    while (!IsCursorBegin() && IsSeparator(CharLeft()))
        MoveCursor(-1);
    while (!IsCursorBegin() && !IsSeparator(CharLeft()))
        MoveCursor(-1);
}

void TextField::JumpRight()
{
    /* Move right to first non-separator character, then move right to the last non-separator character */
    while (!IsCursorEnd() && IsSeparator(CharRight()))
        MoveCursor(1);
    while (!IsCursorEnd() && !IsSeparator(CharRight()))
        MoveCursor(1);
}

/* --- Selection --- */

void TextField::SetSelection(SizeType start, SizeType end)
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

void TextField::GetSelection(SizeType& start, SizeType& end) const
{
    start = GetCursorPosition();
    end = selStart_;
    if (start > end)
        std::swap(start, end);
}

void TextField::SelectAll()
{
    SetSelection(0, GetText().size());
}

void TextField::Deselect()
{
    selectionEnabled = false;
    SetCursorPosition(GetCursorPosition());
}

bool TextField::IsSelected() const
{
    return (GetCursorPosition() != selStart_);
}

bool TextField::IsAllSelected() const
{
    SizeType start, end;
    GetSelection(start, end);
    return (start == 0 && end == GetText().size());
}

String TextField::GetSelectionText() const
{
    SizeType start, end;
    GetSelection(start, end);
    return (start < end ? GetText().substr(start, end - start) : String());
}

/* --- String content --- */

void TextField::RemoveSequenceLeft()
{
    /* Remove all characters before the cursor, until the next separator appears */
    while (!IsCursorBegin() && IsSeparator(CharLeft()))
        RemoveLeft();
    while (!IsCursorBegin() && !IsSeparator(CharLeft()))
        RemoveLeft();
}

void TextField::RemoveSequenceRight()
{
    /* Remove all characters after the cursor, until the next separator appears */
    while (!IsCursorEnd() && IsSeparator(CharRight()))
        RemoveRight();
    while (!IsCursorEnd() && !IsSeparator(CharRight()))
        RemoveRight();
}

bool TextField::IsInsertionActive() const
{
    return insertionEnabled && !IsCursorEnd() && !IsSelected();
}

void TextField::Put(Char chr)
{
    if (chr == Char('\b'))
        RemoveLeft();
    else if (chr == Char(127))
        RemoveRight();
    else
        Insert(chr);
}

void TextField::Put(const String& text)
{
    for (const auto& chr : text)
        Put(chr);
}

bool TextField::IsSeparator(Char chr) const
{
    return !((chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z'));
}


/*
 * ======= Private: =======
 */

TextField::SizeType TextField::ClampedPos(SizeType pos) const
{
    return std::min(pos, GetText().size());
}

void TextField::UpdateCursorRange()
{
    cursorPos_ = ClampedPos(cursorPos_);
    selStart_ = ClampedPos(selStart_);
}


} // /namespace Tg



// ================================================================================
