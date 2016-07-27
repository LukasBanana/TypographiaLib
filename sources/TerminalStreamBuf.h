/*
 * TerminalStreamBuf.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_TERMINAL_STREAM_BUF_H__
#define __TG_TERMINAL_STREAM_BUF_H__


#include <Typo/TextFieldMultiLineString.h>
#include <Typo/Char.h>
#include <iostream>


namespace Tg
{


class TerminalStreamBuf : public std::streambuf
{
    
    public:
        
        TerminalStreamBuf(TextFieldMultiLineString& textField);


    private:
        
        TextFieldMultiLineString& textField_;
        
};


} // /namespace Tg


#endif



// ================================================================================
