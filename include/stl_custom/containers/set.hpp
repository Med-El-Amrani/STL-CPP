#pragma once
#include "stl_custom/containers/rbt.hpp"

namespace stl_custom{

template<typename T>
struct Identity {
	const T& operator()(const T& value) const { return value; }
};

template<typename T, typename Compare = std::less<T>>
class set {
public:
	using iterator = typename rbt<T, T, Identity<T>, Compare>::iterator;
	using const_iterator = typename rbt<T, T, Identity<T>, Compare>::const_iterator;
	
	set() noexcept = default;
	// Rule of five delegated entirely to rbt_
	set(const set&) = default;
	set& operator=(const set&) = default;
	set(set&&) noexcept = default;
	set& operator=(set&&) noexcept = default;

	void swap(set& other) noexcept { tree_.swap(other.tree_); }
	
	std::pair<iterator, bool> insert(const T& value) { return tree_.insert(value); }
	std::pair<iterator, bool> insert(T&& value) { return tree_.insert(std::move(value)); }
	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args) { return tree_.emplace(std::forward<Args>(args)...); }
	bool erase(const T& value) { return tree_.erase(value); }
	iterator erase(iterator pos) { return tree_.erase(pos); }
	void clear() noexcept { tree_.clear(); }

	iterator find(const T& value){ return tree_.find(value); }
	const_iterator find(const T& value) const { return tree_.find(value); }
	bool contains(const T& value) const { return tree_.contains(value); }

	bool empty() const noexcept { return tree_.empty(); }
	std::size_t size() const noexcept { return tree_.size(); }

	iterator begin() noexcept { return tree_.begin(); }
	iterator end() noexcept { return tree_.end(); }
	const_iterator begin() const noexcept { return tree_.begin(); }
	const_iterator end() const noexcept { return tree_.end(); }
	const_iterator cbegin() const noexcept { return tree_.cbegin(); }
	const_iterator cend() const noexcept { return tree_.cend(); }
private:
	rbt<T, T, Identity<T>, Compare> tree_;
};

}
