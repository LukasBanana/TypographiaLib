/*
 * SeparableString.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/SeparableString.h>
#include <Typo/Details.h>


namespace Tg
{


SeparableString::~SeparableString()
{
}

const String& SeparableString::GetSeparators() const
{
    static const auto sep = String(Details::DefaultSeparators<Char>::value);
    return sep;
}

bool SeparableString::IsSeparator(const Char& chr) const
{
    return GetSeparators().find(chr) != String::npos;
}


} // /namespace Tg



// ================================================================================
