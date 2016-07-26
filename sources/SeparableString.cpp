/*
 * SeparableString.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/SeparableString.h>


namespace Tg
{


SeparableString::~SeparableString()
{
}

bool SeparableString::IsSeparator(const Char& chr) const
{
    return !((chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z'));
}


} // /namespace Tg



// ================================================================================
