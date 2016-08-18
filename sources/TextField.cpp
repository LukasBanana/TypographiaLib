/*
 * TextField.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/TextField.h>
#include <algorithm>
#include <iterator>


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

void TextField::Insert(Char chr)
{
    if (IsValidChar(chr))
    {
        /* Replace selection by character */
        auto wasSelected = IsSelected();
        if (wasSelected)
            RemoveSelection();

        /* Insert actual character */
        InsertChar(chr, wasSelected);

        /* Move cursor position */
        MoveCursor(1);

        /* Store memento state */
        StoreMementoForChar(chr);
    }
}

void TextField::Put(Char chr)
{
    /* Disable selection for adding more characters */
    const auto prevSel = selectionEnabled;
    selectionEnabled = false;
    {
        if (chr == Char('\b'))
            RemoveLeft();
        else if (chr == Char(127))
            RemoveRight();
        else
            Insert(chr);
    }
    selectionEnabled = prevSel;
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

/* --- Memento --- */

void TextField::StoreSelection()
{
    selectionStates_.push({ GetCursorPosition(), selStart_ });
}

void TextField::RestoreSelection()
{
    if (!selectionStates_.empty())
    {
        const auto& state = selectionStates_.top();
        SetCursorPosition(state.cursorPos);
        SetSelection(state.selStart, state.cursorPos);
    }
}

void TextField::SetMementoSize(std::size_t size)
{
    if (mementoSize_ != size)
    {
        mementoSize_ = size;
        while (mementoStates_.size() > size)
        {
            --mementoStatesIndex_;
            mementoStates_.pop_front();
        }
    }
}

void TextField::StoreMemento()
{
    if (mementoSize_ > 0)
    {
        /* Remove all memento states after the current state index  */
        if (CanRedo())
            mementoStates_.erase(GetMementoStateIter(mementoStatesIndex_), mementoStates_.end());

        /* Remove older states */
        if (mementoStates_.size() == mementoSize_)
            mementoStates_.pop_front();

        /* Store new memento state */
        mementoStates_.push_back({ GetCursorPosition(), GetText() });

        /* Set new memento state index to the last element, and reset expired flag */
        mementoStatesIndex_ = mementoStates_.size() - 1;
        mementoExpired_     = false;
    }
}

void TextField::Undo()
{
    if (CanUndo())
    {
        if (mementoExpired_)
            StoreMemento();
        RestoreMemento(mementoStatesIndex_ - 1);
    }
}

void TextField::Redo()
{
    if (CanRedo())
        RestoreMemento(mementoStatesIndex_ + 1);
}

bool TextField::CanUndo() const
{
    return (mementoStatesIndex_ > 0);
}

bool TextField::CanRedo() const
{
    return (mementoStatesIndex_ + 1 < mementoStates_.size());
}


/*
 * ======= Protected: =======
 */

void TextField::UpdateCursorRange()
{
    cursorPos_ = ClampedPos(cursorPos_);
    selStart_ = ClampedPos(selStart_);
}


/*
 * ======= Private: =======
 */

TextField::SizeType TextField::ClampedPos(SizeType pos) const
{
    return std::min(pos, GetText().size());
}

void TextField::RestoreMemento(std::size_t index)
{
    if (index < mementoStates_.size())
    {
        auto it = GetMementoStateIter(index);

        SetText(it->text);
        SetCursorPosition(it->cursorPos);

        mementoStatesIndex_ = index;
    }
}

TextField::MementoStateList::const_iterator TextField::GetMementoStateIter(std::size_t index) const
{
    auto it = mementoStates_.begin();
    std::advance(it, index);
    return it;
}

void TextField::StoreMementoForChar(Char chr)
{
    /* Store memento state if a new separator is added after a non-separator */
    if (IsSeparator(chr) && !IsSeparator(prevPutChar_))
        StoreMemento();
    else
        mementoExpired_ = true;
    prevPutChar_ = chr;
}


} // /namespace Tg



// ================================================================================
