/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstring>
#include <cassert>
#include <stdexcept>

namespace pandora {
  namespace memory {
    /// @class FixedSizeString
    /// @brief A container that behaves like a string with a max length, without involving any dynamic allocation. 
    ///        Useful for real-time apps or for UI strings with fixed max size. The string is STL-compliant.
    template <size_t _MaxSize,            // Max length of strings that may be contained in the fixed-size string.
              typename _CharType = char,  // Character type (char, char16_t, char32_t, wchar_t, ...)
              bool _ThrowIfMoreThanMax = false> // When an assigned/appended/padded value exceeds the max, throw (true) or not (false)
    class FixedSizeString final {
    public:
      using value_type = _CharType;
      using size_type = size_t;
      using reference = _CharType&;
      using const_reference = const _CharType&;
      using pointer = _CharType*;
      using const_pointer = const _CharType*;
      using Type = FixedSizeString<_MaxSize,_CharType,_ThrowIfMoreThanMax>;
      static_assert((_MaxSize > 0u), "FixedSizeString: _MaxSize can't be 0.");

      static constexpr size_t npos = static_cast<size_t>(-1); ///< Undefined length or position value


      FixedSizeString() noexcept { clear(); }
      FixedSizeString(const Type& rhs) noexcept { assign(rhs); }
      FixedSizeString(Type&& rhs) noexcept { assign(rhs); }
      ~FixedSizeString() = default;

      /// @brief Create initialized string 
      /// @warning If (length > _MaxSize): throws if _ThrowIfMoreThanMax, or truncates the string
      explicit FixedSizeString(const _CharType* value, size_t length = npos) { assign(value, length); }
      /// @brief Create initialized string
      /// @warning If (length > _MaxSize): throws if _ThrowIfMoreThanMax, or truncates the string
      explicit FixedSizeString(size_t length, _CharType character) { assign(length, character); }

      void swap(Type& rhs) noexcept { Type buffer(*this); _assign(rhs._value,rhs._size); rhs._assign(buffer._value,buffer._size); }
      
      // -- getters --
      
      inline const _CharType* c_str() const noexcept { return this->_value; } ///< Get legacy string content
      inline const _CharType* data() const noexcept { return this->_value; }  ///< Get legacy string content
      
      inline bool empty() const noexcept { return (this->_value[0] == 0); } ///< Verify if the string is empty
      inline size_t size() const noexcept { return this->_size; }   ///< Get current length of the string
      inline size_t length() const noexcept { return this->_size; } ///< Get current length of the string
      constexpr inline bool full() const noexcept  { return (this->_size == _MaxSize); } ///< Verify if the max fixed size of the string is reached
      
      inline size_t max_size() const noexcept { return _MaxSize; }
      inline size_t capacity() const noexcept { return _MaxSize; }
      
      inline _CharType& operator[](size_t index) noexcept { assert(index < this->_size); return this->_value[index]; }
      inline const _CharType& operator[](size_t index) const noexcept { assert(index < this->_size); return this->_value[index]; }
      inline _CharType& at(size_t index) { // throws
        if (index < this->_size)
          return this->_value[index];
        throw std::out_of_range("FixedSizeString: trying to read invalid index.");
      }
      inline const _CharType& at(size_t index) const { // throws
        if (index < this->_size)
          return this->_value[index];
        throw std::out_of_range("FixedSizeString: trying to read invalid index.");
      }
      
      inline _CharType& front() noexcept { assert(!empty()); return this->_value[0]; } ///< Get first character (check not empty() before!)
      inline const _CharType& front() const noexcept { assert(!empty()); return this->_value[0]; } ///< Get first character (check not empty() before!)
      inline _CharType& back() noexcept { assert(!empty()); return this->_value[(this->_size > 0u) ? this->_size - 1u : 0u]; } ///< Get last character (check not empty() before!)
      inline const _CharType& back() const noexcept { assert(!empty()); return this->_value[(this->_size > 0u) ? this->_size - 1u : 0u]; } ///< Get last character (check not empty() before!)
      
      // -- comparisons --

      /// @brief Compare value of current string with another string ('rhs')
      inline int compare(const Type& rhs) const noexcept {
        return memcmp((void*)this->_value, (void*)rhs._value, (this->_size + 1u)*sizeof(_CharType));
      }
      /// @brief Compare value of current string with another string ('rhs')
      inline int compare(const _CharType* rhs) const noexcept {
        if (rhs != nullptr) {
          size_t rhsLength = _getLength(rhs);
          return memcmp((void*)(this->_value), (void*)rhs, (rhsLength <= this->_size) ? (this->_size + 1u) * sizeof(_CharType) : (rhsLength + 1u) * sizeof(_CharType));
        }
        assert(false);
        return 1;
      }

      /// @brief Compare value of a substring of current string with another string ('rhs')
      inline int compare(size_t offset, size_t length, const Type& rhs) const noexcept { 
        return memcmp((void*)&(this->_value[offset]), (void*)&(rhs._value[0]), _minLengthAvailable(offset, length, rhs._size)*sizeof(_CharType));
      }
      /// @brief Compare value of a substring of current string with another string/substring ('rhs')
      inline int compare(size_t offset, size_t length, const _CharType* rhs, size_t rhsLength = npos) const noexcept {
        assert(rhs != nullptr);
        return (rhs != nullptr) ? memcmp((void*)&(this->_value[offset]), rhs, _minLengthAvailable(offset, length, rhsLength)*sizeof(_CharType)) : 1;
      }
      /// @brief Compare value of a substring of current string with another substring ('rhsLength' characters from 'rhs' at 'rhsOffset')
      inline int compare(size_t offset, size_t length, const Type& rhs, size_t rhsOffset, size_t rhsLength) const noexcept { 
        return memcmp((void*)&(this->_value[offset]), (void*)&(rhs._value[rhsOffset]), _minLengthAvailable(offset, length, rhsLength)*sizeof(_CharType));
      }

      inline bool operator==(const Type& rhs) const noexcept { return (this->_size == rhs._size && compare(rhs) == 0); }
      inline bool operator!=(const Type& rhs) const noexcept { return !(this->operator==(rhs)); }

      // -- change size --

      /// @brief Remove all content of the string
      inline void clear() noexcept {
        this->_size = static_cast<size_t>(0u);
        memset((void*)this->_value, 0, (_MaxSize + 1u)*sizeof(_CharType));
      }
      /// @brief Fill the string (until reaching the max fixed size) with a character
      inline void fill(_CharType character) noexcept {
        this->_size = _MaxSize;
        memset((void*)(this->_value), character, _MaxSize*sizeof(_CharType));
      }

      /// @brief Change the size of the string - add padding at the end or truncate the end
      /// @warning If (length > _MaxSize): throws if _ThrowIfMoreThanMax, or uses max size
      inline void resize(size_t length, _CharType paddingChar = static_cast<_CharType>(0x20)) {
        if (length < this->_size) {
          this->_size = length;
          memset((void*)(&(this->_value[length])), 0, (_MaxSize - length)*sizeof(_CharType));
        }
        else
          _append<true>(&paddingChar, length - this->_size);
      }
      /// @brief If current size is less than 'minSize', add padding characters before/after (centered)
      /// @warning If (length > _MaxSize): throws if _ThrowIfMoreThanMax, or uses max size
      inline Type& pad(size_t minSize, _CharType paddingChar = static_cast<_CharType>(' ')) {
        if (minSize > this->_size) {
          size_t padding = minSize - this->_size;
          _append<true>(&paddingChar, padding >> 1);
          _insert<true>(&paddingChar, padding - padding >> 1, 0u);
        }
        return *this;
      }

      // -- assignment --

      /// @brief Set the value of the string with the value of 'rhs'
      inline Type& assign(const _CharType* rhs, size_t length = npos) {
        if (rhs != nullptr)
          _assign(rhs, _getLength(rhs, length));
        return *this;
      }
      /// @brief Set the value of the string with the value of 'rhs'
      inline Type& assign(const Type& rhs) noexcept { _assign(rhs._value, rhs._size); return *this; }
      /// @brief Set the value of the string with a substring of 'rhs'
      inline Type& assign(const Type& rhs, size_t offset, size_t length = npos) noexcept {
        if (offset < rhs._size && length > 0u)
          _assign(&(rhs._value[offset]), (length == npos || (length > rhs._size - offset)) ? (rhs._size - offset) : length);
        return *this;
      }
      /// @brief Set the value of the string with 'character' (repeated 'charNb' times)
      inline Type& assign(size_t charNb, _CharType character) { _assign<true>(&character, charNb); return *this; }

      inline Type& operator=(const Type& rhs) noexcept { return assign(rhs); }
      inline Type& operator=(Type&& rhs) noexcept { return assign(rhs); }
      inline Type& operator=(const _CharType* rhs) { return assign(rhs, npos); }
      
      // -- append --

      /// @brief Append another string ('rhs')
      inline Type& append(const _CharType* rhs, size_t length = npos) {
        assert(rhs != nullptr);
        _append(_getLength(rhs, length));
        return *this;
      }
      /// @brief Append another string ('rhs')
      inline Type& append(const Type& rhs) { _append(rhs._value, rhs._size); return *this; }
      /// @brief Append another substring ('length' character(s) from 'rhs' at 'offset' position)
      inline Type& append(const Type& rhs, size_t offset, size_t length = npos) {
        if (offset < rhs._size && length > 0u)
          _append(&(rhs._value[offset]), (length == npos || (length > rhs._size - offset)) ? (rhs._size - offset) : length);
        return *this;
      }
      /// @brief Append 'character' value ('charNb' times)
      inline Type& append(size_t charNb, _CharType character) { _append<true>(&character, charNb); return *this; }

      /// @brief Append a character at the end of the string (if possible)
      inline bool push_back(_CharType character) {
        if (this->_size < _MaxSize) {
          this->_value[this->_size] = character;
          ++(this->_size);
          return true;
        }
        else if (_ThrowIfMoreThanMax) throw std::out_of_range("FixedSizeString: appending a character exceeds _MaxSize.");
        return false;
      }
      /// @brief Pad the end of the string with a padding character (enough times to reach desired 'minSize' length)
      inline Type& rpad(size_t minSize, _CharType paddingChar = static_cast<_CharType>(' ')) {
        if (minSize > this->_size)
          _append<true>(&paddingChar, minSize - this->_size);
        return *this;
      }

      inline Type& operator+=(const _CharType* rhs) { return append(rhs); }
      inline Type& operator+(const _CharType* rhs) const { Type copy(*this); copy += rhs; return copy; }
      inline Type& operator+=(const Type& rhs) { return append(rhs); }
      inline Type& operator+(const Type& rhs) const { Type copy(*this); copy += rhs; return copy; }

      // -- insert --

      /// @brief Insert another string ('rhs') at 'position'
      inline Type& insert(size_t position, const Type& rhs) { _insertOrAppend(rhs._value, rhs._size, position); return *this; }
      /// @brief Insert another substring ('rhsLength' characters from 'rhs' at 'rhsOffset') at 'position'
      inline Type& insert(size_t position, const Type& rhs, size_t rhsOffset, size_t rhsLength = npos) {
        if (rhsOffset < rhs._size && rhsLength > 0u) {
          if (rhsLength == npos || rhsLength > rhs._size - rhsOffset)
            rhsLength = rhs._size - rhsOffset;
          _insertOrAppend(&(rhs._value[rhsOffset]), rhsLength, position);
        }
        return *this;
      }
      /// @brief Insert another string ('rhs') at 'position'
      inline Type& insert(size_t position, const _CharType* rhs, size_t length = npos) {
        assert(rhs != nullptr);
        _insertOrAppend(rhs, _getLength(rhs, length), position);
        return *this;
      }
      /// @brief Insert a repeated 'character' ('charNb' times) at 'position'
      inline Type& insert(size_t position, size_t charNb, _CharType character) { _insertOrAppend<true>(&character, charNb, position); return *this; }

      /// @brief Pad the beginning of the string with a padding character (enough times to reach desired 'minSize' length)
      inline Type& lpad(size_t minSize, _CharType paddingChar = static_cast<_CharType>(' ')) {
        if (minSize > this->_size)
          _insert<true>(&paddingChar, minSize - this->_size, 0u);
        return *this;
      }
      
      // -- replace --

      /// @brief Replace N character(s) (N = 'length') by another string ('rhs')
      inline Type& replace(size_t offset, size_t length, const Type& rhs) { _replaceOrAppend(rhs._value, rhs._size, offset, length); return *this; }
      /// @brief Replace N character(s) (N = 'length') by another substring ('rhsLength' characters from 'rhs' at 'rhsOffset')
      inline Type& replace(size_t offset, size_t length, const Type& rhs, size_t rhsOffset, size_t rhsLength) {
        if (rhsOffset < rhs._size && rhsLength > 0u) {
          if (rhsLength == npos || rhsLength > rhs._size - rhsOffset)
            rhsLength = rhs._size - rhsOffset;
          _replaceOrAppend(&(rhs._value[rhsOffset]), rhsLength, offset, length);
        }
        return *this;
      }
      /// @brief Replace N character(s) (N = 'length') by another string ('rhs')
      inline Type& replace(size_t offset, size_t length, const char* rhs, size_t rhsLength = npos) {
        if (rhs != nullptr)
          _replaceOrAppend(rhs, _getLength(rhs, rhsLength), offset, length);
        return *this;
      }
      /// @brief Replace N character(s) (N = 'length') by a repeated 'character' ('charNb' times)
      inline Type& replace(size_t offset, size_t length, size_t charNb, char character) { _replaceOrAppend<true>(&character, charNb, offset, length); return *this; }

      /// @brief Replace N character(s) between iterators of another string ('rhs', 'iterEnd' is excluded)
      inline Type& replace(const _CharType* iterFirst, const _CharType* iterEnd, const Type& rhs) {
        assert(iterFirst != nullptr);
        if (iterFirst >= &(this->_value[0]) && iterFirst < iterEnd && iterEnd <= &(this->_value[_MaxSize])) {
          _replace(rhs._value, rhs._size, _iteratorToPosition(iterFirst), static_cast<size_t>(iterEnd - iterFirst));
        }
        return *this;
      }
      /// @brief Replace N character(s) between iterators of another string ('rhs', 'iterEnd' is excluded)
      inline Type& replace(const _CharType* iterFirst, const _CharType* iterEnd, const char* rhs, size_t rhsLength = npos) {
        assert(iterFirst != nullptr && rhs != nullptr);
        if (rhs != nullptr && iterFirst >= &(this->_value[0]) && iterFirst < iterEnd && iterEnd <= &(this->_value[_MaxSize])) {
          _replace(rhs, _getLength(rhs, rhsLength), _iteratorToPosition(iterFirst), static_cast<size_t>(iterEnd - iterFirst));
        }
        return *this;
      }

      // -- erase --

      /// @brief Remove N character(s) at 'offset' position (N = 'length')
      inline Type& erase(size_t offset = 0u, size_t length = npos) noexcept {
        if (offset < this->_size)
          _erase(offset, (offset + length <= this->_size) ? length : this->_size - offset);
        return *this;
      }
      /// @brief Remove character at iterator position
      inline const _CharType* erase(const _CharType* iter) noexcept {
        assert(iter != nullptr);
        if (iter >= &(this->_value[0]) && iter < &(this->_value[_MaxSize]))
          _erase(_iteratorToPosition(iter), static_cast<size_t>(1u));
        return iter;
      }
      /// @brief Remove character between iterator positions ('iterEnd' is excluded)
      inline const _CharType* erase(const _CharType* iterFirst, const _CharType* iterEnd) noexcept {
        assert(iterFirst != nullptr);
        if (iterFirst >= &(this->_value[0]) && iterFirst < iterEnd && iterEnd <= &(this->_value[_MaxSize])) {
          _erase(_iteratorToPosition(iterFirst), static_cast<size_t>(iterEnd - iterFirst));
        }
        return iterFirst;
      }

      /// @brief Remove last character
      inline bool pop_back() noexcept {
        if (this->_size > 0u) {
          --(this->_size);
          this->_value[this->_size] = 0;
          return true;
        }
        return false;
      }

      /// @brief Remove all spaces/tabs/controls chars at the beginning/end of current string
      inline Type& trim() noexcept {
        ltrim();
        return rtrim();
      }
      /// @brief Remove all spaces/tabs/controls chars at the beginning of current string
      inline Type& ltrim() noexcept {
        size_t firstIndex = _getFirstNonTrimmableIndex();
        if (firstIndex > static_cast<size_t>(0u)) {
          this->_size -= firstIndex;
          memmove((void*)&(this->_value[0]), (void*)&(this->_value[firstIndex]), this->_size*sizeof(_CharType));
          memset((void*)&(this->_value[this->_size]), 0, firstIndex);
        }
        return *this;
      }
      /// @brief Remove all spaces/tabs/controls chars at the end of current string
      inline Type& rtrim() noexcept {
        size_t afterLastIndex = _getIndexAfterLastNonTrimmable();
        if (afterLastIndex < this->_size) {
          memset(&(this->_value[afterLastIndex]), 0, this->_size - afterLastIndex);
          this->_size = afterLastIndex;
        }
        return *this;
      }
      
      // -- search --

      /// @brief Extract a substring of current object
      inline Type substr(size_t offset = 0u, size_t length = npos) const noexcept { Type sub; sub._size = copy(sub->_value, offset, length); return std::move(sub); }
      /// @brief Copy a substring of current object into passed array
      inline size_t copy(_CharType* out, size_t offset, size_t length = npos) const noexcept {
        assert(out != nullptr);
        if (offset < this->_size && length > 0u) {
          size_t copySize = (length == npos || (length > this->_size - offset)) ? (this->_size - offset) : length;
          memcpy((void*)out, (void*)(&(this->_value[offset])), copySize*sizeof(_CharType));
          return copySize;
        }
        return 0u;
      }

      /// @brief Find the first position of the substring 'query' ('npos' returned if not found)
      size_t find(const Type& query, size_t offset = 0u) const noexcept {
        return (offset < this->_size) ? _findString(query._value, query._size, &(this->_value[offset]), &(this->_value[this->_size - 1u]) ) : npos;
      }
      size_t find(const _CharType* query, size_t offset = 0u, size_t length = npos) const noexcept {
        assert(query != nullptr);
        if (offset < this->_size)
          return _findString(query, _getLength(query, length), &(this->_value[offset]), &(this->_value[this->_size - 1u]) );
        return npos;
      }
      inline size_t find(_CharType character, size_t offset = 0u) const noexcept {
        return (offset < this->_size) ? _find(character, &(this->_value[offset]), &(this->_value[this->_size - 1u]) ) : npos;
      }

      /// @brief Find the last position of the substring 'query' ('npos' returned if not found)
      size_t rfind(const Type& query, size_t offset = npos) const noexcept {
        return _rfindString(query._value, query._size, &(this->_value[_rOffsetIndex(offset)]), &(this->_value[0]) );
      }
      size_t rfind(const _CharType* query, size_t offset = npos, size_t length = npos) const noexcept {
        assert(query != nullptr);
        return _rfindString(query, _getLength(query, length), &(this->_value[_rOffsetIndex(offset)]), &(this->_value[0]) );
      }
      inline size_t rfind(_CharType character, size_t offset = npos) const noexcept {
        return _rfind(character, &(this->_value[_rOffsetIndex(offset)]), &(this->_value[0]) );
      }

      /// @brief Find the first position of any of the character(s) in 'charList' ('npos' returned if not found)
      inline size_t find_first_of(const Type& charList, size_t offset = 0u) const noexcept {
        return (offset < this->_size) ? _find(charList._value, charList._size, &(this->_value[offset]), &(this->_value[this->_size - 1u]) ) : npos;
      }
      inline size_t find_first_of(const _CharType* charList, size_t offset = 0u, size_t length = npos) const noexcept  {
        assert(charList != nullptr);
        if (offset < this->_size)
          return _find(charList, _getLength(charList, length), &(this->_value[offset]), &(this->_value[this->_size - 1u]) );
        return npos;
      }
      inline size_t find_first_of(_CharType character, size_t offset = 0u) const noexcept { return find(character, offset); }

      /// @brief Find the first position of any character different from the character(s) in 'charList' ('npos' returned if not found)
      inline size_t find_first_not_of(const Type& charList, size_t offset = 0u) const noexcept {
        return (offset < this->_size) ? _findNot(charList._value, charList._size, &(this->_value[offset]), &(this->_value[this->_size - 1u]) ) : npos;
      }
      inline size_t find_first_not_of(const _CharType* charList, size_t offset = 0u, size_t length = npos) const noexcept {
        assert(charList != nullptr);
        if (offset < this->_size)
          return _findNot(charList, _getLength(charList, length), &(this->_value[offset]), &(this->_value[this->_size - 1u]) );
        return npos;
      }
      inline size_t find_first_not_of(_CharType character, size_t offset = 0u) const noexcept {
        return (offset < this->_size) ? _findNot(character, &(this->_value[offset]), &(this->_value[this->_size - 1u]) ) : npos;
      }

      /// @brief Find the last position of any of the character(s) in 'charList' ('npos' returned if not found)
      size_t find_last_of(const Type& charList, size_t offset = npos) const noexcept {
        return _rfind(charList._value, charList._size, &(this->_value[_rOffsetIndex(offset)]), &(this->_value[0]) );
      }
      size_t find_last_of(const _CharType* charList, size_t offset = npos, size_t length = npos) const noexcept {
        assert(charList != nullptr);
        return _rfind(charList, _getLength(charList, length), &(this->_value[_rOffsetIndex(offset)]), &(this->_value[0]) );
      }
      inline size_t find_last_of(_CharType character, size_t offset = npos) const { return rfind(character, offset); }

      /// @brief Find the last position of any character different from the character(s) in 'charList' ('npos' returned if not found)
      size_t find_last_not_of(const Type& charList, size_t offset = npos) const {
        return _rfindNot(charList._value, charList._size, &(this->_value[_rOffsetIndex(offset)]), &(this->_value[0]) );
      }
      size_t find_last_not_of(const _CharType* charList, size_t offset = npos, size_t length = npos) const noexcept {
        assert(charList != nullptr);
        return _rfindNot(charList, _getLength(charList, length), &(this->_value[_rOffsetIndex(offset)]), &(this->_value[0]) );
      }
      inline size_t find_last_not_of(_CharType character, size_t offset = npos) const noexcept {
        return _rfindNot(character, &(this->_value[_rOffsetIndex(offset)]), &(this->_value[0]) );
      }


    private:
      // -- private - operations --

      // assign instance
      template <bool _IsSingleCharValue = false>
      inline void _assign(const _CharType* verifiedValue, size_t verifiedLength) {
        this->_size = _fixAssignSize(verifiedLength);
        if (this->_size > 0u && verifiedValue != this) {
          if (_IsSingleCharValue)
            _setSingleCharValue(0u, *verifiedValue, this->_size);
          else
            memcpy((void*)(this->_value), (void*)verifiedValue, this->_size*sizeof(_CharType));
        }
        if (this->_size < _MaxSize)
          memset((void*)&(this->_value[this->_size]), 0, (_MaxSize - this->_size)*sizeof(_CharType));
      }

      // append instance
      template <bool _IsSingleCharValue = false>
      inline void _append(const _CharType* verifiedValue, size_t verifiedLength) {
        size_t appendSize = _fixAppendSize(verifiedLength);
        if (appendSize > 0u) {
          if (_IsSingleCharValue)
            _setSingleCharValue(this->_size, *verifiedValue, appendSize);
          else
            memcpy((void*)&(this->_value[this->_size]), (void*)verifiedValue, appendSize*sizeof(_CharType));
          this->_size += appendSize;
        }
      }

      // insert instance
      template <bool _IsSingleCharValue = false>
      inline void _insert(const _CharType* verifiedValue, size_t verifiedLength, size_t position) {
        size_t insertSize = _fixAppendSize(verifiedLength);
        if (insertSize > 0u) {
          memmove((void*)&(this->_value[position + insertSize]), (void*)&(this->_value[position]), (this->_size - position)*sizeof(_CharType));
          this->_size += insertSize;
          if (_IsSingleCharValue)
            _setSingleCharValue(position, *verifiedValue, insertSize);
          else
            memcpy((void*)&(this->_value[position]), (void*)verifiedValue, insertSize*sizeof(_CharType));
        }
      }

      // replace by instance
      template <bool _IsSingleCharValue = false>
      inline void _replace(const _CharType* verifiedValue, size_t verifiedLength, size_t position, size_t replacedLength) noexcept {
        size_t newSize = position + verifiedLength;
        if (newSize < _MaxSize) {
          if (position + replacedLength < this->_size && verifiedLength != replacedLength) { // move end of string
            size_t movedSize = this->_size - position - replacedLength;
            newSize += (newSize + movedSize <= _MaxSize) ? movedSize : (movedSize = _MaxSize - newSize);
            memmove((void*)&(this->_value[position + verifiedLength]), (void*)&(this->_value[position + replacedLength]), movedSize*sizeof(_CharType));
          }
          if (newSize < this->_size) {// new string smaller (fill end with 0)
            memset((void*)&(this->_value[this->_size]), 0, (this->_size - newSize)*sizeof(_CharType));
            if (verifiedLength == 0)
              return;
          }
        }
        else { // fill max string
          verifiedLength = this->_size - position;
          newSize = _MaxSize;
        }
        this->_size = newSize;

        if (_IsSingleCharValue)
          _setSingleCharValue(position, *verifiedValue, verifiedLength);
        else
          memcpy((void*)&(this->_value[position]), (void*)verifiedValue, verifiedLength*sizeof(_CharType));
      }

      // erase range
      inline void _erase(size_t verifiedPosition, size_t verifiedLength) noexcept {
        if (verifiedLength > 0u) {
          if (verifiedPosition + verifiedLength < this->_size)
            memmove((void*)&(this->_value[verifiedPosition]), (void*)&(this->_value[verifiedPosition + verifiedLength]), (this->_size - (verifiedPosition + verifiedLength))*sizeof(_CharType));
          this->_size -= verifiedLength;
          memset((void*)&(this->_value[this->_size]), 0, (_MaxSize - this->_size)*sizeof(_CharType));
        }
      }

      // insert or append range, depending on position
      template <bool _IsSingleCharValue = false>
      inline void _insertOrAppend(const _CharType* verifiedValue, size_t verifiedLength, size_t position) noexcept {
        if (position < this->_size)
          _insert<_IsSingleCharValue>(verifiedValue, verifiedLength, position);
        else
          _append<_IsSingleCharValue>(verifiedValue, verifiedLength);
      }
      // replace or append range, depending on position
      template <bool _IsSingleCharValue = false>
      inline void _replaceOrAppend(const _CharType* verifiedValue, size_t verifiedLength, size_t offset, size_t length) noexcept {
        if (offset < this->_size)
          _replace<_IsSingleCharValue>(verifiedValue, verifiedLength, offset, length);
        else
          _append<_IsSingleCharValue>(verifiedValue, verifiedLength);
      }

      // set single repeated char value
      inline void _setSingleCharValue(size_t position, char value, size_t repeats) noexcept {
        memset((void*)&(this->_value[position]), value, repeats*sizeof(_CharType));
      }
      // set single repeated multi-byte char value
      template <typename _Type>
      inline void _setSingleCharValue(size_t position, _Type value, size_t repeats) noexcept {
        const _Type* end = &(this->_value[position + repeats - 1u]);
        for (_Type* it = &(this->_value[position]); it < end; ++it)
          *it = value;
      }

      // -- private - size management --
      
      // get length of string argument
      static inline size_t _getLength(const char* value, size_t maxLengthCheck) noexcept {
        return strnlen(value, maxLengthCheck);
      }
      // get length of wstring argument
      static inline size_t _getLength(const wchar_t* value, size_t maxLengthCheck) noexcept {
        return wcsnlen(value, maxLengthCheck);
      }
      // get length of other multi-byte string argument
      template <typename _Type>
      static inline size_t _getLength(const _Type* value, size_t maxLengthCheck) noexcept {
        const _Type* it = value;
        while (maxLengthCheck && *it != 0) {
          --maxLengthCheck;
          ++it;
        }
        return static_cast<size_t>(it - value);
      }

      // get absolute index from a right-side offset
      constexpr inline size_t _rOffsetIndex(size_t offset) const noexcept {
        return (offset == npos || offset >= this->_size) ? (this->_size - 1u) : (this->_size - offset - 1u);
      }
      // get min available length from two lengths
      inline int _minLengthAvailable(size_t offset, size_t length1,  size_t length2) const noexcept {
        if (offset < this->_size) {
          if (length1 > length2)
            length1 = length2;
          return (offset + length1 <= this->_size) ? length1 : this->_size - offset;
        }
        return 0;
      }

      // fix the length of an assigned string
      inline size_t _fixAssignSize(size_t appendSize) {
        if (appendSize > _MaxSize) {
          if (_ThrowIfMoreThanMax)
            throw std::out_of_range("FixedSizeString: total size exceeds _MaxSize.");
          return _MaxSize;
        }
        return appendSize;
      }
      // fix the length of an appended string
      inline size_t _fixAppendSize(size_t appendSize) {
        if (this->_size + appendSize > _MaxSize) {
          if (_ThrowIfMoreThanMax) 
            throw std::out_of_range("FixedSizeString: total size exceeds _MaxSize.");
          return (_MaxSize - this->_size);
        }
        return appendSize;
      }

      // -- private - searches --

      // find first non-trimmable character in current string
      inline size_t _getFirstNonTrimmableIndex() const noexcept {
        const _CharType* it = &(this->_value[0]);
        while (*it > 0 && *it < 0x20)
          ++it;
        return _iteratorToPosition(it);
      }
      // find last non-trimmable character in current string
      inline size_t _getIndexAfterLastNonTrimmable() const noexcept {
        if (this->_size > static_cast<size_t>(0u)) {
          const _CharType* it = &(this->_value[this->_size - 1u]);
          while (it >= &(this->_value[0]) && static_cast<uint32_t>(*it) < 0x20)
            --it;
          ++it; // index after last -> +1
          return _iteratorToPosition(it);
        }
        return static_cast<size_t>(0u);
      }

      // get position from iteration pointer
      inline size_t _iteratorToPosition(const _CharType* iter) const noexcept { return static_cast<size_t>(iter - &(this->_value[0])); }

      // first first character occurrence
      inline size_t _find(_CharType character, const _CharType* iter, const _CharType* iterLast) const noexcept {
        while (iter <= iterLast && *iter != character)
          ++iter;
        return (*iter == character) ? _iteratorToPosition(iter) : npos;
      }
      // first first character non-occurrence
      inline size_t _findNot(_CharType character, const _CharType* iter, const _CharType* iterLast) const noexcept {
        while (iter <= iterLast && *iter == character)
          ++iter;
        return (*iter != character) ? _iteratorToPosition(iter) : npos;
      }
      // find last character occurrence
      inline size_t _rfind(_CharType character, const _CharType* iter, const _CharType* iterRevLast) const noexcept {
        while (iter >= iterRevLast && *iter != character)
          --iter;
        return (*iter == character) ? _iteratorToPosition(iter) : npos;
      }
      // find last character non-occurrence
      inline size_t _rfindNot(_CharType character, const _CharType* iter, const _CharType* iterRevLast) const noexcept {
        while (iter >= iterRevLast && *iter == character)
          --iter;
        return (*iter != character) ? _iteratorToPosition(iter) : npos;
      }

      // first first character occurrence
      inline size_t _find(const _CharType* charList, size_t charNumber, const _CharType* iter, const _CharType* iterLast) const noexcept {
        const _CharType* charEnd = &(charList[charNumber]);
        while (iter <= iterLast) {
          for (const _CharType* charIt = charList; charIt < charEnd; ++charIt)
            if (*iter == *charIt)
              return _iteratorToPosition(iter);
          ++iter;
        }
        return npos;
      }
      // first first character non-occurrence
      inline size_t _findNot(const _CharType* charList, size_t charNumber, const _CharType* iter, const _CharType* iterLast) const noexcept {
        const _CharType* charEnd = &(charList[charNumber]);
        while (iter <= iterLast) {
          const _CharType* charIt;
          for (charIt = charList; charIt < charEnd; ++charIt)
            if (*iter == *charIt)
              break;
          if (charIt == charEnd)
            return _iteratorToPosition(iter);

          ++iter;
        }
        return npos;
      }
      // find last character occurrence
      inline size_t _rfind(const _CharType* charList, size_t charNumber, const _CharType* iter, const _CharType* iterRevLast) const noexcept {
        const _CharType* charEnd = &(charList[charNumber]);
        while (iter >= iterRevLast) {
          for (const _CharType* charIt = charList; charIt < charEnd; ++charIt)
            if (*iter == *charIt)
              return _iteratorToPosition(iter);
          --iter;
        }
        return npos;
      }
      // find last character non-occurrence
      inline size_t _rfindNot(const _CharType* charList, size_t charNumber, const _CharType* iter, const _CharType* iterRevLast) const noexcept {
        const _CharType* charEnd = &(charList[charNumber]);
        while (iter >= iterRevLast) {
          const _CharType* charIt;
          for (charIt = charList; charIt < charEnd; ++charIt)
            if (*iter == *charIt)
              break;
          if (charIt == charEnd)
            return _iteratorToPosition(iter);

          --iter;
        }
        return npos;
      }

      // first first string occurrence
      inline size_t _findString(const _CharType* query, size_t queryLength, const _CharType* iter, const _CharType* iterLast) const noexcept {
        const _CharType* lastPossibleQuery = iterLast + 1u - queryLength;
        while (iter <= lastPossibleQuery) {
          if (*iter == *query) {
            const _CharType* subIter = iter + queryLength - 1u;
            const _CharType* queryIter = query + queryLength - 1u;
            while (queryIter > query && *subIter == *queryIter) {
              --subIter;
              --queryIter;
            }
            if (queryIter == query)
              return _iteratorToPosition(iter);
          }
          ++iter;
        }
        return npos;
      }
      // find last string occurrence
      inline size_t _rfindString(const _CharType* query, size_t queryLength, const _CharType* iter, const _CharType* iterRevLast) const noexcept {
        iter = iter + 1u - queryLength;
        while (iter >= iterRevLast) {
          if (*iter == *query) {
            const _CharType* subIter = iter + queryLength - 1u;
            const _CharType* queryIter = query + queryLength - 1u;
            while (queryIter > query && *subIter == *queryIter) {
              --subIter;
              --queryIter;
            }
            if (queryIter == query)
              return _iteratorToPosition(iter);
          }
          --iter;
        }
        return npos;
      }

    private:
      _CharType _value[_MaxSize + 1u];
      size_t _size = 0u;
    };

    // -- aliases --
  
    /// @brief Fixed-size version of standard strings
    template <size_t _MaxSize, bool _ThrowIfMoreThanMax = false>
    using fixed_string = FixedSizeString<_MaxSize, char, _ThrowIfMoreThanMax>;

    /// @brief Fixed-size version of UTF-16/UCS-2 strings
    template <size_t _MaxSize, bool _ThrowIfMoreThanMax = false>
    using fixed_u16string = FixedSizeString<_MaxSize, char16_t, _ThrowIfMoreThanMax>;

    /// @brief Fixed-size version of UTF-32 strings
    template <size_t _MaxSize, bool _ThrowIfMoreThanMax = false>
    using fixed_u32string = FixedSizeString<_MaxSize, char32_t, _ThrowIfMoreThanMax>;

    /// @brief Fixed-size version of wide-character strings
    template <size_t _MaxSize, bool _ThrowIfMoreThanMax = false>
    using fixed_wstring = FixedSizeString<_MaxSize, wchar_t, _ThrowIfMoreThanMax>;
  }
}
