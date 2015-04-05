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
		:data{}
	{
	}

	ReadOnly(const T& t)
		:data{ t }
	{
	}

	ReadOnly(const T&& t)
		:data{ std::move(t) }
	{
	}

	ReadOnly& operator=(const T& t)
	{
		data = t;
		return *this;
	}

	T* operator&() { return &data; }

	template <class U>
	U& operator*()
	{
		return *data;
	}
	
	bool operator==(const T& t) const { return data == t; }
	bool operator!=(const T& t) const { return !operator==(t); }

	T data;
};

#endif // !DUNJUN_READONLY_HPP
