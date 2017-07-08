/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : screen ratio
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <unordered_set>

/// @namespace utils
/// General utilities
namespace utils
{
    using delegate = utils::system::Delegate; ///< utils::delegate = Delegate alias
    
    /// @namespace utils.system
    /// System management utilities
    namespace system
    {
        /// @class Delegate
        /// @brief Function delegate - template parameters : return type, function arguments
        template <typename ReturnType, typename ... Params>
        class Delegate
        {
        public:
            /// @brief Create empty delegate (template parameters required)
            Delegate() 
            {
                m_functions.reserve(4);
            }
            /// @brief Create delegate with initial function pointer (template parameters can be implicit)
            /// @param[in] pFunc  Function pointer
            Delegate(ReturnType (*pFunc)(Params...))
            {
                m_functions.reserve(4);
                if (pFunc != nullptr)
                    m_functions.insert(pFunc);
            }
            /// @brief Copy delegate (template parameters can be implicit)
            /// @param[in] other  Other instance
            Delegate(const Delegate<ReturnType, Params...>& other)
            {
                m_functions.reserve(4);
                m_functions = other.m_functions;
            }
            /// @brief Move delegate (template parameters can be implicit)
            /// @param[in] other  Other instance
            Delegate(Delegate<ReturnType, Params...>&& other) : m_functions(std::move(other.m_functions)) {}
            
            
            // -- Execution --
            
            /// @brief Execute functions of delegate with specified arguments
            inline void operator()(Params&... args) const { run(args...); }
            /// @brief Execute functions of delegate with specified arguments
            inline void run(Params&... args) const
            {
                for (auto& it : other)
                    it(args...);
            }
            
            
            // -- Setters --
            
            /// @brief Remove all existing pointers
            inline void clear() noexcept { m_functions.clear(); }
            
            /// @brief Assign function pointer (replace all existing pointers)
            /// @param[in] pFunc  Function pointer
            /// @returns Delegate reference
            inline Delegate<ReturnType, Params...>& operator=(ReturnType (*pFunc)(Params...))
            {
                clear();
                if (pFunc != nullptr)
                    m_functions.insert(pFunc);
                return *this;
            }
            /// @brief Assign copy of delegate
            /// @param[in] other  Other instance
            /// @returns Delegate reference
            inline Delegate<ReturnType, Params...>& operator=(const Delegate<ReturnType, Params...>& other)
            {
                m_functions = other.m_functions;
                return *this;
            }
            /// @brief Move delegate
            /// @param[in] other  Other instance
            /// @returns Delegate reference
            inline Delegate<ReturnType, Params...>& operator=(Delegate<ReturnType, Params...>&& other)
            {
                m_functions = std::move(other.m_functions)
                return *this;
            }
            
            
            /// @brief Add function pointer to delegate
            /// @param[in] pFunc  Function pointer
            inline void operator+=(ReturnType (*pFunc)(Params...))
            {
                if (pFunc != nullptr)
                    m_functions.insert(pFunc);
            }
            /// @brief Add function pointers of another delegate instance
            /// @param[in] other  Other instance
            inline void operator+=(const Delegate<ReturnType, Params...>& other)
            {
                for (auto it : other)
                    m_functions.insert(it);
            }
            /// @brief Add function pointer to delegate
            /// @param[in] pFunc  Function pointer
            /// @returns New delegate
            inline Delegate<ReturnType, Params...> operator+(ReturnType (*pFunc)(Params...)) { Delegate<ReturnType, Params...> copy(*this); copy += pFunc; return copy; }
            /// @brief Add function pointers of another delegate instance
            /// @param[in] other  Other instance
            /// @returns New delegate
            inline Delegate<ReturnType, Params...> operator+(const Delegate<ReturnType, Params...>& other) { Delegate<ReturnType, Params...> copy(*this); copy += other; return copy; }
            
            /// @brief Remove function pointer from delegate
            /// @param[in] pFunc  Function pointer
            inline void operator-=(ReturnType (*pFunc)(Params...))
            {
                if (pFunc != nullptr)
                    m_functions.erase(pFunc);
            }
            /// @brief Remove function pointers present in another delegate instance
            /// @param[in] other  Other instance
            inline void operator-=(const Delegate<ReturnType, Params...>& other)
            {
                for (auto it : other)
                    m_functions.erase(it);
            }
            /// @brief Remove function pointer from delegate
            /// @param[in] pFunc  Function pointer
            /// @returns New delegate
            inline Delegate<ReturnType, Params...> operator-(ReturnType (*pFunc)(Params...)) { Delegate<ReturnType, Params...> copy(*this); copy -= pFunc; return copy; }
            /// @brief Remove function pointers present in another delegate instance
            /// @param[in] other  Other instance
            /// @returns New delegate
            inline Delegate<ReturnType, Params...> operator-(const Delegate<ReturnType, Params...>& other) { Delegate<ReturnType, Params...> copy(*this); copy -= other; return copy; }
            
            
            // -- Getters --
            
            /// @brief Count functions inside delegate
            /// @returns Get current number of functions
            inline size_t size() const noexcept { return m_functions.size(); }
            /// @brief Check if delegate is empty
            /// @returns Empty or not
            inline bool isEmpty() const noexcept { return (m_functions.size() == 0u); }
            
            /// @brief Get contained function pointers
            /// @returns Function pointers
            inline const std::unordered_set<ReturnType (*)(Params...)> data() const noexcept { return m_functions; }
            
            /// @brief Check if delegate contains a specific pointer
            /// @param[in] pFunc  Function pointer
            /// @returns Found or not
            inline bool isSet(ReturnType (*pFunc)(Params...)) const noexcept 
            {
                return (pFunc != nullptr && m_functions.find(pFunc) != m_functions.end());
            }
            
            /// @brief Compare with other delegate
            /// @param[in] pFunc  Function pointer
            /// @returns Equal or not
            inline bool operator==(const Delegate<ReturnType, Params...>& other) const noexcept 
            { 
                return (m_functions == other.m_functions);
            }
            /// @brief Compare with other delegate
            /// @param[in] pFunc  Function pointer
            /// @returns Equal or not
            inline bool operator!=(const Delegate<ReturnType, Params...>& other) const noexcept
            {
                return (m_functions != other.m_functions);
            }
            
            
        private:
            std::unordered_set<ReturnType (*)(Params...)> m_functions; ///< Array of function pointers
        };
        
        
        /// @brief Compare delegate with nullptr - empty
        template <typename ReturnType, typename ... Params>
        inline bool operator==(Delegate<ReturnType, Params...> dlg, nullptr_t empty) { return dlg.isEmpty(); }
        /// @brief Compare delegate with nullptr - empty
        template <typename ReturnType, typename ... Params>
        inline bool operator==(nullptr_t empty, Delegate<ReturnType, Params...> dlg) { return dlg.isEmpty(); }
        /// @brief Compare delegate with nullptr - not empty
        template <typename ReturnType, typename ... Params>
        inline bool operator!=(Delegate<ReturnType, Params...> dlg, nullptr_t empty) { return !dlg.isEmpty(); }
        /// @brief Compare delegate with nullptr - not empty
        template <typename ReturnType, typename ... Params>
        inline bool operator!=(nullptr_t empty, Delegate<ReturnType, Params...> dlg) { return !dlg.isEmpty(); }
    }
}
