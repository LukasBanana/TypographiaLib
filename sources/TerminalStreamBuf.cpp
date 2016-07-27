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
}


} // /namespace Tg



// ================================================================================
