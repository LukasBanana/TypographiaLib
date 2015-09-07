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
    FontGlyphRange glyphRange;

    glyphRange.first   = 0x53F2;//'A';
    glyphRange.last    = 0x53FF;//'z';
    
    fontDesc.height = 32;
    fontDesc.name   = "C:/Windows/Fonts/kaiu.ttf";

    try
    {
        Font font(fontDesc, glyphRange);

        MultiLineString<char> mlStr(font, 100, "test");

	    //todo...
    }
    catch (const std::exception& err)
    {
        std::cerr << err.what() << std::endl;
    }

    #ifdef _WIN32
    system("pause");
    #endif

    return 0;
}

