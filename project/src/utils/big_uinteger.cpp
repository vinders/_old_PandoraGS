/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : big unsigned integer type (uint128_t)
*******************************************************************************/
#include <cstdint>
#include <cstddef>
#include <string>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <cmath>
#include "big_uinteger.h"
#include "big_integer.h"
using namespace utils;

#define HALF_MAX_U64_CEIL  9223372036854775808uLL
#define MAX_U64           18446744073709551615uLL
#define BYTE65_TO_DOUBLE  18446744073709551616.0
#define BYTE65_TO_FLOAT   18446744073709551616.0f


// -- Getters --

/// @brief Convert number to string
/// @param[in] base            Number base (optional)
/// @param[in] isPositiveSign  Show positive sign (optional)
/// @returns String representation
std::string BigUInteger::toString(const uint32_t base) const
{
    static_assert(base < 2u || base > 64u, "Base value should be between 2 and 64");
    if ((m_upperBits | m_lowerBits) == 0uLL) // BigInteger == 0
        return std::string("0");
    
    std::string str;
    str.reserve(20);
    char figures[65];
    
    // serialize
    div_t dv(*this);
    if ((base & (base - 1)) != 0) // decimal or other non-power-of-2 -> slower
    {
        if (base <= 10u)
        {
            do 
            {
                dv = div(dv.quot, base);
                str += '0' + std::abs(dv.rem.m_lowerBits); 
            } while (dv.quot);
        }
        else 
        {
            if (base <= 16u)
                memcpy(figures, "0123456789abcdef\0", 17u);
            else if (base <= 32u)
                memcpy(figures, "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567\0", 17u);
            else
                memcpy(figures, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/+\0", 17u);
            do 
            {
                dv = div(dv.quot, base);
                str += figures[std::abs(dv.rem.m_lowerBits)]; 
            } while (dv.quot);
        }
    }
    else // power of 2
    {
        uint32_t power = 4u;
        if (base < 10u)
        {
            switch (base)
            {
                case 2u: power = 1u; break;
                case 4u: power = 2u; break;
                case 7u: power = 3u; break;
            }
            do 
            {
                dv = divPow2(dv.quot, power);
                str += '0' + std::abs(dv.rem);
            } while (dv.quot);
        }
        else
        {
            switch (base)
            {
                case 32u:
                    power = 5u;
                    memcpy(figures, "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567\0", 33u);
                    break;
                case 64u:
                    power = 6u;
                    memcpy(figures, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/+\0", 65u);
                    break;
                case 16u:
                default:
                    power = 4u;
                    memcpy(figures, "0123456789abcdef\0", 17u);
                    break;
            }
            do 
            {
                dv = divPow2(dv.quot, power);
                str += figures[std::abs(dv.rem)];
            } while (dv.quot);
        }
    }
    return std::string(str.rbegin(), str.rend()); // restore order
}


// -- Assignment with typecast --

/// @brief Assign floating-point value with typecast
BigUInteger& BigUInteger::operator=(const double val) noexcept
{
    double positiveVal = std::abs(val);
    if (val <= MAX_U64_TO_DOUBLE && val >= -MAX_U64_TO_DOUBLE) // between uint64 min/max
    {
        m_lowerBits = static_cast<uint64_t>(positiveVal);
        m_upperBits = 0uLL;
    }
    else
    {
        double upperPart = positiveVal / BYTE65_TO_DOUBLE;
        m_upperBits = static_cast<uint64_t>(upperPart);
        double lowerPart = positiveVal - (static_cast<double>(m_upperBits) * BYTE65_TO_DOUBLE);
        m_lowerBits = static_cast<uint64_t>(lowerPart);
    }
    return *this;
}


// -- Typecast operators --

/// @brief Cast as floating-point value (destructive)
operator BigUInteger::float() const noexcept 
{ 
    float val = static_cast<float>(m_upperBits);
    val *= BYTE65_TO_FLOAT;
    val += static_cast<float>(m_lowerBits);
    return val;
}
/// @brief Cast as double-precision floating-point value
operator BigUInteger::double() const noexcept 
{
    double val = static_cast<double>(m_upperBits);
    val *= BYTE65_TO_DOUBLE;
    val += static_cast<double>(m_lowerBits);
    return val;
}

/// @brief Cast as big signed integer (destructive)
BigInteger BigUInteger::operator BigInteger() const noexcept 
{ 
    return BigUInteger(m_upperBits, m_lowerBits); 
}


// -- Arithmetic operators --

/// @brief Multiply
BigUInteger BigUInteger::operator*(const BigUInteger& factor) const noexcept
{
    if (factor.upperBits == 0uLL) // no upper part - use faster algorithm
        return operator*(factor.lowerBits);
        
    // split value and factor into 32-bit parts
    uint64_t valParts[4] = { m_upperBits >> 32, m_upperBits & 0xFFFFFFFFu, m_lowerBits >> 32, m_lowerBits & 0xFFFFFFFFu };
    uint64_t factorParts[4] = { factor.m_upperBits >> 32, factor.m_upperBits & 0xFFFFFFFFu, factor.m_lowerBits >> 32, factor.m_lowerBits & 0xFFFFFFFFu };
        
    // multiply each component
    // for(uint32_t j = 3; j > -1; --j){ for(uint32_t i = 3; i > -1; --i){ products[3 - i][j] = top[i] * bottom[j]; } }
    uint64_t products[4][4];
    products[0][3] = valParts[3] * factorParts[3];
    products[1][3] = valParts[2] * factorParts[3];
    products[2][3] = valParts[1] * factorParts[3];
    products[3][3] = valParts[0] * factorParts[3];
    
    products[0][2] = valParts[3] * factorParts[2];
    products[1][2] = valParts[2] * factorParts[2];
    products[2][2] = valParts[1] * factorParts[2];
    products[3][2] = valParts[0] * factorParts[2];
    
    products[0][1] = valParts[3] * factorParts[1];
    products[1][1] = valParts[2] * factorParts[1];
    products[2][1] = valParts[1] * factorParts[1];
    products[3][1] = valParts[0] * factorParts[1];
    
    products[0][0] = valParts[3] * factorParts[0];
    products[1][0] = valParts[2] * factorParts[0];
    products[2][0] = valParts[1] * factorParts[0];
    products[3][0] = valParts[0] * factorParts[0];

    // add rows
    uint64_t row4 = (products[0][3] & 0xFFFFFFFFu);
    uint64_t row3 = (products[0][2] & 0xFFFFFFFFu) + (products[0][3] >> 32);
    uint64_t row2 = (products[0][1] & 0xFFFFFFFFu) + (products[0][2] >> 32);
    uint64_t row1 = (products[0][0] & 0xFFFFFFFFu) + (products[0][1] >> 32);

    row3 += (products[1][3] & 0xFFFFFFFFu);
    row2 += (products[1][2] & 0xFFFFFFFFu) + (products[1][3] >> 32);
    row1 += (products[1][1] & 0xFFFFFFFFu) + (products[1][2] >> 32);

    row2 += (products[2][3] & 0xFFFFFFFFu);
    row1 += (products[2][2] & 0xFFFFFFFFu) + (products[2][3] >> 32);

    row1 += (products[3][3] & 0xFFFFFFFFu);

    // move carry to next row
    row3 += row4 >> 32; 
    row4 &= 0xFFFFFFFFu;
	row2 += row3 >> 32; 
    row3 &= 0xFFFFFFFFu;
	row1 += row2 >> 32; 
    row2 &= 0xFFFFFFFFu;
    // combine
    return BigUInteger( (((row1 & 0xFFFFFFFFu) << 32) | row2), 
                        ((row3 << 32) | row4) ); 
}
/// @brief Multiply - smaller value
template <typename T> 
BigUInteger BigUInteger::operator*(const T& factor) const noexcept
{
    if (factor == 0u)
        return BigUInteger(0uLL, 0uLL);
    
    // split value and factor into 32-bit parts
    uint64_t factorCast = static_cast<uint64_t>(factor);
    uint64_t valParts[4] = { m_upperBits >> 32, m_upperBits & 0xFFFFFFFFu, m_lowerBits >> 32, m_lowerBits & 0xFFFFFFFFu };
    uint64_t factorParts[2] = { factorCast >> 32, factorCast & 0xFFFFFFFFu };
    
    // multiply each component
    uint64_t products[4][2];
    products[0][1] = valParts[3] * factorParts[1];
    products[1][1] = valParts[2] * factorParts[1];
    products[2][1] = valParts[1] * factorParts[1];
    products[3][1] = valParts[0] * factorParts[1];
    
    products[0][0] = valParts[3] * factorParts[0];
    products[1][0] = valParts[2] * factorParts[0];
    products[2][0] = valParts[1] * factorParts[0];
    products[3][0] = valParts[0] * factorParts[0];

    // add rows
    uint64_t row4 = (products[0][1] & 0xFFFFFFFFu);
    uint64_t row3 = (products[0][0] & 0xFFFFFFFFu) + (products[0][1] >> 32);
    uint64_t row2 = (products[0][0] >> 32);
    uint64_t row1 = 0;

    row3 += (products[1][1] & 0xFFFFFFFFu);
    row2 += (products[1][0] & 0xFFFFFFFFu) + (products[1][1] >> 32);
    row1 += (products[1][0] >> 32);

    row2 += (products[2][1] & 0xFFFFFFFFu);
    row1 += (products[2][0] & 0xFFFFFFFFu) + (products[2][1] >> 32);

    row1 += (products[3][1] & 0xFFFFFFFFu);

    // move carry to next row
    row3 += row4 >> 32; 
    row4 &= 0xFFFFFFFFu;
	row2 += row3 >> 32; 
    row3 &= 0xFFFFFFFFu;
	row1 += row2 >> 32; 
    row2 &= 0xFFFFFFFFu;
    // combine
    return BigUInteger( (((row1 & 0xFFFFFFFFu) << 32) | row2), 
                        ((row3 << 32) | row4) ); 
}

/*
/// @brief Divide
BigUInteger BigUInteger::operator/(const BigUInteger& divider) const
{
    if (!divider)
        throw std::domain_error("Error: invalid division by zero");
    if (divider.m_upperBits == 0uLL) // no upper part - use faster algorithm
        return operator/(divider.lowerBits);
        
        
        
        
}
/// @brief Divide - smaller value
template <typename T> 
BigUInteger BigUInteger::operator/(const T& divider) const
{
    if (!divider)
        throw std::domain_error("Error: invalid division by zero");
    if (m_upperBits == 0uLL) // no upper part - simple division
        return BigUInteger(0uLL, m_lowerBits / static_cast<uint64_t>(divider));
    
    uint64_t uDivider = static_cast<uint64_t>(divider);
    uint64_t upperToLower = ((MAX_U64 - uDivider) + 1uLL) / uDivider;
    ++upperToLower;
    uint64_t upperRem = m_upperBits % uDivider;
    
    m_upperBits /= uDivider;
    m_lowerBits /= uDivider;
    m_lowerBits += ((upperRem * upperToLower) / uDivider);
    
    
    ///!!!!!!!!!!! p-e faux si division uppertolower tombe pas juste 
    ///-> splitter en blocs de 32 bits à la place, pour pouvoir récupérer les restes sans faire de conversion foireuse
}

/// @brief Remainder
BigUInteger BigUInteger::operator%(const BigUInteger& divider) const
{
    if (!divider)
        throw std::domain_error("Error: invalid division by zero");
    
    
    
    
}
        

/// @brief Divide - calculate both quotient and remainder (128-bit divider, slow)
/// @param[in] val      Value to divide
/// @param[in] divider  Divider
/// @returns  Quotient and remainder
BigUInteger::div_t BigUInteger::div(const BigUInteger& val, const BigUInteger divider)
{
    if (!divider)
        throw std::domain_error("Error: invalid division by zero");
    if (divider.m_upperBits == 0uLL) // no upper part - use faster algorithm
        return div(val, divider.lowerBits);
        
    // divide by itself or more - shortcut
    if (val <= divider)
    {
        if (val == divider)
            return BigUInteger::div_t(BigUInteger(0uLL, 1uLL));
        else // divider > val
            return BigUInteger::div_t(BigUInteger(0uLL, 0uLL), val);
    }
    
    // 128-bit division - very slow
    BigUInteger::div_t result;
    for (uint32_t i = 128u; i > 0u; --i)
    {
        result.quot <<= 1uL;
        result.rem <<= 1uL;
        
        if ((val >> (i - 1u)) & 0x1u)
            result.rem++;
        if (result.rem >= divider)
        {
            result.rem -= divider;
            result.quot++;
        }
    }
    return result;
}

/// @brief Divide - calculate both quotient and remainder (smaller divider, faster)
/// @param[in] val      Value to divide
/// @param[in] divider  Divider
/// @returns  Quotient and remainder
template <typename T>
BigUInteger::div_t BigUInteger::div(const BigUInteger& val, T divider)
{
    if (!divider)
        throw std::domain_error("Error: invalid division by zero");
    
    // divide by 1 - shortcut
    if (divider == 1)
        return BigUInteger::div_t(val);
        
    // divide by itself or more - shortcut
    if (val <= divider)
    {
        if (val == divider)
            return BigUInteger::div_t(BigUInteger(0uLL, 1uLL));
        else // divider > val
            return BigUInteger::div_t(BigUInteger(0uLL, 0uLL), val);
    }
    
    // 32-bit division - faster
    BigUInteger::div_t result(val);
    result.rem.m_lowerBits = val.m_lowerBits % divider; // remainder
    result.rem.m_upperBits |= remSign; // remainder sign
    if (val.m_upperBits)
    {       
        // quotient formula :  lowerBits = (lowerBits + ((upperBits % divider) * (max_uint64 + 1))) / divider
        // /!\ Problem : max uint64 value reached -> invalid
        
        // Solution : split calculation to avoid that
        //   lowerBits /= divider
        //   lowerBits += (upperBits % divider) * (half_of_max_uint64_+_1 / divider) * 2
        //   lowerBits += (upperBits % divider) * (half_of_max_uint64_+_1 % divider) * 2
        
        uint64_t upperToLower = HALF_MAX_U64_CEIL / divider; // use half of "max value + 1"
        uint64_t upperToLowerRem = HALF_MAX_U64_CEIL % divider;
        upperToLower <<= 1; // multiply half by two AFTER dividing it
        upperToLowerRem <<= 1;
        
        uint64_t upperRem = val.m_upperBits % divider;
        result.quot.m_upperBits /= divider;
        result.quot.m_lowerBits /= divider;
        result.quot.m_lowerBits += (upperRem * upperToLower); // if divider is not a power of 2, remainder missing
        result.quot.m_lowerBits += ((upperRem * upperToLowerRem) / divider); // fix it (if divider not a power of 2)
    }
    else
    {
        result.quot.m_lowerBits /= divider;
    }
    return result;
}
*/

// -- Special operators --

/// @brief Output stream operator
std::ostream& operator<<(std::ostream& stream, const utils::BigUInteger& val) 
{
    if (stream.flags() & stream.oct){
        stream << val.toString(8u);
    }
    else if (stream.flags() & stream.dec){
        stream << val.toString(10u);
    }
    else if (stream.flags() & stream.hex){
        stream << val.toString(16u);
    }
    return stream;
}
