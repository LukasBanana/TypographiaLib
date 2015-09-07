/*
 * test1.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Typo.h>
#include <iostream>

#pragma warning(disable : 4996)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace Tg;

void drawPixel(unsigned char c)
{
    const char table[] = { ' ', '.', ',', '+', '*', 'o', 'a', '@' };
    std::cout << table[c >> 5];
}

void drawImage(const Image& image)
{
    auto pixels = image.GetImageBuffer().begin();
    for (unsigned int y = 0; y < image.GetSize().height; ++y)
    {
        for (unsigned int x = 0; x < image.GetSize().width; ++x)
        {
            drawPixel(*pixels);
            ++pixels;
        }
        std::cout << std::endl;
    }
}

int main()
{
    std::cout << "Typographia Test 1" << std::endl;
    std::cout << "==================" << std::endl;

    FontDescription fontDesc;
    FontGlyphRange glyphRange;

    #if 0
    glyphRange.first   = 0x53F2;
    glyphRange.last    = 0x53F4;
    #else
    glyphRange.first   = 32;//'A';
    glyphRange.last    = 255;//'z';
    #endif
    
    fontDesc.height = 60;//32;
    fontDesc.name   = "C:/Windows/Fonts/kaiu.ttf";

    try
    {
        auto fontModel = BuildFont(fontDesc, glyphRange);

        Font font(fontDesc, std::move(fontModel.glyphSet));

        //drawImage(fontModel.image);
        const auto& img = fontModel.image;
        stbi_write_png("font_atlas.png", img.GetSize().width, img.GetSize().height, 1, img.GetImageBuffer().data(), img.GetSize().width);

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

