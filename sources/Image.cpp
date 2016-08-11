/*
 * Image.cpp
 *
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Image.h>
#include <algorithm>
#include <string.h>


namespace Tg
{


Image::Image(const Size& size)
{
    SetSize(size);
}

Image::Image(Image&& rhs)
{
    rhs.MoveImageBuffer(size_, imageBuffer_);
}

Image& Image::operator = (Image&& rhs)
{
    rhs.MoveImageBuffer(size_, imageBuffer_);
    return *this;
}

void Image::SetSize(const Size& size)
{
    size_ = size;
    imageBuffer_.resize(size_.Area());
    std::fill(imageBuffer_.begin(), imageBuffer_.end(), 0);
}

void Image::MoveImageBuffer(Size& size, ImageBuffer& imageBuffer)
{
    size        = size_;
    imageBuffer = std::move(imageBuffer_);
    size_       = Size(0, 0);
}

void Image::PlotImage(unsigned int xOffset, unsigned int yOffset, const Image& image)
{
    /* Check if sub image is placed inside this image */
    auto sz = image.GetSize();

    if (xOffset + sz.width > size_.width || yOffset + sz.height > size_.height)
        return;

    /* Plot each scan line */
    for (unsigned int i = 0; i < sz.height; ++i)
    {
        memcpy(
            PointerOffset(xOffset, yOffset + i),
            image.PointerOffset(0, i),
            sizeof(unsigned char)*sz.width
        );
    }
}

void Image::PlotImage(
    unsigned int xOffset, unsigned int yOffset,
    const Image& image, unsigned int x, unsigned int y,
    unsigned int width, unsigned int height,
    bool accumulate)
{
    /* Check if sub image is placed inside this image */
    auto sz = image.GetSize();

    if (x + width > sz.width || y + height > sz.height || xOffset + width > size_.width || yOffset + height > size_.height)
        return;

    /* Plot each scan line */
    if (accumulate)
    {
        for (unsigned int i = 0; i < height; ++i)
        {
            auto dest = PointerOffset(xOffset, yOffset + i);
            auto src = image.PointerOffset(x, y + i);

            for (unsigned int j = 0; j < width; ++j)
                dest[j] = std::min(255, static_cast<int>(dest[j]) + src[j]);
        }
    }
    else
    {
        for (unsigned int i = 0; i < height; ++i)
        {
            memcpy(
                PointerOffset(xOffset, yOffset + i),
                image.PointerOffset(x, y + i),
                sizeof(unsigned char)*width
            );
        }
    }
}

unsigned char* Image::PointerOffset(unsigned int x, unsigned int y)
{
    return &(imageBuffer_[y*size_.width + x]);
}

const unsigned char* Image::PointerOffset(unsigned int x, unsigned int y) const
{
    return &(imageBuffer_[y*size_.width + x]);
}


} // /namespace Tg



// ================================================================================
