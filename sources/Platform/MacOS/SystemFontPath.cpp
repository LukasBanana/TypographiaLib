/*
 * SystemFontPath.cpp (MacOS)
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/SystemFontPath.h>


namespace Tg
{


std::string SystemFontPath(std::string fontName, int flags)
{
    std::string fontFilename;

    std::string fontPath = "/Library/Fonts/";
    fontFilename = fontPath + fontName + ".ttf";

    return fontFilename;
}


} // /namespace Tg



// ================================================================================
