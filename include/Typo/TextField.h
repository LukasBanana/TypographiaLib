/*
 * TextField.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_TEXT_FIELD_H
#define TG_TEXT_FIELD_H


#include "Char.h"

#include <stack>
#include <string>
#include <list>


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

        //! Moves the cursor into the specified direction.
        virtual void MoveCursor(int direction) = 0;

        //! Jumps to the next left sided space.
        void JumpLeft();
        
        //! Jumps to the next right sided space.
        void JumpRight();

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
        virtual void Insert(Char chr);

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

        /**
        \brief Clears this text field
        \remarks The default implementation is equivalent to:
        \code
        SetText("");
        \endcode
        \see SetText
        */
        virtual void Clear();

        //! Returns true if the specified character is valid. By default 'chr' must be in the range [32, +inf).
        virtual bool IsValidChar(Char chr) const = 0;

        /**
        \brief Returns true if the specified character is a separator.
        \remarks Separators are used to determine where to stop when the cursor jumps left or right.
        \see JumpLeft
        \see JumpRight
        */
        virtual bool IsSeparator(Char chr) const;

        /* --- Memento --- */

        /**
        \brief Stores the current selection state (this is stored in an internal stack).
        \see RestoreSelection
        */
        void StoreSelection();
        
        /**
        \brief Restores the previous selection state (this is restored from an internal stack).
        \see StoreSelection
        */
        void RestoreSelection();

        /**
        \brief Specifies how large the memento history can be. By default 10 memento states can be stored.
        \see StoreMemento
        */
        void SetMementoSize(std::size_t size);

        //! Returns the size of the memento history.
        inline std::size_t GetMementoSize() const
        {
            return mementoSize_;
        }

        /**
        \brief Stores the current text and cursor position state in the memento history.
        \remarks The field 'mementoSize' can be used to specify how many memento states can be stored.
        \see Undo
        \see Redo
        \see SetMementoSize
        */
        virtual void StoreMemento();

        //! Restores the previous state of the text field.
        virtual void Undo();

        //! Restores the next state (if "Undo" was called previously).
        virtual void Redo();

        //! Returns true if "Undo" can be used. Otherwise nothing has been stored in the internal memento.
        virtual bool CanUndo() const;

        //! Returns true if "Redo" can be used. Otherwise the internal memento state is at the end.
        virtual bool CanRedo() const;

        /* === Members === */

        //! Specifies whether the insertion modd is enabled or not. By default false.
        bool insertionEnabled = false;

        //! Specifies whether selection is enabled or disabled. By default false.
        bool selectionEnabled = false;

    protected:

        //! Updates the cursor- and selection start position to the range [0, GetText().size()].
        void UpdateCursorRange();

        /**
        \brief Inserts the specified character at the current cursor position.
        \remarks This is called by the "TextField::Insert" function.
        \see Insert
        */
        virtual void InsertChar(Char chr, bool wasSelected) = 0;

    private:

        struct SelectionState
        {
            SizeType cursorPos, selStart;
        };

        struct MementoState
        {
            SizeType    cursorPos;
            std::string text;
        };

        using MementoStateList = std::list<MementoState>;

        //! Returns the specified position, clamped to the range [0, GetText().size()].
        SizeType ClampedPos(SizeType pos) const;

        //! Restores the specified memento state.
        void RestoreMemento(std::size_t index);

        MementoStateList::const_iterator GetMementoStateIter(std::size_t index) const;

        void StoreMementoForChar(Char chr);

        /* === Members === */

        SizeType                    cursorPos_          = 0;
        SizeType                    selStart_           = 0;

        std::stack<SelectionState>  selectionStates_;

        std::size_t                 mementoSize_        = 10;
        MementoStateList            mementoStates_;
        std::size_t                 mementoStatesIndex_ = 0;
        bool                        mementoExpired_     = false;

        Char                        prevPutChar_        = 0;

};


} // /namespace Tg


#endif



// ================================================================================
