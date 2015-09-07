/*
 * Image.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Image.h>
#include <algorithm>


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

    if (xOffset + sz.width >= size_.width || yOffset + sz.height >= size_.height)
        return;

    /* Plot each scan line */
    for (unsigned int y = 0; y < sz.height; ++y)
    {
        memcpy(
            &(imageBuffer_[(y + yOffset)*size_.width + xOffset]),
            &(image.imageBuffer_[y*sz.width]),
            sizeof(char)*sz.width
        );
    }
}


} // /namespace Tg



// ================================================================================
