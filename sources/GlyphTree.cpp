/*
 * GlyphTree.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "GlyphTree.h"


namespace Tg
{


GlyphTree::GlyphTree(unsigned int width, unsigned int height) :
    rect_( 0, 0, width, height )
{
}

GlyphTree::GlyphTree(const Rect& rect) :
    rect_( rect )
{
}

GlyphTree* GlyphTree::Insert(FontGlyph& glyph, unsigned int width, unsigned int height)
{
    if (childA_)
    {
        /* Try to find a suitable tree node */
        auto node = childA_->Insert(glyph, width, height);

        if (node)
            return node;

        return childB_->Insert(glyph, width, height);
    }

    /* Check if this node already contains a gylph and check if its size fits into this tree node */
    if (glyph_ || width > rect_.Width() || height > rect_.Height())
        return nullptr;

    /* Check if glyph fits exactly into this node */
    if (width == rect_.Width() && height == rect_.Height())
    {
        glyph_ = &glyph;
        glyph_->rect = rect_;
        return this;
    }

    /* Create children and split into two spaces */
    if (rect_.Width() - width > rect_.Height() - height)
    {
        CreateChildA(Rect(rect_.left, rect_.top, rect_.left + width, rect_.bottom));
        CreateChildB(Rect(rect_.left + width, rect_.top, rect_.right, rect_.bottom));
    }
    else
    {
        CreateChildA(Rect(rect_.left, rect_.top, rect_.right, rect_.top + height));
        CreateChildB(Rect(rect_.left, rect_.top + height, rect_.right, rect_.bottom));
    }

    /* Try to insert the glyph into the new first child */
    return childA_->Insert(glyph, width, height);
}

void GlyphTree::Clear()
{
    childA_.reset();
    childB_.reset();
}

GlyphTree* GlyphTree::GetChildA() const
{
    return childA_.get();
}

GlyphTree* GlyphTree::GetChildB() const
{
    return childB_.get();
}

const Rect& GlyphTree::GetRect() const
{
    return rect_;
}

FontGlyph* GlyphTree::GetGlyph() const
{
    return glyph_;
}

void GlyphTree::CreateChild(GlyphTreePtr& child, const Rect& rect)
{
    child = GlyphTreePtr(new GlyphTree(rect));
}

void GlyphTree::CreateChildA(const Rect& rect)
{
    CreateChild(childA_, rect);
}

void GlyphTree::CreateChildB(const Rect& rect)
{
    CreateChild(childB_, rect);
}


} // /namespace Tg



// ================================================================================
