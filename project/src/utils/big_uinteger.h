/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : big unsigned integer type (uint128_t)
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <cmath>

#define uint128_t  utils::BigUInteger


/// @namespace utils
/// General utilities
namespace utils
{
    class BigInteger;
    
    /// @class BigUInteger
    /// @brief 128-bit unsigned integer
    class BigUInteger
    {
    public:
        /// @struct div_t
        /// @brief Division structure
        struct div_t
        {
            BigUInteger quot;
            BigUInteger rem;
            
            /// @brief Create empty division structure
            div_t() : quot(0), rem(0) {}
            /// @brief Create division structure
            div_t(const BigUInteger& quotient) : quot(quotient), rem(0) {}
            /// @brief Create division structure
            div_t(const BigUInteger& quotient, const BigUInteger& remainder) : quot(quotient), rem(remainder) {}
            /// @brief Create division structure
            div_t(BigUInteger&& quotient) : quot(std::move(quotient)), rem(0) {}
        };
    
    
        /// @brief Create big integer
        BigUInteger() noexcept : m_upperBits(0uLL), m_lowerBits(0uLL) {}
        /// @brief Create initialized big integer
        /// @param[in] upper  Upper bits
        /// @param[in] lower  Lower bits
        BigUInteger(const uint64_t upper, const uint64_t lower) noexcept : m_upperBits(upper), m_lowerBits(lower) {}
        
        /// @brief Copy big integer
        /// @param[in] val  Other instance
        BigUInteger(const BigUInteger& val) noexcept : m_upperBits(val.m_upperBits), m_lowerBits(val.m_lowerBits) {}
        /// @brief Move big integer
        /// @param[in] val  Other instance
        BigUInteger(BigUInteger&& val) noexcept : m_upperBits(std::move(val.m_upperBits)), m_lowerBits(std::move(val.m_lowerBits)) {}
        
        
        // -- Constructors with typecast --
        
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigUInteger(const uint64_t val) noexcept : m_upperBits(0uLL), m_lowerBits(val) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigUInteger(const int64_t val) noexcept : m_upperBits(0uLL), m_lowerBits(static_cast<uint64_t>(std::abs(val))) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigUInteger(const uint32_t val) noexcept : m_upperBits(0uLL), m_lowerBits(static_cast<uint64_t>(val)) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigUInteger(const int32_t val) noexcept : m_upperBits(0uLL), m_lowerBits(static_cast<uint64_t>(std::abs(val))) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigUInteger(const uint16_t val) noexcept : m_upperBits(0uLL), m_lowerBits(static_cast<uint64_t>(val)) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigUInteger(const int16_t val) noexcept : m_upperBits(0uLL), m_lowerBits(static_cast<uint64_t>(std::abs(val))) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigUInteger(const uint8_t val) noexcept : m_upperBits(0uLL), m_lowerBits(static_cast<uint64_t>(val)) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigUInteger(const int8_t val) noexcept : m_upperBits(0uLL), m_lowerBits(static_cast<uint64_t>(std::abs(val))) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigUInteger(const bool val) noexcept : m_upperBits(0uLL), m_lowerBits((val) ? 1uLL : 0uLL) {}

        
        // -- Getters --
        
        /// @brief Extract 64-bit parts
        /// @param[out] outUpper  Upper part destination
        /// @param[out] outLower  Lower part destination
        inline void extractParts(uint64_t& upper, uint64_t& lower) const noexcept
        {
            upper = m_upperBits;
            lower = m_lowerBits;
        }
        /// @brief Extract upper 64-bit part
        /// @returns Upper part
        inline uint64_t getUpperBits() const noexcept  { return m_upperBits; }
        /// @brief Extract lower 64-bit part
        /// @returns Lower part
        inline uint64_t getLowerBits() const noexcept  { return m_lowerBits; }
        
        /// @brief Convert number to string
        /// @param[in] base            Number base (optional)
        /// @param[in] isPositiveSign  Show positive sign (optional)
        /// @returns String representation
        std::string toString(const uint32_t base = 10u) const;
        /// @brief Convert number to string
        /// @param[in] base            Number base (optional)
        /// @param[in] isPositiveSign  Show positive sign (optional)
        /// @returns String representation
        inline std::string str(const uint32_t base = 10u, const uint32_t len = 0u) const 
        { 
            std::string str = toString(base, isPositiveSign); 
            if (len != 0u && len > str.size()) 
                str.insert(0, len - str.size(), ' ');
            return str;
        }
        
        
        // -- Assignment --
        
        /// @brief Set value from 64-bit parts
        /// @param[in] upper  Upper part
        /// @param[in] lower  Lower part
        /// @returns Current object
        inline BigUInteger& set(const uint64_t upper, const uint64_t lower) noexcept 
        { 
            m_upperBits = upper; 
            m_lowerBits = lower; 
            return *this; 
        }

        /// @brief Assign copy of big integer
        /// @param[in] val  Other instance
        /// @returns Current object
        inline BigUInteger& operator=(const BigUInteger& val) noexcept { return set(val.m_upperBits, val.m_lowerBits); }
        /// @brief Assign moved big integer
        /// @param[in] val  Other instance
        /// @returns Current object
        inline BigUInteger& operator=(BigUInteger&& val) noexcept 
        { 
            m_upperBits = std::move(val.m_upperBits); 
            m_lowerBits = std::move(val.m_lowerBits); 
            return *this; 
        }
        
        /// @brief Swap value with other instance
        /// @param[out] val  Other instance
        void swap(BigUInteger& val)
        {
            m_upperBits ^= val.m_upperBits;
            val.m_upperBits ^= m_upperBits;
            m_upperBits ^= val.m_upperBits;
            m_lowerBits ^= val.m_lowerBits;
            val.m_lowerBits ^= m_lowerBits;
            m_lowerBits ^= val.m_lowerBits;
        }
        
        
        // -- Assignment with typecast --
        
        /// @brief Assign value with typecast
        inline BigUInteger& operator=(const uint64_t val) noexcept { return set(0uLL, val); }
        /// @brief Assign value with typecast
        inline BigUInteger& operator=(const int64_t val) noexcept  { return set(0uLL, std::abs(val)); }
        /// @brief Assign value with typecast
        inline BigUInteger& operator=(const uint32_t val) noexcept { return set(0uLL, static_cast<uint64_t>(val)); }
        /// @brief Assign value with typecast
        inline BigUInteger& operator=(const int32_t val) noexcept  { return set(0uLL, static_cast<int64_t>(std::abs(val))); }
        /// @brief Assign value with typecast
        inline BigUInteger& operator=(const uint16_t val) noexcept { return set(0uLL, static_cast<uint64_t>(val)); }
        /// @brief Assign value with typecast
        inline BigUInteger& operator=(const int16_t val) noexcept  { return set(0uLL, static_cast<int64_t>(std::abs(val))); }
        /// @brief Assign value with typecast
        inline BigUInteger& operator=(const uint8_t val) noexcept  { return set(0uLL, static_cast<uint64_t>(val)); }
        /// @brief Assign value with typecast
        inline BigUInteger& operator=(const int8_t val) noexcept   { return set(0uLL, static_cast<int64_t>(std::abs(val))); }
        /// @brief Assign value with typecast
        inline BigUInteger& operator=(const bool val) noexcept     { return set(0uLL, (val) ? 1uLL : 0uLL); }
        
        /// @brief Assign integer part of floating-point value with typecast
        inline BigUInteger& operator=(const float val) noexcept    { return operator=(static_cast<double>(val)); }
        /// @brief Assign integer part of double-precision floating-point value with typecast
        BigUInteger& operator=(const double val) noexcept;
        
        
        // -- Typecast operators --
        
        /// @brief Cast as 64-bit integer (destructive)
        inline uint64_t operator uint64_t() const noexcept { return m_lowerBits; }
        /// @brief Cast as 64-bit signed integer (destructive)
        inline int64_t operator int64_t() const noexcept   { return static_cast<int64_t>(m_lowerBits); }
        /// @brief Cast as 32-bit integer (destructive)
        inline uint32_t operator uint32_t() const noexcept { return static_cast<uint32_t>(m_lowerBits); }
        /// @brief Cast as 32-bit signed integer (destructive)
        inline int32_t operator int32_t() const noexcept   { return static_cast<uint32_t>(m_lowerBits); }
        /// @brief Cast as 16-bit integer (destructive)
        inline uint16_t operator uint16_t() const noexcept { return static_cast<uint16_t>(m_lowerBits); }
        /// @brief Cast as 16-bit signed integer (destructive)
        inline int16_t operator int16_t() const noexcept   { return static_cast<uint16_t>(m_lowerBits); }
        /// @brief Cast as 8-bit integer (destructive)
        inline uint8_t operator uint8_t() const noexcept   { return static_cast<uint8_t>(m_lowerBits); }
        /// @brief Cast as 8-bit signed integer (destructive)
        inline int8_t operator int8_t() const noexcept     { return static_cast<uint8_t>(m_lowerBits); }
        
        /// @brief Cast as boolean
        bool operator bool() const noexcept { return ((m_lowerBits | m_upperBits) != 0uLL); }
        
        /// @brief Cast as floating-point value (destructive)
        float operator float() const noexcept;
        /// @brief Cast as double-precision floating-point value
        double operator double() const noexcept;
        
        /// @brief Cast as big signed integer (destructive)
        BigUInteger operator BigInteger() const noexcept;
        
        
        // -- bitwise operators --
        
        /// @brief Bitwise AND operator
        BigUInteger operator&(const BigUInteger& mask) const noexcept
        {
            return BigUInteger((m_upperBits & mask.m_upperBits), (m_lowerBits & mask.m_lowerBits));
        }
        /// @brief Bitwise AND operator - any other type
        template <typename T>
        BigUInteger operator&(const T& mask) const noexcept
        {
            return BigUInteger(0uLL, (m_lowerBits & static_cast<uint64_t>(mask));
        }
        /// @brief Bitwise AND operator
        BigUInteger& operator&=(const BigUInteger& mask) noexcept
        {
            m_upperBits &= mask.m_upperBits;
            m_lowerBits &= mask.m_lowerBits;
            return *this;
        }
        /// @brief Bitwise AND operator - any other type
        template <typename T>
        BigUInteger& operator&=(const T& mask) noexcept
        {
            m_upperBits &= 0uLL;
            m_lowerBits &= static_cast<uint64_t>(mask);
            return *this;
        }
        
        /// @brief Bitwise OR operator
        BigUInteger operator|(const BigUInteger& mask) const noexcept
        {
            return BigUInteger((m_upperBits | mask.m_upperBits), (m_lowerBits | mask.m_lowerBits));
        }
        /// @brief Bitwise OR operator - any other type
        template <typename T>
        BigUInteger operator|(const T& mask) const noexcept
        {
            return BigUInteger(m_upperBits, (m_lowerBits | static_cast<uint64_t>(mask));
        }
        /// @brief Bitwise OR operator
        BigUInteger& operator|=(const BigUInteger& mask) noexcept
        {
            m_upperBits |= mask.m_upperBits;
            m_lowerBits |= mask.m_lowerBits;
            return *this;
        }
        /// @brief Bitwise OR operator - any other type
        template <typename T>
        BigUInteger& operator|=(const T& mask) noexcept
        {
            m_lowerBits |= static_cast<uint64_t>(mask);
            return *this;
        }
        
        /// @brief Bitwise XOR operator
        BigUInteger operator^(const BigUInteger& mask) const noexcept
        {
            return BigUInteger((m_upperBits ^ mask.m_upperBits), (m_lowerBits ^ mask.m_lowerBits));
        }
        /// @brief Bitwise XOR operator - any other type
        template <typename T>
        BigUInteger operator^(const T& mask) const noexcept
        {
            return BigUInteger((m_lowerBits ^ 0uLL, (m_lowerBits ^ static_cast<uint64_t>(mask));
        }
        /// @brief Bitwise XOR operator
        BigUInteger& operator^=(const BigUInteger& mask) noexcept
        {
            m_upperBits ^= mask.m_upperBits;
            m_lowerBits ^= mask.m_lowerBits;
            return *this;
        }
        /// @brief Bitwise XOR operator - any other type
        template <typename T>
        BigUInteger& operator^=(const T& mask) noexcept
        {
            m_upperBits ^= mask.m_upperBits;
            m_lowerBits ^= mask.m_lowerBits;
            return *this;
        }
        
        /// @brief Bitwise NOT operator
        BigUInteger operator~() const noexcept
        {
            return BigUInteger(~m_upperBits, ~m_lowerBits);
        }
        
        /// @brief Bit LEFT-SHIFT operator
        BigUInteger operator<<(const uint32_t factor) const noexcept
        {
            if (factor < 64u)
                return BigUInteger((m_upperBits << factor) | (m_lowerBits >> (64 - factor)), m_lowerBits << factor);
            if (factor == 64u)
                return BigUInteger(m_lowerBits, 0uLL);
            if (factor < 128u)
                return BigUInteger(m_lowerBits << (factor - 64u), 0uLL);
            return BigUInteger();
        }
        /// @brief Bit LEFT-SHIFT operator
        BigUInteger operator<<(const uint64_t factor) const noexcept { return (factor < 128u) ? operator<<(static_cast<uint32_t>(factor)) : BigUInteger(); }
        /// @brief Bit LEFT-SHIFT operator
        BigUInteger operator<<(const uint16_t factor) const noexcept { return operator<<(static_cast<uint32_t>(factor)); }
        /// @brief Bit LEFT-SHIFT operator
        BigUInteger operator<<(const uint8_t factor) const noexcept  { return operator<<(static_cast<uint32_t>(factor)); }
        /// @brief Bit LEFT-SHIFT operator
        BigUInteger& operator<<=(const uint32_t factor) noexcept
        {
            if (factor < 64u)
            {
                m_upperBits = ((m_upperBits << factor) | (m_lowerBits >> (64 - factor)));
                m_lowerBits <<= factor;
            }
            else
            {
                if (factor == 64u)
                    m_upperBits = m_lowerBits;
                else if (factor < 128u)
                    m_upperBits = (m_lowerBits << (factor - 64u));
                else
                    m_upperBits = 0uLL;
                m_lowerBits = 0uLL;
            }
            return *this;
        }
        /// @brief Bit LEFT-SHIFT operator
        BigUInteger& operator<<=(const uint64_t factor) noexcept { return (factor < 128u) ? operator<<=(static_cast<uint32_t>(factor)) : BigUInteger(); }
        /// @brief Bit LEFT-SHIFT operator
        BigUInteger& operator<<=(const uint16_t factor) noexcept { return operator<<=(static_cast<uint32_t>(factor)); }
        /// @brief Bit LEFT-SHIFT operator
        BigUInteger& operator<<=(const uint8_t factor) noexcept  { return operator<<=(static_cast<uint32_t>(factor)); }
        
        /// @brief Bit RIGHT-SHIFT operator
        BigUInteger operator>>(const uint32_t factor) const noexcept
        {
            if (factor < 64u)
                return BigUInteger(m_upperBits >> factor, (m_lowerBits >> factor) | (m_upperBits << (64 - factor)));
            if (factor == 64u)
                return BigUInteger(0uLL, m_upperBits);
            if (factor < 128u)
                return BigUInteger(0uLL, m_upperBits >> (factor - 64u));
            return BigUInteger();
        }
        /// @brief Bit RIGHT-SHIFT operator
        BigUInteger operator>>(const uint64_t factor) const noexcept { return (factor < 128u) ? operator>>(static_cast<uint32_t>(factor)) : BigUInteger(); }
        /// @brief Bit RIGHT-SHIFT operator
        BigUInteger operator>>(const uint16_t factor) const noexcept { return operator>>(static_cast<uint32_t>(factor)); }
        /// @brief Bit RIGHT-SHIFT operator
        BigUInteger operator>>(const uint8_t factor) const noexcept  { return operator>>(static_cast<uint32_t>(factor)); }
        /// @brief Bit RIGHT-SHIFT operator
        BigUInteger& operator>>=(const uint32_t factor) noexcept
        {
            if (factor < 64u)
            {
                m_lowerBits = ((m_lowerBits >> factor) | (m_upperBits << (64 - factor)));
                m_upperBits >>= factor;
            }
            else
            {
                if (factor == 64u)
                    m_lowerBits = m_upperBits;
                else if (factor < 128u)
                    m_lowerBits = (m_upperBits >> (factor - 64u));
                else
                    m_lowerBits = 0uLL;
                m_upperBits = 0uLL;
            }
            return *this;
        }
        /// @brief Bit RIGHT-SHIFT operator
        BigUInteger& operator>>=(const uint64_t factor) noexcept { return (factor < 128u) ? operator>>=(static_cast<uint32_t>(factor)) : BigUInteger(); }
        /// @brief Bit RIGHT-SHIFT operator
        BigUInteger& operator>>=(const uint16_t factor) noexcept { return operator>>=(static_cast<uint32_t>(factor)); }
        /// @brief Bit RIGHT-SHIFT operator
        BigUInteger& operator>>=(const uint8_t factor) noexcept  { return operator>>=(static_cast<uint32_t>(factor)); }
        
        
        // -- Logical operators --
        
        /// @brief Logical NOT operator
        bool operator!() const noexcept { return ((m_lowerBits | m_upperBits) == 0uLL); }
        
        /// @brief Logical AND operator
        bool operator&&(const BigUInteger& val) const noexcept { return (static_cast<bool>(*this) && static_cast<bool>(val)); }
        /// @brief Logical AND operator
        template <typename T>
        bool operator&&(const T& val) const noexcept { return (static_cast<bool>(*this) && static_cast<bool>(val)); }

        /// @brief Logical OR operator
        bool operator||(const BigUInteger& val) const noexcept { return (static_cast<bool>(*this) || static_cast<bool>(val)); }
        /// @brief Logical OR operator
        template <typename T>
        bool operator||(const T& val) const noexcept { return (static_cast<bool>(*this) || static_cast<bool>(val)); }
        
        
        // -- Comparison operators --
        
        /// @brief Compare instances - equality
        bool operator==(const BigUInteger& val) const noexcept { return (m_upperBits == val.m_upperBits && m_lowerBits == val.m_lowerBits); }
        /// @brief Compare instances - equality
        template <typename T>
        bool operator==(const T& val) const noexcept { return (m_upperBits == 0uLL && m_lowerBits == static_cast<uint64_t>(val.m_lowerBits)); }

        /// @brief Compare instances - difference
        bool operator!=(const BigUInteger& val) const noexcept { return (m_upperBits != val.m_upperBits || m_lowerBits != val.m_lowerBits); }
        /// @brief Compare instances - difference
        template <typename T>
        bool operator!=(const T& val) const noexcept { return (m_upperBits != 0uLL || m_lowerBits != static_cast<uint64_t>(val.m_lowerBits)); }

        /// @brief Compare instances - lower
        bool operator<(const BigUInteger& val) const noexcept
        { 
            return (m_upperBits < val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits < val.m_lowerBits) );
        }
        /// @brief Compare instances - lower
        template <typename T>
        bool operator<(const T& val) const noexcept 
        { 
            return (m_upperBits == 0uLL && m_lowerBits < static_cast<uint64_t>(val)); 
        }
        
        /// @brief Compare instances - lower or equal
        bool operator<=(const BigUInteger& val) const noexcept 
        { 
            return (m_upperBits < val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits <= val.m_lowerBits) );
        }
        /// @brief Compare instances - lower or equal
        template <typename T>
        bool operator<=(const T& val) const noexcept
        { 
            return (m_upperBits == 0uLL && m_lowerBits <= static_cast<uint64_t>(val)); 
        }
        
        /// @brief Compare instances - greater
        bool operator>(const BigUInteger& val) const noexcept
        { 
            return (m_upperBits > val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits > val.m_lowerBits) );
        }
        /// @brief Compare instances - greater
        template <typename T>
        bool operator>(const T& val) const noexcept
        { 
            return (m_upperBits != 0uLL || m_lowerBits > static_cast<uint64_t>(val)); 
        }
        
        /// @brief Compare instances - greater or equal
        bool operator>=(const BigUInteger& val) const noexcept 
        {
            return (m_upperBits > val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits >= val.m_lowerBits) );
        }
        /// @brief Compare instances - greater or equal
        template <typename T>
        bool operator>=(const T& val) const noexcept
        { 
            return (m_upperBits != 0uLL || m_lowerBits >= static_cast<uint64_t>(val)); 
        }
       
        
        // -- Arithmetic operators --
        
        /// @brief Add
        inline BigUInteger operator+(const BigUInteger& val) const noexcept
        {
            return BigUInteger(m_upperBits + val.m_upperBits + static_cast<uint64_t>((m_lowerBits + val.m_lowerBits) < m_lowerBits), 
                               m_lowerBits + val.m_lowerBits);
        }
        /// @brief Add - smaller value
        template <typename T> 
        inline BigUInteger operator+(const T& val) const noexcept
        {
            if (val > 0)
                return BigUInteger(m_upperBits + static_cast<uint64_t>((m_lowerBits + static_cast<uint64_t>(val)) < m_lowerBits), m_lowerBits + static_cast<uint64_t>(val));
            else if (val < 0)
                return BigUInteger(m_upperBits - static_cast<uint64_t>((m_lowerBits - static_cast<uint64_t>(-val)) > m_lowerBits), m_lowerBits - static_cast<uint64_t>(-val));
            return *this;
        }
        /// @brief Add to instance
        inline BigUInteger& operator+=(const BigUInteger& val) noexcept
        {
            m_upperBits += val.m_upperBits + static_cast<uint64_t>((m_lowerBits + val.m_lowerBits) < m_lowerBits);
            m_lowerBits += val.m_lowerBits;
            return *this;
        }
        /// @brief Add to instance - smaller value
        template <typename T> 
        inline BigUInteger& operator+=(const T& val) noexcept 
        {
            if (val > 0)
            {
                m_upperBits += static_cast<uint64_t>((m_lowerBits + static_cast<uint64_t>(val)) < m_lowerBits);
                m_lowerBits += static_cast<uint64_t>(val));
            }
            else if (val < 0)
            {
                m_upperBits -= static_cast<uint64_t>((m_lowerBits - static_cast<uint64_t>(-val)) > m_lowerBits);
                m_lowerBits -= static_cast<uint64_t>(-val);
            }
            return *this;
        }

        /// @brief Substract
        inline BigUInteger operator-(const BigUInteger& val) const noexcept
        {
            return BigUInteger(m_upperBits - val.m_upperBits - static_cast<uint64_t>((m_lowerBits - val.m_lowerBits) > m_lowerBits), 
                               m_lowerBits - val.m_lowerBits);
        }
        /// @brief Substract - smaller value
        template <typename T> 
        inline BigUInteger operator-(const T& val) const noexcept
        {
            if (val > 0)
                return BigUInteger(m_upperBits - static_cast<uint64_t>((m_lowerBits - static_cast<uint64_t>(val)) > m_lowerBits), m_lowerBits - static_cast<uint64_t>(val));
            else if (val < 0)
                return BigUInteger(m_upperBits + static_cast<uint64_t>((m_lowerBits + static_cast<uint64_t>(-val)) < m_lowerBits), m_lowerBits + static_cast<uint64_t>(-val));
            return *this;
        }
        /// @brief Substract from instance
        inline BigUInteger& operator-=(const BigUInteger& val) noexcept
        {
            m_upperBits -= val.m_upperBits - static_cast<uint64_t>((m_lowerBits - val.m_lowerBits) > m_lowerBits);
            m_lowerBits -= val.m_lowerBits;
            return *this;
        }
        /// @brief Substract from instance - smaller value
        template <typename T> 
        inline BigUInteger& operator-=(const T& val) noexcept
        {
            if (val > 0)
            {
                m_upperBits -= static_cast<uint64_t>((m_lowerBits - static_cast<uint64_t>(val)) > m_lowerBits);
                m_lowerBits -= static_cast<uint64_t>(val);
            }
            else if (val < 0)
            {
                m_upperBits += static_cast<uint64_t>((m_lowerBits + static_cast<uint64_t>(-val)) < m_lowerBits);
                m_lowerBits += static_cast<uint64_t>(-val));
            }
            return *this;
        }

        /// @brief Multiply
        BigUInteger operator*(const BigUInteger& val) const noexcept;
        /// @brief Multiply - smaller value
        template <typename T> 
        BigUInteger operator*(const T& val) const noexcept;
        /// @brief Multiply instance
        inline BigUInteger& operator*=(const BigUInteger& val) noexcept { return (*this = *this * val); }
        /// @brief Multiply instance - smaller value
        template <typename T>
        inline BigUInteger& operator*=(const T& val) noexcept { return (*this = *this * val); }

        /// @brief Divide
		BigUInteger operator/(const BigUInteger& val) const;
        /// @brief Divide - smaller value
        template <typename T> 
        BigUInteger operator/(const T& val) const;
        /// @brief Divide instance
        inline BigUInteger& operator/=(const BigUInteger& val) { return (*this = *this / val); }
        /// @brief Divide instance - smaller value
        template <typename T> 
        inline BigUInteger& operator/=(const T& val) { return (*this = *this / val); }

        /// @brief Remainder
        BigUInteger operator%(const BigUInteger& val) const;
        /// @brief Remainder - smaller value
        template <typename T> 
        inline BigUInteger operator%(const T& val) const { return (*this - (val * (*this / val))); }
        /// @brief Assign remainder
        inline BigUInteger& operator%=(const BigUInteger& val) { return (*this = *this % val); }
        /// @brief Assign remainder - smaller value
        template <typename T> 
        inline BigUInteger& operator%=(const T& val) { return (*this -= (val * (*this / val))); }
        
        /// @brief Divide - calculate both quotient and remainder (128-bit divider, slow)
        /// @param[in] val      Value to divide
        /// @param[in] divider  Divider
        /// @returns  Quotient and remainder
        static utils::BigUInteger::div_t div(const BigUInteger& val, const BigUInteger divider);
        /// @brief Divide - calculate both quotient and remainder (smaller divider, faster)
        /// @param[in] val      Value to divide
        /// @param[in] divider  Divider
        /// @returns  Quotient and remainder
        template <typename T>
        static utils::BigUInteger::div_t div(const BigUInteger& val, T divider);
        /// @brief Divide by power of 2 - calculate both quotient and remainder (fastest)
        /// @param[in] val    Value to divide
        /// @param[in] power  Power of 2 (power 1 = (val/2) ;  power 3 = (val/8) ;  power 4 = (val/16))
        /// @returns  Quotient and remainder
        static inline BigUInteger::div_t BigUInteger::divPow2(const BigUInteger& val, const uint32_t power = 4u)
        {
            static_assert((power <= 64u), "Max allowed power of 2 is 64");
            BigUInteger::div_t result(BigUInteger((val.m_upperBits >> power), (val.m_lowerBits >> power) | ((val.m_upperBits & 0x0F) << (64u - power))));
            result.rem = BigInteger(0uLL, val.m_lowerBits - (result.quot.m_lowerBits << power));
            return result;
        }
        /// @brief Divide by power of 2 - calculate both quotient and remainder (fastest)
        /// @param[in] val  Value to divide
        /// @returns  Quotient and remainder
        template <uint32_t power = 4u> // power 1 = (val/2) ;  power 3 = (val/8) ;  power 4 = (val/16)
        static inline BigUInteger::div_t BigUInteger::divPow2(const BigUInteger& val)
        {
            static_assert((power <= 64u), "Max allowed power of 2 is 64");
            BigUInteger::div_t result(BigUInteger((val.m_upperBits >> power), (val.m_lowerBits >> power) | ((val.m_upperBits & 0x0F) << (64u - power))));
            result.rem = BigInteger(0uLL, val.m_lowerBits - (result.quot.m_lowerBits << power));
            return result;
        }
        
        
        // -- Unary operators --
        
        /// @brief Pre-increment
        inline BigUInteger& operator++() noexcept 
        {
            if (m_lowerBits < 0xFFFFFFFFFFFFFFFFuLL)
                ++m_lowerBits;
            else
            {
                ++m_upperBits;
                m_lowerBits = 0;
            }
            return *this;
        }
        /// @brief Post-increment
        inline BigUInteger operator++(int) noexcept 
        {
            BigUInteger copy(*this);
            ++(*this);
            return copy;
        }

        /// @brief Pre-decrement
        inline BigUInteger& operator--() noexcept
        {
            if (m_lowerBits > 0uLL)
                --m_lowerBits;
            else
            {
                --m_upperBits;
                m_lowerBits = 0xFFFFFFFFFFFFFFFFuLL;
            }
        }
        /// @brief Post-decrement
        inline BigUInteger operator--(int) noexcept
        {
            BigUInteger copy(*this);
            --(*this);
            return copy;
        }

        /// @brief Value with same sign
        inline BigUInteger operator+() const noexcept { return *this; }
        /// @brief Opposite value
        inline BigUInteger operator-() const noexcept  { return (~(*this) + 1uLL); }
        /// @brief Absolute value
        inline BigUInteger abs() noexcept { return *this; }
        
        
    private:
        uint64_t m_upperBits;  ///< Upper 64 bits
        uint64_t m_lowerBits;  ///< Lower 64 bits
    };
}
    
// declare specialized templates (non-inline)

/// @brief Multiply - smaller value
template <> utils::BigUInteger utils::BigUInteger::operator*<uint32_t>(const uint32_t& val) const noexcept;
template <> utils::BigUInteger utils::BigUInteger::operator*<int32_t>(const int32_t& val) const noexcept;
template <> utils::BigUInteger utils::BigUInteger::operator*<uint64_t>(const uint64_t& val) const noexcept;
template <> utils::BigUInteger utils::BigUInteger::operator*<int64_t>(const int64_t& val) const noexcept;
template <> utils::BigUInteger utils::BigUInteger::operator*<uint16_t>(const uint16_t& val) const noexcept;
template <> utils::BigUInteger utils::BigUInteger::operator*<int16_t>(const int16_t& val) const noexcept;
template <> utils::BigUInteger utils::BigUInteger::operator*<uint8_t>(const uint8_t& val) const noexcept;
template <> utils::BigUInteger utils::BigUInteger::operator*<int8_t>(const int8_t& val) const noexcept;

/// @brief Divide - smaller value
template <> utils::BigUInteger utils::BigUInteger::operator/<uint32_t>(const uint32_t& val) const;
template <> utils::BigUInteger utils::BigUInteger::operator/<int32_t>(const int32_t& val) const;
template <> utils::BigUInteger utils::BigUInteger::operator/<uint64_t>(const uint64_t& val) const;
template <> utils::BigUInteger utils::BigUInteger::operator/<int64_t>(const int64_t& val) const;
template <> utils::BigUInteger utils::BigUInteger::operator/<uint16_t>(const uint16_t& val) const;
template <> utils::BigUInteger utils::BigUInteger::operator/<int16_t>(const int16_t& val) const;
template <> utils::BigUInteger utils::BigUInteger::operator/<uint8_t>(const uint8_t& val) const;
template <> utils::BigUInteger utils::BigUInteger::operator/<int8_t>(const int8_t& val) const;

/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigUInteger::div_t utils::BigUInteger::div<uint32_t>(const utils::BigUInteger& val, uint32_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigUInteger::div_t utils::BigUInteger::div<int32_t>(const utils::BigUInteger& val, int32_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigUInteger::div_t utils::BigUInteger::div<uint64_t>(const utils::BigUInteger& val, uint64_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigUInteger::div_t utils::BigUInteger::div<int64_t>(const utils::BigUInteger& val, int64_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigUInteger::div_t utils::BigUInteger::div<uint16_t>(const utils::BigUInteger& val, uint16_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigUInteger::div_t utils::BigUInteger::div<int16_t>(const utils::BigUInteger& val, int16_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigUInteger::div_t utils::BigUInteger::div<uint8_t>(const utils::BigUInteger& val, uint8_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigUInteger::div_t utils::BigUInteger::div<int8_t>(const utils::BigUInteger& val, int8_t divider);


// -- Special operators --

namespace std 
{
    // Type traits
    template <> 
    struct is_arithmetic<utils::BigUInteger> : std::true_type {};
    template <> 
    struct is_integral<utils::BigUInteger> : std::true_type {};
    template <> 
    struct is_unsigned<utils::BigUInteger> : std::true_type {};
    
    /// @brief Convert to string
    inline std::string to_string(const BigUInteger& val, const uint32_t base = 10u) { return val.toString(base); }
    
    /// @brief Absolute value
    inline utils::BigUInteger abs(const utils::BigUInteger& val) noexcept
    {
        return val;
    }
};

/// @brief Output stream operator
std::ostream& operator<<(std::ostream& stream, const utils::BigUInteger& val);


// -- Bitwise operators (reversed) --

template <typename T> 
inline utils::BigUInteger operator&(const T& lhs, const utils::BigUInteger& rhs) noexcept { return (rhs & lhs); }
template <typename T> 
inline T& operator&=(T& lhs, const utils::BigUInteger& rhs) noexcept { return (lhs = static_cast<T>(rhs & lhs)); }

template <typename T> 
inline utils::BigUInteger operator|(const T& lhs, const utils::BigUInteger& rhs) noexcept { return (rhs | lhs); }
template <typename T> 
inline T& operator|=(T& lhs, const utils::BigUInteger& rhs) noexcept { return (lhs = static_cast<T>(rhs | lhs)); }

template <typename T> 
utils::BigUInteger operator^(const T& lhs, const utils::BigUInteger& rhs) noexcept { return (rhs ^ lhs); }
template <typename T> 
inline T& operator^=(T& lhs, const utils::BigUInteger& rhs) noexcept { return (lhs = static_cast<T>(rhs ^ lhs)); }


// -- Comparison operators (reversed) --

template <typename T> 
inline bool operator==(const T& lhs, const utils::BigUInteger& rhs) noexcept { return (rhs == lhs); }
template <typename T> 
inline bool operator!=(const T& lhs, const utils::BigUInteger& rhs) noexcept { return (rhs != lhs); }

template <typename T> 
inline bool operator<(const T& lhs, const utils::BigUInteger& rhs) noexcept { return (rhs > lhs); }
template <typename T> 
inline bool operator<=(const T& lhs, const utils::BigUInteger& rhs) noexcept { return (rhs >= lhs); }

template <typename T> 
inline bool operator>(const T& lhs, const utils::BigUInteger& rhs) noexcept { return (rhs < lhs); }
template <typename T> 
inline bool operator>=(const T& lhs, const utils::BigUInteger& rhs) noexcept { return (rhs <= lhs); }


// -- Arithmetic operators (reversed) --

template <typename T> 
inline utils::BigUInteger operator+(const T& lhs, const utils::BigUInteger& rhs) noexcept { return rhs + lhs; }
template <typename T> 
inline T& operator+=(T& lhs, const utils::BigUInteger& rhs) noexcept { return (lhs = static_cast<T>(rhs + lhs)); }

template <typename T> 
inline utils::BigUInteger operator-(const T& lhs, const utils::BigUInteger& rhs) noexcept { return -(rhs - lhs); }
template <typename T> 
inline T& operator-=(T& lhs, const utils::BigUInteger& rhs) noexcept { return (lhs = static_cast<T>(-(rhs - lhs))); }

template <typename T> 
inline utils::BigUInteger operator*(const T& lhs, const utils::BigUInteger& rhs) noexcept { return rhs * lhs; }
template <typename T> 
inline T& operator*=(T& lhs, const utils::BigUInteger& rhs) noexcept { return (lhs = static_cast<T>(rhs * lhs)); }

template <typename T> 
inline utils::BigUInteger operator/(const T& lhs, const utils::BigUInteger& rhs) { return utils::BigUInteger(lhs) / rhs; }
template <typename T> 
inline T& operator/=(T& lhs, const utils::BigUInteger& rhs) { return (lhs = static_cast<T>(utils::BigUInteger(lhs) / rhs)); }

template <typename T> 
inline utils::BigUInteger operator%(const T& lhs, const utils::BigUInteger& rhs) { return utils::BigUInteger(lhs) % rhs; }
template <typename T> 
inline T& operator%=(T& lhs, const utils::BigUInteger& rhs) { return (lhs = static_cast<T>(utils::BigUInteger(lhs) % rhs)); }
