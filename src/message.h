#if !defined MESSAGE_H
#define MESSAGE_H

template <size_t N>
class message
{
	uint8_t array[N];
	size_t Size;
public:
	message() : pointer(0)
	{}
	size_t getSize()
	{
		return Size;
	}
	size_t getCapacity()
	{
		return N;
	}
	size_t getSpace()
	{
		return N - Size;
	}
	uint8_t* getData()
	{
		return Data;
	}
	size_t push(uint8_t* data, size_t size)
	{
		if(sizeof(T) > getSpace())
		{
			return getSpace();
		}
		for(size_t i = 0; i < size; ++i)
		{
			array[Size] = data[i];
			++Size;
		}
		return getSpace();
	}
	template <typename T>
	size_t push(T& data) //Возвращает оставшееся место
	{
		if(sizeof(T) > getSpace())
		{
			return getSpace();
		}
		(T*)(array + Size) = data;
		return getSpace();
	}
};

#endif