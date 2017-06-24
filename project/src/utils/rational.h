/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : rational number type (rational32_t, rational64_t, rational128_t)
*******************************************************************************/
#pragma once

#undef min
#undef max

#include <cstdint>
#include <cstddef>
#include <string>
#include <stdexcept>
#include <iostream>
#include <type_traits>
#include "number.h"
#include "assert.h"

/// @namespace utils
/// General utilities
namespace utils
{
    using rational32_t = utils::Rational<int32_t>;
    using rational64_t = utils::Rational<int64_t>;
    
    /// @class Rational
    /// @brief rational number (fraction)
    template <typename T, typename U = T>
    class Rational
    {
    public:
        /// @brief Create rational number
        Rational() noexcept : m_cardinal(0), m_ordinal(1) 
        { 
            static_assert(std::is_integral<T>::value && std::is_integral<U>::value, "Integer template parameters required"); 
        }
        /// @brief Create initialized rational number
        /// @param[in] cardinal  Cardinal number (numerator)
        /// @param[in] ordinal   Ordinal number (denominator)
        Rational(const T cardinal, const U ordinal) noexcept : m_cardinal(cardinal), m_ordinal(ordinal) 
        {
            static_assert(std::is_integral<T>::value && std::is_integral<U>::value, "Integer template parameters required"); 
            if (ordinal <= 0)
                m_ordinal = 1;
        }
        /// @brief Create initialized rational number from integer (ordinal 1)
        /// @param[in] cardinal  Cardinal number (numerator)
        explicit Rational(const int32_t cardinal) noexcept : m_cardinal(static_cast<T>(cardinal)), m_ordinal(1)
        { 
            static_assert(std::is_integral<T>::value && std::is_integral<U>::value, "Integer template parameters required"); 
        }
        /// @brief Create initialized rational number from integer (ordinal 1)
        /// @param[in] cardinal  Cardinal number (numerator)
        explicit Rational(const int64_t cardinal) noexcept : m_cardinal(static_cast<T>(cardinal)), m_ordinal(1)
        { 
            static_assert(std::is_integral<T>::value && std::is_integral<U>::value, "Integer template parameters required"); 
        }
        /// @brief Create initialized rational number from integer (ordinal 1)
        /// @param[in] cardinal  Cardinal number (numerator)
        explicit Rational(const int16_t cardinal) noexcept : m_cardinal(static_cast<T>(cardinal)), m_ordinal(1)
        { 
            static_assert(std::is_integral<T>::value && std::is_integral<U>::value, "Integer template parameters required"); 
        }
        /// @brief Create initialized rational number from integer (ordinal 1)
        /// @param[in] cardinal  Cardinal number (numerator)
        explicit Rational(const int8_t cardinal) noexcept : m_cardinal(static_cast<T>(cardinal)), m_ordinal(1)
        { 
            static_assert(std::is_integral<T>::value && std::is_integral<U>::value, "Integer template parameters required"); 
        }
        
        /// @brief Copy rational number
        /// @param[in] val  Other instance
        explicit Rational(const Rational<T,U>& val) noexcept : m_cardinal(val.m_cardinal), m_ordinal(val.m_ordinal)
        { 
            static_assert(std::is_integral<T>::value && std::is_integral<U>::value, "Integer template parameters required"); 
        }
        /// @brief Move rational number
        /// @param[in] val  Other instance
        explicit Rational(Rational<T,U>&& val) noexcept : m_cardinal(std::move(val.m_cardinal)), m_ordinal(std::move(val.m_ordinal))
        { 
            static_assert(std::is_integral<T>::value && std::is_integral<U>::value, "Integer template parameters required"); 
        }
        
        
        // -- Getters --
        
        /// @brief Get cardinal part
        /// @returns Numerator
        inline T getCardinal() const noexcept  { return m_cardinal; }
        /// @brief Get ordinal part
        /// @returns Denominator
        inline U getOrdinal() const noexcept  { return m_ordinal; }
        
        /// @brief Get greatest common divider between cardinal and ordinal
        inline T getGcd() const noexcept 
        {
            if (m_cardinal == 0) 
                return m_ordinal;
            T remainder;
            T lhs = m_cardinal;
            T rhs = m_ordinal;
            while (rhs != 0)
            {
                remainder = lhs % rhs;
                lhs = rhs;
                rhs = remainder;
            }
            return lhs;
        }
        
        /// @brief Convert rational number to double-precision floating-point value
        /// @returns Double-precision value
        inline double toDouble() const noexcept
        {
            ASSERT(m_ordinal != 0);
            if (m_ordinal == 0)
                return 0;
            return static_cast<double>(m_cardinal) / static_cast<double>(m_ordinal);
        }
        
        /// @brief Convert rational number to percentage integer (floor)
        /// @returns Percentage (floor)
        inline int32_t toPercent() const noexcept
        {
            ASSERT(m_ordinal != 0);
            if (m_ordinal == 0)
                return 0;
            T percent;
            if (std::abs(m_cardinal) < Number::getMaxValue<T>() / 100)
            {
                percent = (m_cardinal * static_cast<T>(100)) / static_cast<T>(m_ordinal);
            }
            else
            {
                percent = (m_cardinal / static_cast<T>(m_ordinal)) * static_cast<T>(100); // two calculations to avoid reaching max int value
                percent += ((m_cardinal % static_cast<T>(m_ordinal)) * static_cast<T>(100)) / static_cast<T>(m_ordinal);
            }
            return static_cast<int32_t>(percent);
        }
        /// @brief Convert rational number to percentage integer (round)
        /// @returns Percentage (round)
        inline int32_t toRoundPercent() const noexcept
        {
            int32_t percent = toPercent();
            double decimals = (toDouble() * 100.0) - static_cast<double>(percent);
            if (decimals < 0.0) 
                return (decimals <= -0.5) ? percent - 1 : percent;
            return (decimals >= 0.5) ? percent + 1 : percent;
        }
        
        /// @brief Convert rational number to string
        /// @param[in] separator  Separator between cardinal and ordinal
        /// @returns String representation
        inline std::string toString(char separator = '/') const noexcept
        {
            if (m_ordinal == 1) // no denominator -> show as integer
                return std::to_string(m_cardinal);
            if (separator == '\0') // fix invalid separators
                separator = '/';
            else if (separator >= '0' && separator <= '9')
                separator = '#';
            return std::to_string(m_cardinal) + std::string(separator) + std::to_string(m_ordinal);
        }
        /// @brief Convert rational number to string
        /// @returns String representation
        inline std::string str() const noexcept
        { 
            return toString('/');
        }
        
        
        // -- Assignment --
        
        /// @brief Set cardinal and ordinal values
        /// @param[in] cardinal  Cardinal number (numerator)
        /// @param[in] ordinal   Ordinal number (denominator)
        /// @returns Current object
        inline Rational<T,U>& set(const T cardinal, const U ordinal) noexcept 
        { 
            m_cardinal = cardinal; 
            m_ordinal = (ordinal > 0) ? ordinal : 1; 
            return *this; 
        }
        /// @brief Extract cardinal part
        /// @returns Numerator
        inline Rational<T,U>& setCardinal(const T value) noexcept  { m_cardinal = value; return *this; }
        
        /// @brief Parse rational number in a string
        /// @param[in] value  String value
        /// @returns Rational number instance
        static inline Rational<T,U> parse(const std::string& value)
        {
            bool isPositive = true;
            const char* valStr = val.c_str();
            
            // extract sign
            if (*valStr == '-')
            {
                isPositive = false;
                ++valStr;
            }
            else if (*valStr == '+')
                ++valStr;

            // extract values
            T cardinal;
            U ordinal = 1;
            valStr += Number::parseUInt<T>(valStr, cardinal);
            if (*valStr != '\0') // explicit ordinal
            {
                ++valStr; // skip separator
                Number::parseUInt<U>(valStr, ordinal);
                if (ordinal == 0)
                    throw std::invalid_argument("Rational.parseString: invalid division by 0.");
            }
            return Rational<T,U>((isPositive) ? cardinal : -cardinal, ordinal);
        }
        
    public:
        /// @brief Assign copy of rational number
        /// @param[in] rhs  Other instance
        /// @returns Current object
        explicit inline Rational<T,U>& operator=(const Rational<T,U>& rhs) noexcept 
        { 
            ASSERT(rhs.m_ordinal != 0);
            return set(rhs.m_cardinal, rhs.m_ordinal); 
        }
        /// @brief Assign moved rational number
        /// @param[in] rhs  Other instance
        /// @returns Current object
        explicit inline Rational<T,U>& operator=(Rational<T,U>&& rhs) noexcept 
        { 
            ASSERT(rhs.m_ordinal != 0);
            m_cardinal = std::move(rhs.m_cardinal); 
            m_ordinal = std::move(rhs.m_ordinal); 
            return *this; 
        }
        
        /// @brief Swap value with other instance
        /// @param[out] rhs  Other instance
        void swap(Rational<T,U>& rhs)
        {
            ASSERT(rhs.m_ordinal != 0);
            m_cardinal ^= rhs.m_cardinal;
            rhs.m_cardinal ^= m_cardinal;
            m_cardinal ^= rhs.m_cardinal;
            m_ordinal ^= rhs.m_ordinal;
            rhs.m_ordinal ^= m_ordinal;
            m_ordinal ^= rhs.m_ordinal;
        }
        
        
        // -- Assignment with typecast --
        
        /// @brief Assign value with typecast
        explicit inline Rational<T,U>& operator=(const uint64_t rhs) noexcept { return set(static_cast<T>(rhs), 1); }
        /// @brief Assign value with typecast
        explicit inline Rational<T,U>& operator=(const int64_t rhs) noexcept  { return set(static_cast<T>(rhs), 1); }
        /// @brief Assign value with typecast
        explicit inline Rational<T,U>& operator=(const uint32_t rhs) noexcept { return set(static_cast<T>(rhs), 1); }
        /// @brief Assign value with typecast
        explicit inline Rational<T,U>& operator=(const int32_t rhs) noexcept  { return set(static_cast<T>(rhs), 1); }
        /// @brief Assign value with typecast
        explicit inline Rational<T,U>& operator=(const uint16_t rhs) noexcept { return set(static_cast<T>(rhs), 1); }
        /// @brief Assign value with typecast
        explicit inline Rational<T,U>& operator=(const int16_t rhs) noexcept  { return set(static_cast<T>(rhs), 1); }
        /// @brief Assign value with typecast
        explicit inline Rational<T,U>& operator=(const uint8_t rhs) noexcept  { return set(static_cast<T>(rhs), 1); }
        /// @brief Assign value with typecast
        explicit inline Rational<T,U>& operator=(const int8_t rhs) noexcept   { return set(static_cast<T>(rhs), 1); }
        /// @brief Assign value with typecast
        explicit inline Rational<T,U>& operator=(const bool rhs) noexcept     { return set((rhs) ? 1 : 0, 1); }
        
        
        // -- Typecast operators --
        
        /// @brief Cast as 64-bit integer (destructive)
        explicit inline operator uint64_t() const noexcept { ASSERT(m_ordinal != 0); return static_cast<uint64_t>(m_cardinal / m_ordinal); }
        /// @brief Cast as 64-bit signed integer (destructive)
        explicit inline operator int64_t() const noexcept   { ASSERT(m_ordinal != 0); return static_cast<int64_t>(m_cardinal / m_ordinal); }
        /// @brief Cast as 32-bit integer (destructive)
        explicit inline operator uint32_t() const noexcept { ASSERT(m_ordinal != 0); return static_cast<uint32_t>(m_cardinal / m_ordinal); }
        /// @brief Cast as 32-bit signed integer (destructive)
        explicit inline operator int32_t() const noexcept   { ASSERT(m_ordinal != 0); return static_cast<int32_t>(m_cardinal / m_ordinal); }
        /// @brief Cast as 16-bit integer (destructive)
        explicit inline operator uint16_t() const noexcept { ASSERT(m_ordinal != 0); return static_cast<uint16_t>(m_cardinal / m_ordinal); }
        /// @brief Cast as 16-bit signed integer (destructive)
        explicit inline operator int16_t() const noexcept   { ASSERT(m_ordinal != 0); return static_cast<int16_t>(m_cardinal / m_ordinal); }
        /// @brief Cast as 8-bit integer (destructive)
        explicit inline operator uint8_t() const noexcept   { ASSERT(m_ordinal != 0); return static_cast<uint8_t>(m_cardinal / m_ordinal); }
        /// @brief Cast as 8-bit signed integer (destructive)
        explicit inline operator int8_t() const noexcept     { ASSERT(m_ordinal != 0); return static_cast<int8_t>(m_cardinal / m_ordinal); }
        
        /// @brief Cast as boolean
        explicit inline operator bool() const noexcept { return (m_cardinal != 0); }
        
        /// @brief Cast as floating-point value (destructive)
        explicit inline operator float() const noexcept   { return static_cast<float>(toDouble()); }
        /// @brief Cast as double-precision floating-point value
        explicit inline operator double() const noexcept { return toDouble(); }
        
        
        // -- Logical operators --
        
        /// @brief Logical NOT operator
        inline bool operator!() const noexcept { return (m_cardinal == 0); }
        
        /// @brief Logical AND operator
        inline bool operator&&(const Rational<T,U>& rhs) const noexcept { return (static_cast<bool>(*this) && static_cast<bool>(rhs)); }
        /// @brief Logical AND operator
        template <typename T>
        inline bool operator&&(const T& rhs) const noexcept { return (static_cast<bool>(*this) && static_cast<bool>(rhs)); }

        /// @brief Logical OR operator
        inline bool operator||(const Rational<T,U>& rhs) const noexcept { return (static_cast<bool>(*this) || static_cast<bool>(rhs)); }
        /// @brief Logical OR operator
        template <typename T>
        inline bool operator||(const T& rhs) const noexcept { return (static_cast<bool>(*this) || static_cast<bool>(rhs)); }
        
        
        // -- Comparison operators --
        
        /// @brief Compare instances
        template <typename V, typename W>
        inline int32_t compare(const Rational<V,W>& rhs) const noexcept
        {
            ASSERT(m_ordinal != 0); 
            ASSERT(rhs.m_ordinal != 0); 
            if ((m_ordinal & rhs.m_ordinal) == 0)
                return 0;
            T floorLhs = m_cardinal / static_cast<T>(m_ordinal);
            T floorRhs = static_cast<T>(rhs.m_cardinal / rhs.m_ordinal);
            if (floorLhs == floorRhs)
            {
                T remLhs = (m_cardinal % static_cast<T>(m_ordinal)) * static_cast<T>(rhs.m_ordinal);
                T remRhs = (static_cast<T>(rhs.m_cardinal % rhs.m_ordinal)) * static_cast<T>(m_ordinal);
                if (remLhs == remRhs)
                    return 0;
                else
                    return (remLhs < remRhs) ? -1 : 1;
            }
            else
            {
                return (floorLhs < floorRhs) ? -1 : 1;
            }
        }
        /// @brief Compare instances
        template <typename V>
        inline int32_t compare(const V rhs) const noexcept
        {
            ASSERT(m_ordinal != 0); 
            if (m_ordinal == 0)
                return 0;
            T floor = m_cardinal / static_cast<T>(m_ordinal);
            if (floor == static_cast<T>(rhs))
            {
                if (m_cardinal % static_cast<T>(m_ordinal) == 0)
                    return 0;
                else
                    return 1;
            }
            else
            {
                return (floor < static_cast<T>(rhs)) ? -1 : 1;
            }
        }
        
        /// @brief Compare instances - strict equality
        template <typename V, typename W>
        inline bool strictlyEquals(const Rational<V,W>& rhs) const noexcept { return (m_cardinal == rhs.m_cardinal && m_ordinal == rhs.m_ordinal); }
        /// @brief Compare instances - equality
        template <typename V, typename W>
        inline bool equals(const Rational<V,W>& rhs) const noexcept { return (compare(rhs) == 0); }
        /// @brief Compare instances - equality
        template <typename V, typename W>
        inline bool operator==(const Rational<V,W>& rhs) const noexcept { return (compare(rhs) == 0); }
        /// @brief Compare instances - equality
        template <typename V>
        inline bool operator==(const V rhs) const noexcept { return (compare(rhs) == 0); }

        /// @brief Compare instances - difference
        template <typename V, typename W>
        inline bool operator!=(const Rational<V,W>& rhs) const noexcept { return (compare(rhs) != 0); }
        /// @brief Compare instances - difference
        template <typename V>
        inline bool operator!=(const V rhs) const noexcept { return (compare(rhs) != 0); }

        /// @brief Compare instances - lower
        template <typename V, typename W>
        bool operator<(const Rational<V,W>& rhs) const noexcept { return (compare(rhs) < 0); }
        /// @brief Compare instances - lower
        template <typename V>
        bool operator<(const V rhs) const noexcept { return (compare(rhs) < 0); }
        
        /// @brief Compare instances - lower or equal
        template <typename V, typename W>
        bool operator<=(const Rational<V,W>& rhs) const noexcept { return (compare(rhs) <= 0); }
        /// @brief Compare instances - lower or equal
        template <typename V>
        bool operator<=(const V rhs) const noexcept { return (compare(rhs) <= 0); }
        
        /// @brief Compare instances - greater
        template <typename V, typename W>
        bool operator>(const Rational<V,W>& rhs) const noexcept { return (compare(rhs) > 0); }
        /// @brief Compare instances - greater
        template <typename V>
        bool operator>(const V rhs) const noexcept { return (compare(rhs) > 0); }
        
        /// @brief Compare instances - greater or equal
        template <typename V, typename W>
        bool operator>=(const Rational<V,W>& rhs) const noexcept { return (compare(rhs) >= 0); }
        /// @brief Compare instances - greater or equal
        template <typename V>
        bool operator>=(const V rhs) const noexcept { return (compare(rhs) >= 0); }
       
        
        // -- Arithmetic operators --
        
        /// @brief Simplify cardinal/ordinal
        /// @returns Current instance
        Rational<T,U>& simplify() noexcept
        {
            T gcd = getGcd();
            if (gcd > 1)
            {
                m_cardinal /= gcd;
                m_ordinal /= gcd;
            }
            return *this;
        }
        
        /// @brief Invert cardinal/ordinal
        /// @returns Current instance
        Rational<T,U>& invert()
        {
            if (m_cardinal == 0)
                throw std::domain_error("Error: invalid division by zero");
            
            U cardinal = m_cardinal;
            if (m_cardinal > 0)
            {
                m_cardinal = m_ordinal;
                m_ordinal = cardinal;
            }
            else
            {
                m_cardinal = -m_ordinal;
                m_ordinal = -cardinal;
            }
        }
        
        /// @brief Add
        template <typename V, typename W>
        Rational<T,U> operator+(const Rational<V,W>& rhs) const noexcept
        {
            Rational<T,U> data(*this);
            data += rhs;
            return data;
        }
        /// @brief Add
        template <typename V>
        Rational<T,U> operator+(const V rhs) const noexcept
        {
            Rational<T,U> data(*this);
            data += rhs;
            return data;
        }
        /// @brief Add to instance
        template <typename V, typename W>
        Rational<T,U>& operator+=(const Rational<V,W>& rhs) noexcept
        {
            m_cardinal = m_cardinal * rhs.m_ordinal + rhs.m_cardinal * m_ordinal;
            m_ordinal *= rhs.m_ordinal;
            simplify();
            return *this;
        }
        /// @brief Add to instance
        template <typename V>
        Rational<T,U>& operator+=(const V rhs) noexcept
        {
            m_cardinal += rhs * m_ordinal;
            simplify();
            return *this;
        }

        /// @brief Substract
        template <typename V, typename W>
        Rational<T,U> operator-(const Rational<V,W>& rhs) const noexcept
        {
            Rational<T,U> data(*this);
            data -= rhs;
            return data;
        }
        /// @brief Substract
        template <typename V>
        Rational<T,U> operator-(const V rhs) const noexcept
        {
            Rational<T,U> data(*this);
            data -= rhs;
            return data;
        }
        /// @brief Substract from instance
        template <typename V, typename W>
        Rational<T,U>& operator-=(const Rational<V,W>& rhs) noexcept
        {
            m_cardinal = m_cardinal * rhs.m_ordinal - rhs.m_cardinal * m_ordinal;
            m_ordinal *= rhs.m_ordinal;
            simplify();
            return *this;
        }
        /// @brief Substract from instance
        template <typename V>
        Rational<T,U>& operator-=(const V rhs) noexcept
        {
            m_cardinal -= rhs * m_ordinal;
            simplify();
            return *this;
        }

        /// @brief Multiply
        template <typename V, typename W>
        Rational<T,U> operator*(const Rational<V,W>& rhs) const noexcept
        {
            Rational<T,U> data(*this);
            data *= rhs;
            return data;
        }
        /// @brief Multiply
        template <typename V>
        Rational<T,U> operator*(const V rhs) const noexcept
        {
            Rational<T,U> data(*this);
            data *= rhs;
            return data;
        }
        /// @brief Multiply instance
        template <typename V, typename W>
        Rational<T,U>& operator*=(const Rational<V,W>& rhs) noexcept
        {
            m_cardinal *= rhs.m_cardinal;
            m_ordinal *= rhs.m_ordinal;
            simplify();
            return *this;
        }
        /// @brief Multiply instance
        template <typename V>
        Rational<T,U>& operator*=(const V rhs) noexcept
        {
            m_cardinal *= rhs;
            simplify();
            return *this;
        }

        /// @brief Divide
		template <typename V, typename W>
        Rational<T,U> operator/(const Rational<V,W>& rhs) const
        {
            Rational<T,U> data(*this);
            data /= rhs;
            return data;
        }
        /// @brief Divide
        template <typename V>
        Rational<T,U> operator/(const V rhs) const
        {
            Rational<T,U> data(*this);
            data /= rhs;
            return data;
        }
        /// @brief Divide instance
        template <typename V, typename W>
        Rational<T,U>& operator/=(const Rational<V,W>& rhs)
        {
            if (rhs.m_cardinal == 0)
                throw std::domain_error("Error: invalid division by zero");
            m_cardinal *= rhs.m_ordinal;
            m_ordinal *= rhs.m_cardinal;
            simplify();
            return *this;
        }
        /// @brief Divide instance
        template <typename V>
        Rational<T,U>& operator/=(const V rhs)
        {
            if (rhs == 0)
                throw std::domain_error("Error: invalid division by zero");
            m_ordinal *= rhs;
            simplify();
            return *this;
        }


        // -- Unary operators --
        
        /// @brief Pre-increment (add 1/1)
        inline Rational<T,U>& operator++() noexcept 
        {
            m_cardinal += m_ordinal;
            return *this;
        }
        /// @brief Post-increment (add 1/1)
        inline Rational<T,U> operator++(int) noexcept 
        {
            Rational<T,U> copy(*this);
            m_cardinal += m_ordinal;
            return copy;
        }

        /// @brief Pre-decrement (substract 1/1)
        inline Rational<T,U>& operator--() noexcept
        {
            m_cardinal -= m_ordinal;
        }
        /// @brief Post-decrement (substract 1/1)
        inline Rational<T,U> operator--(int) noexcept
        {
            Rational<T,U> copy(*this);
            m_cardinal -= m_ordinal;
            return copy;
        }

        /// @brief Value with same sign
        inline Rational<T,U> operator+() const noexcept { return *this; }
        /// @brief Opposite value
        inline Rational<T,U> operator-() const noexcept  { return Rational<T,U>(-m_cardinal, m_ordinal); }
        /// @brief Absolute value
        inline Rational<T,U> abs() noexcept { return (m_cardinal > 0) ? *this : -(*this); }
        
        
    private:
        T m_cardinal;  ///< Cardinal number (numerator)
        U m_ordinal;   ///< Ordinal number (denominator)
    };
}
    
    
// -- Special operators --

namespace std 
{
    /// @brief Convert to string
    template <typename T, typename U>
    inline std::string to_string(const utils::Rational<T,U>& val, const char sep = '/') { return val.toString(sep); }
    
    /// @brief Absolute value
    template <typename T, typename U>
    inline utils::Rational<T,U> abs(const utils::Rational<T,U>& val) noexcept
    {
        return val.abs();
    }
};

/// @brief Output stream operator
template <typename T, typename U>
std::ostream& operator<<(std::ostream& stream, const utils::Rational<T,U>& rhs)
{
    stream << rhs.toString('/');
    return stream;
}


// -- Comparison operators (reversed) --

/// @brief Compare equality between rational and other type
template <typename T, typename U, typename V> 
inline bool operator==(const V& lhs, const utils::Rational<T,U>& rhs) noexcept { return (rhs == lhs); }
/// @brief Compare difference between rational and other type
template <typename T, typename U, typename V> 
inline bool operator!=(const V& lhs, const utils::Rational<T,U>& rhs) noexcept { return (rhs != lhs); }

/// @brief Compare rational and other type - smaller
template <typename T, typename U, typename V> 
inline bool operator<(const V& lhs, const utils::Rational<T,U>& rhs) noexcept { return (rhs > lhs); }
/// @brief Compare rational and other type - smaller or equal
template <typename T, typename U, typename V> 
inline bool operator<=(const V& lhs, const utils::Rational<T,U>& rhs) noexcept { return (rhs >= lhs); }

/// @brief Compare rational and other type - greater
template <typename T, typename U, typename V> 
inline bool operator>(const V& lhs, const utils::Rational<T,U>& rhs) noexcept { return (rhs < lhs); }
/// @brief Compare rational and other type - greater or equal
template <typename T>, typename U, typename V 
inline bool operator>=(const V& lhs, const utils::Rational<T,U>& rhs) noexcept { return (rhs <= lhs); }


// -- Arithmetic operators (reversed) --

/// @brief Add rational to other type
template <typename T, typename U, typename V> 
inline utils::Rational<T,U> operator+(const V& lhs, const utils::Rational<T,U>& rhs) noexcept { return rhs + lhs; }
/// @brief Add rational to other type
template <typename T, typename U, typename V> 
inline V& operator+=(V& lhs, const utils::Rational<T,U>& rhs) noexcept { return (lhs = static_cast<V>(rhs + lhs)); }

/// @brief Substract rational from other type
template <typename T, typename U, typename V> 
inline utils::Rational<T,U> operator-(const V& lhs, const utils::Rational<T,U>& rhs) noexcept { return -(rhs - lhs); }
/// @brief Substract rational from other type
template <typename T, typename U, typename V> 
inline V& operator-=(V& lhs, const utils::Rational<T,U>& rhs) noexcept { return (lhs = static_cast<V>(-(rhs - lhs))); }

/// @brief Multiply other type by rational
template <typename T, typename U, typename V> 
inline utils::Rational<T,U> operator*(const V& lhs, const utils::Rational<T,U>& rhs) noexcept { return rhs * lhs; }
/// @brief Multiply other type by rational
template <typename T, typename U, typename V> 
inline V& operator*=(V& lhs, const utils::Rational<T,U>& rhs) noexcept { return (lhs = static_cast<V>(rhs * lhs)); }

/// @brief Divide other type by rational
template <typename T, typename U, typename V> 
inline utils::Rational<T,U> operator/(const V& lhs, const utils::Rational<T,U>& rhs) { return utils::Rational<T,U>(lhs) / rhs; }
/// @brief Divide other type by rational
template <typename T, typename U, typename V> 
inline V& operator/=(V& lhs, const utils::Rational<T,U>& rhs) { return (lhs = static_cast<V>(utils::Rational<T,U>(lhs) / rhs)); }
