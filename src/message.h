#if !defined MESSAGE_H
#define MESSAGE_H

template <size_t N>
class message
{
	uint8_t array[N];
	size_t Size;
	size_t readPtr;
public:
	message() : Size(0), readPtr(0) 
	{}

	size_t getSize() const
	{
		return Size;
	}

	size_t getCapacity() const
	{
		return N;
	}

	size_t getSpace() const
	{
		return N - Size;
	}

	uint8_t* getData()
	{
		return array;
	}

	const uint8_t* getData() const
	{
		return array;
	}

	uint8_t* getEnd()
	{
		return array + Size;
	}

	void addSize(size_t size)
	{
		Size = (Size + size > N) ? N : Size + size;
	}
	
	size_t push(const uint8_t* data, size_t size)
	{
		if(size > getSpace())
		{
			return getSpace();
		}
		memcpy(getEnd(), data, size);
		Size += size;
		return getSpace();
	}

	size_t push(const char* data)
	{
		return push((const uint8_t*)data, strlen(data) + 1);
	}

	template <typename T>
	size_t push(const T& data) //Возвращает оставшееся место
	{
		static_assert(IS_TRIVIALLY_COPYABLE(T), "T must be trivially copyable (POD-like) for memcpy safety.");
		if(sizeof(T) > getSpace())
		{
			return getSpace();
		}
		memcpy(getEnd(), &data, sizeof(T));
		Size += sizeof(T);
		return getSpace();
	}

	size_t pop(uint8_t* data, size_t size)
	{
		if(size > Size)
		{
			return Size;
		}
		memcpy(data, array, size);
		memmove(array, array + size, Size - size);
		Size -= size;
		return Size;
	}

	template <typename T>
	T pop() //Возвращает оставшийся размер
	{
		static_assert(IS_TRIVIALLY_COPYABLE(T), "T must be trivially copyable (POD-like) for memcpy safety.");
		T data;
		if(sizeof(T) > Size)
		{
			memset(&data, 0, sizeof(T));
			return data;
		}
		pop((uint8_t*)&data, sizeof(T));
		return data;
	}

	void clear()
	{
		Size = 0;
	}

	size_t getReadPtr() const
	{
		return readPtr;
	}

	void setReadPtr(size_t ptr)
	{
		if(ptr > Size)
			readPtr = Size;
		else
			readPtr = ptr;
	}

	template <typename T>
	T read()
	{
		static_assert(IS_TRIVIALLY_COPYABLE(T), "T must be trivially copyable (POD-like) for memcpy safety.");
		T data;
		if(readPtr + sizeof(T) > Size)
		{
			memset(&data, 0, sizeof(T));
			return data;
		}
		memcpy(&data, array + readPtr, sizeof(T));
		readPtr += sizeof(T);
		return data;
	}
	
	char* readString()
	{
		if(readPtr >= Size)
			return nullptr;
		char* start = (char*)(array + readPtr);
		size_t len = strnlen(start, Size - readPtr);
		if(readPtr + len >= Size) //Нет завершающего нуля
			return nullptr;
		readPtr += len + 1;
		return start;
	}
};

#endif