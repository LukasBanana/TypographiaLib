/*
 * Terminal.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_TERMINAL_H__
#define __TG_TERMINAL_H__


#include "TextFieldMultiLineString.h"
#include <iostream>
#include <memory>


namespace Tg
{


//! Terminal class, representing the 'model' within the model-view-controller pattern.
class Terminal
{
    
    private:

        TextFieldMultiLineString        textField_;
        std::unique_ptr<std::streambuf> streamBuf_;

    public:
        
        Terminal(const FontGlyphSet& glyphSet, int maxWidth);

        const TextFieldMultiLineString& GetTextField() const
        {
            return textField_;
        }

        //! Terminal input stream.
        std::istream in;

        //! Terminal output stream.
        std::ostream out;

};


} // /namespace Tg


#endif



// ================================================================================
