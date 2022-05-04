#pragma once
#include <new>
#include <chrono>
#include <gsl/gsl>
using namespace std::chrono_literals;
#if !_DEBUG
#include <stdio.h>
#define LOG_INFO(...) 
#define STRINGIFY_TOCKEN(TOCKEN) #TOCKEN
#define STRINGIFY_VALUE(EXPRESSION) STRINGIFY_TOCKEN(STRINGIFY_VALUE)
#define CHECK(...) for(bool v =!(__VA_ARGS__);v;v = false)printf("%s\n", STRINGIFY_VALUE(__VA_ARGS__))
#else
#include <assert.h>
#define LOG_INFO(...)
#define CHECK assert
#endif



// DEFINE_SUBTYPE_CHECKER_METAFUNCTION macro makes available template type METAFUNCTION_NAME, that can be used to detect if SUBTYPE_CHECK_FOR is defined in type T.
//
// code example:
//
//	DEFINE_SUBTYPE_CHECKER_METAFUNCTION( has_deleter_type, deleter_type );
// 
// class A {
// public:
//		using deleter_type = std::default_delete<B>; // not defined at A
// };
// 
// class B {};
// 
// 
// static_assert( has_deleter_type_v< A > ); // OK, nothing wrong
// static_assert( has_deleter_type_v< B >, "deleter is not defined" ); // ERROR, "deleter is not defined", because it is really is not defined
#define DEFINE_SUBTYPE_CHECKER_METAFUNCTION(METAFUNCTION_NAME, SUBTYPE_CHECK_FOR)					\
	template <typename T, typename = void>struct METAFUNCTION_NAME : public std::false_type {};		\
	template <typename T> struct METAFUNCTION_NAME<T, std::void_t<typename T::SUBTYPE_CHECK_FOR>> :	\
		public std::true_type { using type = typename T::SUBTYPE_CHECK_FOR; };						\
	template<typename T> constexpr bool METAFUNCTION_NAME##_v = METAFUNCTION_NAME<T>::value;		// All that stuff (under macro) is SFINAE

namespace std{ // C++ 17 hasn't std::span, but C++ 20 has. To hide that difference that trick is used
	using gsl::span;
}


//deffered gives an ability to defferedly create objects.
//
// Conceptually it is similar to std::optional, except the fact that object lifetime (including ctor & dtor call) completely have to be managed by it's user.
// 
// Main purpose of that class is to be a preallocated storage for objects of type T - that is most useful, for example, if we have an array of objects that are not default constructible.
// Lets say, that type T is not default constructible (!reasonably!). So, without deferred<T>, we could create an array of T only in two ways:
// 
// Way 1 - Create array of pointers to objects on heap, as follows:
// 
//	std::array<T*, Count> objects; // conceptually, it doesn't matter if T a raw pointer (such as T*) or a smart pointer (such as std::unique_ptr<T>)
// 
//	void init_objects( ... ) {
//		for ( auto& object_pointer : objects )
//			object_pointer = new T( ... ); // or std::make_unique<T>( ... ), or std::make_shared<T>( ... ) e.t.c.
//	}
// 
// Way 2 - Create a dynamic container, as follows:
// 
//	std::vector<T> objects;
//	
//	void init_objects( ... ) {
//		for ( ... )
//			objects.emplace_back(...);
//	}
// 
// Notice: both of these alternatives (1,2) are using dynamic memory allocation.
// 
// But... Why don't to do so:
//
//	std::array<deferred<T>, Count> objects; // No dynamic memory allocation here ...
// 
//	void init_objects( ... ) {
//		for ( auto &object : objects )
//			object.create( ... );			// .. and here also no dynamic memory allocation.
//	}
// 
// ? - Notice, last case uses COMPLETLY NO dynamic memory allocations.
// And it could be preferrable when completly no need using dynamic memory allocations
template<typename T>
class deferred {
public:
	deferred()noexcept;
	template<typename...Ts>
	T& create(Ts&&...as)noexcept {
		return *new(&instance_) T(std::forward<Ts>(as)...);
	}
	void destroy()noexcept {
		reinterpret_cast<T&>(instance_).~T();
	}
	// as_pointer - use case:
	// std::array<T, N> a;
	// method(a.data(), a.size()); // OK
	//
	// std::array<deferred<T>, N> a;
	// method(a.data()->as_pointer(), a.size()); // OK
	T* as_pointer()noexcept {
		return reinterpret_cast<T*>(&instance_);
	}
	const T* as_pointer()const noexcept {
		return const_cast<deferred*>(this)->as_pointer();
	}
	T& instance()noexcept {
		return *as_pointer();
	}
	const T& instance()const noexcept {
		return *as_pointer();
	}
	T* operator->()noexcept {
		return as_pointer();
	}
	const T* operator->()const noexcept {
		return as_pointer();
	}
	T& operator*()noexcept {
		return instance();
	}
	const T& operator*()const noexcept {
		return instance();
	}

private:
	std::aligned_storage_t<sizeof(T), alignof(T)> instance_;
private:
	deferred(const deferred&) = delete;
	deferred(deferred&&) = delete;
};
template<typename T>
deferred<T>::deferred()noexcept {
	static_assert((sizeof(deferred<T>) == sizeof(T)) && (alignof(deferred<T>) == alignof(T)));
}

