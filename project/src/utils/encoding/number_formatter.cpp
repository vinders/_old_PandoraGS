/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : number formatter
*******************************************************************************/
#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <cmath>
#include "number_formatter.h"

using namespace utils::encoding;


/// @brief Convert number to binary string
/// @param[in] val  Value to format
template <typename T, uint32_t Base>
std::string NumberFormatter::format(const T val)
{
    static_assert(Base >= 2 && Base <= 64);
    std::string serialized;
    serialized.reserve(40 - Base/2); // big enough for any number
    
    // set sign
    bool isNegative = false;
    std::div_t buffer(val);
    if (val < static_cast<T>(0))
    {
        isNegative = true;
        buffer = std::div_t(-val);
    }

    // serialize reversed digits
    if (Base <= 10u)
    {
        do 
        {
            buffer = std::div(buffer.quot, Base);
            serialized += '0' + buffer.rem; 
        } while (buffer.quot);
    }
    else if (Base <= 64u)
    {
        char figures[65];
        if (Base <= 16u)
            memcpy(figures, "0123456789abcdef\0", 17u);
        else if (Base <= 32u)
            memcpy(figures, "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567\0", 33u);
        else
            memcpy(figures, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/+\0", 65u);
        
        do 
        {
            buffer = std::div(buffer.quot, Base);
            serialized += figures[std::abs(buffer.rem)]; 
        } while (buffer.quot);
    }
    
    // append sign
    if (isNegative)
        serialized += '-';
    return std::string(serialized.rbegin(), serialized.rend()); // reverse order
}


/// @brief Convert number to binary string
/// @param[in] val  Value to format
template <typename T, uint32_t Base>
std::string NumberFormatter::format64(const T val)
{
    static_assert(Base >= 2 && Base <= 64);
    std::string serialized;
    serialized.reserve(40 - Base/2); // big enough for any number
    
    // set sign
    bool isNegative = false;
    std::lldiv_t buffer(val);
    if (val < static_cast<T>(0))
    {
        isNegative = true;
        buffer = std::lldiv_t(-val);
    }

    // serialize reversed digits
    if (Base <= 10u)
    {
        do 
        {
            buffer = std::lldiv(buffer.quot, Base);
            serialized += '0' + buffer.rem; 
        } while (buffer.quot);
    }
    else if (Base <= 64u)
    {
        char figures[65];
        if (Base <= 16u)
            memcpy(figures, "0123456789abcdef\0", 17u);
        else if (Base <= 32u)
            memcpy(figures, "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567\0", 33u);
        else
            memcpy(figures, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/+\0", 65u);
        
        do 
        {
            buffer = std::lldiv(buffer.quot, Base);
            serialized += figures[std::abs(buffer.rem)]; 
        } while (buffer.quot);
    }
    
    // append sign
    if (isNegative)
        serialized += '-';
    return std::string(serialized.rbegin(), serialized.rend()); // reverse order
}
