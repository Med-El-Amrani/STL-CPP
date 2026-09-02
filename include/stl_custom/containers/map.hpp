#pragma once
#include <utility>
#include "stl_custom/containers/rbt.hpp"

namespace stl_custom {

template<typename Key, typename Value>
struct KeyOfPair {
	const Key& operator()(const std::pair<const Key, Value>& p) const {
		return p.first;
	}
};

template<typename Key, typename value, typename Compare = std::less<Key>>
class map {
public:
	using value_type = std::pair<const Key, Value>;
	using tree_type = rbt<Key, value_type, KeyOfPair<Key, Value>, Compare>;
	using iterator = typename tree_type::iterator;
	using const_iterator = typename tree_type::const_iterator;

	map() noexcept = default;
	map(const map&) = default;
	map& operator=(const map&) = default;
	map(map&&) noexcept = default;
	map& operator=(map&&) noexcept = default;

	void swap(map& other) noexcept { tree_.swap(other.tree_); }

	std::pair<iterator, bool> insert(const Kay& key, const Value& value) {
		return tree_.insert(value_type(key, value));
	}
	std::pair<iterator, bool> insert(const Key& key, Value&& value){
		return tree_.insert(value_type(key, std::move(value)));
	}
	template<typename... Args>
	std::pair<iterator, bool> emplace(const Key& key, Args&&... args);
	bool erase(const Key& key) { return tree_.erase(key); }
	iterator erase(iterator pos) { return tree_.erase(pos); }
	void clear() { tree_.clear(); }

	Value& operator[](const Key& key); //insert-default-if-missing 

	iterator find(const Key& key) { return tree_.find(key); }
	const_iterator find(const Key& key) const { return tree_.find(key); }
	bool contains(const Key& key) const { return tree_.contains(key); }
	Value& at(const Key& key);
	const Value& at(const Key& key) const;

	bool empty() const noexcept { return tree_.empty(); }
	std::size_t size() const noexcept { return tree_.size(); }
	
	iterator begin() noexcept { return tree_.begin(); }
	iterator end() noexcept { return tree_.end(); }
	const_iterator begin() const noexcept { return tree_.begin(); }
	const_iterator end() const noexcept { return tree_.end(); }
	const_iterator cbegin() const noexcept { return tree_.cbegin(); }
	const_iterator cend() const noexcept { return tree_.cend(); }
private:
	tree_type tree_;
};

}
