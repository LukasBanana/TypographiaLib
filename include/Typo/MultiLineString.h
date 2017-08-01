/*
 * MultiLineString.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_MULTI_LINE_STRING_H
#define TG_MULTI_LINE_STRING_H


#include "Char.h"
#include "Font.h"

#include <vector>


namespace Tg
{


/**
\brief Multi-line string class.
\remarks This can be used to easily manage multi-line text inside a restricted area.
*/
class MultiLineString
{
    
    public:
        
        //! String size type alias.
        using SizeType = String::size_type;

        struct TextLine
        {
            TextLine()
            {
                // dummy (can not be defaulted for clang compiler!)
            }
            TextLine(const String& text, int width) :
                text  { text  },
                width { width }
            {
            }

            String  text;
            int     width = 0;
        };
        
        MultiLineString(const FontGlyphSet& glyphSet, int maxWidth, const String& text);
        
        MultiLineString& operator = (const String& str);
        
        MultiLineString& operator += (const String& str);
        
        MultiLineString& operator += (const Char& chr);
        
        inline operator const String& () const
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
        \brief Inserts the specified character at the specified location.
        \param[in] lineIndex Specifies the line where to insert the new character.
        \param[in] positionInLine Specifies the position within the specified line string.
        This value must be in the range [0, line.size()], i.e. it can also be at the end of the string (not only line.size() - 1).
        \param[in] chr Specifies the new character. This can also be a new line character.
        \param[in] replace Specifies whether to replace the previous character or insert a new one. By default false.
        \remarks If the position (i.e. 'lineIndex' and 'positionInLine') are invalid, this function call has no effect.
        */
        void Insert(SizeType lineIndex, SizeType positionInLine, const Char& chr, bool replace = false);

        /**
        \brief Removes the character at the specified location.
        \param[in] lineIndex Specifies the line where to remove a character.
        \param[in] positionInLine Specifies the position within the specified line string.
        This value must be in the range [0, line.size()), i.e. it can not be at the end of the string (only line.size() - 1).
        \remarks If the position (i.e. 'lineIndex' and 'positionInLine') are invalid, this function call has no effect.
        */
        void Remove(SizeType lineIndex, SizeType positionInLine);

        /**
        \brief Converts the specified position into a string index within the main text string.
        \param[in] lineIndex Specifies the index of the text line.
        \param[in] positionInLine Specifies the position within the specified line string.
        This value must be in the range [0, line.size()], i.e. it can also be at the end of the string (not only line.size() - 1).
        \return Position within the main text string or 'String::npos' if the specified location is invalid.
        \remarks The return value is in the range [0, GetText().size()], i.e. it can exceed the main text position by 1 character!
        \see GetText
        \see GetTextPosition
        */
        SizeType GetTextIndex(SizeType lineIndex, SizeType positionInLine) const;

        /**
        \brief Converts the specified string index within the main text string into a position.
        \param[in] textIndex Specifies the index of the text line. This must be in the
        range [0, GetText().size()], i.e. it can also be at the end of the string (not only line.size() - 1).
        \param[out] lineIndex Specifies the resulting index of the text line.
        \param[out] positionInLine Specifies the resulting position within the resulting line string.
        \see GetText
        \see GetTextIndex
        */
        void GetTextPosition(SizeType textIndex, SizeType& lineIndex, SizeType& positionInLine) const;

        /**
        \brief Sets the new font glyph set and updates all lines.
        \see GetLines
        */
        void SetGlyphSet(const FontGlyphSet& glyphSet);
        
        //! Returns the current font glyph set for this multi-line string.
        inline const FontGlyphSet& GetGlyphSet() const
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
        inline int GetMaxWidth() const
        {
            return maxWidth_;
        }
        
        //! Returns the width of the widest line.
        inline int GetWidth() const
        {
            return width_;
        }
        
        //! Sets the content of the multi-line string and resets all lines.
        void SetText(const String& text);

        //! Returns the base text.
        inline const String& GetText() const
        {
            return text_;
        }
        
        /**
        \brief Returns the list of all text lines.
        \see TextLine
        */
        inline const std::vector<TextLine>& GetLines() const
        {
            return lines_;
        }

        //! Returns the width of the specified character
        virtual int CharWidth(const Char& chr) const;
        
        //! Returns true if the specified character is a new-line character, i.e. '\n' (line-feed) or '\r' (carriage return).
        bool IsNewLine(const Char& chr) const;
        
        //! Returns true if the specified character is a space character, i.e. ' ' or '\t'.
        bool IsSpace(const Char& chr) const;

    private:
        
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

        //! Rebuilds all text lines from the main text.
        void RebuildLines();
        
        /**
        \brief Appends the next sub-string from the main text to the text lines.
        \return Index within the main text after appending the new sub-string.
        */
        SizeType AppendLinesFromSubText(SizeType offset);

        //! Appends a sub-string from the main text to the text lines.
        SizeType AppendLineFromSubText(SizeType start, SizeType end, int subTextWidth);

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
