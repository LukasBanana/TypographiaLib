/*
 * Terminal.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Terminal.h>
#include "TerminalStreamBuf.h"


namespace Tg
{


Terminal::Terminal(const FontGlyphSet& glyphSet, int maxWidth) :
    textField_  ( glyphSet, maxWidth, ""            ),
    streamBuf_  ( new TerminalStreamBuf(textField_) ),
    in          ( streamBuf_.get()                  ),
    out         ( streamBuf_.get()                  )
{
}



} // /namespace Tg



// ================================================================================
