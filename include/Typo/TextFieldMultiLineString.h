/*
 * TextFieldMultiLineString.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_TEXT_FIELD_MULTI_LINE_STRING_H__
#define __TG_TEXT_FIELD_MULTI_LINE_STRING_H__


#include "MultiLineString.h"
#include "Point.h"


namespace Tg
{


/**
\brief Text field multi-line string class.
\remarks In a model-view-controller pattern, this is the model for a user input text field.
*/
class TextFieldMultiLineString : private MultiLineString
{
    
    public:
        
        using Point = Point<SizeType>;

        TextFieldMultiLineString(const FontGlyphSet& glyphSet, int maxWidth, const String& text);

        TextFieldMultiLineString& operator = (const String& str);
        
        TextFieldMultiLineString& operator += (const String& str);
        
        TextFieldMultiLineString& operator += (const Char& chr);
        
        inline operator const String& () const
        {
            return MultiLineString::operator const Tg::String &();
        }

        /* --- Cursor operations --- */
        
        //! Sets the new cursor position. This will be clamped to the range [0, GetText().size()] for X and [0, GetLines().size()) for Y.
        void SetCursorPosition(const Point& position);

        inline void SetCursorPosition(SizeType positionX, SizeType positionY)
        {
            SetCursorPosition({ positionX, positionY });
        }

        //! Returns the current cursor position. This is always in the range [0, GetText().size()] for X and [0, GetLines().size()) for Y.
        inline const Point& GetCursorPosition() const
        {
            return cursorPos_;
        }

        //! Returns true if the cursor X position is at the beginning.
        bool IsCursorBegin() const;
        
        //! Returns true if the cursor X position is at the end.
        bool IsCursorEnd() const;

        //! Returns true if the cursor Y position is at the top.
        bool IsCursorTop() const;

        //! Returns true if the cursor Y position is at the bottom.
        bool IsCursorBottom() const;

        //! Moves the cursor into the specified X direction.
        void MoveCursorX(int direction);
        
        //! Moves the cursor into the specified Y direction.
        void MoveCursorY(int direction);
        
        //! Moves the cursor into the specified direction.
        void MoveCursor(int directionX, int directionY);
        
        //! Moves the cursor X position to the beginning.
        void MoveCursorBegin();
        
        //! Moves the cursor X position to the end.
        void MoveCursorEnd();

        //! Moves the cursor Y position to the top.
        void MoveCursorTop();

        //! Moves the cursor Y position to the bottom.
        void MoveCursorBottom();

        //! Jumps to the next left sided space.
        void JumpLeft();
        
        //! Jumps to the next right sided space.
        void JumpRight();

        //! Jumps a specific amount of lines up.
        //void JumpUp();

        //! Jumps a specific amount of lines down.
        //void JumpDown();

        /* --- Selection operations --- */

        /**
        \brief Sets the new selection area.
        \param[in] start Specifies the selection start. This may also be larger than 'end'.
        \param[in] end Specifies the selection end (or rather the new cursor position).
        \remarks This also modifies the cursor position.
        \see SetCursorPosition
        \see selectionEnabled
        */
        void SetSelection(const Point& start, const Point& end);

        /**
        \brief Retrieves the selection range, so that 'start' is always less than or equal to 'end'.
        \see selectionEnabled
        */
        void GetSelection(Point& start, Point& end) const;

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
        Returns the current character which stands immediately before the cursor X position.
        If the cursor is at the very beginning of the text field, the return value is '\0'.
        */
        Char CharLeft() const;
        
        /**
        Returns the current character which stands immediately after the cursor X position.
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
        inline void SetText(const String& text);

        //! Returns the content of the text field.
        inline const String& GetText() const
        {
            return MultiLineString::GetText();
        }

        //! Returns the content of the current line (where the cursor is located).
        const String& GetLineText() const;

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
        
#if 0
        //! Returns the iterator to the string at the specified cursor position.
        String::iterator Iter();

        //! Returns the constant iterator to the string at the specified cursor position.
        String::const_iterator Iter() const;
#endif

        //! Returns the specified X position, clamped to the range { [0, GetText().size()], [0, GetLines().size()) }.
        Point ClampedPos(Point pos) const;

        //! Updates the cursor- and selection start position to the range [0, GetText().size()] for X and [0, GetLines().size()) for Y.
        void UpdateCursorRange();

        /* === Member === */

        Point cursorPos_;
        Point selStart_;
        
};


} // /namespace Tg


#endif



// ================================================================================
