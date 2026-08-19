#pragma once
#include <new>
#include <optional>
#include <array>
#include <utility>

namespace stl_custom{

template<typename T, std::size_t N>
class ObjectPool{
public:
	class Handle{
	public:
		Handle(): pool_(nullptr), index_(npos) {}
		Handle(const Handle&) = delete;
		Handle& operator=(const Handle&) = delete;
		Handle(Handle&& other): pool_(other.pool_), index_(other.index_) {
			other.pool_ = nullptr;
			other.index_ = npos;
		}
		Handle& operator=(Handle&& other){
		if(this != &other){
		pool_->destroyAt(index_);
		pool_ = other.pool_;
		index_=other.index_;
		other.pool_ = nullptr;
		other.index_ = npos;
		}
		return *this;
		}
		~Handle(){
		if(pool_ && index_!=npos){
			pool_->destroyAt(index_);
			pool_ = nullptr; index_=npos;
		}
		}
		T& operator*(){
			return *get();
		}
		T* operator->() {
			return get();
		}
	private:
		friend class ObjectPool;
		Handle(ObjectPool* pool, std::size_t index): pool_(pool), index_(index) {}
		T* get() { return pool_->slotPtr(index_); }
		static constexpr std::size_t npos=static_cast<std::size_t>(-1);
		ObjectPool* pool_;
		std::size_t index_;
	};
	ObjectPool() = default;
	ObjectPool(const ObjectPool&) = delete;
	ObjectPool& operator=(const ObjectPool&) = delete;
	ObjectPool(ObjectPool&&) = delete;
	ObjectPool& operator=(ObjectPool&&) = delete;

	~ObjectPool(){
		for(std::size_t i = 0; i< N; ++i)
			if(occupied_[i]) destroyAt(i);
	}

	template<typename... Args>
	std::optional<Handle> acquire(Args&&... args){
		for(std::size_t i=0 ; i < N; i++){
		if(!occupied_[i]){
		::new (static_cast<void*>(&storage_[i])) T(std::forward<Args>(args)...);
		occupied_[i] = true;
		++liveCount_;
		return Handle(this, i);
		}
		}
		return std::nullopt;
	}

	std::size_t liveCount() const {return liveCount_;}
	static constexpr std::size_t capacity() {return N;}
private:
	T* slotPtr(std::size_t i){
		return std::launder(reinterpret_cast<T*>(&storage_[i]));
	}
	void destroyAt(std::size_t i){
		slotPtr(i)->~T();
		occupied_[i] = false;
		--liveCount_;
	}

	struct alignas(alignof(T)) Slot {
		unsigned char bytes[sizeof(T)];
	};

	std::array<Slot, N> storage_{};
	std::array<bool, N> occupied_{};
	std::size_t liveCount_ = 0;
};
}