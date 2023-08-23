// Sequence.h - Time series data class
//
// Alex Gascoigne, August 2023

#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <initializer_list>

template <class T>
class Sequence
{
public:

	Sequence(std::initializer_list<T> list, long _offset = 0)
	{
		offset = _offset;
		data.resize(list.size());
		std::copy(std::begin(list), std::end(list), data.begin());
	}

	// Create a sequence of length n, with all values v
	Sequence(std::size_t n, T v=T(0), long _offset = 0) {
		assign(n, v, _offset);
	}

	Sequence(std::vector<T> _data, long _offset=0) : data(_data), offset(_offset) { };

	Sequence& assign(std::size_t n, T v = T(0), long _offset = 0)
	{
		data.assign(n, v);
		offset = _offset;
		return *this;
	}

	// operator[] access element based on value of n relative to offset
	T& operator[](long pos)
	{
		return data[pos - offset];
	}

	// Flip a sequence structure, y = x[-n]
	Sequence &flip() {
		std::reverse(data.begin(), data.end());
		offset = -(offset + static_cast<long>(data.size()) - 1);
		return *this;
	}

	// Shift a sequence structure x by integer amount n0,
	// y[n] = x[n - n0]
	Sequence &shift(int n0) {
		offset = offset+n0;
		return *this;
	}

	// Remove zeros from beginning and end of sequence
	Sequence& trim() {
		auto ztest = [](auto& x) { return x != 0; };
		auto f = std::find_if(data.begin(), data.end(), ztest);
		if (f == data.end()) {
			return *this;
		}
		offset += static_cast<long>(std::distance(data.begin(), f));
		data.erase(data.begin(), f);
		auto e = std::find_if(data.rbegin(), data.rend(), ztest);
		if (e == data.rend()) {
			return *this;
		}
		auto ef = e.base(); // create forward iterator
		data.erase(ef, data.end());
		return *this;
	}

	Sequence() : offset(0) {};
	~Sequence() = default;
	Sequence(Sequence const& rhs) = default;
	Sequence& operator=(Sequence const& rhs) = default;

	Sequence(Sequence&& rhs) = default;
	Sequence& operator=(Sequence&& rhs) = default;

	std::vector<T> data;
	long offset; // the n value of first entry, may be negative (size_t will not work here)

};

// Zero pad sequences to be the same length
template <class T>
void extend(Sequence<T>& x, Sequence<T>& y)
{
	long left = static_cast<long>(std::min(x.offset, y.offset));
	long right = static_cast<long>(std::max(x.offset + static_cast<long>(x.data.size()) - 1,
											y.offset + static_cast<long>(y.data.size()) - 1));

	long xladd = x.offset - left;
	if (xladd > 0)
		x.data.insert(x.data.begin(), xladd, T(0));
	long xradd = right - (x.offset + static_cast<long>(x.data.size()) - 1);
	if (xradd > 0)
		x.data.insert(x.data.end(), xradd, T(0));

	long yladd = y.offset - left;
	if (yladd > 0) 
		y.data.insert(y.data.begin(), yladd, T(0));
	long yradd = right - (y.offset + static_cast<long>(y.data.size()) - 1);
	if (yradd > 0)
		y.data.insert(y.data.end(), yradd, T(0));
}

template<class T, class F_bi>
Sequence<T> elementwise(const Sequence<T> &x, const Sequence<T>& y)
{
	auto left = std::min(x.offset, y.offset);
	auto right = std::max(x.offset + x.data.size() - 1, y.offset + y.data.size() - 1);
	Sequence<T> sresult(right - left + 1);

	auto& result = sresult.data;

	// copy x to result
	std::copy(x.data.begin(), x.data.end(), result.begin()+(x.offset-left));

	// do operation on result, y
	auto result_off = y.offset - left;

	std::transform(result.begin()+result_off,
				   result.begin()+(result_off + y.data.size()),
		           y.data.begin(), result.begin()+result_off, F_bi());

	return sresult.trim();
}

// Add x and y.
template <class T>
inline Sequence<T> operator+(const Sequence<T>& x, const Sequence<T>& y)
{
	return elementwise<T, std::plus<T>>(x, y);
}

// Substract x and y
template <class T>
inline Sequence<T> operator-(const Sequence<T> &x, const Sequence<T> &y)
{
	return elementwise<T, std::minus<T>>(x, y);
}

// Elementwise multiplication of x and y
template <class T>
inline Sequence<T> operator*(const Sequence<T>& x, const Sequence<T>& y)
{
	return elementwise<T, std::multiplies<T>>(x, y);
}

// Convolve x and h, returning sequence y
template <class T>
Sequence<T> conv(const Sequence<T>& x, const Sequence<T>& y)
{
	int N = x.data.size();
	int M = y.data.size();
	int P = M + N - 1;

	Sequence<T> result(P, T(0));

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
			result[i + j] += x.data[i] * y.data[j];
	return result;
}

template <class T>
std::ostream& operator<<(std::ostream& s, const Sequence<T>& x)
{
	for (auto it = x.data.begin(); it != x.data.end(); ++it) {
		s << *it << " ";
	}
	s << "offset: " << x.offset << std::endl;
	return s;
}

template <class T>
std::istream& operator>>(std::istream& s, Sequence<T>& x)
{
	// size offset data
	int size;
	int offset;

	s >> size;
	s >> offset;
	x.data.resize(size);
	x.offset = offset;

	auto it = x.data.begin();
	while (size--) {
		s >> *it;
		it++;
	}

	return s;
}
