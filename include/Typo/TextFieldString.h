/*
 * TextFieldString.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_TEXT_FIELD_STRING_H__
#define __TG_TEXT_FIELD_STRING_H__


#include "SeparableString.h"

#include <string>
#include <algorithm>


namespace Tg
{


/**
\brief Text field string template class.
\remarks In a model-view-controller pattern, this is the model for a user input text field.
*/
template <typename T>
class TextFieldString : public SeparableString<T>
{
    
    public:
        
        //! String size type alias.
        using SizeType = typename StringType::size_type;

        TextFieldString() = default;

        TextFieldString(const StringType& str) :
            text_( str )
        {
        }

        TextFieldString<T>& operator = (const StringType& str)
        {
            SetText(str);
            return *this;
        }
        
        TextFieldString<T>& operator += (const StringType& str)
        {
            for (const auto& chr : str)
                Insert(chr);
            return *this;
        }
        
        TextFieldString<T>& operator += (const T& chr)
        {
            Insert(str);
            return *this;
        }
        
        operator const StringType& () const
        {
            return text_;
        }
        
        //! Returns true if the cursor is at the beginning.
        bool IsCursorBegin() const
        {
            return cursorPos_ == 0;
        }
        
        //! Returns true if the cursor is at the end.
        bool IsCursorEnd() const
        {
            return cursorPos_ == text_.size();
        }

        //! Moves the cursor into the specified direction.
        void MoveCursor(int direction)
        {
            if (direction < 0)
            {
                auto dir = static_cast<SizeType>(-direction);
                if (cursorPos_ > dir)
                    cursorPos_ -= dir;
                else
                    cursorPos_ = 0;
            }
            else if (direction > 0)
            {
                auto dir = static_cast<SizeType>(direction);
                cursorPos_ += dir;

                if (cursorPos_ > text_.size())
                    cursorPos_ = text_.size();
            }
        }
        
        //! Moves the cursor to the beginning.
        void MoveCursorBegin()
        {
            cursorPos_ = 0;
        }
        
        //! Moves the cursor to the end.
        void MoveCursorEnd()
        {
            cursorPos_ = text_.size();
        }

        //! Jumps to the next left sided space.
        void JumpLeft()
        {
            if (IsSeparator(CharLeft()))
            {
                while (!IsCursorBegin() && IsSeparator(CharLeft()))
                    --cursorPos_;
            }
            while (!IsCursorBegin() && !IsSeparator(CharLeft()))
                --cursorPos_;
        }
        
        //! Jumps to the next right sided space.
        void JumpRight()
        {
            if (IsSeparator(CharRight()))
            {
                while (!IsCursorEnd() && IsSeparator(CharRight()))
                    ++cursorPos_;
            }
            while (!IsCursorEnd() && !IsSeparator(CharRight()))
                ++cursorPos_;
        }

        /**
        Returns the current character which stands immediately before the cursor position.
        If the cursor is at the very beginning of the text field, the return value is '\0'.
        */
        T CharLeft() const
        {
            return !IsCursorBegin() ? text_[cursorPos_ - 1] : T(0);
        }
        
        /**
        Returns the current character which stands immediately after the cursor position.
        If the cursor is at the very end of the text field, the return value is '\0'.
        */
        T CharRight() const
        {
            return !IsCursorEnd() ? text_[cursorPos_] : T(0);
        }

        //! Removes the character on the left of the current cursor position.
        void RemoveLeft()
        {
            if (!IsCursorBegin())
            {
                /* Remove character and then move cursor left */
                --cursorPos_;
                if (IsCursorEnd())
                    text_.pop_back();
                else
                    text_.erase(Iter());
            }
        }

        //! Removes the character on the right of the current cursor position.
        void RemoveRight()
        {
            if (!IsCursorEnd())
            {
                /* Only remove character without moving the cursor */
                if ((cursorPos_ + 1) == text_.size())
                    text_.pop_back();
                else
                    text_.erase(Iter());
            }
        }

        //! Removes the character sequence on the left of the current cursor position until the next separator appears.
        void RemoveSequenceLeft()
        {
            /* Remove all characters before the cursor, until the next separator appears */
            while (!IsCursorBegin() && IsSeparator(CharLeft()))
                RemoveLeft();
            while (!IsCursorBegin() && !IsSeparator(CharLeft()))
                RemoveLeft();
        }

        //! Removes the character sequence on the right of the current cursor position until the next separator appears.
        void RemoveSequenceRight()
        {
            /* Remove all characters after the cursor, until the next separator appears */
            while (!IsCursorEnd() && IsSeparator(CharRight()))
                RemoveRight();
            while (!IsCursorEnd() && !IsSeparator(CharRight()))
                RemoveRight();
        }

        /**
        \brief Inserts the specified character.
        \see insertionEnabled
        */
        void Insert(const T& chr)
        {
            if (IsValidChar(chr))
            {
                if (IsCursorEnd())
                {
                    /* Push back the new character */
                    text_ += chr;
                }
                else
                {
                    /* Insert the new character */
                    if (insertionEnabled)
                        text_[cursorPos_] = chr;
                    else
                        text_.insert(Iter(), chr);
                }

                /* Move cursor position */
                ++cursorPos_;
            }
        }

        /**
        \brief Inserts the specified character with some exceptions.
        \param[in] chr Specifies the new character. Special characters are:
        - '\b' which will remove the character before the cursor.
        - char(127) which will remove all characters before the cursor until the next separator appears.
        \see Insert
        */
        virtual void Put(const T& chr)
        {
            if (chr == T('\b'))
                RemoveLeft();
            else if (chr == T(127))
                RemoveRight();
            else if (unsigned(chr) >= 32)
                Insert(chr);
        }
        
        //! Inserts the specified text.
        virtual void Put(const StringType& text)
        {
            for (const auto& chr : text)
                Put(chr);
        }

        //! Sets the content of the text field and clamps the cursor position.
        void SetText(const StringType& text)
        {
            text_ = text;
            ClampCursorPos();
        }

        //! Returns the content of the text field.
        const StringType& GetText() const
        {
            return text_;
        }

        //! Returns the current (horizontal) cursor position. This is always in the range [0 .. GetText().size()].
        SizeType GetCursorPosition() const
        {
            return cursorPos_;
        }

        //! Specifies whether the insertion modd is enabled or not. By default false.
        bool insertionEnabled = false;

    protected:
        
        //! Returns true if the specified character is valid. By default 'chr' must be in the range [32, +inf).
        virtual bool IsValidChar(const T& chr) const
        {
            return unsigned(chr) >= 32;
        }

    private:
        
        //! Returns the iterator to the string at the specified cursor position.
        typename StringType::iterator Iter()
        {
            return text_.begin() + cursorPos_;
        }

        //! Returns the constant iterator to the string at the specified cursor position.
        typename StringType::const_iterator Iter() const
        {
            return text_.begin() + cursorPos_;
        }

        void ClampCursorPos()
        {
            cursorPos_ = std::max(0, cursorPos_.size());
        }

        StringType  text_;
        SizeType    cursorPos_ = 0;
        
};


} // /namespace Tg


#endif



// ================================================================================
