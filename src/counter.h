#ifndef __COUNTER
#define __COUNTER

#include <list>

template <class T = float>
class Counter
{
public:
	Counter() : Counter(10)
	{}

	Counter(size_t max_size)
	{
		this->max_size = max_size;
	}

	T getAverage() const
	{
		T avg = 0;
		for(auto it=values.begin(); it!=values.end(); ++it)
			avg += *it;
		avg = avg / values.size();
		return avg;
	}

	void push(T v)
	{
		values.push_back(v);
		if(values.size() >= max_size)
			values.pop_front();
	}

private:
	std::list<T> values;
	size_t max_size;
};

#endif
