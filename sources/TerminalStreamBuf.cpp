/*
 * TerminalStreamBuf.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "TerminalStreamBuf.h"


namespace Tg
{


TerminalStreamBuf::TerminalStreamBuf(TextFieldMultiLineString& textField) :
    textField_( textField )
{
    textField_.insertionEnabled = true;
}

TerminalStreamBuf::int_type TerminalStreamBuf::overflow(int_type c)
{
    if (c != traits_type::eof())
    {
        auto chr = traits_type::to_char_type(c);
        if (chr == '\r')
            textField_.MoveCursorBegin();
        else
            textField_.Put(chr);
    }
    return c;
}


} // /namespace Tg



// ================================================================================
