/*
 * Terminal.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_TERMINAL_H
#define TG_TERMINAL_H


#include "TextFieldMultiLineString.h"
#include <iostream>
#include <memory>


namespace Tg
{


//! Terminal class, representing the 'model' within the model-view-controller pattern.
class Terminal
{
    
    private:

        std::unique_ptr<std::streambuf> streamBuf_;

    public:
        
        Terminal(const FontGlyphSet& glyphSet, int maxWidth);

        //! Text field multi line string.
        TextFieldMultiLineString textField;

        //! Terminal input stream.
        std::istream in;

        //! Terminal output stream.
        std::ostream out;

};


} // /namespace Tg


#endif



// ================================================================================
