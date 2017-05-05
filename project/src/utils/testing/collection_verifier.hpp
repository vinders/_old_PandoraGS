/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : collection verification toolset
*******************************************************************************/
#pragma once

#include <cstddef>
#include <vector>
#include <list>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include <algorithm>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.testing
    /// Testing utilities
    namespace testing
    {
        /// @class CollectionVerifier
        /// @brief Collections verifications
        class CollectionVerifier
        {
        public:
            // CollectionVerifier.isEmpty ({})
            template <typename T>
            static bool isEmpty(const std::vector<T>& vec) { return (vec.empty()); }
            template <typename T>
            static bool isEmpty(const std::list<T>& linkedList) { return (linkedList.empty()); }
            template <typename KeyType>
            static bool isEmpty(const std::set<KeyType>& hashSet) { return (hashSet.empty()); }
            template <typename KeyType>
            static bool isEmpty(const std::unordered_set<KeyType>& hashSet) { return (hashSet.empty()); }
            template <typename KeyType, typename T>
            static bool isEmpty(const std::map<KeyType,T>& hashMap) { return (hashMap.empty()); }
            template <typename KeyType, typename T>
            static bool isEmpty(const std::unordered_map<KeyType,T>& hashMap) { return (hashMap.empty()); }
            
            // CollectionVerifier.isNotEmpty ({...})
            template <typename T>
            static bool isNotEmpty(const std::vector<T>& vec) { return (!vec.empty()); }
            template <typename T>
            static bool isNotEmpty(const std::list<T>& linkedList) { return (!linkedList.empty()); }
            template <typename KeyType>
            static bool isNotEmpty(const std::set<KeyType>& hashSet) { return (!hashSet.empty()); }
            template <typename KeyType>
            static bool isNotEmpty(const std::unordered_set<KeyType>& hashSet) { return (!hashSet.empty()); }
            template <typename KeyType, typename T>
            static bool isNotEmpty(const std::map<KeyType,T>& hashMap) { return (!hashMap.empty()); }
            template <typename KeyType, typename T>
            static bool isNotEmpty(const std::unordered_map<KeyType,T>& hashMap) { return (!hashMap.empty()); }
            
            // CollectionVerifier.contains ({..., item, ...})
            template <typename T>
            static bool contains(const T* pArray, const size_t length, const T item) 
            { 
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
            template <typename T>
            static bool contains(const std::vector<T>& vec, const T item) { return (std::find(vec.begin(), vec.end(), item) != vec.end()); }
            template <typename T>
            static bool contains(const std::list<T>& lst, const T item) { return (std::find(lst.begin(), lst.end(), item) != lst.end()); }
            template <typename KeyType>
            static bool contains(const std::set<KeyType>& hashSet, const KeyType key) { return (hashSet.find(key) != hashSet.end()); }
            template <typename KeyType>
            static bool contains(const std::unordered_set<KeyType>& hashSet, const KeyType key) { return (hashSet.find(key) != hashSet.end()); }
            template <typename KeyType, typename T>
            static bool contains(const std::map<KeyType,T>& hashMap, const KeyType key) { return (hashMap.find(key) != hashMap.end()); }
            template <typename KeyType, typename T>
            static bool contains(const std::map<KeyType,T>& hashMap, const KeyType key, const T item) { return (hashMap.find(key) != hashMap.end() && hashMap.at(key) == item); }
            template <typename KeyType, typename T>
            static bool contains(const std::unordered_map<KeyType,T>& hashMap, const KeyType key) { return (hashMap.find(key) != hashMap.end()); }
            template <typename KeyType, typename T>
            static bool contains(const std::unordered_map<KeyType,T>& hashMap, const KeyType key, const T item) { return (hashMap.find(key) != hashMap.end() && hashMap.at(key) == item); }
            
            // CollectionVerifier.lacks ({...!=item})
            template <typename T>
            static bool lacks(const T* pArray, const size_t length, const T item) 
            { 
                bool isFound = false;
                for (uint32_t i = 0; i < length; ++i)
                {
                    if (pArray[i] == item)
                    {
                        isFound = true;
                        break;
                    }
                }
                return !isFound; 
            }
            template <typename T>
            static bool lacks(const std::vector<T>& vec, const T item) { return (std::find(vec.begin(), vec.end(), item) == vec.end()); }
            template <typename T>
            static bool lacks(const std::list<T>& lst, const T item) { return (std::find(lst.begin(), lst.end(), item) == lst.end()); }
            template <typename KeyType>
            static bool lacks(const std::set<KeyType>& hashSet, const KeyType key) { return (hashSet.find(key) == hashSet.end()); }
            template <typename KeyType>
            static bool lacks(const std::unordered_set<KeyType>& hashSet, const KeyType key) { return (hashSet.find(key) == hashSet.end()); }
            template <typename KeyType, typename T>
            static bool lacks(const std::map<KeyType,T>& hashMap, const KeyType key) { return (hashMap.find(key) == hashMap.end()); }
            template <typename KeyType, typename T>
            static bool lacks(const std::map<KeyType,T>& hashMap, const KeyType key, const T item) { return (hashMap.find(key) == hashMap.end() || hashMap.at(key) != item); }
            template <typename KeyType, typename T>
            static bool lacks(const std::unordered_map<KeyType,T>& hashMap, const KeyType key) { return (hashMap.find(key) == hashMap.end()); }
            template <typename KeyType, typename T>
            static bool lacks(const std::unordered_map<KeyType,T>& hashMap, const KeyType key, const T item) { return (hashMap.find(key) == hashMap.end() || hashMap.at(key) != item); }
        };
    }
}
