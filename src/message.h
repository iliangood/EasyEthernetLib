#if !defined MESSAGE_H
#define MESSAGE_H

template <size_t N>
class message
{
	uint8_t array[N];
	size_t Size;
public:
	message() : Size(0)
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
	size_t push(T& data) //Возвращает оставшееся место
	{
		if(sizeof(T) > getSpace())
		{
			return getSpace();
		}
		memcpy(getEnd(), &data, sizeof(T));
		Size += sizeof(T);
		return getSpace();
	}

	void clear()
	{
		Size = 0;
	}
};

#endif