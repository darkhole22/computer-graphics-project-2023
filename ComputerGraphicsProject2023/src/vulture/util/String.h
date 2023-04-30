#pragma once
#include "vulture/util/Types.h"

#include <string>
#include <iostream>

// No header file should include Logger.h
#define VU_STRING_LOG(msg) std::cout << "[STRING ERROR][" << __LINE__ << "]\n\t" << msg << std::endl

namespace vulture {

/*
* @brief Compute the length of a null terminated string.
* 
* @param str the null terminated input string.
* 
* @return the length of the input string.
*/
constexpr u64 strlen(const char* str)
{
	// This needs to be reinplemented because the standard library
	// doesn't have a constexpr implementation.
	u64 size = 0;
	if (str) 
		while (str[size]) 
			++size;
	return size;
}

/*
* @brief Makes a copy of a null terminated string into a provided buffer.
*
* @param dest the destination buffer
* @param src the source null terminated string.
*/
constexpr void strcpy(char* dest, const char* src)
{
	// This needs to be reinplemented because the standard library
	// doesn't have a constexpr implementation.
	u64 i = 0;
	if (dest && src)
	{
		while (src[i])
		{
			dest[i] = src[i];
			++i;
		}
		dest[i] = src[i]; // null terminator copy
	}
}

/*
* @brief Compares the first len character of two string.
* Implementation from https://mgronhol.github.io/fast-strcmp/
* 
* @param s1 the first string.
* @param s2 the second string.
* @param len the number of characters to compare.
*
* @return 0 if the two string are equals.
*/
constexpr i32 strncmp(const char* s1, const char* s2, u64 len)
{
	// This needs to be reinplemented because the standard library
	// doesn't have a constexpr implementation.
	u64 fast = len / sizeof(size_t) + 1;
	u64 offset = (fast - 1) * sizeof(size_t);
	u32 current_block = 0;

	if (len <= sizeof(size_t)) { fast = 0; }

	u64* lptr0 = (u64*)s1;
	u64* lptr1 = (u64*)s2;

	while (current_block < fast) 
	{
		if ((lptr0[current_block] ^ lptr1[current_block])) 
			for (u32 pos = current_block * sizeof(u64); pos < len; ++pos)
				if ((s1[pos] ^ s2[pos]) || (s1[pos] == 0) || (s2[pos] == 0))
					return  (i32)((u8)s1[pos] - (u8)s2[pos]);
		++current_block;
	}

	while (len > offset) 
	{
		if ((s1[offset] ^ s2[offset])) 
			return (i32)((u8)s1[offset] - (u8)s2[offset]);
		++offset;
	}

	return 0;
}

/*
* @brief Compares two null terminated string.
*
* @param s1 the first string.
* @param s2 the second string.
*
* @return 0 if the two string are equals.
*/
constexpr i32 strcmp(const char* s1, const char* s2)
{
	// This needs to be reinplemented because the standard library
	// doesn't have a constexpr implementation.
	u64 i = 0;
	if (s1 == s2) return 0;
	if (s1 && s2)
	{
		while (s1[i] && s2[i])
		{
			if (s1[i] != s2[i]) return s1[i] - s2[i];
			++i;
		}
		return s1[i] - s2[i];
	}
	return static_cast<i32>(s1 - s2);
}

/*
* @brief A class to handle dynamic string.
* This class uses small string optimization so for string less than
* 22 caracters no allocation is required.
* 
* This class also have utf8 convenience methods and iterator.
* All the methods of this class are constexpr.
*/
class String
{
public:
	/*
	* @brief A templated Iterator type to iterate over the String class.
	*/
	template <typename T> struct Iterator_T
	{
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = i64;
		using value_type = T;
		using pointer = T*;
		using reference = T&;

		constexpr Iterator_T() : m_Ptr(nullptr) {}
		constexpr Iterator_T(pointer ptr) : m_Ptr(ptr) {}
		constexpr Iterator_T(const Iterator_T& ptr) = default;
		constexpr Iterator_T& operator=(const Iterator_T& ptr) = default;

		constexpr Iterator_T& operator++() { ++m_Ptr; return *this; }
		constexpr Iterator_T operator++(int) { Iterator_T tmp = *this; ++(*this); return tmp; }
		
		constexpr Iterator_T& operator--() { --m_Ptr; return *this; }
		constexpr Iterator_T operator--(int) { Iterator_T tmp = *this; --(*this); return tmp; }

		constexpr auto operator<=>(const Iterator_T& other) const { return m_Ptr <=> other.m_Ptr; }
		constexpr bool operator==(const Iterator_T& other) const { return std::is_eq(*this <=> other); }

		constexpr reference operator*() { return *m_Ptr; }
		constexpr const reference operator*() const { return *m_Ptr; }

		constexpr ~Iterator_T() = default;
	private:
		pointer m_Ptr;
	};

	/*
	* @brief A templated Iterator type that provides a way to iterate
	* over the String class by UTF-8 codepoints.
	*/
	template <class T> struct UTF8Iterator_T
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = i64;
		using value_type = i32;

		constexpr UTF8Iterator_T() : UTF8Iterator_T(nullptr) {}
		constexpr UTF8Iterator_T(T* ptr) : m_Ptr(ptr), m_Codepoint(0) { computeCodepoint(); }
		constexpr UTF8Iterator_T(const UTF8Iterator_T& ptr) = default;
		constexpr UTF8Iterator_T& operator=(const UTF8Iterator_T& ptr) = default;

		constexpr UTF8Iterator_T& operator++()
		{ 
			++m_Ptr;
			computeCodepoint();
			return *this; 
		}

		constexpr UTF8Iterator_T operator++(int) { UTF8Iterator_T tmp = *this; ++(*this); return tmp; }

		constexpr auto operator<=>(const UTF8Iterator_T& other) const { return m_Ptr <=> other.m_Ptr; }
		constexpr bool operator==(const UTF8Iterator_T& other) const { return std::is_eq(*this <=> other); }

		constexpr value_type operator*() const { return m_Codepoint; }

		constexpr ~UTF8Iterator_T() = default;
	private:
		T* m_Ptr;
		value_type m_Codepoint;

		constexpr void computeCodepoint()
		{
			m_Codepoint = *m_Ptr;
			if (m_Codepoint >= 0x7F && (m_Codepoint & 0xE0) == 0xC0)
			{
				m_Codepoint =
					((m_Ptr[0] & 0b00011111) << 6) +
					((m_Ptr[1] & 0b00111111));
				++m_Ptr;
			}
			else if ((m_Codepoint & 0xF0) == 0xE0)
			{
				m_Codepoint =
					((m_Ptr[0] & 0b00001111) << 12) +
					((m_Ptr[1] & 0b00011111) << 6) +
					((m_Ptr[2] & 0b00111111));
				m_Ptr += 2;
			}
			else if ((m_Codepoint & 0xF8) == 0xF0)
			{
				m_Codepoint =
					((m_Ptr[0] & 0b00000111) << 18) +
					((m_Ptr[0] & 0b00001111) << 12) +
					((m_Ptr[1] & 0b00011111) << 6) +
					((m_Ptr[2] & 0b00111111));
				m_Ptr += 3;
			}
			else
			{
				m_Ptr += 3;
				VU_STRING_LOG("Trying to compute a codepoint longer than 4 byte! Skipping...");
			}
		}
	};
	
	/*
	* @brief A specification of the Templated iterator.
	*/
	using Iterator = Iterator_T<char>;
	/*
	* @brief A specification of the Templated iterator for constant iteration.
	*/
	using ConstIterator = Iterator_T<const char>;

	/*
	* @brief A specification of the UTF8 Templated iterator.
	*/
	using UTF8Iterator = UTF8Iterator_T<char>;
	/*
	* @brief A specification of the UTF8 Templated iterator for constant iteration.
	*/
	using ConstUTF8Iterator = UTF8Iterator_T<const char>;

	/*
	* @brief Construct an empty String.
	*/
	constexpr String() noexcept : m_Data() {}

	/*
	* @brief Copy contructor.
	* 
	* @param other the String to copy.
	*/
	constexpr String(const String& other) noexcept : m_Data()
	{
		if (!(other.m_Data.head & DYNAMIC_STRING_MASK))
		{
			copyData(m_Data, other.m_Data);
		}
		else
		{
			u64 otherLen = other.length();
			createDynamicState(otherLen + 1);
			if (m_Data.data)
			{
				strcpy(getDynamicString(), other.cString());
			}
			else
			{
				VU_STRING_LOG("Failed allocation.");
			}
		}
	}

	/*
	* @brief Move constructor.
	* 
	* @param other the String to move.
	*/
	constexpr String(String&& other) noexcept : m_Data()
	{
		copyData(m_Data, other.m_Data);
		zeroData(other.m_Data);
	}

	/*
	* @brief Construct the String for a null terminated string
	* 
	* @param str the input null terminated string.
	*/
	constexpr String(const char* str) noexcept : m_Data()
	{
		u64 inLen = strlen(str);

		if (inLen <= MAX_SMALL_STRING_LEN)
		{
			strcpy(m_Data.str, str);
			m_Data.head = static_cast<u8>(inLen);
		}
		else
		{
			createDynamicState(inLen + 1);
			if (m_Data.data)
			{
				strcpy(getDynamicString(), str);
				getDynamicData()->size = inLen;
			}
			else
			{
				VU_STRING_LOG("Failed allocation.");
			}
		}
	}

	/*
	* @brief Construct the String for a std::string.
	*
	* @param str the input string.
	*/
	constexpr String(const std::string& str) noexcept : String(str.c_str()) {}

	/*
	* @brief Assigns the copy of the provided String.
	*
	* @param other the String to copy.
	*
	* @return *this.
	*/
	constexpr String& operator=(const String& other) noexcept
	{
		if (this != &other)
		{
			cleanup();

			if (!(other.m_Data.head & DYNAMIC_STRING_MASK))
			{
				copyData(m_Data, other.m_Data);
			}
			else
			{
				u64 otherLen = other.length();
				createDynamicState(otherLen + 1);
				if (m_Data.data)
				{
					strcpy(getDynamicString(), other.cString());
					getDynamicData()->size = other.length();
				}
				else
				{
					VU_STRING_LOG("Failed allocation.");
				}
			}
		}
		return *this;
	}

	/*
	* @brief Moves of the provided String.
	*
	* @param other the String to move.
	*
	* @return *this.
	*/
	constexpr String& operator=(String&& other) noexcept
	{
		if (this != &other)
		{
			cleanup();
			copyData(m_Data, other.m_Data);
			zeroData(other.m_Data);
		}
		return *this;
	}

	/*
	* @brief Assigns the copy of the provided null terminated string.
	*
	* @param str the null terminated string to copy.
	*
	* @return *this.
	*/
	constexpr String& operator=(const char* str) noexcept
	{
		cleanup();
		u64 inLen = strlen(str);

		if (inLen <= MAX_SMALL_STRING_LEN)
		{
			strcpy(m_Data.str, str);
			m_Data.head = static_cast<u8>(inLen);
		}
		else
		{
			createDynamicState(inLen + 1);
			if (m_Data.data)
			{
				strcpy(getDynamicString(), str);
				getDynamicData()->size = inLen;
			}
			else
			{
				VU_STRING_LOG("Failed allocation.");
			}
		}
		return *this;
	}

	/*
	* @brief Assigns the copy of the provided std::string.
	*
	* @param str the std::string to copy.
	*
	* @return *this.
	*/
	constexpr String& operator=(const std::string& str) noexcept
	{
		return *this = str.c_str();
	}

	/*
	* @brief Assigns a character.
	*
	* @param ch teh provided character.
	*
	* @return *this.
	*/
	constexpr String& operator=(char ch) noexcept
	{
		cleanup();
		zeroData(m_Data);

		m_Data.str[0] = ch;
		m_Data.head = 1;
		return *this;
	}

	/*
	* @brief Provides a way to access individual characters of the String class by index.
	* It returns a reference to the character at the specified index.
	*
	* @param position the index of the desired character.
	*
	* @return a referance to the specified character.
	*/
	constexpr char& operator[](u64 position) noexcept
	{
		if (m_Data.head & DYNAMIC_STRING_MASK)
		{
			DynamicData* dynamicData = getDynamicData();
			char* dynamicStr = getDynamicString();
			if (dynamicData->size > position)
			{
				return dynamicStr[position];
			}
	
			VU_STRING_LOG("Trying to index (" << position << ") out of bound[len:" << dynamicData->size << "].");
			return dynamicStr[dynamicData->size - 1];
		}
		if (m_Data.head > position)
		{
			return m_Data.str[position];
		}
		VU_STRING_LOG("Trying to index (" << position << ") out of bound[len:" << m_Data.head << "].");
		return m_Data.str[m_Data.head - 1];
	}

	/*
	* @brief Provides a way to access individual characters of the String class by index.
	* It returns a const reference to the character at the specified index.
	*
	* @param position the index of the desired character.
	*
	* @return a const referance to the specified character.
	*/
	constexpr const char& operator[](u64 position) const noexcept
	{
		return (*const_cast<String*>(this))[position];
	}

	/*
	* @brief Provide a pointer of the first element of the internal charater buffer.
	*
	* @return a pointer of the first character.
	*/
	constexpr char* data() noexcept
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
		{
			return m_Data.str;
		}
		else
		{
			return getDynamicString();
		}
	}

	/*
	* @brief Provide a c-style pointer of the internal charater buffer.
	*
	* @return a c-style string.
	*/
	constexpr const char* cString() const noexcept
	{
		// This is safe because the return value of cString is a 'const char*'.
		return const_cast<String*>(this)->data();
	}

	/*
	* @brief An iterator pointing to the beginning of the string.
	*
	* @return an iterator pointing to the beginning of the string.
	*/
	constexpr Iterator begin()
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return Iterator(m_Data.str);
		else return Iterator(getDynamicString());
	}

	/*
	* @brief A const iterator pointing to the beginning of the string.
	*
	* @return a const iterator pointing to the beginning of the string.
	*/
	constexpr ConstIterator begin() const
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return ConstIterator(m_Data.str);
		else return ConstIterator(getDynamicString());
	}

	/*
	* @brief An iterator pointing to the end of the string.
	*
	* @return an iterator pointing to the end of the string.
	*/
	constexpr Iterator end()
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return Iterator(m_Data.str + m_Data.head);
		else return Iterator(getDynamicString() + getDynamicData()->size);
	}

	/*
	* @brief A const iterator pointing to the beginning of the string.
	*
	* @return a const iterator pointing to the beginning of the string.
	*/
	constexpr ConstIterator end() const
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return ConstIterator(m_Data.str + m_Data.head);
		else return ConstIterator(getDynamicString() + getDynamicData()->size);
	}

	/*
	* @brief An utf8 iterator pointing to the beginning of the string.
	*
	* @return an iterator pointing to the beginning of the string.
	*/
	constexpr UTF8Iterator utf8begin()
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return UTF8Iterator(m_Data.str);
		else return UTF8Iterator(getDynamicString());
	}

	/*
	* @brief An utf8 iterator pointing to the end of the string.
	*
	* @return an iterator pointing to the end of the string.
	*/
	constexpr UTF8Iterator utf8end()
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return UTF8Iterator(m_Data.str + m_Data.head);
		else return UTF8Iterator(getDynamicString() + getDynamicData()->size);
	}

	/*
	* @brief A const utf8 iterator pointing to the beginning of the string.
	*
	* @return a const iterator pointing to the beginning of the string.
	*/
	constexpr ConstUTF8Iterator utf8begin() const
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return ConstUTF8Iterator(m_Data.str);
		else return ConstUTF8Iterator(getDynamicString());
	}

	/*
	* @brief A const utf8 iterator pointing to the end of the string.
	*
	* @return a const iterator pointing to the end of the string.
	*/
	constexpr ConstUTF8Iterator utf8end() const
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return ConstUTF8Iterator(m_Data.str + m_Data.head);
		else return ConstUTF8Iterator(getDynamicString() + getDynamicData()->size);
	}

	/*
	* @brief Checks if the String is empty. 
	* This is equivalent to length() == 0.
	* 
	* @return true if the String if empty.
	*/
	constexpr bool isEmpty() const noexcept
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
		{
			return m_Data.head == 0;
		}
		else
		{
			return getDynamicData()->size == 0;
		}
	}

	/*
	* @brief Gets the string length.
	* 
	* @return the length of the String.
	*/
	constexpr u64 length() const noexcept
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
		{
			return m_Data.head;
		}
		else
		{
			return getDynamicData()->size;
		}
	}

	/*
	* @brief Gets the utf8 string length.
	*
	* @return the utf8 length of the String.
	*/
	constexpr u64 utf8length() const noexcept
	{
		u64 length = 0;
		auto it = utf8begin();
		while (it++ != utf8end())
		{
			++length;
		}
		return length;
	}

	/*
	* @brief Gets the internal capacity.
	* This include space for the null terminator, so the maximum length
	* the string can have before resiziong is capacity() - 1.
	*
	* @return the internal buffer capacity.
	*/
	constexpr u64 capacity() const noexcept
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
		{
			return MAX_SMALL_STRING_LEN + 1;
		}
		else
		{
			return getDynamicData()->capacity;
		}
	}

	/*
	* @brief After this method is called the internal 
	* capacity will be grather than the provided size.
	* This method naver decrease the internal capacity.
	*
	* @param size the target size.
	*/
	constexpr void reserve(u64 size) noexcept
	{
		grow(size, true);
	}

	/*
	* @brief Clears the String. After this method is called the length() == 0,
	* and the dynamic buffer is freed (if present).
	*/
	constexpr void clear() noexcept
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
		{
			zeroData(m_Data);
		}
		else
		{
			getDynamicData()->size = 0;
		}
	}

	/*
	* @brief Resize the string to have length of size.
	* If this method increase the length of the String the added
	* character will be filled whith the provded character.
	*
	* @param size the target size.
	* @param ch the charcater to fill the added characters.
	*/
	constexpr void resize(u64 size, char ch = '\0')
	{
		if (capacity() > size)
		{
			// shrink??
			if (!(m_Data.head & DYNAMIC_STRING_MASK))
			{
				m_Data.head = static_cast<u8>(size);
				m_Data.str[size] = 0;
			}
			else
			{
				getDynamicData()->size = size;
				getDynamicString()[size] = 0;
			}
		}
		else
		{
			grow(size + 1);

			auto it = end();
			getDynamicData()->size = size;
			while (it++ != end())
				*it = ch;
		}
	}

	/*
	* @brief Checks if the provided string is equal to this.
	*
	* @param other the String to compare to.
	*
	* @return true if the provided String is equals to this.
	*/
	constexpr bool operator==(const String& other) const noexcept
	{
		if (m_Data.head == other.m_Data.head)
		{
			if (m_Data.head & DYNAMIC_STRING_MASK)
			{
				if (m_Data.data == other.m_Data.data) return true;

				u64 len = length();
				u64 oLen = other.length();
				return strncmp(getDynamicString(), other.getDynamicString(), len > oLen ? oLen : len) == 0;
			}
			else
			{
				return m_Data.data == other.m_Data.data &&
					m_Data.reserved_0 == other.m_Data.reserved_0 &&
					m_Data.reserved_1 == other.m_Data.reserved_1;
			}
		}
		return false;
	}

	/*
	* @brief Provides a way to compare two String objects for ordering.
	*
	* @param other the String to compare to.
	*
	* @return 0 if provide string is equal.
	* @return > 1 if the provided string should go after this.
	* @return < 1 if the provided string should go before this.
	*/
	constexpr auto operator<=>(const String& other) const noexcept
	{
		u64 len = length();
		u64 oLen = other.length();
		return strncmp(cString(), other.cString(), len > oLen ? oLen : len);
	}

	/*
	* @brief The destructor of this class.
	*/
	constexpr ~String()
	{
		cleanup();
	}

	/*
	* @brief Concatenates two String.
	*
	* @param left the first String.
	* @param right the second String.
	*
	* @return the concatenated String.
	*/
	friend constexpr String operator+(const String& left, const String& right) noexcept
	{
		String result{};
		result.resize(left.length() + right.length());
		if (left.length() > 0)
		strcpy(&result[0], &left[0]);
		if (right.length() > 0)
		strcpy(&result[left.length()], &right[0]);
		return result;
	}

	/*
	* @brief Concatenates a String and a null terminated string.
	*
	* @param left the String.
	* @param right the null terminated string.
	*
	* @return the concatenated String.
	*/
	friend constexpr String operator+(const String& left, const char* right) noexcept
	{
		u64 rLength = strlen(right);
		String result{};
		result.resize(left.length() + rLength);
		if (left.length() > 0)
		strcpy(&result[0], &left[0]);
		strcpy(&result[left.length()], right);
		return result;
	}

	/*
	* @brief Concatenates a null terminated string and a String.
	*
	* @param left the null terminated string.
	* @param right the String.
	*
	* @return the concatenated String.
	*/
	friend constexpr String operator+(const char* left, const String& right) noexcept
	{
		u64 lLength = strlen(left);
		String result{};
		result.resize(right.length() + lLength);
		strcpy(&result[0], left);
		if (right.length() > 0)
		strcpy(&result[lLength], &right[0]);
		return result;
	}

	// TODO other + operator
private:
	constexpr static u8 DYNAMIC_STRING_MASK = 0b10000000;
	constexpr static u8 MAX_SMALL_STRING_LEN = 22;

	union FieldData
	{
		struct
		{
			void* data;
			u64 reserved_0;
			u64 reserved_1;
		};
		struct
		{
			char str[23];
			/*
			 * @brief Contains infomation about the use of the field data.
			 * The most significant bit is set to 1 if the string is stored dynamically
			 * and 0 if SSO is in use.
			 *
			 * The remaining bits store the length of the small string if in use.
			 */
			u8 head;
		};
	};
	// static_assert(sizeof(FieldData) == 3 * sizeof(u64));

	/*
	 * @brief
	 */
	struct DynamicData
	{
		/*
		 * @brief The string size.
		 */
		u64 size;

		/*
		 * @brief The capacity of the buffer containing the string.
		 * If the buffer is full (capacity == size + 1) because of the null terminator.
		 */
		u64 capacity;
	};
	constexpr static u64 DYNAMIC_OFFSET = sizeof(DynamicData);

	FieldData m_Data;

	constexpr static void copyData(FieldData& dest, const FieldData& src)
	{
		dest.data = src.data;
		dest.reserved_0 = src.reserved_0;
		dest.reserved_1 = src.reserved_1;
	}

	constexpr static void zeroData(FieldData& data)
	{
		data.data = 0;
		data.reserved_0 = 0;
		data.reserved_1 = 0;
	}

	constexpr DynamicData* getDynamicData()
	{
		return static_cast<DynamicData*>(m_Data.data);
	}

	constexpr const DynamicData* getDynamicData() const
	{
		return static_cast<DynamicData*>(m_Data.data);
	}

	constexpr char* getDynamicString()
	{
		return static_cast<char*>(m_Data.data) + DYNAMIC_OFFSET;
	}

	constexpr const char* getDynamicString() const
	{
		return static_cast<char*>(m_Data.data) + DYNAMIC_OFFSET;
	}

	constexpr void createDynamicState(u64 size)
	{
		m_Data.head = DYNAMIC_STRING_MASK;
		m_Data.data = new u8[sizeof(DynamicData) + size];
		if (m_Data.data)
		{
			DynamicData* dynamicData = static_cast<DynamicData*>(m_Data.data);
			dynamicData->size = 0;
			dynamicData->capacity = size;
		}
		else
		{
			VU_STRING_LOG("Failed allocation.");
		}
	}

	constexpr void cleanup()
	{
		if (m_Data.head & DYNAMIC_STRING_MASK)
		{
			delete[] static_cast<u8*>(m_Data.data);
		}
		zeroData(m_Data);
	}

	constexpr void grow(u64 targetSize, bool exact = false)
	{
		u64 size = capacity();
		if (exact)
		{
			size = targetSize;
		}
		else
		{
			while (size <= targetSize)
			{
				size *= 2;
			}
		}
		if (size <= capacity()) return;

		// Allocate new buffer
		void* newData = new u8[sizeof(DynamicData) + size];

		// Copy old data
		DynamicData* newDynamicData = static_cast<DynamicData*>(newData);
		newDynamicData->size = length();
		newDynamicData->capacity = size;
		strcpy(static_cast<char*>(newData) + DYNAMIC_OFFSET, cString());

		// Update internal state
		cleanup();
		m_Data.head = DYNAMIC_STRING_MASK;
		m_Data.data = newData;
	}
};

/*
* @brief Generate a String from a format string and variadic arguments.
*
* @param format a null terminated format string.
* @param args the arguments.
*
* @return the formatted String.
*/
template<typename ... Args>
String stringFormat(const char* format, Args ...args)
{
	String s;
	int size = std::snprintf(nullptr, 0, format, args ...);
	if (size > 0)
	{
		s.resize(size);
		std::snprintf(s.data(), static_cast<size_t>(size) + 1, format, args ...);
	}
	return s;
}

/*
* @brief Generate a String from a format String and variadic arguments.
*
* @param format a format String.
* @param args the arguments.
*
* @return the formatted String.
*/
template<typename ... Args>
String stringFormat(const String& format, Args ...args)
{
	return stringFormat(format.cString(), std::forward<Args>(args)...);
}

} // namespace vulture

namespace std {

	/*
	 * @brief A simple implementation of the Fowler�Noll�Vo hash function.
	 * See https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function for reference.
	 */
	template<> struct hash<vulture::String>
	{
		size_t operator()(const vulture::String& str) const
		{
			const char* s = str.cString();
			auto* buffer = reinterpret_cast<const unsigned char*>(s);
			size_t h = 0;
			while (*buffer)
			{
				h ^= (size_t)buffer++;
				h += (h << 1) + (h << 4) + (h << 5) +
					(h << 7) + (h << 8) + (h << 40);
			}

			return h;
		}
	};

	/*
	* @brief
	*
	* @param str
	*
	* @return
	*/
	inline ostream& operator<<(ostream& os, const vulture::String& str)
	{
		return os << str.cString();
	}

	/*
	* @brief
	*
	* @param str
	*
	* @return
	*/
inline istream& operator>>(istream& is, vulture::String& str)
{
	std::string s;
	is >> s;
	str = s.c_str();
	return is;
}

};

#undef VU_STRING_LOG
