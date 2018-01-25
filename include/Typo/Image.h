/*
 * Image.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_IMAEG_H
#define TG_IMAEG_H


#include "Size.h"
#include <vector>
#include <cstdint>


namespace Tg
{


class Image
{
    
    public:
        
        using ImageBuffer = std::vector<std::uint8_t>;

        Image() = default;
        Image(const Image&) = default;

        Image(const Size& size);

        Image(Image&& rhs);

        Image& operator = (Image&& rhs);

        void SetSize(const Size& size);

        //! Moves this image buffer into the destination 'imageBuffer'. This also sets the image size to (0, 0).
        void MoveImageBuffer(Size& size, ImageBuffer& imageBuffer);

        //! Plots the specified sub image 'image' into this image at the specified offset position.
        void PlotImage(unsigned int xOffset, unsigned int yOffset, const Image& image);

        //! Plots parts of the specified sub image 'image' into this image at the specified offset position.
        void PlotImage(
            unsigned int xOffset, unsigned int yOffset,
            const Image& image,
            unsigned int x, unsigned int y,
            unsigned int width, unsigned int height,
            bool accumulate = false
        );

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
        
        unsigned char* PointerOffset(unsigned int x, unsigned int y);
        const unsigned char* PointerOffset(unsigned int x, unsigned int y) const;

        Size        size_;
        ImageBuffer imageBuffer_; //!< Gray scaled image buffer with (width*height) elements.

};


} // /namespace Tg


#endif



// ================================================================================
