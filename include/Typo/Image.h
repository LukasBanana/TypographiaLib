/*
 * Image.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_IMAEG_H__
#define __TG_IMAEG_H__


#include "Size.h"

#include <vector>


namespace Tg
{


class Image
{
    
    public:

        Image() = default;
        Image(const Image&) = default;

        Image(const Size& size);

        Image(Image&& rhs);

        void SetSize(const Size& size);

        const Size& GetSize() const
        {
            return size_;
        }

        const std::vector<unsigned char>& GetImageBuffer() const
        {
            return imageBuffer_;
        }

    private:
        
        Size                        size_;
        std::vector<unsigned char>  imageBuffer_; //!< Gray scaled image buffer with (width*height) elements.

};


} // /namespace Tg


#endif



// ================================================================================
