/*
 * TextFieldMultiLineString.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef TG_TEXT_FIELD_MULTI_LINE_STRING_H
#define TG_TEXT_FIELD_MULTI_LINE_STRING_H


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

        /* --- Text position conversion --- */

        /**
        \brief Returns the index (beginning with 0) of the specified position within this text field.
        \remarks This can be used in conjunction with function like "SetSelection".
        \see GetTextPosition
        */
        SizeType GetTextIndex(const Point& position) const;

        /**
        \brief Returns the position of the specified index (beginning with 0) within this text field.
        \remarks This can be used in conjunction with function like "SetSelectionCoordinate".
        \see GetTextIndex
        */
        Point GetTextPosition(SizeType index) const;

        /**
        \brief Converts the specified X coordinate into a text position index.
        \param[in] coordinateX Specifies the X coordinate which is to be converted.
        This will be clamped to the range [0, GetTextLine()].
        \remarks This can be used for fonts with non-uniform glyph sizes.
        */
        SizeType GetXPositionFromCoordinate(SizeType coordinateX, std::size_t lineIndex) const;

        /**
        \brief Converts the specified text position index into an X coordinate.
        \remarks This can be used for fonts with non-uniform glyph sizes.
        */
        SizeType GetXCoordinateFromPosition(SizeType positionX, std::size_t lineIndex) const;

        /* --- Cursor operations --- */
        
        //! Sets the new cursor XY coordinate. This will be clamped to the range [0, GetText().size()] for X and [0, GetLines().size()) for Y.
        void SetCursorCoordinate(Point position);

        inline void SetCursorCoordinate(SizeType positionX, SizeType positionY)
        {
            SetCursorCoordinate({ positionX, positionY });
        }

        //! Returns the current cursor XY coordinate. This is always in the range [0, GetText().size()] for X and [0, GetLines().size()) for Y.
        Point GetCursorCoordinate() const;

        //! Returns true if the cursor Y position is at the top.
        bool IsCursorTop() const;

        //! Returns true if the cursor Y position is at the bottom.
        bool IsCursorBottom() const;

        //! Moves the cursor into the specified X direction.
        void MoveCursor(int direction) override;
        
        //! Moves the cursor into the specified Y direction.
        void MoveCursorLine(int direction);
        
        //! Moves the cursor X position to the beginning.
        void MoveCursorBegin();
        
        //! Moves the cursor X position to the end.
        void MoveCursorEnd();

        //! Moves the cursor Y position to the top.
        void MoveCursorTop();

        //! Moves the cursor Y position to the bottom.
        void MoveCursorBottom();

        //! Jumps to the previous text passage (separated by empty lines).
        void JumpUp();

        //! Jumps to the next text passage (separated by empty lines).
        void JumpDown();

        /* --- Selection operations --- */

        //! \see SetSelection
        void SetSelectionCoordinate(const Point&  start, const Point& end);

        //! \see GetSelection
        void GetSelectionCoordinate(Point& start, Point& end) const;

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

        //! Returns the current line (where the cursor is located.
        const MultiLineString::TextLine& GetLine() const;

        //! Returns the specified line.
        const MultiLineString::TextLine& GetLine(std::size_t lineIndex) const;

        //! Returns the content of the current line (where the cursor is located).
        const String& GetLineText() const;

        //! Returns the content of the specified line.
        const String& GetLineText(std::size_t lineIndex) const;

        /* === Members === */

        //! Specifies whether cursor movement wraps around complete lines. By default false.
        bool wrapLines = false;

    private:

        void InsertChar(Char chr, bool wasSelected) override;
        
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

        //! Stores the current cursor coordinate X (uses GetXCoordinateFromPosition).
        void StoreCursorCoordX();

        //! Restores the previous cursor coordinate X (uses GetXPositionFromCoordinate) at the specified Y coordinate.
        void RestoreCursorCoordX(SizeType lineIndex);

        /* === Member === */

        MultiLineString text_;

        SizeType        storedCursorCoordX_ = 0;
        
};


} // /namespace Tg


#endif



// ================================================================================
