/*
 * TextField.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_TEXT_FIELD_H__
#define __TG_TEXT_FIELD_H__


#include "SeparableString.h"


namespace Tg
{


//! Text field interface.
class TextField
{
    
    public:
        
        virtual ~TextField()
        {
        }

        /* --- Selection --- */

        /**
        \brief Selects the entire string content.
        \see IsAllSelected
        */
        virtual void SelectAll() = 0;

        /**
        \brief Deselects the current selection.
        \remarks This also disables selection state (see selectionEnabled).
        \see selectionEnabled
        */
        virtual void Deselect() = 0;

        /**
        \brief Returns true if any string part is currently being selected.
        \see GetSelection
        \see IsAllSelected
        */
        virtual bool IsSelected() const = 0;

        /**
        \brief Returns true if the entire string part is currently being selected.
        \see SelectAll
        \see IsSelected
        */
        virtual bool IsAllSelected() const = 0;

        //! Returns the selected text.
        virtual String GetSelectionText() const = 0;

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
        virtual void RemoveSequenceLeft() = 0;

        //! Removes the character sequence on the right of the current cursor position until the next separator appears.
        virtual void RemoveSequenceRight() = 0;

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
        virtual bool IsInsertionActive() const = 0;

        /**
        \brief Inserts the specified character at the current cursor position or replaces the current selection.
        \see insertionEnabled
        \see RemoveSelection
        */
        virtual void Insert(const Char& chr) = 0;

        /**
        \brief Inserts the specified character with some exceptions.
        \param[in] chr Specifies the new character. Special characters are:
        - '\b' which will remove the character before the cursor.
        - char(127) which will remove all characters before the cursor until the next separator appears.
        \see Insert
        */
        virtual void Put(const Char& chr) = 0;
        
        //! Inserts the specified text.
        virtual void Put(const String& text) = 0;

        //! Sets the content of the text field and clamps the cursor position.
        virtual void SetText(const String& text) = 0;

        //! Returns the content of the text field.
        virtual const String& GetText() const = 0;

        //! Returns true if the specified character is valid. By default 'chr' must be in the range [32, +inf).
        virtual bool IsValidChar(const Char& chr) const = 0;

        /* --- Memento --- */

        //! Restores the previous state of the text field.
        virtual void Undo() = 0;

        //! Restores the next state (if "Undo" was called previously).
        virtual void Redo() = 0;

        //! Returns true if "Undo" can be used. Otherwise nothing has been stored in the internal memento.
        virtual bool CanUndo() const = 0;

        //! Returns true if "Redo" can be used. Otherwise the internal memento state is at the end.
        virtual bool CanRedo() const = 0;

};


} // /namespace Tg


#endif



// ================================================================================
