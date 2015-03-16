#ifndef DUNJUN_READONLY_HPP
#define DUNJUN_READONLY_HPP



template <class T, class Super>
class ReadOnly
{
public:
	operator const T&() const
	{
		return data;
	}
private:
	friend Super;

	ReadOnly()
		:data()
	{
	}

	ReadOnly(const T& t)
		:data(t)
	{
	}

	ReadOnly& operator=(const T& t)
	{
		data = t;
		return *this;
	}

	T* operator&() { return &data; }

	T data;
};

#endif // !DUNJUN_READONLY_HPP
