/*
 * TextField.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_TEXT_FIELD_H__
#define __TG_TEXT_FIELD_H__


#include "Char.h"


namespace Tg
{


//! Base class for all text fields.
class TextField
{
    
    public:
        
        //! String size type alias.
        using SizeType = String::size_type;

        virtual ~TextField();

        /* --- Cursor operations --- */
        
        //! Sets the new cursor position. This will be clamped to the range [0, GetText().size()].
        void SetCursorPosition(SizeType position);

        //! Returns the current cursor position. This is always in the range [0, GetText().size()].
        inline SizeType GetCursorPosition() const
        {
            return cursorPos_;
        }

        //! Returns true if the cursor is at the beginning.
        bool IsCursorBegin() const;
        
        //! Returns true if the cursor is at the end.
        bool IsCursorEnd() const;

        //! Jumps to the next left sided space.
        virtual void JumpLeft() = 0;
        
        //! Jumps to the next right sided space.
        virtual void JumpRight() = 0;

        /* --- Selection --- */

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

        /**
        \brief Selects the entire string content.
        \see IsAllSelected
        */
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
        \see IsAllSelected
        */
        bool IsSelected() const;

        /**
        \brief Returns true if the entire string part is currently being selected.
        \see SelectAll
        \see IsSelected
        */
        bool IsAllSelected() const;

        //! Returns the selected text.
        String GetSelectionText() const;

        /* --- String content --- */

        /**
        Returns the current character which stands immediately before the cursor position.
        If the cursor is at the very beginning of the text field, the return value is '\0'.
        */
        virtual Char CharLeft() const = 0;
        
        /**
        Returns the current character which stands immediately after the cursor position.
        If the cursor is at the very end of the text field, the return value is '\0'.
        */
        virtual Char CharRight() const = 0;

        //! Removes the character on the left of the current cursor position.
        virtual void RemoveLeft() = 0;

        //! Removes the character on the right of the current cursor position.
        virtual void RemoveRight() = 0;

        //! Removes the character sequence on the left of the current cursor position until the next separator appears.
        virtual void RemoveSequenceLeft();

        //! Removes the character sequence on the right of the current cursor position until the next separator appears.
        virtual void RemoveSequenceRight();

        /**
        \brief Removes the characters which are currently being selected.
        \see SetSelection
        */
        virtual void RemoveSelection() = 0;

        /**
        \brief Returns true if insertion mode is active.
        \return True if 'insertionEnabled' is true, the cursor is not at the end, and nothing is selected.
        \see insertionEnabled
        \see IsCursorEnd
        \see IsSelected
        */
        virtual bool IsInsertionActive() const;

        /**
        \brief Inserts the specified character at the current cursor position or replaces the current selection.
        \see insertionEnabled
        \see RemoveSelection
        */
        virtual void Insert(Char chr) = 0;

        /**
        \brief Inserts the specified character with some exceptions.
        \param[in] chr Specifies the new character. Special characters are:
        - '\b' which will remove the character before the cursor.
        - char(127) which will remove all characters before the cursor until the next separator appears.
        \see Insert
        */
        virtual void Put(Char chr);
        
        //! Inserts the specified text.
        virtual void Put(const String& text);

        //! Sets the content of the text field and clamps the cursor position.
        virtual void SetText(const String& text) = 0;

        //! Returns the content of the text field.
        virtual const String& GetText() const = 0;

        //! Returns true if the specified character is valid. By default 'chr' must be in the range [32, +inf).
        virtual bool IsValidChar(Char chr) const = 0;

        /**
        \brief Returns true if the specified character is a separator.
        \see GetSeparators
        */
        virtual bool IsSeparator(Char chr) const;

#if 0
        /* --- Memento --- */

        //! Restores the previous state of the text field.
        virtual void Undo() = 0;

        //! Restores the next state (if "Undo" was called previously).
        virtual void Redo() = 0;

        //! Returns true if "Undo" can be used. Otherwise nothing has been stored in the internal memento.
        virtual bool CanUndo() const = 0;

        //! Returns true if "Redo" can be used. Otherwise the internal memento state is at the end.
        virtual bool CanRedo() const = 0;
#endif

        /* === Members === */

        //! Specifies whether the insertion modd is enabled or not. By default false.
        bool insertionEnabled = false;

        //! Specifies whether selection is enabled or disabled. By default false.
        bool selectionEnabled = false;

    protected:

        //! Updates the cursor- and selection start position to the range [0, GetText().size()].
        void UpdateCursorRange();

    private:

        //! Returns the specified position, clamped to the range [0, GetText().size()].
        SizeType ClampedPos(SizeType pos) const;

        SizeType cursorPos_ = 0;
        SizeType selStart_  = 0;

};


} // /namespace Tg


#endif



// ================================================================================
