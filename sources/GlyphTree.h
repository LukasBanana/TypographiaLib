/*
 * GlyphTree.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_GLYPH_TREE_H
#define TG_GLYPH_TREE_H


#include <Typo/Font.h>
#include <Typo/Size.h>
#include <memory>


namespace Tg
{


class GlyphTree;

using GlyphTreePtr = std::unique_ptr<GlyphTree>;

class GlyphTree
{

    public:

        GlyphTree() = default;
        GlyphTree(const Size& size);
        GlyphTree(const Rect& rect);

        GlyphTree(const GlyphTree&) = delete;
        GlyphTree& operator = (const GlyphTree&) = delete;

        /**
        Tries to insert the specified glyph object into the tree.
        \param[in] glyph Pointer to the glyph object.
        \param[in] width Specifies the glyph width.
        \param[in] width Specifies the glyph height.
        \return Raw pointer to the glyph tree node which contains the inserted glyph.
        */
        GlyphTree* Insert(FontGlyph& glyph);

        //! Deletes its child tree node.
        void Clear();

        //! Restes the glyph tree. This also clears all child node.
        void Reset(const Size& size);

        //! Returns the first child node.
        GlyphTree* GetChildA() const;

        //! Returns the second child node.
        GlyphTree* GetChildB() const;

        //! Returns the rectangle where this node is embedded.
        const Rect& GetRect() const;

        //! Returns the data container.
        FontGlyph* GetGlyph() const;

    private:

        void CreateChild(GlyphTreePtr& child, const Rect& rect);
        void CreateChildA(const Rect& rect);
        void CreateChildB(const Rect& rect);

        GlyphTreePtr    childA_;            //!< First child node.
        GlyphTreePtr    childB_;            //!< Second child node.

        Rect            rect_;              //!< Rectangle where the glyph is stored.

        FontGlyph*      glyph_  = nullptr;  //!< Reference to the glyph data.

};


} // /namespace Tg


#endif



// ================================================================================
