/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
	MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
	~MinHeap() {}

	void clear() { _data.clear(); }

	// For the following member functions,
	// We don't respond for the case vector "_data" is empty!
	const Data& operator [] (size_t i) const { return _data[i]; }   
	Data& operator [] (size_t i) { return _data[i]; }

	size_t size() const { return _data.size(); }

	// TODO
	const Data& min() const { return _data[0]; }
	void insert(const Data& d)
	{
		_data.push_back(d);
		size_t t = _data.size();
		while(t > 1)
		{
			size_t p = t / 2;
			if(!(d < _data[p - 1]))
				break;
			_data[t - 1] = _data[p - 1];
			t = p;
		}
		_data[t - 1] = d;
	}
	void delMin() { delData(0); }
	void delData(size_t i)
	{
		size_t t = i + 1, p = 2 * t;
		while(p <= _data.size())
		{
			if(p < _data.size())		//have right side brother
				if(_data[p] < _data[p - 1])	// right < left
					++p;
			if(!(_data[p - 1] < _data.back()))
				break;
			_data[t - 1] = _data[p - 1];
			t = p;
			p = t * 2;
		}
		Data& d = _data.back();
		while(t > 1)
		{
			p = t / 2;
			if(!(d < _data[p - 1]))
				break;
			_data[t - 1] = _data[p - 1];
			t = p;
		}
		_data[t - 1] = d;
		_data.pop_back();
	}

private:
	// DO NOT add or change data members
	vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
