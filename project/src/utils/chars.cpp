/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : character management toolset
*******************************************************************************/
#include <cstdint>
#include "chars.h"
using namespace utils;


/// @brief Check whether a character from a string is located after an non-escaped mark
/// @param[in] data        Entire string containing the character
/// @param[in] pos         Position of the character inside the string
/// @param[in] mark        Mark indication character (default: quotation mark ('"'))
/// @param[in] escapeChar  Escape indication character (default: backslash)
/// @returns Escaped after mark (true) or not
/// @warning Does not check if the position is out of range
template <typename T>
static bool utils::chars::isAfterMark(const T* data, const uint32_t pos, const T mark, const T escapeChar)
{
    if (pos == 0u || data == nullptr)
        return false;
    --pos;
    
    // count non-escaped marks before position
    bool isOpenMark = false, isFound, isEsc;
    const T* it = &data[pos];
    while (pos >= 0)
    {
        isFound = (*it == mark);
        --pos;
        --it;
        
        if (isFound)
        {
            isEsc = false;
            while (pos >= 0 && *it == escapeChar)
            {
                isEsc = !isEsc;
                --pos;
                --it;
            }
            if (!isEsc)
                isOpenMark = !isOpenMark;
        }
    }
    return isOpenMark;
}


/// @brief Get unicode character traits
/// @param[in] ucode  Unicode character code
/// @returns Character traits for specified unicode character
/// @source  en.wikipedia.org/wiki/List_of_Unicode_characters
template <typename T>
static utils::chars::char_traits_t utils::chars::getCharTraits(const T ucode) noexcept 
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
            else if (ucode >= 0x00A2u) // special alphabetic signs (copyright, pounds, ...)
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
                if (ucode == 0x00DFu) // scharfes-S
                    return char_traits_t { char_family_t::latin, false, ucode }; 
                else if (ucode == 0x00FFu) // y with diaeresis
                    return char_traits_t { char_family_t::latin, false, 0x0178u }; 
                else if (ucode != 0x00F7u)
                    return char_traits_t { char_family_t::latin, false, ucode - 0x0020u }; 
            }
        }
        else if (ucode == 0x0040u || (ucode >= 0x0024u && ucode <= 0x0026u)) // Latin basic signs (ASCII) : $, %, &
            return char_traits_t { char_family_t::latin, false, ucode };
    }
    else if (ucode <= 0x024Fu)
    {
        // Latin extended-A
        if (ucode <= 0x017Fu)
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
                return char_traits_t { char_family_t::latin, false, ucode }; // kra, 'n
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
                    return char_traits_t { char_family_t::latin, false, 0x018Eu }; // turned e
            }
            else if (ucode == 0x01F0u)
                return char_traits_t { char_family_t::latin, false, ucode }; // j with caron
            else if (ucode == 0x01F3u)
                return char_traits_t { char_family_t::latin, false, 0x01F1u }; // grouped dz
            else
                return char_traits_t { char_family_t::latin, true, 0x01F3u }; // grouped DZ, grouped Dz
        }
        else if (ucode >= 0x0246u)
        {
            return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::latin, true, ucode + 0x0001u }
                                             : char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; 
        }
        else if (ucode <= 0x01BFu)
        {
            switch (ucode)
            {
                case 0x0182u: 
                case 0x0184u: 
                case 0x0187u: 
                case 0x018Bu: 
                case 0x0191u: 
                case 0x0198u: 
                case 0x01A0u: 
                case 0x01A2u: 
                case 0x01A4u: 
                case 0x01A7u: 
                case 0x01ACu: 
                case 0x01AFu: 
                case 0x01B3u: 
                case 0x01B5u: 
                case 0x01BCu: return char_traits_t { char_family_t::latin, true, ucode + 0x0001u }; break;
                
                case 0x0183u: 
                case 0x0185u: 
                case 0x0188u: 
                case 0x018Cu: 
                case 0x0192u: 
                case 0x0199u: 
                case 0x01A1u: 
                case 0x01A3u: 
                case 0x01A5u: 
                case 0x01A8u: 
                case 0x01ADu: 
                case 0x01B0u: 
                case 0x01B4u: 
                case 0x01B6u: 
                case 0x01BDu: return char_traits_t { char_family_t::latin, false, ucode - 0x0001u }; break;
                
                case 0x0180u: 
                case 0x018Du: 
                case 0x0195u: 
                case 0x019Au: 
                case 0x019Bu: 
                case 0x019Eu: 
                case 0x01ABu: 
                case 0x01BAu: 
                case 0x01BEu: return char_traits_t { char_family_t::latin, false, ucode }; break;
                
                case 0x018Eu: return char_traits_t { char_family_t::latin, true, 0x01DDu }; break;
                default:      return char_traits_t { char_family_t::latin, true, ucode }; break;
            }
        }
        else if (ucode >= 0x01C4u)
        {
            if (ucode <= 0x01CCu)
            {
                switch (ucode)
                {
                    case 0x01C4u: 
                    case 0x01C7u: 
                    case 0x01CAu: return char_traits_t { char_family_t::latin, true, ucode + 0x0002u }; break;
                    case 0x01C5u: 
                    case 0x01C8u: 
                    case 0x01CBu: return char_traits_t { char_family_t::latin, true, ucode + 0x0001u }; break;
                    default:      return char_traits_t { char_family_t::latin, true, ucode - 0x0002u }; break;
                }
            }
            else if (ucode >= 0x023A)
            {
                switch (ucode)
                {
                    case 0x023Bu: return char_traits_t { char_family_t::latin, true, 0x023Cu }; break;
                    case 0x023Cu: return char_traits_t { char_family_t::latin, false, 0x023Bu }; break;
                    case 0x0241u: return char_traits_t { char_family_t::latin, true, 0x0242u }; break;
                    case 0x0242u: return char_traits_t { char_family_t::latin, false, 0x0241u }; break;
                    case 0x023Fu:
                    case 0x0240u: return char_traits_t { char_family_t::latin, false, ucode }; break;
                    default:      return char_traits_t { char_family_t::latin, true, ucode }; break;
                }
            }
            else
                return char_traits_t { char_family_t::latin, false, ucode };
        }
    }
    
    // Greek
    else if (ucode <= 0x03FFu)
    {
        if (ucode <= 0x03CBu && ucode >= 0x0391u)
        {
            if (ucode <= 0x03ABu)
            {
                return char_traits_t { char_family_t::greek, true, ucode + 0x0020u };
            }
            else if (ucode >= 0x03B1u)
            {
                return char_traits_t { char_family_t::greek, false, ucode - 0x0020u };
            }
            else // 03AC - 03B0
            {
                //...
                return char_traits_t { char_family_t::greek, false, ucode };
                //...
            }
        }
        else if (ucode >= 0x0370u)
        {
            if (ucode <= 0x0377u)
            {
                if (ucode != 0x0374u && ucode != 0x0375u)
                    return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::greek, true, ucode + 0x0001u }
                                                     : char_traits_t { char_family_t::greek, false, ucode - 0x0001u }; 
            }
            else if (ucode >= 0x0388u)
            {
                if (ucode >= 0x03D8u && ucode <= 0x03EFu)
                {
                    return ((ucode & 0x0001u) == 0u) ? char_traits_t { char_family_t::greek, true, ucode + 0x0001u }
                                                     : char_traits_t { char_family_t::greek, false, ucode - 0x0001u }; 
                }
                else if (ucode <= 0x0390u) // 0388 - 0390
                {
                    //...
                    return char_traits_t { char_family_t::greek, false, ucode };
                    //...
                }
                else // 03CC - 03D7, 03F0 - 03FF
                {
                    //...
                    return char_traits_t { char_family_t::greek, false, ucode };
                    //...
                }
            }
            else if (ucode == 0x037Fu)
            {
                return char_traits_t { char_family_t::greek, true, 0x03F3u };
            }
            else if (ucode <= 0x037Du && ucode >= 0x037Bu))
            {
                return char_traits_t { char_family_t::greek, true, ucode };
            }
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
                if (ucode <= 0x1E95u || ucode >= 0x1EA0u)
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
                if (ucode <= 0x1FAFu)
                {
                    if ((ucode & 0xFFF0u) != 0x1F70u)
                    {
                        if ((ucode & 0x000Fu) <= 7u)
                        {
                            if (ucode != 0x1F16u && ucode != 0x1F17u && ucode != 0x1F46u && ucode != 0x1F47u)
                                return char_traits_t { char_family_t::greek, false, ucode + 0x0008u };
                        }
                        else
                        {
                            if (ucode != 0x1F1Eu && ucode != 0x1F1Fu && ucode != 0x1F4Eu && ucode != 0x1F4Fu)
                                return char_traits_t { char_family_t::greek, true, ucode - 0x0008u };
                        }
                    }
                    else // 1F70 - 1F7F
                    {
                        //...
                        return char_traits_t { char_family_t::greek, false, ucode };
                        //...
                    }
                }
                else // 1FB0 - 1FFF
                {
                    //...
                    return char_traits_t { char_family_t::greek, false, ucode };
                    //...
                }
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
static utils::chars::char_family_t utils::chars::getCharFamily(const T ucode) noexcept 
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
        else if (ucode == 0x0040u || (ucode >= 0x0024u && ucode <= 0x0026u))
            return char_family_t::latin; // Latin basic signs (ASCII)
    }
    else if (ucode <= 0x00FFu)
    {
        if (ucode >= 0x00C0u && ucode != 0x00D7u && ucode != 0x00F7u)
            return char_family_t::latin; // Latin-1 supplement
    }
    else if (ucode <= 0x024Fu)
    {
        if (ucode <= 0x017Fu || ucode >= 0x01C4u)
            return char_family_t::latin; // Latin extended-A/B
    }
    else if (ucode <= 0x03FFu)
    {
        if (ucode >= 0x0370u && (ucode >= 0x388u || ucode <= 0x0373u || ucode == 0x0386u || ucode == 0x0376u || ucode == 0x0377u || ucode == 0x037Fu || (ucode >= 0x037Bu && ucode <= 0x037Du)))
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
