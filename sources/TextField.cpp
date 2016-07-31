/*
 * TextField.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/TextField.h>


namespace Tg
{


TextField::~TextField()
{
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


} // /namespace Tg



// ================================================================================
