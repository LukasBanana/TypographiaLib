/*
 * SeparableString.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_SEPARABLE_STRING_H__
#define __TG_SEPARABLE_STRING_H__


#include "Details.h"
#include "Char.h"


namespace Tg
{


/**
\brief Base class for all separable strings.
\see MultiLineString
\see TextFieldString
*/
class SeparableString
{
    
    public:
        
        //! String size type alias.
        using SizeType = String::size_type;

        virtual ~SeparableString();
        
        //! Returns a string with all separator characters.
        virtual const String& GetSeparators() const;
        
        /**
        Returns true if the specified character is a separator.
        \see GetSeparators
        */
        bool IsSeparator(const Char& chr) const;

};


} // /namespace Tg


#endif



// ================================================================================
