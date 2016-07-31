/*
 * TextFieldMultiLineString.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_TEXT_FIELD_MULTI_LINE_STRING_H__
#define __TG_TEXT_FIELD_MULTI_LINE_STRING_H__


#include "MultiLineString.h"
#include "TextField.h"
#include "Point.h"


namespace Tg
{


/**
\brief Text field multi-line string class.
\remarks In a model-view-controller pattern, this is the model for a user input text field.
*/
class TextFieldMultiLineString : public TextField
{
    
    public:
        
        TextFieldMultiLineString(const FontGlyphSet& glyphSet, int maxWidth, const String& text);

        TextFieldMultiLineString& operator = (const String& str);
        
        TextFieldMultiLineString& operator += (const String& str);
        
        TextFieldMultiLineString& operator += (Char chr);
        
        inline operator const String& () const
        {
            return text_;
        }

        /* --- Cursor operations --- */
        
        //! Sets the cursor position by the specified text index.
        void SetCursorIndex(SizeType index);

        //! Returns the text index of the current cursor position.
        inline SizeType GetCursorIndex() const
        {
            return cursorPos_;
        }

        //! Sets the new cursor XY coordinate. This will be clamped to the range [0, GetText().size()] for X and [0, GetLines().size()) for Y.
        void SetCursorCoordinate(Point position);

        inline void SetCursorCoordinate(SizeType positionX, SizeType positionY)
        {
            SetCursorCoordinate({ positionX, positionY });
        }

        //! Returns the current cursor XY coordinate. This is always in the range [0, GetText().size()] for X and [0, GetLines().size()) for Y.
        Point GetCursorCoordinate() const;

        //! Returns true if the cursor is at the beginning.
        bool IsCursorBegin() const override;
        
        //! Returns true if the cursor is at the end.
        bool IsCursorEnd() const override;

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
        void JumpLeft() override;
        
        //! Jumps to the next right sided space.
        void JumpRight() override;

        //! Jumps to the previous text passage (separated by empty lines).
        void JumpUp();

        //! Jumps to the next text passage (separated by empty lines).
        void JumpDown();

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

        /**
        \brief Selects the entire string content.
        \see IsAllSelected
        */
        void SelectAll() override;

        /**
        \brief Deselects the current selection.
        \remarks This also disables selection state (see selectionEnabled).
        \see selectionEnabled
        */
        void Deselect() override;

        /**
        \brief Returns true if any string part is currently being selected.
        \see GetSelection
        \see IsAllSelected
        */
        bool IsSelected() const override;

        /**
        \brief Returns true if the entire string part is currently being selected.
        \see SelectAll
        \see IsSelected
        */
        bool IsAllSelected() const override;

        /**
        \brief Returns the selected text.
        \remarks This is different to TextFieldString::GetSelectionText due to multiple lines:
        Each line break contains the new line character '\n' at the end,
        even if the line break only appears due to the restricted string area.
        */
        String GetSelectionText() const override;

        /* --- String content --- */

        /**
        Returns the current character which stands immediately before the cursor X position.
        If the cursor is at the very beginning of the text field, the return value is '\0'.
        */
        Char CharLeft() const override;
        
        /**
        Returns the current character which stands immediately after the cursor X position.
        If the cursor is at the very end of the text field, the return value is '\0'.
        */
        Char CharRight() const override;

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

        //! \see MultiLineString::SetGlyphSet
        inline void SetGlyphSet(const FontGlyphSet& glyphSet)
        {
            text_.SetGlyphSet(glyphSet);
        }
        
        //! \see MultiLineString::GetGlyphSet
        inline const FontGlyphSet& GetGlyphSet() const
        {
            return text_.GetGlyphSet();
        }

        //! Sets the new maximal width.
        void SetMaxWidth(int maxWidth);

        //! \see MultiLineString::GetMaxWidth
        inline int GetMaxWidth() const
        {
            return text_.GetMaxWidth();
        }

        //! \see MultiLineString::GetWidth
        inline int GetWidth() const
        {
            return text_.GetWidth();
        }

        //! Sets the content of the text field and clamps the cursor position.
        void SetText(const String& text) override;

        //! \see MultiLineString::GetText
        const String& GetText() const override;

        //! \see MultiLineString::GetLines
        inline const std::vector<MultiLineString::TextLine>& GetLines() const
        {
            return text_.GetLines();
        }

        //! Returns the content of the current line (where the cursor is located).
        const String& GetLineText() const;

        //! Returns the content of the specified line.
        const String& GetLineText(std::size_t lineIndex) const;

        /* === Members === */

        //! Specifies whether selection is enabled or disabled. By default false.
        bool selectionEnabled   = false;

        //! Specifies whether cursor movement wraps around complete lines. By default false.
        bool wrapLines          = false;

    private:
        
        //! Returns the specified X position, clamped to the range { [0, GetText().size()], [0, GetLines().size()) }.
        SizeType ClampedPos(SizeType pos) const;

        //! Updates the cursor- and selection start position to the range [0, GetText().size()] for X and [0, GetLines().size()) for Y.
        void UpdateCursorRange();

        /**
        \brief Returns true if the line above the cursor is empty (also true if the cursor is at the top).
        \remarks This function must not be called, if the cursor is at the top!
        */
        bool IsUpperLineEmpty() const;

        /**
        \brief Returns true if the line below the cursor is empty (also true if the cursor is at the bottom).
        \remarks This function must not be called, if the cursor is at the bottom!
        */
        bool IsLowerLineEmpty() const;

        void StoreCursorCoordX();
        void RestoreCursorCoordX();

        /* === Member === */

        MultiLineString text_;

        SizeType        cursorPos_          = 0;
        SizeType        selStart_           = 0;

        SizeType        storedCursorCoordX_ = 0;
        
};


} // /namespace Tg


#endif



// ================================================================================
