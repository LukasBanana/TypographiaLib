/*
 * SystemFontPath.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_SYSTEM_FONT_PATH_H__
#define __TG_SYSTEM_FONT_PATH_H__


#include <string>


namespace Tg
{


/**
\brief Returns the operating-system specific font path.
\param[in] fontName Specifies the font name (without file extension).
\param[in] flags Specifies the font flags. This can be a bitwise OR combination of the values of the 'FontFlags' enumeration.
\see FontFlags
*/
std::string SystemFontPath(std::string fontName, int flags = 0);


} // /namespace Tg


#endif



// ================================================================================
