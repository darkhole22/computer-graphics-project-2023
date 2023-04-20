#pragma once
#include "vulture/util/Types.h"

#include <cstdlib> // std::malloc, std::free

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

class String
{
public:
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

	constexpr char operator[](u64 position) const noexcept
	{
		return (*this)[position];
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
	
	// TODO iterators
	/*
	begin()
	end()
	rbegin()
	rend()
	utf8begin()
	utf8end()
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

	// TODO u64 utf8length() const noexcept;

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
		if (capacity() < size)
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
			
			getDynamicData()->size = size;

			// TODO use iterator to update resized string
		}
	}

	constexpr bool operator==(const String& other) const noexcept
	{
		if (m_Data.head == other.m_Data.head)
		{
			if (m_Data.head & DYNAMIC_STRING_MASK)
			{
				if (m_Data.data == other.m_Data.data) return true;

				return strcmp(getDynamicString(), other.getDynamicString());
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

	constexpr ~String()
	{
		cleanup();
	}
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

using VString = std::string;

template<typename ... Args>
VString stringFormat(const VString& format, Args ...args)
{
	VString s;
	int size = std::snprintf(nullptr, 0, format.c_str(), args ...);
	if (size > 0) 
	{
		s.resize(size);
		std::snprintf(s.data(), static_cast<size_t>(size) + 1, format.c_str(), args ...);
	}
	return s;
}

} // namespace vulture
