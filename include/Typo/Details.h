/*
 * Details.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_DETAILS_H__
#define __TG_DETAILS_H__


namespace Tg
{


//! Namespace for internal templates
namespace Details
{


template <typename T>
struct DefaultSeparators
{
    static const T* value;
};

const char* DefaultSeparators<char>::value = " \t\"',;.:-()[]{}/\\";

const wchar_t* DefaultSeparators<wchar_t>::value = L" \t\"',;.:-()[]{}/\\";


} // /namespace Details


} // /namespace Tg


#endif



// ================================================================================
