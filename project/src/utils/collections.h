/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : collection toolset
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include "compare.h"
#include "assert.h"

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace collections
    /// @brief Collection toolset
    namespace collections
    {
        // -- Size tools --
    
        /// @brief Check whether a collection is empty
        /// @param[in] collection  Standard collection (vector/list/forward_list/array/deque/set/map/unordered_set/unordered_map)
        /// @returns Empty or not
        template <typename StlCont>
        inline bool isEmpty(const StlCont& collection) { return collection.empty(); }

        /// @brief Get length of a collection
        /// @param[in] collection  Standard collection (vector/list/forward_list/array/deque/set/map/unordered_set/unordered_map)
        /// @returns Length of collection
        template <typename StlCont>
        inline uint32_t size(const StlCont& collection) { return collection.size(); }
        
        
        // -- Cleanup tools --
        
        /// @brief Remove all elements from a collection
        /// @param[in] collection  Standard collection (vector/list/forward_list/array/deque/set/map/unordered_set/unordered_map) of elements (no pointers)
        template <typename StlCont>
        inline void clear(const StlCont& collection) { collection.clear(); }
        
        /// @brief Remove all elements from a collection of simple pointers, and destroy referenced objects
        /// @param[in] collection  Standard collection (vector/list/forward_list/array/deque/set/map/unordered_set/unordered_map) of pointers (e.g.: std::vector<int*>)
        template <typename StlCont>
        inline void clearBasePtr(const StlCont& collection) 
        { 
            for (const auto& item : collection)
                if (item != nullptr)
                    delete item;
            collection.clear(); 
        }
        /// @brief Remove all elements from a collection of fixed-sized array-pointers, and destroy referenced arrays
        /// @param[in] collection  Standard collection (vector/list/forward_list/array/deque/set/map/unordered_set/unordered_map) of array-pointers (e.g.: std::vector<char[]>)
        template <typename StlCont>
        inline void clearFixedArrayPtr(const StlCont& collection) 
        { 
            for (const auto& item : collection)
                if (item != nullptr)
                    delete [] item;
            collection.clear(); 
        }
        
        
        // -- Shift tools --
        
        /// @brief Left-shift the values of an array (rows to the right will be zeroed)
        /// @param[in] pArray  Array of elements
        /// @param[in] length  Size of the array
        /// @param[in] rows    Number of rows to use to shift values
        template <typename T>
        inline void lshift(T* pArray, size_t length, uint32_t rows)
        {
            ASSERT(pArray != nullptr);
            if (length > 1u)
            {
                if (rows < length)
                {
                    memmove(pArray, &pArray[rows], (length - rows) * sizeof(T));
                    memset(&pArray[length - rows], 0, rows * sizeof(T));
                }
                else
                    memset(pArray, 0, length);
            }
        }
        /// @brief Left-shift the values of an array
        /// @param[in] pArray      Array of elements
        /// @param[in] length      Size of the array
        /// @param[in] rows        Number of rows to use to shift values
        /// @param[in] emptyValue  Empty value to use for rows to the right
        template <typename T>
        inline void lshift(T* pArray, size_t length, uint32_t rows, const T& emptyValue)
        {
            ASSERT(pArray != nullptr);
            if (length > 1u)
            {
                if (rows < length)
                {
                    memmove(pArray, &pArray[rows], (length - rows) * sizeof(T));
                    pArray = &pArray[length - rows];
                    while (rows > 0)
                    {
                        *pArray = emptyValue;
                        ++pArray;
                        --rows;
                    }
                }
                else
                {
                    while (length > 0)
                    {
                        *pArray = emptyValue;
                        ++pArray;
                        --length;
                    }
                }
            }
        }
        /// @brief Left-shift the values of an array
        /// @param[in] collection  Standard array (vector/list/forward_list/array)
        /// @param[in] rows        Number of rows to use to shift values
        /// @param[in] emptyValue  Empty value to use for rows to the right
        template <typename StlCont>
        inline void lshift(StlCont& collection, const uint32_t rows, const T& emptyValue) 
        {
            if (collection.size() > 1u)
            {
                if (rows < collection.size())
                {
                    for (uint32_t i = rows; i < collection.size(); ++i)
                        collection[i - rows] = std::move(collection[i]);
                    for (uint32_t i = collection.size() - rows; i < collection.size(); ++i)
                        collection[i] = emptyValue;
                }
                else
                {
                    for (auto& it : collection)
                        it = emptyValue;
                }
            }
        }
        
        /// @brief Left-shift the values of an array (circular shift)
        /// @param[in] pArray  Array of elements
        /// @param[in] length  Size of the array
        /// @param[in] rows    Number of rows to use to shift values
        template <typename T>
        inline void lshiftCircular(T* pArray, size_t length, uint32_t rows)
        {
            ASSERT(pArray != nullptr);
            if (length > 1u)
            {
                rows %= length;
                if (rows > 0u)
                {
                    T* pBuffer = new T[rows];
                    memcpy(pBuffer, pArray, rows * sizeof(T));
                    memmove(pArray, &pArray[rows], (length - rows) * sizeof(T));
                    memcpy(&pArray[length - rows], pBuffer, rows * sizeof(T));
                    delete [] pBuffer;
                }
            }
        }
        /// @brief Left-shift the values of an array (circular shift)
        /// @param[in] collection  Standard array (vector/list/forward_list/array)
        /// @param[in] rows        Number of rows to use to shift values
        template <typename StlCont>
        inline void lshiftCircular(StlCont& collection, uint32_t rows) 
        {
            if (collection.size() > 1u)
            {
                rows %= collection.size();
                if (rows > 0u)
                {
                    std::vector<T> buffer;
                    buffer.reserve(rows);
                    for (uint32_t i = 0; i < rows; ++i)
                        buffer.push_back(std::move(collection[i]));
                    for (uint32_t i = rows; i < collection.size(); ++i)
                        collection[i - rows] = std::move(collection[i]);
                    for (uint32_t i = 0; i < rows; ++i)
                        collection[collection.size() + i - rows] = std::move(buffer[i]);
                }
            }
        }
        
        /// @brief Right-shift the values of an array (rows to the left will be zeroed)
        /// @param[in] pArray  Array of elements
        /// @param[in] length  Size of the array
        /// @param[in] rows    Number of rows to use to shift values
        template <typename T>
        inline void rshift(T* pArray, size_t length, uint32_t rows)
        {
            ASSERT(pArray != nullptr);
            if (length > 1u)
            {
                if (rows < length)
                {
                    memmove(&pArray[rows], pArray, (length - rows) * sizeof(T));
                    memset(pArray, 0, rows * sizeof(T));
                }
                else
                    memset(pArray, 0, length);
            }
        }
        /// @brief Right-shift the values of an array
        /// @param[in] pArray      Array of elements
        /// @param[in] length      Size of the array
        /// @param[in] rows        Number of rows to use to shift values
        /// @param[in] emptyValue  Empty value to use for rows to the left
        template <typename T>
        inline void rshift(T* pArray, size_t length, uint32_t rows, const T& emptyValue)
        {
            ASSERT(pArray != nullptr);
            if (length > 1u)
            {
                if (rows < length)
                {
                    memmove(&pArray[rows], pArray, (length - rows) * sizeof(T));
                    while (rows > 0)
                    {
                        *pArray = emptyValue;
                        ++pArray;
                        --rows;
                    }
                }
                else
                {
                    while (length > 0)
                    {
                        *pArray = emptyValue;
                        ++pArray;
                        --length;
                    }
                }
            }
        }
        /// @brief Right-shift the values of an array
        /// @param[in] collection  Standard array (vector/list/forward_list/array)
        /// @param[in] rows        Number of rows to use to shift values
        /// @param[in] emptyValue  Empty value to use for rows to the left
        template <typename StlCont>
        inline void rshift(StlCont& collection, const uint32_t rows, const T& emptyValue) 
        {
            if (collection.size() > 1u)
            {
                if (rows < collection.size())
                {
                    for (uint32_t i = collection.size() - rows; i >= rows; --i)
                        collection[i] = std::move(collection[i - rows]);
                    for (uint32_t i = 0; i < rows; ++i)
                        collection[i] = emptyValue;
                }
                else
                {
                    for (auto& it : collection)
                        it = emptyValue;
                }
            }
        }
        
        /// @brief Right-shift the values of an array (circular shift)
        /// @param[in] pArray  Array of elements
        /// @param[in] length  Size of the array
        /// @param[in] rows    Number of rows to use to shift values
        template <typename T>
        inline void rshiftCircular(T* pArray, size_t length, uint32_t rows)
        {
            ASSERT(pArray != nullptr);
            if (length > 1u)
            {
                rows %= length;
                if (rows > 0u)
                {
                    T* pBuffer = new T[rows];
                    memcpy(pBuffer, &pArray[length - rows], rows * sizeof(T));
                    memmove(&pArray[rows], pArray, (length - rows) * sizeof(T));
                    memcpy(pArray, pBuffer, rows * sizeof(T));
                    delete [] pBuffer;
                }
            }
        }
        /// @brief Right-shift the values of an array (circular shift)
        /// @param[in] collection  Standard array (vector/list/forward_list/array)
        /// @param[in] rows        Number of rows to use to shift values
        template <typename StlCont>
        inline void rshiftCircular(StlCont& collection, uint32_t rows) 
        {
            if (collection.size() > 1u)
            {
                rows %= collection.size();
                if (rows > 0u)
                {
                    std::vector<T> buffer;
                    buffer.reserve(rows);
                    for (uint32_t i = collection.size() - rows; i < collection.size(); ++i)
                        buffer.push_back(std::move(collection[i]));
                    for (uint32_t i = collection.size() - rows; i >= rows; --i)
                        collection[i] = std::move(collection[i - rows]);
                    for (uint32_t i = 0; i < rows; ++i)
                        collection[i] = std::move(buffer[i]);
                }
            }
        }
        
        
        // -- Search for values --

        /// @brief Check whether a collection contains a value
        /// @param[in] pArray  Array of elements
        /// @param[in] length  Size of the array
        /// @param[in] item    Searched value
        /// @returns Found or not
        template <typename T>
        inline bool contains(const T* pArray, const size_t length, const T& item) 
        { 
            ASSERT(pArray != nullptr);
            if (pArray == nullptr)
                return false;
            
            bool isFound = false;
            for (uint32_t i = 0; i < length; ++i)
            {
                if (pArray[i] == item)
                {
                    isFound = true;
                    break;
                }
            }
            return isFound; 
        }
        /// @brief Check whether a set contains a key
        /// @param[in] vec   Standard hash-set
        /// @param[in] key   Searched key
        /// @returns Found or not
        template <typename KeyType>
        inline bool contains(const std::set<KeyType>& hashSet, const KeyType& key) { return (hashSet.find(key) != hashSet.end()); }
        /// @brief Check whether an unordered_set contains a key
        /// @param[in] vec   Standard hash-set
        /// @param[in] key   Searched key
        /// @returns Found or not
        template <typename KeyType>
        inline bool contains(const std::unordered_set<KeyType>& hashSet, const KeyType& key) { return (hashSet.find(key) != hashSet.end()); }
        /// @brief Check whether a map contains a key
        /// @param[in] vec   Standard hash-map
        /// @param[in] key   Searched key
        /// @returns Found or not
        template <typename KeyType, typename T>
        inline bool contains(const std::map<KeyType,T>& hashMap, const KeyType& key) { return (hashMap.find(key) != hashMap.end()); }
        /// @brief Check whether a map contains a key and a specific value for that key
        /// @param[in] vec   Standard hash-map
        /// @param[in] key   Searched key
        /// @param[in] item  Searched value
        /// @returns Found or not
        template <typename KeyType, typename T>
        inline bool contains(const std::map<KeyType,T>& hashMap, const KeyType& key, const T& item) { return (hashMap.find(key) != hashMap.end() && hashMap.at(key) == item); }
        /// @brief Check whether a unordered_map contains a key
        /// @param[in] vec   Standard hash-map
        /// @param[in] key   Searched key
        /// @returns Found or not
        template <typename KeyType, typename T>
        inline bool contains(const std::unordered_map<KeyType,T>& hashMap, const KeyType& key) { return (hashMap.find(key) != hashMap.end()); }
        /// @brief Check whether an unordered_map contains a key and a specific value for that key
        /// @param[in] vec   Standard hash-map
        /// @param[in] key   Searched key
        /// @param[in] item  Searched value
        /// @returns Found or not
        template <typename KeyType, typename T>
        inline bool contains(const std::unordered_map<KeyType,T>& hashMap, const KeyType& key, const T& item) { return (hashMap.find(key) != hashMap.end() && hashMap.at(key) == item); }
        /// @brief Check whether a collection contains a value
        /// @param[in] vec   Standard non-hashed collection (vector/list/forward_list/array/deque)
        /// @param[in] item  Searched value
        /// @returns Found or not
        template <typename StlCont, typename T>
        inline bool contains(const StlCont& vec, const T& item) { return (std::find(vec.begin(), vec.end(), item) != vec.end()); }
        
        
        // -- Count occurrences --
        
        /// @brief Count occurrences of true comparisons in array (count values equal to, count values different from, count values lower than, ...)
        /// @param[in] pArray  Array of elements
        /// @param[in] length  Size of the array
        /// @param[in] value   Searched value
        /// @returns Total number of results
        /// @see compare_type_t
        template <utils::compare_type_t Comparison, typename T>
        inline uint32_t count(const T* pArray, const size_t size, const T& value)
        {
            ASSERT(pArray != nullptr);
            if (pArray == nullptr)
                return 0u;
            
            uint32_t total = 0u;
            for (uint32_t i = 0u; i < size; ++i)
            {
                ++pArray;
                if (utils::compare<T, Comparison>(*pArray, value))
                    ++total;
            }
            return total;
        }
        /// @brief Count occurrences in array
        /// @param[in] pArray  Array of elements
        /// @param[in] length  Size of the array
        /// @param[in] value   Searched value
        /// @returns Total number of results
        template <typename T>
        inline uint32_t count(const T* pArray, const size_t size, const T& value) { return count<utils::compare_type_t::equal, T>(pArray, size, value); }
        /// @brief Count occurrences of true comparisons in collection (count values equal to, count values different from, count values lower than, ...)
        /// @param[in] collection  Standard collection (vector/list/forward_list/array/deque/set/map/unordered_set/unordered_map)
        /// @param[in] value       Searched value
        /// @returns Total number of results
        /// @see compare_type_t
        template <utils::compare_type_t Comparison, typename StlCont, typename T>
        inline uint32_t count(const StlCont& collection, const T& value) 
        { 
            uint32_t total = 0u;
            for (const auto& item : collection)
            {
                if (utils::compare<T, Comparison>(item, value))
                    ++total;
            }
            return total;
        }
        /// @brief Count occurrences in collection
        /// @param[in] collection  Standard collection (vector/list/forward_list/array/deque/set/map/unordered_set/unordered_map)
        /// @param[in] value       Searched value
        /// @returns Total number of results
        template <typename StlCont, typename T>
        inline uint32_t count(const StlCont& collection, const T& value) { return count<utils::compare_type_t::equal, StlCont, T>(collection, value); }
        
        
        // -- Count dereferenced occurrences --
        
        /// @brief Count occurrences of true comparisons in array of pointers (count values equal to, count values different from, count values lower than, ...)
        /// @param[in] pArray  Array of pointers
        /// @param[in] length  Size of the array
        /// @param[in] value   Searched value
        /// @returns Total number of results
        /// @see compare_type_t
        template <utils::compare_type_t Comparison, typename T>
        inline uint32_t countPtrValues(const T** pPtrArray, const size_t size, const T& value)
        {
            ASSERT(pPtrArray != nullptr);
            if (pPtrArray == nullptr)
                return 0u;
            
            uint32_t total = 0u;
            for (uint32_t i = 0u; i < size; ++i)
            {
                ++pPtrArray;
                if (*pPtrArray != nullptr && utils::compare<T, Comparison>(*(*pPtrArray), value))
                    ++total;
            }
            return total;
        }
        /// @brief Count occurrences in array of pointers
        /// @param[in] pArray  Array of pointers
        /// @param[in] length  Size of the array
        /// @param[in] value   Searched value
        /// @returns Total number of results
        template <typename T>
        inline uint32_t countPtrValues(const T** pPtrArray, const size_t size, const T& value) { return countPtrValues<utils::compare_type_t::equal, T>(pPtrArray, size, value); }
        /// @brief Count occurrences of true comparisons in collection of pointers (count values equal to, count values different from, count values lower than, ...)
        /// @param[in] collection  Standard collection of pointers (vector/list/set/map/unordered_set/unordered_map)
        /// @param[in] value       Searched value
        /// @returns Total number of results
        /// @see compare_type_t
        template <utils::compare_type_t Comparison, typename StlCont, typename T>
        inline uint32_t countPtrValues(const StlCont& collection, const T& value) 
        { 
            uint32_t total = 0u;
            for (const auto& item : collection)
            {
                if (item != nullptr && utils::compare<T, Comparison>(*item, value))
                    ++total;
            }
            return total;
        }
        /// @brief Count occurrences in collection of pointers
        /// @param[in] collection  Standard collection of pointers (vector/list/forward_list/array/deque/set/map/unordered_set/unordered_map)
        /// @param[in] value       Searched value
        /// @returns Total number of results
        template <typename StlCont, typename T>
        inline uint32_t countPtrValues(const StlCont& collection, const T& value) { return countPtr<utils::compare_type_t::equal, StlCont, T>(collection, value); }
        
        
        // -- Count distinct values in a collection --
        
        /// @brief Count distinct values in an array
        /// @param[in] pArray  Array of elements
        /// @param[in] length  Size of the array
        /// @returns Number of distinct values
        template <typename T>
        inline uint32_t countDistinct(const T* pArray, const size_t size)
        {
            ASSERT(pArray != nullptr);
            if (pArray == nullptr)
                return 0u;
            
            uint32_t total = 0u;
            std::set<T> found;
            found.reserve(size);
            for (uint32_t i = 0u; i < size; ++i)
            {
                ++pArray;
                if (!contains(found, *pArray))
                {
                    found.insert(*pArray);
                    ++total;
                }
            }
            return total;
        }
        /// @brief Count distinct values in a collection
        /// @param[in] collection  Standard collection (vector/list/forward_list/array/deque/set/map/unordered_set/unordered_map)
        /// @returns Number of distinct values
        template <typename T, typename StlCont>
        inline uint32_t countDistinct(const StlCont& collection) 
        { 
            uint32_t total = 0u;
            std::set<T> found;
            found.reserve(collection.size());
            for (const auto& item : collection)
            {
                if (!contains(found, item))
                {
                    found.insert(item);
                    ++total;
                }
            }
            return total;
        }
        
        
        // -- Count distinct dereferenced values in a collection --
        
        /// @brief Count distinct non-null values in an array of pointers
        /// @param[in] pArray  Array of pointers
        /// @param[in] length  Size of the array
        /// @returns Number of distinct values
        template <typename T>
        inline uint32_t countDistinctPtrValues(const T** pPtrArray, const size_t size)
        {
            ASSERT(pPtrArray != nullptr);
            if (pPtrArray == nullptr)
                return 0u;
            
            uint32_t total = 0u;
            std::set<T> found;
            found.reserve(size);
            for (uint32_t i = 0u; i < size; ++i)
            {
                ++pPtrArray;
                if (*pPtrArray != nullptr && !contains(found, **pPtrArray))
                {
                    found.insert(**pPtrArray);
                    ++total;
                }
            }
            return total;
        }
        /// @brief Count distinct non-null values in a collection of pointers
        /// @param[in] collection  Standard collection of pointers (vector/list/forward_list/array/deque/set/map/unordered_set/unordered_map)
        /// @returns Number of distinct values
        template <typename StlCont, typename T>
        inline uint32_t countDistinctPtrValues(const StlCont& collection) 
        { 
            uint32_t total = 0u;
            std::set<T> found;
            found.reserve(collection.size());
            for (const auto& item : collection)
            {
                if (item != nullptr && !contains(found, *item))
                {
                    found.insert(*item);
                    ++total;
                }
            }
            return total;
        }
    }
}
