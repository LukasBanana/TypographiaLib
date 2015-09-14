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

const String& SeparableString::GetSeparators() const
{
    static const String sep = STRING_LITERAL(" \t\"',;.:-()[]{}/\\");
    return sep;
}

bool SeparableString::IsSeparator(const Char& chr) const
{
    return GetSeparators().find(chr) != String::npos;
}


} // /namespace Tg



// ================================================================================
