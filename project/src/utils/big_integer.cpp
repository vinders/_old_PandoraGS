/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : big integer type (int128_t)
*******************************************************************************/
#include <cstdint>
#include <cstddef>
#include <string>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <cmath>
#include "big_integer.h"
#include "big_uinteger.h"
using namespace utils;

#define _INT64_SIGN_BIT_UMASK_   0x8000000000000000uLL
#define _INT64_SIGN_BIT_MASK_    0x8000000000000000LL

#define HALF_MAX_U64_CEIL  9223372036854775808uLL
#define MAX_I64_TO_DOUBLE  9223372036854775807.0
#define MIN_I64_TO_DOUBLE -9223372036854775808.0
#define MAX_U64_TO_DOUBLE 18446744073709551615.0
#define BYTE65_TO_DOUBLE  18446744073709551616.0
#define BYTE65_TO_FLOAT   18446744073709551616.0f


// -- Getters --

/// @brief Convert number to string
/// @param[in] base            Number base (optional)
/// @param[in] isPositiveSign  Show positive sign (optional)
/// @returns String representation
std::string BigInteger::toString(const uint32_t base, const bool isPositiveSign) const
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
        if (base == 10u)
        {
            if (m_upperBits & _INT64_SIGN_BIT_UMASK_)
                div_t dv(-(*this));
            do 
            {
                dv = div(dv.quot, base);
                str += '0' + std::abs(dv.rem.m_lowerBits); 
            } while (dv.quot);
                
            if (m_upperBits & _INT64_SIGN_BIT_UMASK_)
                str += "-";
            else if (isPositiveSign)
                str += "+";
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
BigInteger& BigInteger::operator=(const double val) noexcept
{
    if (val <= MAX_I64_TO_DOUBLE && val >= MIN_I64_TO_DOUBLE) // between int64 min/max
    {
        m_lowerBits = std::abs(static_cast<uint64_t>(val));
        m_upperBits = 0uLL;
        if (val < 0.0)
        {
            m_upperBits = ~m_upperBits; // negative sign
            m_lowerBits = ~m_lowerBits;
            BigInteger::operator++();
        }
        return *this;
    }
    else if (val > 0.0)
    {
        if (val <= MAX_U64_TO_DOUBLE) // below max uint64
        {
            m_lowerBits = static_cast<uint64_t>(val);
            m_upperBits = 0uLL;
            return *this;
        }
        else // too many decimals for int64, val > 0
        {
            double upperPart = val / BYTE65_TO_DOUBLE;
            m_upperBits = static_cast<uint64_t>(upperPart);
            double lowerPart = val - (static_cast<double>(m_upperBits) * BYTE65_TO_DOUBLE);
            m_lowerBits = static_cast<uint64_t>(lowerPart);
            return *this;
        }
    }
    else // too many decimals for int64, val < 0
    {
        double upperPart = -val / BYTE65_TO_DOUBLE; // absolute value
        m_upperBits = static_cast<uint64_t>(upperPart);
        double lowerPart = -val - (static_cast<double>(m_upperBits) * BYTE65_TO_DOUBLE);
        m_lowerBits = static_cast<uint64_t>(lowerPart);
        
        m_upperBits = ~m_upperBits; // negative sign
        m_lowerBits = ~m_lowerBits;
        BigInteger::operator++();
    }
    return *this;
}


// -- Typecast operators --

/// @brief Cast as floating-point value (destructive)
operator BigInteger::float() const noexcept 
{ 
    double val = static_cast<double>(*this);
    return static_cast<float>(val);
}
/// @brief Cast as double-precision floating-point value
operator BigInteger::double() const noexcept 
{
    if (m_upperBits & _INT64_SIGN_BIT_UMASK_)
    {
        BigInteger opposite = -(*this);
        double val = static_cast<double>(opposite.m_upperBits);
        val *= BYTE65_TO_FLOAT;
        val += static_cast<double>(opposite.m_lowerBits);
        return -val;
    }
    else
    {
        double val = static_cast<double>(m_upperBits);
        val *= BYTE65_TO_FLOAT;
        val += static_cast<double>(m_lowerBits);
        return val;
    }
}

/// @brief Cast as big unsigned integer (destructive)
BigUInteger BigInteger::operator BigUInteger() const noexcept 
{ 
    if (m_upperBits & _INT64_SIGN_BIT_UMASK_)
    {
        BigInteger opposite = -(*this);
        return BigUInteger(opposite.m_upperBits, opposite.m_lowerBits);
    }
    else
    {
        return BigUInteger(m_upperBits, m_lowerBits);
    }
}


// -- Arithmetic operators --

/// @brief Add to instance
BigInteger& BigInteger::operator+=(const BigInteger& val) noexcept
{
    if ((val.m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL)
    {
        m_upperBits += val.m_upperBits;
        BigInteger::operator+=(val.m_lowerBits);
    }
    else
    {
        BigInteger::operator-=(-val);
    }
}
/// @brief Add to instance - smaller value
template <typename T> 
BigInteger& BigInteger::operator+=(const T& val) noexcept 
{
    if (val > 0)
    {
        if (m_lowerBits + val < m_lowerBits)
            ++m_upperBits;
        m_lowerBits += val;
    }
    else if (val < 0)
    {
        BigInteger::operator-=(-val);
    }
    return *this;
}

/// @brief Substract from instance
BigInteger& BigInteger::operator-=(const BigInteger& val) noexcept
{
    if ((val.m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL)
    {
        m_upperBits -= val.m_upperBits;
        BigInteger::operator+=(val.m_lowerBits);
    }
    else
    {
        BigInteger::operator+=(-val);
    }
}
/// @brief Substract from instance - smaller value
template <typename T> 
BigInteger& BigInteger::operator-=(const T& val) noexcept
{
    if (val > 0)
    {
        if (m_lowerBits - val > m_lowerBits)
            --m_upperBits;
        m_lowerBits -= val;
    }
    else if (val < 0)
    {
        BigInteger::operator+=(-val);
    }
    return *this;
}
/*
/// @brief Multiply
BigInteger BigInteger::operator*(const BigInteger& val) const noexcept
{
    // set sign
    uint64_t prodSign = ((m_upperBits ^ val.m_upperBits) & _INT64_SIGN_BIT_UMASK_);
    
    // split value and factor into 32-bit parts
    uint64_t valParts[4] = { (m_upperBits & _INT64_SIGN_STRIP_UMASK_) >> 32, m_upperBits & 0xFFFFFFFFu, m_lowerBits >> 32, m_lowerBits & 0xFFFFFFFFu };
    uint64_t factorParts[4] = { (factor.m_upperBits & _INT64_SIGN_STRIP_UMASK_) >> 32, factor.m_upperBits & 0xFFFFFFFFu, factor.m_lowerBits >> 32, factor.m_lowerBits & 0xFFFFFFFFu };
        
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
    return BigUInteger( ((((row1 & 0xFFFFFFFFu) << 32) | row2) | prodSign), 
                        ((row3 << 32) | row4) ); 
}
/// @brief Multiply - smaller value
template <typename T> 
BigInteger BigInteger::operator*(const T& factor) const noexcept
{
    // set sign
    uint64_t prodSign = (m_upperBits & _INT64_SIGN_BIT_UMASK_);
    if (factor <= 0)
    {
        if (factor == 0)
            return BigInteger(0uLL, 0uLL);
        prodSign ^= _INT64_SIGN_BIT_UMASK_;
    }
    
    // split value and factor into 32-bit parts
    uint64_t valParts[4] = { (m_upperBits & _INT64_SIGN_STRIP_UMASK_) >> 32, m_upperBits & 0xFFFFFFFFu, m_lowerBits >> 32, m_lowerBits & 0xFFFFFFFFu };
    uint64_t factorCast = static_cast<uint64_t>(std::abs(factor));
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
    return BigUInteger( ((((row1 & 0xFFFFFFFFu) << 32) | row2) | prodSign), 
                        ((row3 << 32) | row4) ); 
}

/// @brief Divide
BigInteger BigInteger::operator/(const BigInteger& val) const
{
    
}
/// @brief Divide - smaller value
template <typename T> 
BigInteger BigInteger::operator/(const T& val) const
{
    
}

/// @brief Remainder
BigInteger BigInteger::operator%(const BigInteger& val) const
{
    
}
        

/// @brief Divide - calculate both quotient and remainder (128-bit divider, slow)
/// @param[in] val      Value to divide
/// @param[in] divider  Divider
/// @returns  Quotient and remainder
BigInteger::div_t BigInteger::div(const BigInteger& val, const BigInteger divider)
{
    if (!divider)
        throw std::domain_error("Error: invalid division by zero");
    if (divider.m_upperBits == 0uLL) // no upper part - use faster algorithm
        return div(val, divider.lowerBits);
    if (divider.m_upperBits == _INT64_SIGN_BIT_UMASK_)
    {
        if (divider.m_lowerBits == 1uLL) // divide by -1 - shortcut
            return BigInteger::div_t(-val);
        if ((divider.lowerBits & _INT64_SIGN_BIT_UMASK_) == 0uLL) // no upper part except sign and lower part with max 63 bits - use faster algorithm
            return div(val, -(static_cast<int64_t>(divider.lowerBits)));
    }
    
    // define result sign
    uint64_t quotSign = ((val.m_upperBits ^ divider.m_upperBits) & _INT64_SIGN_BIT_UMASK_);
    uint64_t remSign = (divider.m_upperBits & _INT64_SIGN_BIT_UMASK_);
    // remove signs from operators
    BigInteger uVal(stripInt64SignBit(val.m_upperBits), val.m_lowerBits)
    BigInteger uDivider(stripInt64SignBit(divider.m_upperBits), divider.m_lowerBits);
    
    // divide by itself or more - shortcut
    if (uVal <= uDivider)
    {
        if (uVal == uDivider)
            return BigInteger::div_t(BigInteger(quotSign, 1uLL));
        else // divider > val
            return BigInteger::div_t(BigInteger(0uLL, 0uLL), BigInteger((val.upperBits | remSign), val.lowerBits));
    }
    
    // 128-bit division - very slow
    BigInteger::div_t result;
    for (uint32_t i = 128u; i > 0u; --i)
    {
        result.quot <<= 1uL;
        result.rem <<= 1uL;
        
        if ((uVal >> (i - 1u)) & 0x1u)
            result.rem++;
        if (result.rem >= uDivider)
        {
            result.rem -= uDivider;
            result.quot++;
        }
    }
    result.quot.m_upperBits |= quotSign;
    return result;
}

/// @brief Divide - calculate both quotient and remainder (smaller divider, faster)
/// @param[in] val      Value to divide
/// @param[in] divider  Divider
/// @returns  Quotient and remainder
template <typename T>
BigInteger::div_t BigInteger::div(const BigInteger& val, T divider)
{
    if (!divider)
        throw std::domain_error("Error: invalid division by zero");
    
    // divide by 1 - shortcut
    if (divider == 1)
        return BigInteger::div_t(val);
    if (divider == -1)
        return BigInteger::div_t(-val);
    
    // define result sign
    uint64_t quotSign = val.m_upperBits & _INT64_SIGN_BIT_UMASK_;
    uint64_t remSign = 0uLL;
    if (divider < 0)
    {
        quotSign ^= _INT64_SIGN_BIT_UMASK_;
        remSign = _INT64_SIGN_BIT_UMASK_;
        divider = std::abs(divider);
    }
    // remove signs from operators
    BigInteger::div_t result(BigInteger(stripInt64SignBit(val.m_upperBits), val.m_lowerBits)); // unsigned source value
    uint64_t uDivider = static_cast<uint64_t>(divider); // unsigned divider
    
    // divide by itself or more - shortcut
    if (result.quot <= uDivider)
    {
        if (result.quot == uDivider)
        {
            return BigInteger::div_t(BigInteger(quotSign, 1uLL));
        }
        else // divider > val
            return BigInteger::div_t(BigInteger(0uLL, 0uLL), BigInteger((result.quot.upperBits | remSign), result.quot.lowerBits));
    }
    
    // 32-bit division - faster
    result.rem.m_lowerBits = result.quot.m_lowerBits % uDivider; // remainder
    result.rem.m_upperBits |= remSign; // remainder sign
    if (result.quot.m_upperBits)
    {       
        // quotient formula :  lowerBits = (lowerBits + ((upperBits % divider) * (max_uint64 + 1))) / divider
        // /!\ Problem : max uint64 value reached -> invalid
        
        // Solution : split calculation to avoid that
        //   lowerBits /= divider
        //   lowerBits += (upperBits % divider) * (half_of_max_uint64_+_1 / divider) * 2
        //   lowerBits += (upperBits % divider) * (half_of_max_uint64_+_1 % divider) * 2
        
        uint64_t upperToLower = HALF_MAX_U64_CEIL / uDivider; // use half of "max value + 1"
        uint64_t upperToLowerRem = HALF_MAX_U64_CEIL % uDivider;
        upperToLower <<= 1; // multiply half by two AFTER dividing it
        upperToLowerRem <<= 1;
        
        uint64_t upperRem = result.quot.m_upperBits % uDivider;
        result.quot.m_upperBits /= uDivider;
        result.quot.m_lowerBits /= uDivider;
        result.quot.m_lowerBits += (upperRem * upperToLower); // if divider is not a power of 2, remainder missing
        result.quot.m_lowerBits += ((upperRem * upperToLowerRem) / uDivider); // fix it (if divider not a power of 2)
    }
    else
    {
        result.quot.m_lowerBits /= uDivider;
    }
    result.quot.m_upperBits |= quotSign;
    return result;
}
*/

// -- Special operators --

/// @brief Output stream operator
std::ostream& operator<<(std::ostream& stream, const utils::BigInteger& val) 
{
    if (stream.flags() & stream.oct){
        stream << val.toString(8u);
    }
    else if (stream.flags() & stream.dec){
        stream << val.toString(10u, ((stream.flags() & std::showpos) != 0));
    }
    else if (stream.flags() & stream.hex){
        stream << val.toString(16u);
    }
    return stream;
}
