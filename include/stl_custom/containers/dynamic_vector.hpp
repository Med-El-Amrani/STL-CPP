#pragma once
#include <cstddef>
#include <new>
#include <utility>
#include <type_traits>
#include <stdexcept>
#include "stl_custom/iterators/random_access_iterator.hpp"

namespace stl_custom {

template<typename T>
class dynamic_vector {
public:
	using iterator = random_access_iterator<T>;
	using const_iterator = random_access_iterator<const T>;

	dynamic_vector(): data_(nullptr), size_(0), capacity_(0) {}
	// copy constrcutor : deep-copy 
	dynamic_vector(const dynamic_vector& other) 
		: data_(nullptr), size_(0), capacity_(0) {
		reserve(other.size_);
		std::size_t i = 0;
		try{
		for(; i < other.size_; i++){
			::new (static_cast<void*>(data_ + i)) T(other.data_[i]);
		}
		} catch(...) {
			for(std::size_t j=0; j < i; ++j){ data_[j].~T(); }
			::operator delete(data_);
			throw;
		}
		size_ = other.size_;
	}
	// move constructor
	dynamic_vector(dynamic_vector&& other) noexcept 
		: data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
		other.data_ = nullptr;
		other.size_ = 0;
		other.capacity_ = 0;
	}

	// copy assignement 
	dynamic_vector& operator=(const dynamic_vector& other) {
		if(this == &other) return *this;
		dynamic_vector tmp(other);
		swap(tmp);
		return *this;
	}
	dynamic_vector& operator=(dynamic_vector&& other) noexcept {
		if(this==&other) return *this;
		destroy_all();
		::operator delete(data_);
		data_ = other.data_;
		size_ = other.size_;
		capacity_ = other.capacity_;

		other.data_ = nullptr;
		other.size_ = 0;
		other.capacity_ = 0;

		return *this;
	}

	// destructor : destroys all elements then frees raw storage
	~dynamic_vector() {
		destroy_all();
		::operator delete(data_);
	}

	// copy
	void push_back(const T& e) {
		ensure_capacity(size_ + 1);
		::new (static_cast<void*>(data_+size_)) T(e);
		++size_;
	}
	
	// move
	void push_back(T&& e) {
		ensure_capacity(size_ +1);
		::new (static_cast<void*>(data_+ size_)) T(std::move(e));
		++size_;
	}

	// construct directly in the container
	template<typename... Args>
	T& emplace_back(Args&&... args){
		ensure_capacity(size_ + 1);
		T* slot = ::new (static_cast<void*>(data_ + size_)) 
				T(std::forward<Args>(args)...);
		++size_;
		return *slot;
	}
	
	// destroys the last element and decerment the logical size
	void pop_back(){
		--size_;
		data_[size_].~T();
	}

	// destroys every element
	void clear() noexcept {
		destroy_all();
		size_ = 0;
	}

	// ensure capacity is at least new_cap
	void reserve(std::size_t cap){
		if(cap <= capacity_) return;
		reallocate(cap);
	}

	//unchecked element access
	T& operator[](std::size_t idx) {return data_[idx];}
	const T& operator[](std::size_t idx) const { return data_[idx];}

	//bounds-checked element access, throws std::out_of_range on invalid idx
	T& at(std::size_t idx) {
		if(idx >= size_) throw std::out_of_range("dynamic_vector::at");
		return data_[idx];
	}
	const T& at(std::size_t idx) const {
		if(idx >= size_) throw std::out_of_range("dynamic_vector::at");
		return data_[idx];
	}
	// utilities
	std::size_t size() const noexcept { return size_; }
	std::size_t capacity() const noexcept { return capacity_; }
	bool empty() const noexcept { return size_ == 0; }

	// iterators
	iterator begin() noexcept { return iterator(data_);  }
	iterator end() noexcept { return iterator(data_ + size_); }
	const_iterator begin() const noexcept { return const_iterator(data_); }
	const_iterator end() const noexcept { return const_iterator(data_ + size_); }
	const_iterator cbegin() const noexcept { return const_iterator(data_); }
	const_iterator cend() const noexcept { return const_iterator(data_ + size_); }
	
	void swap(dynamic_vector& other) noexcept {
		std::swap(data_, other.data_);
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
	}
private:
	T* data_;
	std::size_t size_;
	std::size_t capacity_;

	// grow storage (doubled) if needed to fit at least min_cap elements
	void ensure_capacity(std::size_t min_cap){
		if(min_cap <= capacity_) return;
		std::size_t new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
		if(new_cap < min_cap) new_cap = min_cap;
		reallocate(new_cap);
	}
	
	void reallocate(std::size_t new_cap){
		T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
		std::size_t constructed = 0;
		
		try {
		for(; constructed < size_ ; ++constructed){
			if constexpr (std::is_nothrow_move_constructible_v<T> ||
					!std::is_copy_constructible_v<T>) {
				::new (static_cast<void*>(new_data + constructed)) 
					T(std::move(data_[constructed]));
			} else {
				::new (static_cast<void*>(new_data + constructed))
					T(data_[constructed]);
			}
		}
		} catch(...) {
			for(std::size_t i=0; i < constructed; ++i) new_data[i].~T();
			::operator delete(new_data);
			throw;
		}
		
		destroy_all();
		::operator delete(data_);
		data_ = new_data;
		capacity_ = new_cap;
	}
	
	void destroy_all() noexcept {
		for(std::size_t i=0; i<size_; i++) data_[i].~T();
	}
};
} // stl_custom
