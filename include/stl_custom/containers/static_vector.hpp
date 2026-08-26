#pragma once
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include "stl_custom/iterators/random_access_iterator.hpp"
#include <type_traits>

namespace stl_custom {

template<typename T, std::size_t N>
class static_vector {
	static_assert(N > 0 , "static_vector shall have positive size");
public:
	using iterator = random_access_iterator<T>;
	using const_iterator = random_access_iterator<const T>;

	static_vector() {}
	static_vector(const std::initializer_list<T>& list) {
		std::size_t i = 0;
		for( const auto& e: list){
			if(i >= N) break;
			::new (static_cast<void*>(data_[i].bytes)) T(e);
			++size_;
			++i;
		}
	}
	//copy constructor
	static_vector(const static_vector& other) {
		for(std::size_t i=0; i < other.size() && i< N; ++i){
			const T* ptr = std::launder(reinterpret_cast<const T*>(other.data_[i].bytes));
			::new (static_cast<void*>(data_[i].bytes)) T(*ptr);
			++size_;
		}
	}
	
	// move constructor
	static_vector(static_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
		for(std::size_t i=0; i < other.size() && i < N; ++i) {
			T* src = std::launder(reinterpret_cast<T*>(other.data_[i].bytes));
			::new (static_cast<void*>(data_[i].bytes)) T(std::move(*src));
			src->~T();
			++size_;
		}
		other.size_ = 0;
	}
	// copy assignement operator
	static_vector& operator=(const static_vector& other) {
		if(this == &other) return *this;
		for(std::size_t i = 0; i < size_; ++i) {
			std::launder(reinterpret_cast<T*>(data_[i].bytes))->~T();
		}
		size_ = 0;
                for(std::size_t i=0; i < other.size() && i< N; ++i){
                        const T* ptr = std::launder(reinterpret_cast<const T*>(other.data_[i].bytes));
                        ::new (static_cast<void*>(data_[i].bytes)) T(*ptr);
                        ++size_;
                }
		return *this;
	}
	
	// move assignement operator
	static_vector& operator=(static_vector&& other){
		if(this != &other){
                for(std::size_t i = 0; i < size_; ++i) {
                        std::launder(reinterpret_cast<T*>(data_[i].bytes))->~T();
                }
                size_ = 0;
                for(std::size_t i=0; i < other.size() && i< N; ++i){
                        T* ptr = std::launder(reinterpret_cast<T*>(other.data_[i].bytes));
                        ::new (static_cast<void*>(data_[i].bytes)) T(std::move(*ptr));
			ptr->~T();
                        ++size_;
                }

		}
		return *this;
	}

	~static_vector() {
		for(std::size_t i=0; i < size_; ++i) {
			T* ptr = std::launder(reinterpret_cast<T*>(data_[i].bytes));
			ptr->~T();
		}
	}

	void push_back(const T& element) {
		if(size_ == N) throw std::length_error("buffer is full");
		::new (static_cast<void*>(data_[size_].bytes)) T(element);
		++size_;
	}

	void push_back(T&& element){
		if(size_ == N) throw std::length_error("buffer is full");
		::new (static_cast<void*>(data_[size_].bytes)) T(std::move(element));
		++size_;
	}

	
	template<typename... Args>
	T& emplace_back(Args&&... args) {
		if(size_ == N) throw std::length_error("buffer is full");
		T* new_element = ::new (static_cast<void*>(data_[size_].bytes)) T(std::forward<Args>(args)...);
		++size_;
		return *new_element;
	}

	void pop_back(){
		--size_;
		T* ptr = std::launder(reinterpret_cast<T*>(data_[size_].bytes));
		ptr->~T();
	}
	
	std::size_t capacity() const noexcept { return N; }
	std::size_t size() const noexcept { return size_; }
	bool empty() const noexcept { return size_ == 0;}

	iterator begin() noexcept { return iterator(std::launder(reinterpret_cast<T*>(data_))); }
	iterator end() noexcept { return iterator(std::launder(reinterpret_cast<T*>(data_ + size_))); }
	const_iterator begin() const noexcept { return const_iterator(std::launder(reinterpret_cast<const T*>(data_))); }
	const_iterator end() const noexcept { return const_iterator(std::launder(reinterpret_cast<const T*>(data_ + size_))); }
	const_iterator cbegin() const noexcept { return const_iterator(std::launder(reinterpret_cast<const T*>(data_))); }
	const_iterator cend() const noexcept { return const_iterator(std::launder(reinterpret_cast<const T*>(data_ + size_))); }
	
	// unchecked random access
	T& operator[](std::size_t idx){
		return *std::launder(reinterpret_cast<T*>(data_[idx].bytes));
	}
	const T& operator[](std::size_t idx) const {
                return *std::launder(reinterpret_cast<const T*>(data_[idx].bytes));
	}
	// bouds-checked random access operator
	T& at(std::size_t idx) { 
		if(idx >= size_) throw std::out_of_range("static_vector::at");
                return *std::launder(reinterpret_cast<T*>(data_[idx].bytes));
	}
	const T& at(std::size_t idx) const {
		if(idx >= size_) throw std::out_of_range("static_vector::at");
                return *std::launder(reinterpret_cast<const T*>(data_[idx].bytes));
	}
	
private:
	struct alignas(alignof(T)) Slot{ unsigned char bytes[sizeof(T)]; };
	Slot data_[N];
	std::size_t size_ = 0;
};

}
