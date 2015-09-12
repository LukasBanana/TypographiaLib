/*
 * SeparableString.h
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __TG_SEPARABLE_STRING_H__
#define __TG_SEPARABLE_STRING_H__


#include "Details.h"

#include <string>


namespace Tg
{


/**
\brief Base template class for all separable strings.
\see MultiLineString
\see TextFieldString
*/
template <typename T>
class SeparableString
{
    
    public:
        
        //! Basic string type alias.
        using StringType = std::basic_string<T>;
        
        virtual ~SeparableString()
        {
        }
        
        //! Returns a string with all separator characters.
        virtual const StringType& GetSeparators() const
        {
            static const StringType sep = StringType(Details::DefaultSeparators<T>::value);
            return sep;
        }
        
        /**
        Returns true if the specified character is a separator.
        \see GetSeparators
        */
        bool IsSeparator(const T& chr) const
        {
            return GetSeparators().find(chr) != StringType::npos;
        }

};


} // /namespace Tg


#endif



// ================================================================================
