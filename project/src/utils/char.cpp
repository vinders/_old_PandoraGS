/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : character management toolset
*******************************************************************************/
#include <cstdint>
#include "char.h"
using namespace utils;


/// @brief Get unicode character traits
/// @param[in] ucode  Unicode character code
/// @returns Character traits for specified unicode character
/// @source  en.wikipedia.org/wiki/List_of_Unicode_characters
template <typename T>
static utils::Char::char_traits_t utils::Char::getCharTraits(const T ucode) noexcept 
{ 
    if (ucode <= 0x00FFu)
    {
        // Latin basic (ASCII)
        if (ucode <= 0x007Au && ucode >= 0x0041u)
        {
            if (ucode <= 0x005Au) // upper-case
                return char_traits_t { char_family_t::latin, true, ucode + 0x0020u }; 
            else if (ucode >= 0x0061u) // lower-case
                return char_traits_t { char_family_t::latin, false, ucode - 0x0020u }; 
            else if (ucode >= 0x00A2u) // special alphabetic characters
            {
                if (ucode <= 0x00A5u || ucode == 0x00A9u || ucode == 0x00AEu || ucode == 0x00B5u)
                    return char_traits_t { char_family_t::latin, false, ucode }; 
            }
        }
        
        // Latin-1 supplement
        else if (ucode >= 0x00C0u)
        {
            if (ucode <= 0x00DEu)
            {
                if (ucode != 0x00D7u)
                    return char_traits_t { char_family_t::latin, true, ucode + 0x0020u }; 
            }
            else
            {
                if (ucode == 0x00DFu)
                    return char_traits_t { char_family_t::latin, false, ucode }; 
                else if (ucode == 0x00FFu)
                    return char_traits_t { char_family_t::latin, false, 0x0178u }; 
                else if (ucode != 0x00F7u)
                    return char_traits_t { char_family_t::latin, false, ucode - 0x0020u }; 
            }
        }
    }
    else if (ucode <= 0x024Fu)
    {
        // Latin extended-A
        if (ucode <= 0x017Fu && ucode >= 0x0100u)
        {
            if (ucode <= 0x0177u && (ucode <= 0x0137u || ucode >= 0x014Au))
            {
                return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                 : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
            }
            else if (ucode >= 0x0179u || (ucode <= 0x0148u && ucode >= 0x0139u))
            {
                return ((ucode & 0x0001u) != 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                 : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
            }
            else if (ucode == 0x0178u)
                return char_traits_t { char_family_t::latin, true, 0x00FFu };
            else
                return char_traits_t { char_family_t::latin, false, ucode };
        }
        
        // Latin extended-B
        else if (ucode >= 0x01CDu && ucode <= 0x0233u)
        {
            if (ucode <= 0x01DCu)
            {
                return ((ucode & 0x0001u) != 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                 : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
            }
            else if (ucode <= 0x01EFu || ucode >= 0x01F4u)
            {
                if (ucode != 0x01DDu)
                {
                    return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                     : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
                }
                else
                    return char_traits_t { char_family_t::latin, false, ucode };
            }
            else if (ucode == 0x01F0u)
                return char_traits_t { char_family_t::latin, false, ucode };
            else
                return char_traits_t { char_family_t::latin, true, ucode };
        }
        else if (ucode >= 0x0246u)
        {
            return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                             : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
        }
        else if (ucode <= 0x01BFu || ucode >= 0x00C4u)
            return char_traits_t { char_family_t::latin, true, ucode };
    }
    
    // Greek
    else if (ucode <= 0x03FFu)
    {
        if (ucode >= 0x0370u)
        {
            //...
            return char_traits_t { char_family_t::greek, false, ucode };
            //...
        }
    }
    
    // Cyrillic
    else if (ucode <= 0x0520u)
    {
        //...
        return char_traits_t { char_family_t::cyrillic, false, ucode };
        //...
    }
    
    else if (ucode >= 0x1C80u)
    {
        if (ucode >= 0x1E00u) 
        {
            // Latin extended additional
            if (ucode <= 0x1EFFu)
            {
                if (ucode <= 0x1E95u || ucode >= 0x1E9Eu)
                {
                    return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                                     : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
                }
                else
                    return char_traits_t { char_family_t::latin, false, ucode };
            }

            // Greek extended
            else if (ucode <= 0x1FFFu)
            {
                //...
                return char_traits_t { char_family_t::greek, false, ucode };
                //...
            }
            
            // Cyrillic extended-A
            else if (ucode <= 0x2DFFu)
            {
                if (ucode >= 0x2DE0u)
                {
                    //...
                    return char_traits_t { char_family_t::cyrillic, false, ucode };
                    //...
                }
            }
            // Cyrillic extended-B
            else if (ucode >= 0xA640u && ucode <= 0xA69Fu)
            {
                //...
                return char_traits_t { char_family_t::cyrillic, false, ucode };
                //...
            }
        }
        // Cyrillic extended-C
        else if (ucode <= 0x1C8Fu)
        {
            //...
            return char_traits_t { char_family_t::cyrillic, false, ucode };
            //...
        }
    }
    return char_traits_t { char_family_t::not_alpha, false, Ucode }; 
}


/// @brief Get unicode character alphabetic family
/// @param[in] ucode  Unicode character code
/// @returns Alphabetic character family
/// @source  en.wikipedia.org/wiki/List_of_Unicode_characters
template <typename T>
static utils::Char::char_family_t utils::Char::getCharFamily(const T ucode) noexcept 
{ 
    if (ucode <= 0x00B5u)
    {
        if (ucode >= 0x0041u)
        {
            if (ucode <= 0x007Au && (ucode >= 0x0061u || ucode <= 0x005Au))
                return char_family_t::latin; // Latin basic (ASCII)
            else if (ucode >= 0x00A2u && (ucode <= 0x00A5u || ucode == 0x00A9u || ucode == 0x00AEu || ucode == 0x00B5u))
                return char_family_t::latin; // Latin basic - special characters (ASCII)
        }
    }
    else if (ucode <= 0x00FFu)
    {
        if (ucode >= 0x00C0u && ucode != 0x00D7u && ucode != 0x00F7u)
            return char_family_t::latin; // Latin-1 supplement
    }
    else if (ucode <= 0x024Fu)
    {
        if ((ucode >= 0x00C4u && ucode <= 0x01BFu) || (ucode >= 0x01CDu && ucode <= 0x0233u) || ucode >= 0x0246u)
            return char_family_t::latin; // Latin extended-A/B
    }
    else if (ucode <= 0x03FFu)
    {
        if (ucode >= 0x0370u)
            return char_family_t::greek; // Greek
    }
    else if (ucode <= 0x0520u)
    {
        return char_family_t::cyrillic; // Cyrillic
    }
    
    else if (ucode >= 0x1C80u)
    {
        if (ucode >= 0x1E00u) 
        {
            if (ucode <= 0x1EFFu)
            {
                return char_family_t::latin; // Latin extended additional
            }
            else if (ucode <= 0x1FFFu)
            {
                return char_family_t::greek; // Greek extended
            }
            else if (ucode <= 0x2DFFu)
            {
                if (ucode >= 0x2DE0u)
                    return char_family_t::cyrillic; // Cyrillic extended-A
            }
            else if (ucode >= 0xA640u && ucode <= 0xA69Fu)
            {
                return char_family_t::cyrillic; // Cyrillic extended-B
            }
        }
        else if (ucode <= 0x1C8Fu)
        {
            return char_family_t::cyrillic; // Cyrillic extended-C
        }
    }
    return char_family_t::not_alpha;
}
