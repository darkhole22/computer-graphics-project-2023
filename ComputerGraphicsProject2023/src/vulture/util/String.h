#pragma once
#include "vulture/util/Types.h"

#include <string>

namespace vulture {

constexpr u64 strlen(const char* str)
{
	u64 size = 0;
	if (str) 
		while (str[size]) 
			++size;
	return size;
}

constexpr void strcpy(char* dest, const char* src)
{
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

constexpr bool strcmp(const char* s1, const char* s2)
{
	u64 i = 0;
	if (s1 == s2) return true;
	if (s1 && s2)
	{
		while (s1[i] && s2[i])
		{
			if (s1[i] != s2[i]) return false;
		}
		return s1[i] == s2[i];
	}
	return false;
}

// Implementation from https://mgronhol.github.io/fast-strcmp/
constexpr i32 strncmp(const char* ptr0, const char* ptr1, u64 len)
{
	u64 fast = len / sizeof(size_t) + 1;
	u64 offset = (fast - 1) * sizeof(size_t);
	u32 current_block = 0;

	if (len <= sizeof(size_t)) { fast = 0; }

	u64* lptr0 = (u64*)ptr0;
	u64* lptr1 = (u64*)ptr1;

	while (current_block < fast) 
	{
		if ((lptr0[current_block] ^ lptr1[current_block])) 
			for (u32 pos = current_block * sizeof(u64); pos < len; ++pos)
				if ((ptr0[pos] ^ ptr1[pos]) || (ptr0[pos] == 0) || (ptr1[pos] == 0))
					return  (i32)((u8)ptr0[pos] - (u8)ptr1[pos]);
		++current_block;
	}

	while (len > offset) 
	{
		if ((ptr0[offset] ^ ptr1[offset])) 
			return (i32)((u8)ptr0[offset] - (u8)ptr1[offset]);
		++offset;
	}

	return 0;
}

class String
{
public:
	template <class T>
	struct Iterator_T
	{
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = i64;
		using value_type = typename T;
		using pointer = typename T*;
		using reference = typename T&;

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

	template <class T>
	struct UTF8Iterator_T
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = i64;
		using value_type = i32;
		// using pointer = i32*;
		// using reference = i32&;

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

		// constexpr reference operator*() { return *m_Ptr; }
		constexpr value_type operator*() const { return m_Codepoint; }
		// constexpr pointer operator->() { return m_Ptr; }
		// constexpr const pointer operator->() const { return m_Ptr; }

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
				// TODO log error
			}
		}
	};
	
	using Iterator = Iterator_T<char>;
	using ConstIterator = Iterator_T<const char>;

	using UTF8Iterator = UTF8Iterator_T<char>;
	using ConstUTF8Iterator = UTF8Iterator_T<const char>;

	constexpr String() noexcept : m_Data() {}

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
			} // TODO else log error on failed allocation
		}
	}

	constexpr String(String&& other) noexcept : m_Data()
	{
		copyData(m_Data, other.m_Data);
		zeroData(other.m_Data);
	}

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
			} // TODO else log error on failed allocation
		}
	}

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
				} // TODO else log error on failed allocation
			}
		}
		return *this;
	}

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
			} // TODO else log error on failed allocation
		}
		return *this;
	}

	constexpr String& operator=(char ch) noexcept
	{
		cleanup();
		zeroData(m_Data);

		m_Data.str[0] = ch;
		m_Data.head = 1;
		return *this;
	}

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
			// TODO log error, returning last valid character
			return dynamicStr[dynamicData->size - 1];
		}
		if (m_Data.head > position)
		{
			return m_Data.str[position];
		}
		// TODO log error, returning last valid character
		return m_Data.str[m_Data.head - 1];
	}

	constexpr const char& operator[](u64 position) const noexcept
	{
		return (*const_cast<String*>(this))[position];
	}

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

	constexpr const char* cString() const noexcept
	{
		// This is safe because the return value of cString is a 'const char*'.
		return const_cast<String*>(this)->data();
	}
	
	constexpr Iterator begin()
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return Iterator(m_Data.str);
		else return Iterator(getDynamicString());
	}

	constexpr ConstIterator begin() const
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return ConstIterator(m_Data.str);
		else return ConstIterator(getDynamicString());
	}

	constexpr Iterator end()
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return Iterator(m_Data.str + m_Data.head);
		else return Iterator(getDynamicString() + getDynamicData()->size);
	}

	constexpr ConstIterator end() const
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return ConstIterator(m_Data.str + m_Data.head);
		else return ConstIterator(getDynamicString() + getDynamicData()->size);
	}

	constexpr UTF8Iterator utf8begin()
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return UTF8Iterator(m_Data.str);
		else return UTF8Iterator(getDynamicString());
	}

	constexpr UTF8Iterator utf8end()
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return UTF8Iterator(m_Data.str + m_Data.head);
		else return UTF8Iterator(getDynamicString() + getDynamicData()->size);
	}

	constexpr ConstUTF8Iterator utf8begin() const
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return ConstUTF8Iterator(m_Data.str);
		else return ConstUTF8Iterator(getDynamicString());
	}

	constexpr ConstUTF8Iterator utf8end() const
	{
		if (!(m_Data.head & DYNAMIC_STRING_MASK))
			return ConstUTF8Iterator(m_Data.str + m_Data.head);
		else return ConstUTF8Iterator(getDynamicString() + getDynamicData()->size);
	}

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

	u64 utf8length() const noexcept
	{
		u64 length = 0;
		auto it = utf8begin();
		while (it++ != utf8end())
		{
			++length;
		}
		return length;
	}

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

	// size <- null terminator included
	constexpr void reserve(u64 size) noexcept
	{
		grow(size, true);
	}

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

	constexpr auto operator<=>(const String& other) const noexcept
	{
		u64 len = length();
		u64 oLen = other.length();
		return strncmp(cString(), other.cString(), len > oLen ? oLen : len);
	}

	constexpr ~String()
	{
		cleanup();
	}

	friend constexpr String operator+(const String& left, const String& right) noexcept
	{
		String result{};
		result.resize(left.length() + right.length() + 1);
		strcpy(&result[0], &left[0]);
		strcpy(&result[left.length()], &right[0]);
		return result;
	}

	friend constexpr String operator+(const String& left, const char* right) noexcept
	{
		u64 rLength = strlen(right);
		String result{};
		result.resize(left.length() + rLength + 1);
		strcpy(&result[0], &left[0]);
		strcpy(&result[left.length()], right);
		return result;
	}

	friend constexpr String operator+(const char* left, const String& right) noexcept 
	{
		u64 lLength = strlen(left);
		String result{};
		result.resize(right.length() + lLength + 1);
		strcpy(&result[0], left);
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
		} // TODO else log error on failed allocation
	}

	constexpr void cleanup()
	{
		if (m_Data.head & DYNAMIC_STRING_MASK)
		{
			delete[] m_Data.data;
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

} // namespace vulture

namespace std {

/*
 * @brief A simple implementation of the Fowler–Noll–Vo hash function.
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

};
