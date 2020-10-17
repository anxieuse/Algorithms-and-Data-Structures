#pragma once

template <class T>
class TVector
{
public:
	TVector();
	TVector(const TVector &v);
	TVector(const size_t n, const T &val = T());
	~TVector();

	void PushBack(const T &val);
	void Erase(const size_t index);
	void Resize(const size_t n, const T &val = T());
	void Clear();
	size_t Size() const;
	bool Empty() const;

	T &operator[](const size_t i);
	TVector &operator=(const TVector &v);

private:
	T *begin;
	size_t size;
	size_t cap;

	void Copy(const TVector &v);
};

template <class T>
TVector<T>::TVector()
{
	begin = new T[1];
	size = 0;
	cap = 1;
}

template <class T>
TVector<T>::TVector(const TVector &v)
{
	begin = new T[1];
	size = 0;
	cap = 1;

	if (this != &v)
		Copy(v);
}

template <class T>
TVector<T>::TVector(const size_t n, const T &val)
{
	begin = new T[n + 1];

	for (size_t i = 0; i < n; ++i)
		begin[i] = val;

	size = n;
	cap = n + 1;
}

template <class T>
TVector<T>::~TVector()
{
	delete[] begin;
}

template <class T>
void TVector<T>::PushBack(const T &val)
{
	if (size == cap)
	{
		size_t oldSize = size;

		Resize(size * 2);

		size = oldSize;
	}

	begin[size++] = val;
}

template <class T>
void TVector<T>::Erase(const size_t index)
{
	for (size_t i = index; i < size - 1; ++i)
		begin[i] = begin[i + 1];

	size--;
}

template <class T>
void TVector<T>::Resize(const size_t n, const T &val)
{
	const size_t copySize = n < size ? n : size;
	T *_buffer = new T[n + 1];

	for (size_t i = 0; i < copySize; ++i)
		_buffer[i] = begin[i];

	for (size_t i = copySize; i < n; ++i)
		_buffer[i] = val;

	delete[] begin;

	begin = _buffer;
	size = n;
	cap = n + 1;
}

template <class T>
void TVector<T>::Clear()
{
	delete[] begin;

	begin = new T[1];
	size = 0;
	cap = 1;
}

template <class T>
size_t TVector<T>::Size() const
{
	return size;
}

template <class T>
bool TVector<T>::Empty() const
{
	return size == 0;
}

template <class T>
T &TVector<T>::operator[](const size_t i)
{
	return begin[i];
}

template <class T>
TVector<T> &TVector<T>::operator=(const TVector &v)
{
	if (this != &v)
		Copy(v);

	return *this;
}

template <class T>
void TVector<T>::Copy(const TVector &v)
{
	const size_t n = v.Size();

	delete[] begin;

	begin = new T[n + 1];

	for (size_t i = 0; i < n; ++i)
		begin[i] = v.begin[i];

	size = n;
	cap = n + 1;
}