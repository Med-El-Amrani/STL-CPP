#pragma once
#include <cstddef>
#include <stdexcept>
#include <new>
#include <utility>
#include <type_traits>

namespace stl_custom {

enum class overflow_policy { reject, overwrite_oldest };

template<typename T, std::size_t N, overflow_policy Policy = overflow_policy::reject>
class ring_buffer {
	static_assert(N>0, "ring_buffer : size of buffer shall be greater than zero");
public:
	ring_buffer() = default;
 	~ring_buffer() {
		clear();
	}
	// copy constructor
	ring_buffer(const ring_buffer& other) {
		std::size_t i = 0;
		try{
			for(; i < other.size_; i++){
				T* src = std::launder(reinterpret_cast<T*>(&other.data_[(other.head_ + i)%N]));
				::new (static_cast<void*>(&data_[i])) T(*src);
			}
		} catch(...) {
			for(std::size_t j=0; j < i; j++){
				reinterpret_cast<T*>(&data_[j])->~T();
			}
			throw;
		}
                size_ = other.size_;
                head_ = 0;
                tail_ = size_ % N;

	}
	// copy assignement operator
	ring_buffer& operator=(const ring_buffer& other){
		
		if(this != &other) {
			clear();
			size_ = 0; head_ = 0; tail_ = 0; // safe empty state
			std::size_t i = 0;
			try{
				for(; i < other.size_; i++){
                        		T* src = std::launder(reinterpret_cast<T*>(&other.data_[(other.head_ + i)%N]));
                       			::new (static_cast<void*>(&data_[i])) T(*src);
                		}
			} catch(...){
				for(std::size_t j = 0; j < i; j++) reinterpret_cast<T*>(&data_[j])->~T();
				throw;
			}
                	size_ = other.size_;
                	head_ = 0;
                	tail_ = size_ % N;
		}
		return *this;
	}
	// move constructor 
	ring_buffer(ring_buffer&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
               for(std::size_t i = 0; i < other.size_; i++){
                        T* src = std::launder(reinterpret_cast<T*>(&other.data_[(other.head_+i)%N]));
                        ::new (static_cast<void*>(&data_[i])) T(std::move(*src));
                        src->~T();
               }
               size_ = other.size_;
               head_ = 0;
               tail_ = size_ % N; 
                        
               other.size_ = 0;
               other.head_ = 0;
               other.tail_ = 0;
	}
	// move assingement operator
	ring_buffer& operator=(ring_buffer&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
		if(this!= &other){
			clear();
	                for(std::size_t i = 0; i < other.size_; i++){
        	                T* src = std::launder(reinterpret_cast<T*>(&other.data_[(other.head_+i)%N]));
                	        ::new (static_cast<void*>(&data_[i])) T(std::move(*src));
				src->~T();
      		        }
                	size_ = other.size_;
               	 	head_ = 0;
                	tail_ = size_ % N;
			
			other.size_ = 0;
			other.head_ = 0;
			other.tail_ = 0;
		}
		return *this;
	}
	
	// modifiers
	void push_back(const T& other){
		if constexpr(Policy == overflow_policy::reject){
			if(size_ == N){
				throw std::overflow_error("ring_buffer::push_back full buffer");
			}
			::new (static_cast<void*>(&data_[tail_])) T(other);
			tail_ = (tail_ + 1)%N;
			++size_;
		} else {
			if(size_ == N){
				reinterpret_cast<T*>(&data_[tail_])->~T(); // destroy evicted element
			}
			::new (static_cast<void*>(&data_[tail_])) T(other);
			if(size_ == N) head_ = (head_ + 1)%N;
			tail_ = (tail_ + 1)%N;
			if(size_ < N) ++size_;
		}
	}
	void push_back(T&& other){
                if constexpr(Policy == overflow_policy::reject){
                        if(size_ == N){
                                throw std::overflow_error("ring_buffer::emplace_back full buffer");
                        }
                        ::new (static_cast<void*>(&data_[tail_])) T(std::move(other));
                        tail_ = (tail_ + 1)%N;
                        ++size_;
                } else {
                        if(size_ == N){
                                reinterpret_cast<T*>(&data_[tail_])->~T(); // destroy evicted element
                        }
                        ::new (static_cast<void*>(&data_[tail_])) T(std::move(other));
                        if(size_ == N) head_ = (head_ + 1)%N;
                        tail_ = (tail_ + 1)%N;
                        if(size_ < N) ++size_;
                }
        }

	
	template<typename... Args>
	T& emplace_back(Args&&... args){
		T* result;
		if constexpr(Policy == overflow_policy::reject){
                        if(size_ == N){
                                throw std::overflow_error("ring_buffer::push_back full buffer");
                        }
                        result = ::new (static_cast<void*>(&data_[tail_])) T(std::forward<Args>(args)...);
                        tail_ = (tail_ + 1)%N;
                        ++size_;
                } else {
                        if(size_ == N){
                                reinterpret_cast<T*>(&data_[tail_])->~T(); // destroy evicted element
                        }
                        result = ::new (static_cast<void*>(&data_[tail_])) T(std::forward<Args>(args)...);
                        if(size_ == N) head_ = (head_ + 1)%N;
                        tail_ = (tail_ + 1)%N;
                        if(size_ < N) ++size_;
                }
		return *result;
	}


	void pop_front(){
		if(size_ == 0) throw std::underflow_error("ring_buffer::pop_front empty buffer");
		reinterpret_cast<T*>(&data_[head_])->~T();
		head_ = (head_ + 1)%N;
		--size_;
	}

	void swap(ring_buffer& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
		if(this == &other) return;
	
		ring_buffer tmp(std::move(*this));
		*this = std::move(other);
		other = std::move(tmp);
	}

	void clear() noexcept {
		for(std::size_t i=0; i<size_; i++){
			reinterpret_cast<T*>(&data_[(head_ + i)%N])->~T();
		}
	}

	// accessers
	T& front(){
		if(size_ == 0) throw std::underflow_error("ring_buffer::front empty buffer");
		return *(reinterpret_cast<T*>(&data_[head_]));
	}
	const T& front() const {
		if(size_ == 0) throw std::underflow_error("ring_buffer::front empty buffer");
                return *(reinterpret_cast<T*>(&data_[head_]));
	}

	T& back() {
                if(size_ == 0) throw std::underflow_error("ring_buffer::back empty buffer");
                return *(reinterpret_cast<T*>(&data_[(tail_ + N - 1)%N]));
	}
	const T& back() const {
                if(size_ == 0) throw std::underflow_error("ring_buffer::back empty buffer");
                return *(reinterpret_cast<T*>(&data_[(tail_ + N -1)%N]));
	}

	bool empty() const noexcept { return size_ == 0;}
	bool full() const noexcept { return size_ == N; }
	std::size_t size() const noexcept { return size_; };
	std::size_t capacity() const noexcept { return N; }


private:
	struct alignas(alignof(T)) Slot {
		unsigned char bytes[sizeof(T)];
	};
	
	Slot data_[N];
	std::size_t head_ = 0;
	std::size_t tail_ = 0;
	std::size_t size_ = 0;
};

}

