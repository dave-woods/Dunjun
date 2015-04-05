#ifndef DUNJUN_COMMON_HPP
#define DUNJUN_COMMON_HPP

#include <Dunjun/Types.hpp>

#include <cassert>
#include <functional>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <stdexcept>

#define GLOBAL static
#define INTERNAL static
#define LOCAL_PERSIST static

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace Dunjun
{
namespace
{
	template <class T, class... Args>
	INTERNAL std::unique_ptr<T> make_unique_helper(std::false_type, Args&&... args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}

	template <class T, class... Args>
	INTERNAL std::unique_ptr<T> make_unique_helper(std::true_type, Args&&... args)
	{
		static_assert(std::extent<T>::value == 0, "make_unique<T[N]>() is forbidden, please use make_unique<T[]>().");

		typedef typename std::remove_extent<T>::type U;

		return std::unique_ptr<T>(new U[sizeof...(Args)]{std::forward<Args>(args)...});
	}
} // namespace 

template <class T, class... Args>
inline std::unique_ptr<T> make_unique(Args&&... args)
{
	return make_unique_helper<T>(std::is_array<T>(), std::forward<Args>(args)...);
}

template<typename T, typename U>
inline T pseudo_cast(const U& from)
{
	T to = T(0);
	std::memcpy(&to, &from, (sizeof(T) < sizeof(U)) ? sizeof(T) : sizeof(U));
	return to;
}

} // namespace Dunjun

#endif
