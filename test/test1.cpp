/*
 * test1.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Typo.h>
#include <iostream>
#include <chrono>

#ifndef TG_UNICODE

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
    glyphRange.first   = 32;
    glyphRange.last    = 255;
    #endif

    fontDesc.height = 60;//32;
    fontDesc.name = (
        //"C:/Windows/Fonts/times.ttf"
        //"C:/Windows/Fonts/kaiu.ttf"
        "C:/Windows/Fonts/ITCEDSCR.ttf"
    );

    // Text field test
    TextFieldString textField;

    textField += STRING_LITERAL("Hello, World");
    textField.MoveCursorBegin();        std::cout << std::string(textField) << std::endl;
    textField.MoveCursor(3);            std::cout << std::string(textField) << std::endl;
    textField.RemoveSequenceRight();    std::cout << std::string(textField) << std::endl;
    textField.Put(char(127));           std::cout << std::string(textField) << std::endl;
    textField.Put("ABC");               std::cout << std::string(textField) << std::endl;

    try
    {
        auto startTime = std::chrono::system_clock::now();
        auto fontModel = BuildFont(fontDesc, glyphRange);
        auto endTime = std::chrono::system_clock::now();
        std::cout << "font build time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << std::endl;

        std::string text = (
            //"Hello,\n\nWorld! This is a small programming test with the TypographiaLib ;-)"
            "TypographiaLib\n" \
            "--------------\n" \
            "This is a simple C++ font library"
            //"abcdefghijklmnopqrstuvwxyz"
            //"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        );

        auto textImg = PlotTextImage(fontModel, text);
        saveImagePNG(textImg, "text_image.png");

        auto textImgMl = PlotMultiLineTextImage(fontModel, text, 400, 60);
        saveImagePNG(textImgMl, "text_image_ml.png");

        #if 0
        // Example outputs
        //saveImagePNG(BuildFont({ "C:/Windows/Fonts/times.ttf", 34 }, { 32, 255 }).image, "fontatlas_times.png");
        //saveImagePNG(BuildFont({ "C:/Windows/Fonts/kaiu.ttf", 34 }, { 0x8c22, 0x8c35 }).image, "fontatlas_kaiu.png");
        saveImagePNG(BuildFont({ "C:/Windows/Fonts/ITCEDSCR.TTF", 60 }, { 32, 128 }).image, "fontatlas_edwardian_script.png");
        #endif

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

#else

int main()
{
    return 0;
}

#endif

