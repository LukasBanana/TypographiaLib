/*
 * Char.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_CHAR_H
#define TG_CHAR_H


#include "Config.h"

#include <string>


namespace Tg
{


#ifdef TG_UNICODE

//! Basic character type. If the macro 'TG_UNICODE' is defined, this is a 'wchar_t', otherwise 'char'.
using Char = wchar_t;

//! Basic string type. If the macro 'TG_UNICODE' is defined, this is 'std::wstring', otherwise 'std::string'.
using String = std::wstring;

//! Macro for string literals. If the macro 'TG_UNICODE' is defined, this is a unicode string literal, otherwise an ASCII string literal.
#define STRING_LITERAL(s) ( L ## s )

#else

//! Basic character type. If the macro 'TG_UNICODE' is defined, this is a 'wchar_t', otherwise 'char'.
using Char = char;

//! Basic string type. If the macro 'TG_UNICODE' is defined, this is 'std::wstring', otherwise 'std::string'.
using String = std::string;

//! Macro for string literals. If the macro 'TG_UNICODE' is defined, this is a unicode string literal, otherwise an ASCII string literal.
#define STRING_LITERAL(s) ( s )

#endif


} // /namespace Tg


#endif



// ================================================================================
