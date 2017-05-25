/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : big integer type (int128_t)
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <cmath>

#define int128_t  utils::BigInteger

#define _INT64_SIGN_BIT_UMASK_   0x8000000000000000uLL
#define _INT64_SIGN_BIT_MASK_    0x8000000000000000LL


/// @namespace utils
/// General utilities
namespace utils
{
    class BigUInteger;
    
    /// @class BigInteger
    /// @brief 128-bit signed integer
    class BigInteger
    {
    public:
        /// @struct div_t
        /// @brief Division structure
        struct div_t
        {
            BigInteger quot;
            BigInteger rem;
            
            /// @brief Create empty division structure
            div_t() : quot(0), rem(0) {}
            /// @brief Create division structure
            div_t(const BigInteger& quotient) : quot(quotient), rem(0) {}
            /// @brief Create division structure
            div_t(const BigInteger& quotient, const BigInteger& remainder) : quot(quotient), rem(remainder) {}
            /// @brief Create division structure
            div_t(BigInteger&& quotient) : quot(std::move(quotient)), rem(0) {}
        };
    
    
        /// @brief Create big integer
        BigInteger() noexcept : m_upperBits(0uLL), m_lowerBits(0uLL) {}
        /// @brief Create initialized big integer
        /// @param[in] upper  Upper bits
        /// @param[in] lower  Lower bits
        BigInteger(const uint64_t upper, const uint64_t lower) noexcept : m_upperBits(upper), m_lowerBits(lower) {}
        
        /// @brief Copy big integer
        /// @param[in] val  Other instance
        BigInteger(const BigInteger& val) noexcept : m_upperBits(val.m_upperBits), m_lowerBits(val.m_lowerBits) {}
        /// @brief Move big integer
        /// @param[in] val  Other instance
        BigInteger(BigInteger&& val) noexcept : m_upperBits(std::move(val.m_upperBits)), m_lowerBits(std::move(val.m_lowerBits)) {}
        
        
        // -- Constructors with typecast --
        
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigInteger(const uint64_t val) noexcept : m_upperBits(0uLL), m_lowerBits(val) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigInteger(const int64_t val) noexcept
        {
            if (val < 0)
            {
                m_upperBits(0xFFFFFFFFFFFFFFFFuLL);
                m_lowerBits(val | _INT64_SIGN_BIT_UMASK_);
            }
            else
            {
                m_upperBits(0uLL);
                m_lowerBits(static_cast<uint64_t>(val));
            }
        }
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigInteger(const uint32_t val) noexcept : m_upperBits(0uLL), m_lowerBits(static_cast<uint64_t>(val)) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigInteger(const int32_t val) noexcept 
        {
            if (val < 0)
            {
                m_upperBits(0xFFFFFFFFFFFFFFFFuLL);
                m_lowerBits(val | 0xFFFFFFFF80000000uLL);
            }
            else
            {
                m_upperBits(0uLL);
                m_lowerBits(static_cast<uint64_t>(val));
            }
        }
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigInteger(const uint16_t val) noexcept : m_upperBits(0uLL), m_lowerBits(static_cast<uint64_t>(val)) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigInteger(const int16_t val) noexcept
        {
            if (val < 0)
            {
                m_upperBits(0xFFFFFFFFFFFFFFFFuLL);
                m_lowerBits(val | 0xFFFFFFFFFFFF8000uLL);
            }
            else
            {
                m_upperBits(0uLL);
                m_lowerBits(static_cast<uint64_t>(val));
            }
        }
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigInteger(const uint8_t val) noexcept : m_upperBits(0uLL), m_lowerBits(static_cast<uint64_t>(val)) {}
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigInteger(const int8_t val) noexcept
        {
            if (val < 0)
            {
                m_upperBits(0xFFFFFFFFFFFFFFFFuLL);
                m_lowerBits(val | 0xFFFFFFFFFFFFFF80uLL);
            }
            else
            {
                m_upperBits(0uLL);
                m_lowerBits(static_cast<uint64_t>(val));
            }
        }
        /// @brief Create initialized big integer
        /// @param[in] val  Value to cast
        BigInteger(const bool val) noexcept : m_upperBits(0uLL), m_lowerBits((val) ? 1uLL : 0uLL) {}

        
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
        std::string toString(const uint32_t base = 10u, const bool isPositiveSign = false) const;
        /// @brief Convert number to string
        /// @param[in] base            Number base (optional)
        /// @param[in] isPositiveSign  Show positive sign (optional)
        /// @returns String representation
        inline std::string str(const uint32_t base = 10u, const uint32_t len = 0u, const bool isPositiveSign = false) const 
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
        inline BigInteger& set(const uint64_t upper, const uint64_t lower) noexcept 
        { 
            m_upperBits = upper; 
            m_lowerBits = lower; 
            return *this; 
        }
        
        /// @brief Assign copy of big integer
        /// @param[in] val  Other instance
        /// @returns Current object
        inline BigInteger& operator=(const BigInteger& val) noexcept { return set(val.m_upperBits, val.m_lowerBits); }
        /// @brief Assign moved big integer
        /// @param[in] val  Other instance
        /// @returns Current object
        inline BigInteger& operator=(BigInteger&& val) noexcept 
        { 
            m_upperBits = std::move(val.m_upperBits); 
            m_lowerBits = std::move(val.m_lowerBits); 
            return *this; 
        }
        
        /// @brief Swap value with other instance
        /// @param[out] val  Other instance
        void swap(BigInteger& val)
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
        inline BigInteger& operator=(const uint64_t val) noexcept { return set(0uLL, val); }
        /// @brief Assign value with typecast
        inline BigInteger& operator=(const int64_t val) noexcept
        {
            if (val < 0)
            {
                m_upperBits(0xFFFFFFFFFFFFFFFFuLL);
                m_lowerBits(val | _INT64_SIGN_BIT_UMASK_);
            }
            else
            {
                m_upperBits(0uLL);
                m_lowerBits(static_cast<uint64_t>(val));
            }
        }
        /// @brief Assign value with typecast
        inline BigInteger& operator=(const uint32_t val) noexcept { return set(0uLL, static_cast<uint64_t>(val)); }
        /// @brief Assign value with typecast
        inline BigInteger& operator=(const int32_t val) noexcept
        {
            if (val < 0)
            {
                m_upperBits(0xFFFFFFFFFFFFFFFFuLL);
                m_lowerBits(val | 0xFFFFFFFF80000000uLL);
            }
            else
            {
                m_upperBits(0uLL);
                m_lowerBits(static_cast<uint64_t>(val));
            }
        }
        /// @brief Assign value with typecast
        inline BigInteger& operator=(const uint16_t val) noexcept { return set(0uLL, static_cast<uint64_t>(val)); }
        /// @brief Assign value with typecast
        inline BigInteger& operator=(const int16_t val) noexcept
        {
            if (val < 0)
            {
                m_upperBits(0xFFFFFFFFFFFFFFFFuLL);
                m_lowerBits(val | 0xFFFFFFFFFFFF8000uLL);
            }
            else
            {
                m_upperBits(0uLL);
                m_lowerBits(static_cast<uint64_t>(val));
            }
        }
        /// @brief Assign value with typecast
        inline BigInteger& operator=(const uint8_t val) noexcept  { return set(0uLL, static_cast<uint64_t>(val)); }
        /// @brief Assign value with typecast
        inline BigInteger& operator=(const int8_t val) noexcept
        {
            if (val < 0)
            {
                m_upperBits(0xFFFFFFFFFFFFFFFFuLL);
                m_lowerBits(val | 0xFFFFFFFFFFFFFF80uLL);
            }
            else
            {
                m_upperBits(0uLL);
                m_lowerBits(static_cast<uint64_t>(val));
            }
        }
        /// @brief Assign value with typecast
        inline BigInteger& operator=(const bool val) noexcept     { return set(0uLL, (val) ? 1uLL : 0uLL); }
        
        /// @brief Assign integer part of floating-point value with typecast
        inline BigInteger& operator=(const float val) noexcept    { return BigInteger::operator=(static_cast<const double>(val)); }
        /// @brief Assign integer part of double-precision floating-point value with typecast
        BigInteger& operator=(const double val) noexcept;
        
        
        // -- Typecast operators --
        
        /// @brief Cast as 64-bit integer (destructive)
        inline uint64_t operator uint64_t() const noexcept { return m_lowerBits; }
        /// @brief Cast as 64-bit signed integer (destructive)
        inline int64_t operator int64_t() const noexcept  { return m_lowerBits; }
        /// @brief Cast as 32-bit integer (destructive)
        inline uint32_t operator uint32_t() const noexcept { return m_lowerBits; }
        /// @brief Cast as 32-bit signed integer (destructive)
        inline int32_t operator int32_t() const noexcept  { return m_lowerBits; }
        /// @brief Cast as 16-bit integer (destructive)
        inline uint16_t operator uint16_t() const noexcept { return m_lowerBits; }
        /// @brief Cast as 16-bit signed integer (destructive)
        inline int16_t operator int16_t() const noexcept  { return m_lowerBits; }
        /// @brief Cast as 8-bit integer (destructive)
        inline uint8_t operator uint8_t() const noexcept  { return m_lowerBits; }
        /// @brief Cast as 8-bit signed integer (destructive)
        inline int8_t operator int8_t() const noexcept   { return m_lowerBits; }
        
        /// @brief Cast as boolean
        bool operator bool() const noexcept     { return ((m_lowerBits | m_upperBits) != 0uLL); }
        
        /// @brief Cast as floating-point value (destructive)
        float operator float() const noexcept;
        /// @brief Cast as double-precision floating-point value
        double operator double() const noexcept;
        
        /// @brief Cast as big unsigned integer (destructive)
        BigUInteger operator BigUInteger() const noexcept;
        
        
        // -- bitwise operators --
        
        /// @brief Bitwise AND operator
        BigInteger operator&(const BigInteger& mask) const noexcept
        {
            return BigInteger((m_upperBits & mask.m_upperBits), (m_lowerBits & mask.m_lowerBits));
        }
        /// @brief Bitwise AND operator - any other type
        template <typename T>
        BigInteger operator&(const T& mask) const noexcept
        {
            return BigInteger(0uLL, (m_lowerBits & static_cast<uint64_t>(mask));
        }
        /// @brief Bitwise AND operator
        BigInteger& operator&=(const BigInteger& mask) noexcept
        {
            m_upperBits &= mask.m_upperBits;
            m_lowerBits &= mask.m_lowerBits;
            return *this;
        }
        /// @brief Bitwise AND operator - any other type
        template <typename T>
        BigInteger& operator&=(const T& mask) noexcept
        {
            m_upperBits &= 0uLL;
            m_lowerBits &= static_cast<uint64_t>(mask);
            return *this;
        }
        
        /// @brief Bitwise OR operator
        BigInteger operator|(const BigInteger& mask) const noexcept
        {
            return BigInteger((m_upperBits | mask.m_upperBits), (m_lowerBits | mask.m_lowerBits));
        }
        /// @brief Bitwise OR operator - any other type
        template <typename T>
        BigInteger operator|(const T& mask) const noexcept
        {
            return BigInteger(m_upperBits, (m_lowerBits | static_cast<uint64_t>(mask));
        }
        /// @brief Bitwise OR operator
        BigInteger& operator|=(const BigInteger& mask) noexcept
        {
            m_upperBits |= mask.m_upperBits;
            m_lowerBits |= mask.m_lowerBits;
            return *this;
        }
        /// @brief Bitwise OR operator - any other type
        template <typename T>
        BigInteger& operator|=(const T& mask) noexcept
        {
            m_lowerBits |= static_cast<uint64_t>(mask);
            return *this;
        }
        
        /// @brief Bitwise XOR operator
        BigInteger operator^(const BigInteger& mask) const noexcept
        {
            return BigInteger((m_upperBits ^ mask.m_upperBits), (m_lowerBits ^ mask.m_lowerBits));
        }
        /// @brief Bitwise XOR operator - any other type
        template <typename T>
        BigInteger operator^(const T& mask) const noexcept
        {
            return BigInteger((m_lowerBits ^ 0uLL, (m_lowerBits ^ static_cast<uint64_t>(mask));
        }
        /// @brief Bitwise XOR operator
        BigInteger& operator^=(const BigInteger& mask) noexcept
        {
            m_upperBits ^= mask.m_upperBits;
            m_lowerBits ^= mask.m_lowerBits;
            return *this;
        }
        /// @brief Bitwise XOR operator - any other type
        template <typename T>
        BigInteger& operator^=(const T& mask) noexcept
        {
            m_upperBits ^= mask.m_upperBits;
            m_lowerBits ^= mask.m_lowerBits;
            return *this;
        }
        
        /// @brief Bitwise NOT operator
        BigInteger operator~() const noexcept
        {
            return BigInteger(~m_upperBits, ~m_lowerBits);
        }
        
        /// @brief Bit LEFT-SHIFT operator
        BigInteger operator<<(const uint32_t factor) const noexcept
        {
            if (factor < 64u)
                return BigInteger((m_upperBits << factor) | (m_lowerBits >> (64 - factor)), m_lowerBits << factor);
            if (factor == 64u)
                return BigInteger(m_lowerBits, 0uLL);
            if (factor < 128u)
                return BigInteger(m_lowerBits << (factor - 64u), 0uLL);
            return BigInteger();
        }
        /// @brief Bit LEFT-SHIFT operator
        BigInteger operator<<(const uint64_t factor) const noexcept { return (factor < 128u) ? operator<<(static_cast<uint32_t>(factor)) : BigInteger(); }
        /// @brief Bit LEFT-SHIFT operator
        BigInteger operator<<(const uint16_t factor) const noexcept { return operator<<(static_cast<uint32_t>(factor)); }
        /// @brief Bit LEFT-SHIFT operator
        BigInteger operator<<(const uint8_t factor) const noexcept  { return operator<<(static_cast<uint32_t>(factor)); }
        /// @brief Bit LEFT-SHIFT operator
        BigInteger& operator<<=(const uint32_t factor) noexcept
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
        BigInteger& operator<<=(const uint64_t factor) noexcept { return (factor < 128u) ? operator<<=(static_cast<uint32_t>(factor)) : BigInteger(); }
        /// @brief Bit LEFT-SHIFT operator
        BigInteger& operator<<=(const uint16_t factor) noexcept { return operator<<=(static_cast<uint32_t>(factor)); }
        /// @brief Bit LEFT-SHIFT operator
        BigInteger& operator<<=(const uint8_t factor) noexcept  { return operator<<=(static_cast<uint32_t>(factor)); }
        
        /// @brief Bit RIGHT-SHIFT operator
        BigInteger operator>>(const uint32_t factor) const noexcept
        {
            if (factor < 64u)
                return BigInteger(m_upperBits >> factor, (m_lowerBits >> factor) | (m_upperBits << (64 - factor)));
            if (factor == 64u)
                return BigInteger(0uLL, m_upperBits);
            if (factor < 128u)
                return BigInteger(0uLL, m_upperBits >> (factor - 64u));
            return BigInteger();
        }
        /// @brief Bit RIGHT-SHIFT operator
        BigInteger operator>>(const uint64_t factor) const noexcept { return (factor < 128u) ? operator>>(static_cast<uint32_t>(factor)) : BigInteger(); }
        /// @brief Bit RIGHT-SHIFT operator
        BigInteger operator>>(const uint16_t factor) const noexcept { return operator>>(static_cast<uint32_t>(factor)); }
        /// @brief Bit RIGHT-SHIFT operator
        BigInteger operator>>(const uint8_t factor) const noexcept  { return operator>>(static_cast<uint32_t>(factor)); }
        /// @brief Bit RIGHT-SHIFT operator
        BigInteger& operator>>=(const uint32_t factor) noexcept
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
        BigInteger& operator>>=(const uint64_t factor) noexcept { return (factor < 128u) ? operator>>=(static_cast<uint32_t>(factor)) : BigInteger(); }
        /// @brief Bit RIGHT-SHIFT operator
        BigInteger& operator>>=(const uint16_t factor) noexcept { return operator>>=(static_cast<uint32_t>(factor)); }
        /// @brief Bit RIGHT-SHIFT operator
        BigInteger& operator>>=(const uint8_t factor) noexcept  { return operator>>=(static_cast<uint32_t>(factor)); }
        
        
        // -- Logical operators --
        
        /// @brief Logical NOT operator
        bool operator!() const noexcept { return ((m_lowerBits | m_upperBits) == 0uLL); }
        
        /// @brief Logical AND operator
        bool operator&&(const BigInteger& val) const noexcept { return (static_cast<bool>(*this) && static_cast<bool>(val)); }
        /// @brief Logical AND operator
        template <typename T>
        bool operator&&(const T& val) const noexcept { return (static_cast<bool>(*this) && static_cast<bool>(val)); }

        /// @brief Logical OR operator
        bool operator||(const BigInteger& val) const noexcept { return (static_cast<bool>(*this) || static_cast<bool>(val)); }
        /// @brief Logical OR operator
        template <typename T>
        bool operator||(const T& val) const noexcept { return (static_cast<bool>(*this) || static_cast<bool>(val)); }
        
        
        // -- Comparison operators --
        
        /// @brief Compare instances - equality
        bool operator==(const BigInteger& val) const noexcept { return (m_upperBits == val.m_upperBits && m_lowerBits == val.m_lowerBits); }
        /// @brief Compare instances - equality
        bool operator==(const uint32_t& val) const noexcept { return (m_upperBits == 0uLL && m_lowerBits == val); }
        /// @brief Compare instances - equality
        bool operator==(const int32_t& val) const noexcept 
        { 
            if (val >= 0)
                return (m_upperBits == 0uLL && m_lowerBits == val);
            else
                return (m_upperBits == 0xFFFFFFFFFFFFFFFFuLL && m_lowerBits == (val | 0xFFFFFFFF80000000uLL)); 
        }
        /// @brief Compare instances - equality
        bool operator==(const uint64_t& val) const noexcept { return (m_upperBits == 0uLL && m_lowerBits == val); }
        /// @brief Compare instances - equality
        bool operator==(const int64_t& val) const noexcept 
        { 
            if (val >= 0)
                return (m_upperBits == 0uLL && m_lowerBits == val);
            else
                return (m_upperBits == 0xFFFFFFFFFFFFFFFFuLL && m_lowerBits == (val | _INT64_SIGN_BIT_UMASK_)); 
        }
        /// @brief Compare instances - equality
        bool operator==(const uint16_t& val) const noexcept { return (m_upperBits == 0uLL && m_lowerBits == val); }
        /// @brief Compare instances - equality
        bool operator==(const int16_t& val) const noexcept 
        { 
            if (val >= 0)
                return (m_upperBits == 0uLL && m_lowerBits == val);
            else
                return (m_upperBits == 0xFFFFFFFFFFFFFFFFuLL && m_lowerBits == (val | 0xFFFFFFFFFFFF8000uLL)); 
        }
        /// @brief Compare instances - equality
        bool operator==(const uint8_t& val) const noexcept { return (m_upperBits == 0uLL && m_lowerBits == val); }
        /// @brief Compare instances - equality
        bool operator==(const int8_t& val) const noexcept 
        { 
            if (val >= 0)
                return (m_upperBits == 0uLL && m_lowerBits == val);
            else
                return (m_upperBits == 0xFFFFFFFFFFFFFFFFuLL && m_lowerBits == (val | 0xFFFFFFFFFFFFFF80uLL)); 
        }
        /// @brief Compare instances - equality
        bool operator==(const bool& val) const noexcept { return static_cast<bool>(*this) == val; }
        /// @brief Compare instances - equality
        bool operator==(const float& val) const noexcept { return static_cast<float>(*this) == val; }
        /// @brief Compare instances - equality
        bool operator==(const double& val) const noexcept { return static_cast<double>(*this) == val; }

        /// @brief Compare instances - difference
        bool operator!=(const BigInteger& val) const noexcept { return (m_upperBits != val.m_upperBits || m_lowerBits != val.m_lowerBits); }
        /// @brief Compare instances - difference
        template <typename T>
        bool operator!=(const T& val) const noexcept { return !(*this == val); }

        /// @brief Compare instances - lower
        bool operator<(const BigInteger& val) const noexcept
        { 
            if ((val.m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL) // positive
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) != 0uLL || m_upperBits < val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits < val.m_lowerBits) );
            else // negative
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) != 0uLL && (m_upperBits < val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits < val.m_lowerBits)) );
        }
        /// @brief Compare instances - lower
        template <typename T>
        bool operator<(const T& val) const noexcept 
        { 
            if (val >= 0) // positive
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) != 0uLL || (m_upperBits == 0uLL && m_lowerBits < static_cast<uint64_t>(val))); 
            else // negative
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) != 0uLL && (m_upperBits < 0xFFFFFFFFFFFFFFFFuLL || (~m_lowerBits) > static_cast<uint64_t>(~val)) );
        }
        
        /// @brief Compare instances - lower or equal
        bool operator<=(const BigInteger& val) const noexcept
        { 
            if ((val.m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL) // positive
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) != 0uLL || m_upperBits < val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits <= val.m_lowerBits) );
            else // negative
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) != 0uLL && (m_upperBits < val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits <= val.m_lowerBits)) );
        }
        /// @brief Compare instances - lower or equal
        template <typename T>
        bool operator<=(const T& val) const noexcept
        { 
            if (val >= 0) // positive
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) != 0uLL || (m_upperBits == 0uLL && m_lowerBits <= static_cast<uint64_t>(val))); 
            else // negative
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) != 0uLL && (m_upperBits < 0xFFFFFFFFFFFFFFFFuLL || (~m_lowerBits) >= static_cast<uint64_t>(~val)) );
        }
        
        /// @brief Compare instances - greater
        bool operator>(const BigInteger& val) const noexcept
        { 
            if ((val.m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL) // positive
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL && (m_upperBits > val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits > val.m_lowerBits)) );
            else // negative
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL || m_upperBits > val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits > val.m_lowerBits) );
        }
        /// @brief Compare instances - greater
        template <typename T>
        bool operator>(const T& val) const noexcept
        { 
            if (val >= 0) // positive
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL && (m_upperBits > 0uLL || m_lowerBits > static_cast<uint64_t>(val))); 
            else // negative
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL || (m_upperBits == 0xFFFFFFFFFFFFFFFFuLL && (~m_lowerBits) < static_cast<uint64_t>(~val)) );
        }
        
        /// @brief Compare instances - greater or equal
        bool operator>=(const BigInteger& val) const noexcept
        {
            if ((val.m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL) // positive
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL && (m_upperBits > val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits >= val.m_lowerBits)) );
            else // negative
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL || m_upperBits > val.m_upperBits || (m_upperBits == val.m_upperBits && m_lowerBits >= val.m_lowerBits) );
        }
        /// @brief Compare instances - greater or equal
        template <typename T>
        bool operator>=(const T& val) const noexcept
        { 
            if (val >= 0) // positive
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL && (m_upperBits > 0uLL || m_lowerBits >= static_cast<uint64_t>(val))); 
            else // negative
                return ((m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL || (m_upperBits == 0xFFFFFFFFFFFFFFFFuLL && (~m_lowerBits) <= static_cast<uint64_t>(~val)) );
        }

        
        // -- Arithmetic operators --
        
        /// @brief Add
        BigInteger operator+(const BigInteger& val) const noexcept { BigInteger newVal(*this); newVal += val; return newVal; }
        /// @brief Add - smaller value
        template <typename T> 
        BigInteger operator+(const T& val) const noexcept { BigInteger newVal(*this); newVal += val; return newVal; }
        /// @brief Add to instance
        BigInteger& operator+=(const BigInteger& val) noexcept;
        /// @brief Add to instance - smaller value
        template <typename T> 
        BigInteger& operator+=(const T& val) noexcept;

        /// @brief Substract
        BigInteger operator-(const BigInteger& val) const noexcept { BigInteger newVal(*this); newVal -= val; return newVal; }
        /// @brief Substract - smaller value
        template <typename T> 
        BigInteger operator-(const T& val) const noexcept { BigInteger newVal(*this); newVal -= val; return newVal; }
        /// @brief Substract from instance
        BigInteger& operator-=(const BigInteger& val) noexcept;
        /// @brief Substract from instance - smaller value
        template <typename T> 
        BigInteger& operator-=(const T& val) noexcept;

        /// @brief Multiply
        BigInteger operator*(const BigInteger& val) const noexcept;
        /// @brief Multiply - smaller value
        template <typename T> 
        BigInteger operator*(const T& val) const noexcept;
        /// @brief Multiply instance
        inline BigInteger& operator*=(const BigInteger& val) noexcept { return (*this = *this * val); }
        /// @brief Multiply instance - smaller value
        template <typename T>
        inline BigInteger& operator*=(const T& val) noexcept { return (*this = *this * val); }

        /// @brief Divide
		BigInteger operator/(const BigInteger& val) const;
        /// @brief Divide - smaller value
        template <typename T> 
        BigInteger operator/(const T& val) const;
        /// @brief Divide instance
        inline BigInteger& operator/=(const BigInteger& val) { return (*this = *this / val); }
        /// @brief Divide instance - smaller value
        template <typename T> 
        inline BigInteger& operator/=(const T& val) { return (*this = *this / val); }

        /// @brief Remainder
        BigInteger operator%(const BigInteger& val) const;
        /// @brief Remainder - smaller value
        template <typename T> 
        inline BigInteger operator%(const T& val) const { return (*this - (val * (*this / val))); }
        /// @brief Assign remainder
        inline BigInteger& operator%=(const BigInteger& val) { return (*this = *this % val); }
        /// @brief Assign remainder - smaller value
        template <typename T> 
        inline BigInteger& operator%=(const T& val) { return (*this -= (val * (*this / val))); }
        
        /// @brief Divide - calculate both quotient and remainder (128-bit divider, slow)
        /// @param[in] val      Value to divide
        /// @param[in] divider  Divider
        /// @returns  Quotient and remainder
        static utils::BigInteger::div_t div(const BigInteger& val, const BigInteger divider);
        /// @brief Divide - calculate both quotient and remainder (smaller divider, faster)
        /// @param[in] val      Value to divide
        /// @param[in] divider  Divider
        /// @returns  Quotient and remainder
        template <typename T>
        static utils::BigInteger::div_t div(const BigInteger& val, T divider);
        /// @brief Divide by power of 2 - calculate both quotient and remainder (fastest)
        /// @param[in] val    Value to divide
        /// @param[in] power  Power of 2 (power 1 = (val/2) ;  power 3 = (val/8) ;  power 4 = (val/16))
        /// @returns  Quotient and remainder
        static inline BigInteger::div_t BigInteger::divPow2(const BigInteger& val, const uint32_t power = 4u)
        {
            static_assert((power <= 64u), "Max allowed power of 2 is 64");
            if ((val.m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL)
            {
                BigInteger::div_t result(BigInteger((val.m_upperBits >> power), (val.m_lowerBits >> power) | (val.m_upperBits << (64u - power))));
                result.rem = BigInteger(0uLL, val.m_lowerBits - (result.quot.m_lowerBits << power));
                return result;
            }
            else
            {
                BigInteger::div_t result(BigInteger((val.m_upperBits >> power) | (0xFFFFFFFFFFFFFFFFuLL << (64u - power)), (val.m_lowerBits >> power) | (val.m_upperBits << (64u - power))));
                if (result.quot != val)
                    result.rem = BigInteger(0uLL, val.m_lowerBits - (result.quot.m_lowerBits << power));
                else
                    result = BigInteger::div_t(0uLL, val);
                return result;
            }
        }
        /// @brief Divide by power of 2 - calculate both quotient and remainder (fastest)
        /// @param[in] val  Value to divide
        /// @returns  Quotient and remainder
        template <uint32_t power = 4u> // power 1 = (val/2) ;  power 3 = (val/8) ;  power 4 = (val/16)
        static inline BigInteger::div_t BigInteger::divPow2(const BigInteger& val)
        {
            static_assert((power <= 64u), "Max allowed power of 2 is 64");
            if ((val.m_upperBits & _INT64_SIGN_BIT_UMASK_) == 0uLL)
            {
                BigInteger::div_t result(BigInteger((val.m_upperBits >> power), (val.m_lowerBits >> power) | (val.m_upperBits << (64u - power))));
                result.rem = BigInteger(0uLL, val.m_lowerBits - (result.quot.m_lowerBits << power));
                return result;
            }
            else
            {
                BigInteger::div_t result(BigInteger((val.m_upperBits >> power) | (0xFFFFFFFFFFFFFFFFuLL << (64u - power)), (val.m_lowerBits >> power) | (val.m_upperBits << (64u - power))));
                if (result.quot != val)
                    result.rem = BigInteger(0uLL, val.m_lowerBits - (result.quot.m_lowerBits << power));
                else
                    result = BigInteger::div_t(0uLL, val);
                return result;
            }
        }
        
        
        // -- Unary operators --
        
        /// @brief Pre-increment
        inline BigInteger& operator++() noexcept 
        {
            if (m_lowerBits < 0xFFFFFFFFFFFFFFFFuLL)
            {
                ++m_lowerBits;
            }
            else
            {
                ++m_upperBits;
                m_lowerBits = 0uLL;
            }
            return *this;
        }
        /// @brief Post-increment
        inline BigInteger operator++(int) noexcept 
        {
            BigInteger copy(*this);
            ++(*this);
            return copy;
        }

        /// @brief Pre-decrement
        inline BigInteger& operator--() noexcept
        {
            if (m_lowerBits > 0uLL)
            {
                --m_lowerBits;
            }
            else
            {
                --m_upperBits;
                m_lowerBits = 0xFFFFFFFFFFFFFFFFuLL;
            }
            return *this;
        }
        /// @brief Post-decrement
        inline BigInteger operator--(int) noexcept
        {
            BigInteger copy(*this);
            --(*this);
            return copy;
        }

        /// @brief Value with same sign
        inline BigInteger operator+() const noexcept { return *this; }
        /// @brief Opposite value
        inline BigInteger operator-() const noexcept  { BigInteger opposite(~m_upperBits, ~m_lowerBits); ++opposite; return opposite; }
        /// @brief Absolute value
        inline BigInteger abs() noexcept { return (m_upperBits & _INT64_SIGN_BIT_UMASK_) ? -(*this) : *this; }
        
        
    private:
        uint64_t m_upperBits;  ///< Upper 64 bits
        uint64_t m_lowerBits;  ///< Lower 64 bits
    };
}

// -- Special operators --

namespace std 
{
    // Type traits
    template <> 
    struct is_arithmetic<utils::BigInteger> : std::true_type {};
    template <> 
    struct is_integral<utils::BigInteger> : std::true_type {};
    template <> 
    struct is_unsigned<utils::BigInteger> : std::false_type {};
    
    /// @brief Convert to string
    inline std::string to_string(const BigInteger& val, const uint32_t base = 10u) { return val.toString(base); }
    
    /// @brief Absolute value
    inline utils::BigInteger abs(const utils::BigInteger& val) noexcept
    {
        return val.abs();
    }
};

/// @brief Output stream operator
std::ostream& operator<<(std::ostream& stream, const utils::BigInteger& val);



// declare specialized templates (non-inline)

/// @brief Add - smaller value
template <> utils::BigInteger utils::BigInteger::operator+<uint32_t>(const uint32_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator+<int32_t>(const int32_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator+<uint64_t>(const uint64_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator+<int64_t>(const int64_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator+<uint16_t>(const uint16_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator+<int16_t>(const int16_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator+<uint8_t>(const uint8_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator+<int8_t>(const int8_t& val) const noexcept;
/// @brief Add instance - smaller value
template <> utils::BigInteger& utils::BigInteger::operator+=<uint32_t>(const uint32_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator+=<int32_t>(const int32_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator+=<uint64_t>(const uint64_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator+=<int64_t>(const int64_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator+=<uint16_t>(const uint16_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator+=<int16_t>(const int16_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator+=<uint8_t>(const uint8_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator+=<int8_t>(const int8_t& val) noexcept;

/// @brief Substract - smaller value
template <> utils::BigInteger utils::BigInteger::operator-<uint32_t>(const uint32_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator-<int32_t>(const int32_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator-<uint64_t>(const uint64_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator-<int64_t>(const int64_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator-<uint16_t>(const uint16_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator-<int16_t>(const int16_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator-<uint8_t>(const uint8_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator-<int8_t>(const int8_t& val) const noexcept;
/// @brief Substract instance - smaller value
template <> utils::BigInteger& utils::BigInteger::operator-=<uint32_t>(const uint32_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator-=<int32_t>(const int32_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator-=<uint64_t>(const uint64_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator-=<int64_t>(const int64_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator-=<uint16_t>(const uint16_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator-=<int16_t>(const int16_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator-=<uint8_t>(const uint8_t& val) noexcept;
template <> utils::BigInteger& utils::BigInteger::operator-=<int8_t>(const int8_t& val) noexcept;

/// @brief Multiply - smaller value
template <> utils::BigInteger utils::BigInteger::operator*<uint32_t>(const uint32_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator*<int32_t>(const int32_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator*<uint64_t>(const uint64_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator*<int64_t>(const int64_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator*<uint16_t>(const uint16_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator*<int16_t>(const int16_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator*<uint8_t>(const uint8_t& val) const noexcept;
template <> utils::BigInteger utils::BigInteger::operator*<int8_t>(const int8_t& val) const noexcept;

/// @brief Divide - smaller value
template <> utils::BigInteger utils::BigInteger::operator/<uint32_t>(const uint32_t& val) const;
template <> utils::BigInteger utils::BigInteger::operator/<int32_t>(const int32_t& val) const;
template <> utils::BigInteger utils::BigInteger::operator/<uint64_t>(const uint64_t& val) const;
template <> utils::BigInteger utils::BigInteger::operator/<int64_t>(const int64_t& val) const;
template <> utils::BigInteger utils::BigInteger::operator/<uint16_t>(const uint16_t& val) const;
template <> utils::BigInteger utils::BigInteger::operator/<int16_t>(const int16_t& val) const;
template <> utils::BigInteger utils::BigInteger::operator/<uint8_t>(const uint8_t& val) const;
template <> utils::BigInteger utils::BigInteger::operator/<int8_t>(const int8_t& val) const;

/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigInteger::div_t utils::BigInteger::div<uint32_t>(const utils::BigInteger& val, uint32_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigInteger::div_t utils::BigInteger::div<int32_t>(const utils::BigInteger& val, int32_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigInteger::div_t utils::BigInteger::div<uint64_t>(const utils::BigInteger& val, uint64_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigInteger::div_t utils::BigInteger::div<int64_t>(const utils::BigInteger& val, int64_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigInteger::div_t utils::BigInteger::div<uint16_t>(const utils::BigInteger& val, uint16_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigInteger::div_t utils::BigInteger::div<int16_t>(const utils::BigInteger& val, int16_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigInteger::div_t utils::BigInteger::div<uint8_t>(const utils::BigInteger& val, uint8_t divider);
/// @brief Divide - calculate both quotient and remainder (faster)
template <> utils::BigInteger::div_t utils::BigInteger::div<int8_t>(const utils::BigInteger& val, int8_t divider);



// -- Bitwise operators (reversed) --

template <typename T> 
inline utils::BigInteger operator&(const T& lhs, const utils::BigInteger& rhs) noexcept { return (rhs & lhs); }
template <typename T> 
inline T& operator&=(T& lhs, const utils::BigInteger& rhs) noexcept { return (lhs = static_cast<T>(rhs & lhs)); }

template <typename T> 
inline utils::BigInteger operator|(const T& lhs, const utils::BigInteger& rhs) noexcept { return (rhs | lhs); }
template <typename T> 
inline T& operator|=(T& lhs, const utils::BigInteger& rhs) noexcept { return (lhs = static_cast<T>(rhs | lhs)); }

template <typename T> 
utils::BigInteger operator^(const T& lhs, const utils::BigInteger& rhs) noexcept { return (rhs ^ lhs); }
template <typename T> 
inline T& operator^=(T& lhs, const utils::BigInteger& rhs) noexcept { return (lhs = static_cast<T>(rhs ^ lhs)); }


// -- Comparison operators (reversed) --

template <typename T> 
inline bool operator==(const T& lhs, const utils::BigInteger& rhs) noexcept { return (rhs == lhs); }
template <typename T> 
inline bool operator!=(const T& lhs, const utils::BigInteger& rhs) noexcept { return (rhs != lhs); }

template <typename T> 
inline bool operator<(const T& lhs, const utils::BigInteger& rhs) noexcept { return (rhs > lhs); }
template <typename T> 
inline bool operator<=(const T& lhs, const utils::BigInteger& rhs) noexcept { return (rhs >= lhs); }

template <typename T> 
inline bool operator>(const T& lhs, const utils::BigInteger& rhs) noexcept { return (rhs < lhs); }
template <typename T> 
inline bool operator>=(const T& lhs, const utils::BigInteger& rhs) noexcept { return (rhs <= lhs); }


// -- Arithmetic operators (reversed) --

template <typename T> 
inline utils::BigInteger operator+(const T& lhs, const utils::BigInteger& rhs) noexcept { return rhs + lhs; }
template <typename T> 
inline T& operator+=(T& lhs, const utils::BigInteger& rhs) noexcept { return (lhs = static_cast<T>(rhs + lhs)); }

template <typename T> 
inline utils::BigInteger operator-(const T& lhs, const utils::BigInteger& rhs) noexcept { return -(rhs - lhs); }
template <typename T> 
inline T& operator-=(T& lhs, const utils::BigInteger& rhs) noexcept { return (lhs = static_cast<T>(-(rhs - lhs))); }

template <typename T> 
inline utils::BigInteger operator*(const T& lhs, const utils::BigInteger& rhs) noexcept { return rhs * lhs; }
template <typename T> 
inline T& operator*=(T& lhs, const utils::BigInteger& rhs) noexcept { return (lhs = static_cast<T>(rhs * lhs)); }

template <typename T> 
inline utils::BigInteger operator/(const T& lhs, const utils::BigInteger& rhs) { return utils::BigInteger(lhs) / rhs; }
template <typename T> 
inline T& operator/=(T& lhs, const utils::BigInteger& rhs) { return (lhs = static_cast<T>(utils::BigInteger(lhs) / rhs)); }

template <typename T> 
inline utils::BigInteger operator%(const T& lhs, const utils::BigInteger& rhs) { return utils::BigInteger(lhs) % rhs; }
template <typename T> 
inline T& operator%=(T& lhs, const utils::BigInteger& rhs) { return (lhs = static_cast<T>(utils::BigInteger(lhs) % rhs)); }
