/*
 * test1.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Typo.h>
#include <iostream>

using namespace Tg;

int main()
{
    std::cout << "Typographia Test 1" << std::endl;
    std::cout << "==================" << std::endl;

    FontDescription fontDesc;
    FontGlyphSet glyphSet;

    glyphSet.glyphRange.first   = 'a';
    glyphSet.glyphRange.last    = 'z';
    
    for (auto chr = 'a'; chr <= 'z'; ++chr)
    {
        FontGlyph glyph;
        glyph.drawnWidth = 45;
        glyphSet.glyphs.push_back(glyph);
    }

    Font font(fontDesc, glyphSet);

    MultiLineString<char> mlStr(font, 100, "test");

	//todo...

    #ifdef _WIN32
    system("pause");
    #endif

    return 0;
}

