/*
 * test1.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Typo.h>
#include <iostream>
#include <chrono>

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

void saveImagePNG(const Image& image, const std::string& filename)
{
    stbi_write_png(filename.c_str(), image.GetSize().width, image.GetSize().height, 1, image.GetImageBuffer().data(), image.GetSize().width);
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
    fontDesc.name   = "C:/Windows/Fonts/times.ttf";//"C:/Windows/Fonts/kaiu.ttf";

    try
    {
        auto startTime = std::chrono::system_clock::now();
        auto fontModel = BuildFont(fontDesc, glyphRange);
        auto endTime = std::chrono::system_clock::now();
        std::cout << "font build time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << std::endl;

        std::wstring text = (
            //L"Hello,\n\nWorld! This is a small programming test with the TypographiaLib ;-)"
            L"TypographiaLib\n" \
            L"--------------\n" \
            L"This is a simple C++ font library"
            //L"abcdefghijklmnopqrstuvwxyz"
            //L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        );

        auto textImg = PlotTextImage(fontModel, text);
        saveImagePNG(textImg, "text_image.png");

        auto textImgMl = PlotMultiLineTextImage(fontModel, text, 400, 60);
        saveImagePNG(textImgMl, "text_image_ml.png");

        //drawImage(fontModel.image);
        saveImagePNG(fontModel.image, "font_atlas.png");

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

