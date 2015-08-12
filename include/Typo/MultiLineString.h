/*
 * MultiLineString.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_MULTI_LINE_STRING_H__
#define __TG_MULTI_LINE_STRING_H__


#include "Font.h"

#include <vector>
#include <string>
#include <algorithm>


namespace Tg
{


//! Namespace for internal templates
namespace Details
{

template <typename T>
struct DefaultSeparators
{
    static const T* value;
};

const char* DefaultSeparators<char>::value = " \t,;.:-()[]{}/\\";

const wchar_t* DefaultSeparators<wchar_t>::value = L" \t,;.:-()[]{}/\\";

} // /namespace Details


template <typename T>
class MultiLineString
{
    
    public:
        
        using StringType = std::basic_string<T>;
        
        struct TextLine
        {
            StringType  text;
            int         width;
        };
        
        MultiLineString(const Font& font, int maxWidth, const StringType& text) :
            font_       ( font     ),
            maxWidth_   ( maxWidth ),
            width_      ( 0        ),
            text_       ( text     )
        {
            ResetLines();
        }
        
        virtual ~MultiLineString()
        {
        }
        
        MultiLineString<T>& operator = (const StringType& str)
        {
            text_ = str;
            ResetLines();
            return *this;
        }
        
        MultiLineString<T>& operator += (const StringType& str)
        {
            for (const auto& chr : str)
                PushBack(chr);
            return *this;
        }
        
        MultiLineString<T>& operator += (const T& chr)
        {
            PushBack(chr);
            return *this;
        }
        
        operator const StringType& () const
        {
            return text_;
        }
        
        /**
        \brief Appends the sepcified character to the end of the base string and updates the affected line.
        \param[in] chr Specifies the new character.
        \see GetText
        */
        void PushBack(const T& chr)
        {
            /* Get glyph set from font */
            const auto& glyphSet = font_.GetGlyphSet();
            
            if (IsNewLine(chr))
            {
                /* Append empty line */
                AppendLine();
            }
            else
            {
                /* Get width of new character */
                int width = glyphSet[chr].TotalWidth();
                
                if (lines_.empty())
                {
                    /* Add first character without width-check */
                    AppendLine(chr, width);
                }
                else
                {
                    /* Try to append character to last line */
                    auto& line = lines_.back();
                    
                    /* Check if character fits into last line */
                    if (FitIntoLine(line.width + width))
                    {
                        /* Update line and widest width */
                        line.width += width;
                        line.text += chr;
                        UpdateWidestWidth(line.width);
                    }
                    else
                    {
                        /*
                        Reset lines, because separators may change
                        the current (last) line and the new line
                        */
                        ResetLines();
                    }
                }
            }
        }
        
        /**
        \brief Removes the last character from the base string and updates the affected line.
        \see GetText
        */
        void PopBack()
        {
            if (lines_.empty())
                return;
            
            /* Remove last character from last line */
            auto& line = lines_.back();
            auto chr = line.back();
            
            line.pop_back();
            
            if (line.empty())
            {
                /* Remove last line if it's empty */
                lines_.pop_back();
            }
            else
            {
                /* Update width in current line */
                const auto& glyphSet = font_.GetGlyphSet();
                int width = glyphSet[chr].TotalWidth();
                line.width -= width;
            }
            
            /* Always update widest width when characters are removed */
            UpdateWidestWidth();
        }
        
        /**
        \brief Sets the new font and updates all lines.
        \see GetLines
        */
        void SetFont(const Font& font)
        {
            font_ = font;
            ResetLines();
        }
        
        //! Returns the currnet font for this multi-line string.
        const Font& GetFont() const
        {
            return font_;
        }
        
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
        
        //! Returns the base text.
        const StringType& GetText() const
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
        
        //! Returns a string with all separator characters.
        virtual const StringType& GetSeparators() const
        {
            static const StringType sep = StringType(Details::DefaultSeparators<T>::value);
            return sep;
        }
        
    private:
        
        /**
        Returns true if the specified character is a new-line character,
        i.e. '\n' (line-feed) or '\r' (carriage return).
        */
        bool IsNewLine(const T& chr) const
        {
            return chr == T('\n') || chr == T('\r');
        }
        
        /**
        Returns true if the specified character is a separator.
        \see GetSeparators
        */
        bool IsSeparator(const T& chr) const
        {
            return GetSeparators().find(chr) != StringType::npos;
        }
        
        //! Returns true if the specified width fits into a line, i.e. does not exceed the maximal width.
        bool FitIntoLine(int width) const
        {
            return width <= GetMaxWidth();
        }
        
        /**
        Updates the widest width by 'inserting' the specified width,
        i.e. newWidth = max{ oldWidth, width }.
        \see GetWidth
        */
        void UpdateWidestWidth(int width)
        {
            width_ = std::max(width_, width);
        }
        
        /**
        Updates the widest width by finding the maximum of all lines' widths,
        i.e. newWidth = max{ line[0].width, line[1].width, ..., line[n - 1].width }.
        \see GetWidth
        */
        void UpdateWidestWidth()
        {
            width_ = 0;
            for (const auto& line : lines_)
                width_ = std::max(width_, line.width);
        }
        
        //! Appends a new text line with the specified string and width.
        void AppendLine(const StringType& text, int width)
        {
            lines_.push_back({ text, width });
            UpdateWidestWidth(width);
        }
        
        //! Appends a new text line with the specified character and width.
        void AppendLine(const T& chr, int width)
        {
            AppendLine(StringType(1, chr), 0);
        }
        
        //! Appends a new empty text line.
        void AppendLine()
        {
            AppendLine(StringType(), 0);
        }
        
        //! Resets all text lines.
        void ResetLines()
        {
            /* Reset line strings */
            lines_.clear();
            width_ = 0;
            
            if (text_.empty())
                return;
            
            /* Get glyph set from font */
            const auto& glyphSet = font_.GetGlyphSet();
            int nextWidth = 0, width = 0, sepWidth = 0;
            T chr = 0;
            
            /* Setup all wrapped lines */
            for (std::size_t pos = 0, sepLen = 0, len = 0, num = text_.size(); pos < num; pos += len)
            {
                /* Find maximal length for current line */
                for (len = 0, nextWidth = 0, width = 0; pos + len < num && FitIntoLine(nextWidth); ++len)
                {
                    /* Store new line width */
                    width = nextWidth;
                    
                    /* Check for separator of previous character */
                    if (IsSeparator(chr))
                    {
                        sepLen = len;
                        sepWidth = width;
                    }
                    
                    /* Get current character from base string */
                    chr = text_[pos + len];
                    
                    /* Break line for new-line character */
                    if (IsNewLine(chr))
                        break;
                    
                    /* Add width of current character */
                    nextWidth += glyphSet[chr].TotalWidth();
                }
                
                /* Clamp line size to a minimum */
                if (len == 0 && nextWidth > 0 && pos < num)
                {
                    ++len;
                    width = nextWidth;
                }
                else if (sepLen > 0)
                {
                    len = sepLen;
                    width = sepWidth;
                }
                
                /* Add new line to list */
                if (len > 0)
                    AppendLine(text_.substr(pos, len), width);
                else
                    AppendLine();
            }
        }
        
        const Font&             font_;
        
        int                     maxWidth_;
        int                     width_;
        
        StringType              text_;
        std::vector<TextLine>   lines_;
        
};


} // /namespace Tg


#endif



// ================================================================================
