#pragma once
#include <cstddef>
#include <utility>
#include <functional>
#include <tuple>
#include <stdexcept>
#include "stl_custom/containers/doubly_list.hpp"
#include "stl_custom/containers/dynamic_vector.hpp"

namespace stl_custom {

template<typename Key, typename Value, typename Hash = std::hash<Key>, typename KeyEqual=std::equal_to<Key>>
class hash_map{
public:
	using value_type = std::pair<const Key, Value>;
	
	explicit hash_map(std::size_t initial_bucket_count = 16)
		: buckets_(initial_bucket_count == 0 ? 1 : initial_bucket_count) {}

	~hash_map() = default;
	// std::vector<doubly_list<value_type>>'s own copy/move constructor delegates
	// element-wise to doubly_list's copy/move constructor, so defaulting these is coerrect here

	hash_map(const hash_map& other) = default;
	hash_map& operator=(const hash_map& other) = default;
	hash_map(hash_map&& other) noexcept = default;
	hash_map& operator=(hash_map&& other) noexcept = default;
	
	void swap(hash_map& other) noexcept {
		buckets_.swap(other.buckets_);
		std::swap(size_, other.size_);
		std::swap(hash_, other.hash_);
		std::swap(key_equal_, other.key_equal_);
		std::swap(max_load_factor_, other.max_load_factor_);
	}
	// modifiers
	std::pair<Value*, bool> insert(const Key& key, const Value& value) {
		std::size_t idx = bucket_index(key, buckets_.size());
		for(auto& kv: buckets_[idx]){
			if(key_equal_(kv.first, key)){
				return {&kv.second, false};
			}
		}
		maybe_grow();
		idx = bucket_index(key, buckets_.size()); // bucket_count may have changed
		buckets_[idx].push_back(value_type(key, value));
		++size_;
		return {&buckets_[idx].back().second, true};
	}
	
	std::pair<Value*, bool> insert(const Key& key, Value&& value){
		std::size_t idx = bucket_index(key, buckets_.size());
		for(auto& kv: buckets_[idx]){
			if(key_equal_(kv.first, key)){
				return {&kv.second, false};
			}
		}
		maybe_grow();
		idx = bucket_index(key, buckets_.size()); // buckets_.size() may have changed
		buckets_[idx].push_back({key, std::move(value)});
		++size_;
		return {&buckets_[idx].back().second, true};
	}
	
	template<typename... Args>
	std::pair<Value*, bool> emplace(const Key& key, Args&&... args){
		std::size_t idx = bucket_index(key, buckets_.size());
		for(auto& kv: buckets_[idx]){
			if(key_equal_(kv.first, key)){
				return {&kv.second, false};
			}
		}
		maybe_grow();
		idx = bucket_index(key, buckets_.size());
		Value& ref = buckets_[idx].emplace_back(
			std::piecewise_construct,
			std::forward_as_tuple(key),
			std::forward_as_tuple(std::forward<Args>(args)...)
		).second;
		++size_;
		return {&ref, true};
	}
	bool erase(const Key& key) {
		std::size_t idx = bucket_index(key, buckets_.size());
		for(auto it=buckets_[idx].begin(); it!=buckets_[idx].end(); ++it){
			if(key_equal_(it->first, key)){
				buckets_[idx].erase(it);
				--size_;
				return true;
			}
		}
		return false;
	}
	void clear() noexcept {
		for(auto& bucket: buckets_){
			bucket.clear();
		}
		size_ = 0;
	}
	Value& operator[](const Key& key){ // insert-default-if-missing
		auto [ptr, inserted] = insert(key, Value());
		return *ptr;
	}
	// lookup
	Value* find(const Key& key){
		std::size_t idx = bucket_index(key, buckets_.size());
		for(auto& kv: buckets_[idx]){
			if(key_equal_(kv.first,key)){
				return &kv.second;
			}
		}
		return nullptr;
	}
	const Value* find(const Key& key) const {
                std::size_t idx = bucket_index(key, buckets_.size());
                for(const auto& kv: buckets_[idx]){
                        if(key_equal_(kv.first,key)){
                                return &kv.second;
                        }
                }
                return nullptr;
	}
	bool contains(const Key& key) const {
               return find(key) != nullptr;
	}
	Value& at(const Key& key){
		Value* v = find(key);
		if(v == nullptr) throw std::out_of_range("hash_map::at key not found");
		return *v;
	}
	const Value& at(const Key& key) const {
		const Value* v = find(key);
		if( v == nullptr) throw std::out_of_range("hash_map::at key not found");
		return *v;
	}

	// accessors
	bool empty() const noexcept { return size_ == 0; }
	std::size_t size() const noexcept { return size_; }
	std::size_t bucket_count() const noexcept { return buckets_.size(); }
	float load_factor() const noexcept {
		return static_cast<float>(size_) / static_cast<float>(buckets_.size());
	}
	
	float max_load_factor() const noexcept { return max_load_factor_; }
	void max_load_factor(float ml) { max_load_factor_ = ml; }

	void rehash(std::size_t new_bucket_count){
		if(new_bucket_count <= buckets_.size()) return;
		dynamic_vector<doubly_list<value_type>> new_buckets(new_bucket_count);
		for(auto& bucket : buckets_){
			for(auto& kv: bucket){
			std::size_t idx = bucket_index(kv.first, new_bucket_count);
			new_buckets[idx].push_back(std::move(kv));
			}
		}
		buckets_ = std::move(new_buckets);
	}

private:
	std::size_t bucket_index(const Key& key, std::size_t bucket_count) const {
		return hash_(key) % bucket_count;
	}

	// grows the table (doubling) if the load factor would exceed the configured
	// threshold once one more element is added
	void maybe_grow(){
		if(static_cast<float>(size_ + 1) > max_load_factor_ * static_cast<float>(buckets_.size())){
			rehash(buckets_.size() * 2);
		}
	}

	dynamic_vector<doubly_list<value_type>> buckets_;
	std::size_t size_ = 0;
	Hash hash_{};
	KeyEqual key_equal_{};
	float max_load_factor_ = 1.0f;
};

}
