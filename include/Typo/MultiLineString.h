/*
 * MultiLineString.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_MULTI_LINE_STRING_H__
#define __TG_MULTI_LINE_STRING_H__


#include "Font.h"
#include "SeparableString.h"

#include <vector>


namespace Tg
{


/**
\brief Multi-line string class.
\remarks This can be used to easily manage multi-line text inside a restricted area.
*/
class MultiLineString : public SeparableString
{
    
    public:
        
        struct TextLine
        {
            String  text;
            int     width;
        };
        
        MultiLineString(const FontGlyphSet& glyphSet, int maxWidth, const String& text);
        
        MultiLineString& operator = (const String& str);
        
        MultiLineString& operator += (const String& str);
        
        MultiLineString& operator += (const Char& chr);
        
        operator const String& () const
        {
            return text_;
        }
        
        /**
        \brief Appends the sepcified character to the end of the base string and updates the affected line.
        \param[in] chr Specifies the new character.
        \see GetText
        */
        void PushBack(const Char& chr);
        
        /**
        \brief Removes the last character from the base string and updates the affected line.
        \see GetText
        */
        void PopBack();
        
        /**
        \brief Sets the new font glyph set and updates all lines.
        \see GetLines
        */
        void SetGlyphSet(const FontGlyphSet& glyphSet);
        
        //! Returns the current font glyph set for this multi-line string.
        const FontGlyphSet& GetGlyphSet() const
        {
            return *glyphSet_;
        }

        //! Sets the new maximal width.
        void SetMaxWidth(int maxWidth);
        
        /**
        \brief Returns the maximal width.
        \remarks If this is too small, it might be smaller than the value of 'GetWidth',
        because at least one single character must fit into each line.
        */
        int GetMaxWidth() const
        {
            return maxWidth_;
        }
        
        //! Returns the width of the widest line.
        int GetWidth() const
        {
            return width_;
        }
        
        //! Sets the content of the multi-line string and resets all lines.
        void SetText(const String& text);

        //! Returns the base text.
        const String& GetText() const
        {
            return text_;
        }
        
        /**
        \brief Returns the list of all text lines.
        \see TextLine
        */
        const std::vector<TextLine>& GetLines() const
        {
            return lines_;
        }
        
    protected:
        
        //! Returns the width of the specified character
        virtual int CharWidth(const Char& chr) const;
        
    private:
        
        /**
        Returns true if the specified character is a new-line character,
        i.e. '\n' (line-feed) or '\r' (carriage return).
        */
        bool IsNewLine(const Char& chr) const;
        
        //! Returns true if the specified width fits into a line, i.e. does not exceed the maximal width.
        bool FitIntoLine(int width) const;
        
        /**
        Updates the widest width by 'inserting' the specified width,
        i.e. newWidth = max{ oldWidth, width }.
        \see GetWidth
        */
        void UpdateWidestWidth(int width);
        
        /**
        Updates the widest width by finding the maximum of all lines' widths,
        i.e. newWidth = max{ line[0].width, line[1].width, ..., line[n - 1].width }.
        \see GetWidth
        */
        void UpdateWidestWidth();
        
        //! Appends a new text line with the specified string and width.
        void AppendLine(const String& text, int width);
        
        //! Appends a new text line with the specified character and width.
        void AppendLine(const Char& chr, int width);
        
        //! Appends a new empty text line.
        void AppendLine();

        //! Resets all text lines.
        void ResetLines();
        
        /* === Member === */

        const FontGlyphSet*     glyphSet_;
        
        int                     maxWidth_;
        int                     width_;
        
        String                  text_;
        std::vector<TextLine>   lines_;
        
};


} // /namespace Tg


#endif



// ================================================================================
