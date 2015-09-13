/*
 * TextFieldString.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_TEXT_FIELD_STRING_H__
#define __TG_TEXT_FIELD_STRING_H__


#include "SeparableString.h"


namespace Tg
{


/**
\brief Text field string template class.
\remarks In a model-view-controller pattern, this is the model for a user input text field.
*/
class TextFieldString : public SeparableString
{
    
    public:
        
        TextFieldString() = default;

        TextFieldString(const String& str);

        TextFieldString& operator = (const String& str);
        
        TextFieldString& operator += (const String& str);
        
        TextFieldString& operator += (const Char& chr);
        
        operator const String& () const
        {
            return text_;
        }

        /* --- Cursor operations --- */
        
        //! Sets the new cursor position. This will be clamped to the range [0, GetText().size()].
        void SetCursorPosition(SizeType position);

        //! Returns the current (horizontal) cursor position. This is always in the range [0, GetText().size()].
        SizeType GetCursorPosition() const
        {
            return cursorPos_;
        }

        //! Returns true if the cursor is at the beginning.
        bool IsCursorBegin() const;
        
        //! Returns true if the cursor is at the end.
        bool IsCursorEnd() const;

        //! Moves the cursor into the specified direction.
        void MoveCursor(int direction);
        
        //! Moves the cursor to the beginning.
        void MoveCursorBegin();
        
        //! Moves the cursor to the end.
        void MoveCursorEnd();

        //! Jumps to the next left sided space.
        void JumpLeft();
        
        //! Jumps to the next right sided space.
        void JumpRight();

        /* --- Selection operations --- */

        /**
        \brief Sets the new selection area.
        \param[in] start Specifies the selection start. This may also be larger than 'end'.
        \param[in] end Specifies the selection end (or rather the new cursor psoition).
        \remarks This also modifies the cursor position.
        \see SetCursorPosition
        \see selectionEnabled
        */
        void SetSelection(SizeType start, SizeType end);

        /**
        \brief Retrieves the selection range, so that 'start' is always less than or equal to 'end'.
        \see selectionEnabled
        */
        void GetSelection(SizeType& start, SizeType& end) const;

        //! Selects the entire string content.
        void SelectAll();

        /**
        \brief Deselects the current selection.
        \remarks This also disables selection state (see selectionEnabled).
        \see selectionEnabled
        */
        void Deselect();

        /**
        \brief Returns true if any string part is currently being selected.
        \see GetSelection
        */
        bool IsSelected() const;

        //! Returns the selected text.
        String GetSelectionText() const;

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
        void RemoveLeft();

        //! Removes the character on the right of the current cursor position.
        void RemoveRight();

        //! Removes the character sequence on the left of the current cursor position until the next separator appears.
        void RemoveSequenceLeft();

        //! Removes the character sequence on the right of the current cursor position until the next separator appears.
        void RemoveSequenceRight();

        /**
        \brief Removes the characters which are currently being selected.
        \see SetSelection
        */
        void RemoveSelection();

        /**
        \brief Returns true if insertion mode is active.
        \return True if 'insertionEnabled' is true, the cursor is not at the end, and nothing is selected.
        \see insertionEnabled
        \see IsCursorEnd
        \see IsSelected
        */
        bool IsInsertionActive() const;

        /**
        \brief Inserts the specified character at the current cursor position or replaces the current selection.
        \see insertionEnabled
        \see RemoveSelection
        */
        void Insert(const Char& chr);

        /**
        \brief Inserts the specified character with some exceptions.
        \param[in] chr Specifies the new character. Special characters are:
        - '\b' which will remove the character before the cursor.
        - char(127) which will remove all characters before the cursor until the next separator appears.
        \see Insert
        */
        virtual void Put(const Char& chr);
        
        //! Inserts the specified text.
        virtual void Put(const String& text);

        //! Sets the content of the text field and clamps the cursor position.
        void SetText(const String& text);

        //! Returns the content of the text field.
        const String& GetText() const
        {
            return text_;
        }

        /* === Members === */

        //! Specifies whether the insertion modd is enabled or not. By default false.
        bool insertionEnabled = false;

        //! Specifies whether selection is enabled or disabled. By default false.
        bool selectionEnabled = false;

        //! Specifies whether the cursor can be moved in a loop or not. By default false.
        bool cursorLoopEnabled = false;

    protected:
        
        //! Returns true if the specified character is valid. By default 'chr' must be in the range [32, +inf).
        virtual bool IsValidChar(const Char& chr) const;

    private:
        
        //! Returns the iterator to the string at the specified cursor position.
        String::iterator Iter();

        //! Returns the constant iterator to the string at the specified cursor position.
        String::const_iterator Iter() const;

        //! Returns the specified position, clamped to the range [0, GetText().size()].
        SizeType ClampedPos(SizeType pos) const;

        //! Updates the cursor- and selection start position to the range [0, GetText().size()].
        void UpdateCursorRange();

        /* === Member === */

        String      text_;
        SizeType    cursorPos_  = 0;
        SizeType    selStart_   = 0;
        
};


} // /namespace Tg


#endif



// ================================================================================
