/*
 * TerminalStreamBuf.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_TERMINAL_STREAM_BUF_H
#define TG_TERMINAL_STREAM_BUF_H


#include <Typo/TextFieldMultiLineString.h>
#include <Typo/Char.h>
#include <iostream>


namespace Tg
{


class TerminalStreamBuf : public std::streambuf
{

    public:

        TerminalStreamBuf(TextFieldMultiLineString& textField);

        /* --- Base class overloading --- */

        int_type overflow(int_type c) override;

    private:

        TextFieldMultiLineString& textField_;

};


} // /namespace Tg


#endif



// ================================================================================
