/*
 * TextFieldString.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_TEXT_FIELD_STRING_H__
#define __TG_TEXT_FIELD_STRING_H__


#include "TextField.h"


namespace Tg
{


/**
\brief Text field string class.
\remarks In a model-view-controller pattern, this is the model for a user input text field.
*/
class TextFieldString : public TextField
{
    
    public:
        
        TextFieldString() = default;

        TextFieldString(const String& str);

        TextFieldString& operator = (const String& str);
        
        TextFieldString& operator += (const String& str);
        
        TextFieldString& operator += (const Char& chr);
        
        inline operator const String& () const
        {
            return text_;
        }

        /* --- Cursor operations --- */
        
        //! Moves the cursor into the specified direction.
        void MoveCursor(int direction);
        
        //! Moves the cursor to the beginning.
        void MoveCursorBegin();
        
        //! Moves the cursor to the end.
        void MoveCursorEnd();

        //! Jumps to the next left sided space.
        void JumpLeft() override;
        
        //! Jumps to the next right sided space.
        void JumpRight() override;

        /* --- String content --- */

        /**
        Returns the current character which stands immediately before the cursor position.
        If the cursor is at the very beginning of the text field, the return value is '\0'.
        */
        Char CharLeft() const;
        
        /**
        Returns the current character which stands immediately after the cursor position.
        If the cursor is at the very end of the text field, the return value is '\0'.
        */
        Char CharRight() const;

        //! Removes the character on the left of the current cursor position.
        void RemoveLeft() override;

        //! Removes the character on the right of the current cursor position.
        void RemoveRight() override;

        /**
        \brief Removes the characters which are currently being selected.
        \see SetSelection
        */
        void RemoveSelection() override;

        /**
        \brief Inserts the specified character at the current cursor position or replaces the current selection.
        \see insertionEnabled
        \see RemoveSelection
        */
        void Insert(Char chr) override;

        //! Returns true if the specified character is valid. By default 'chr' must be in the range [32, +inf).
        bool IsValidChar(Char chr) const override;

        //! Sets the content of the text field and clamps the cursor position.
        void SetText(const String& text) override;

        //! Returns the content of the text field.
        const String& GetText() const override;

        /* === Members === */

        //! Specifies whether the cursor can be moved in a loop or not. By default false.
        bool cursorLoopEnabled  = false;

    private:
        
        //! Returns the iterator to the string at the specified cursor position.
        String::iterator Iter();

        //! Returns the constant iterator to the string at the specified cursor position.
        String::const_iterator Iter() const;

        /* === Member === */

        String text_;
        
};


} // /namespace Tg


#endif



// ================================================================================
