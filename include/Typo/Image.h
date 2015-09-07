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
        
        using ImageBuffer = std::vector<unsigned char>;

        Image() = default;
        Image(const Image&) = default;

        Image(const Size& size);

        Image(Image&& rhs);

        void SetSize(const Size& size);

        //! Moves this image buffer into the destination 'imageBuffer'. This also sets the image size to (0, 0).
        void MoveImageBuffer(Size& size, ImageBuffer& imageBuffer);

        //! Plots the specified sub image 'image' into this image at the specified offset position.
        void PlotImage(unsigned int xOffset, unsigned int yOffset, const Image& image);

        const Size& GetSize() const
        {
            return size_;
        }

        const ImageBuffer& GetImageBuffer() const
        {
            return imageBuffer_;
        }

        ImageBuffer::iterator ImageBufferBegin()
        {
            return imageBuffer_.begin();
        }

        ImageBuffer::iterator ImageBufferEnd()
        {
            return imageBuffer_.end();
        }

    private:
        
        Size        size_;
        ImageBuffer imageBuffer_; //!< Gray scaled image buffer with (width*height) elements.

};


} // /namespace Tg


#endif



// ================================================================================
