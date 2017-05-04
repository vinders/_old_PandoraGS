/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : advanced verification toolset
*******************************************************************************/
#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <string>
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
    /// @class Verifier
    /// @brief Advanced verification toolset
    class Verifier
    {
    public:
        /// @class Verifier.Boolean
        /// @brief Booleans assertions
        class Boolean
        {
        public:
            // Boolean.isTrue (true)
            template <typename T>
            static bool isTrue(const T condition) { return ((condition) ? true : false); }
            // Boolean.isFalse (false)
            template <typename T>
            static bool isFalse(const T condition) { return ((condition) ? false : true); }
        };
        
        /// @class Verifier.Object
        /// @brief Objects assertions
        class Object
        {
        public:
            // Object.isNull (nullptr)
            template <typename T>
            static bool isNull(const T* pObject) { return (pObject == nullptr); }
            // Object.isNotNull (!=nullptr)
            template <typename T>
            static bool isNotNull(const T* pObject) { return (pObject != nullptr); }
            
            // Object.isInstanceOf <className>
            template <typename ClassType, typename T>
            static bool isInstanceOf(const T* pObject) { return (dynamic_cast<ClassType*>(pObject) != nullptr); }
            // Object.isNotInstanceOf <className>
            template <typename ClassType, typename T>
            static bool isNotInstanceOf(const T* pObject) { return (dynamic_cast<ClassType*>(pObject) == nullptr); }
        };
        
        /// @class Verifier.Compare
        /// @brief Comparisons assertions
        class Compare
        {
        public:
            //Compare.isEqual (==)
            template <typename T>
            static bool isEqual(const T first, const T second) { return (first == second); }
            //Compare.isNotEqual (!=)
            template <typename T>
            static bool isNotEqual(const T first, const T second) { return (first != second); }
            
            //Compare.isLowerThan (<)
            template <typename T>
            static bool isLowerThan(const T first, const T second) { return (first < second); }
            //Compare.isLowerEqual (<=)
            template <typename T>
            static bool isLowerEqual(const T first, const T second) { return (first <= second); }
            //Compare.isHigherThan (>)
            template <typename T>
            static bool isHigherThan(const T first, const T second) { return (first > second); }
            //Compare.isHigherEqual (>=)
            template <typename T>
            static bool isHigherEqual(const T first, const T second) { return (first >= second); }
        };
        
        /// @class Verifier.String
        /// @brief Strings assertions
        class String
        {
        public:
            // String.isEmpty ("")
            static bool isEmpty(const char* data) { return (data == nullptr || data[0] == '\0'); }
            static bool isEmpty(const std::string data) { return (data.empty()); }
            static bool isEmpty(const std::wstring data) { return (data.empty()); }
            template <typename T>
            static bool isEmpty(const std::basic_string<T> data) { return (data.empty()); }
            // String.isNotEmpty ("...")
            static bool isNotEmpty(const char* data) { return (data != nullptr && data[0] != '\0'); }
            static bool isNotEmpty(const std::string data) { return (!data.empty()); }
            static bool isNotEmpty(const std::wstring data) { return (!data.empty()); }
            template <typename T>
            static bool isNotEmpty(const std::basic_string<T> data) { return (!data.empty()); }
            
            // String.isEqual (==)
            static bool isEqual(const char* first, const char* second) { return (first == second || strcmp(first, second) == 0); }
            static bool isEqual(const std::string first, const std::string second) { return (first.compare(second) == 0); }
            static bool isEqual(const std::wstring first, const std::wstring second) { return (first.compare(second) == 0); }
            template <typename T>
            static bool isEqual(const std::basic_string<T> first, const std::basic_string<T> second) { return (data.empty()); }
            // String.isNotEqual (!=)
            static bool isNotEqual(const char* first, const char* second) { return (first != second || strcmp(first, second) != 0); }
            static bool isNotEqual(const std::string first, const std::string second) { return (first.compare(second) != 0); }
            static bool isNotEqual(const std::wstring first, const std::wstring second) { return (first.compare(second) != 0); }
            template <typename T>
            static bool isNotEqual(const std::basic_string<T> first, const std::basic_string<T> second) { return (first.compare(second) != 0); }
            
            // String.startsWith (Abc[...])
            static bool startsWith(const std::string data, const std::string pattern) { return (data.find(pattern) == 0u); }
            static bool startsWith(const std::wstring data, const std::wstring pattern) { return (data.find(pattern) == 0u); }
            // String.endsWith ([...]abc)
            static bool endsWith(const std::string data, const std::string pattern) 
            { 
                size_t pos = data.find(pattern); 
                return (pos != std::string::npos && pos + pattern.size() == data.size()); 
            }
            static bool endsWith(const std::wstring data, const std::wstring pattern) 
            { 
                size_t pos = data.find(pattern); 
                return (pos != std::wstring::npos && pos + pattern.size() == data.size()); 
            }
            // String.contains ([...]abc[...])
            static bool contains(const std::string data, const std::string pattern) { return (data.find(pattern) != std::string::npos); }
            static bool contains(const std::wstring data, const std::wstring pattern) { return (data.find(pattern) != std::wstring::npos); }
        };
        
        /// @class Verifier.Collection
        /// @brief Collections assertions
        class Collection
        {
        public:
            // Collection.isEmpty ({})
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
            
            // Collection.isNotEmpty ({...})
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
            
            // Collection.contains ({..., item, ...})
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
            
            // Collection.lacks ({...!=item})
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
    };
}
