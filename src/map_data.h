#ifndef MAP_DATA_H
#define MAP_DATA_H

#include <vector>
#include <stdint.h>
#include <cstdio>

template<typename T>
class MapData
{
protected:
	std::vector<uint8_t> data_;

public:
	// return 0 on success
	int read(FILE *f);

	int write(FILE *f) const;

	const T& operator[](size_t i) const;
	T& operator[](size_t i);
};

template<typename T>
int MapData<T>::read(FILE *f)
{
	uint32_t numElements = 0;
	int ret = fread(&numElements, 4, 1, f);
	if (ret != 1)
		return 1;

	data_.reserve(numElements);

	T element;
	for (uint32_t i = 0; !feof(f) && i < numElements; ++i)
	{
		ret = fread(&element, sizeof(element), 1, f);
		if (ret != 1)
			return 2;

		data_.push_back(element);
	}

	return data_.size() == numElements ? 0 : 3;
}

template<typename T>
int MapData<T>::write(FILE *f) const
{
	uint32_t size = static_cast<uint32_t>(data_.size());
	int ret = fwrite(&size, 4, 1, f);
	if (ret != 1)
		return 1;

	for (typename std::vector<T>::const_iterator i = data_.begin(); i != data_.end(); ++i)
	{
		ret = fwrite(&data_[i], sizeof(T), 1, f);
		if (ret != 1)
			return 2;
	}

	return 0;
}

template<typename T>
const T& MapData<T>::operator[](size_t i) const
{
	return data_[i];
}

template<typename T>
T& MapData<T>::operator[](size_t i)
{
	return data_[i];
}

#endif

