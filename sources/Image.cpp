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

Image::Image(Image&& rhs) :
    size_       ( rhs.size_                   ),
    imageBuffer_( std::move(rhs.imageBuffer_) )
{
}

void Image::SetSize(const Size& size)
{
    size_ = size;
    imageBuffer_.resize(size_.Area());
    std::fill(imageBuffer_.begin(), imageBuffer_.end(), 0);
}


} // /namespace Tg



// ================================================================================
