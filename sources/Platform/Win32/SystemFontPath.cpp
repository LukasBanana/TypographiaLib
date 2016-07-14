/*
 * SystemFontPath.cpp (Win32)
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "GetFontFile.h"

#include <Typo/SystemFontPath.h>


namespace Tg
{


std::string SystemFontPath(std::string fontName, int flags)
{
    static const std::string fontPath = "C:\\Windows\\Fonts\\";

    std::string filename;
    if (GetFontFile(fontName, filename))
        return fontPath + filename;

    return "";
}


} // /namespace Tg



// ================================================================================
