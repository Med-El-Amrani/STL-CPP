#pragma once
#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include "stl_custom/containers/rbt.hpp"

namespace stl_custom {

template<typename Key, typename Value>
struct KeyOfPair {
	const Key& operator()(const std::pair<const Key, Value>& p) const {
		return p.first;
	}
};

template<typename Key, typename Value, typename Compare = std::less<Key>>
class map {
public:
	using value_type = std::pair<const Key, Value>;
	using allocator_type = std::allocator<value_type>;
	using key_compare = Compare;
	using tree_type = rbt<Key, value_type, KeyOfPair<Key, Value>, Compare>;
	using iterator = typename tree_type::iterator;
	using const_iterator = typename tree_type::const_iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	class value_compare {
		friend class map;
	protected:
		explicit value_compare(const key_compare& comp) : comp_(comp) {}
		key_compare comp_;
	public:
		bool operator()(const value_type& lhs, const value_type& rhs) const {
			return comp_(lhs.first, rhs.first);
		}
	};

	map() noexcept = default;
	explicit map(const Compare& comp) : tree_(comp) {}

	template<typename InputIt>
	map(InputIt first, InputIt last) : map() {
		for (; first != last; ++first)
			insert(*first);
	}
	map(const map&) = default;
	map& operator=(const map&) = default;
	map(map&&) noexcept = default;
	map& operator=(map&&) noexcept = default;

	void swap(map& other) noexcept { tree_.swap(other.tree_); }

	std::pair<iterator, bool> insert(const value_type& value) {
		return tree_.insert(value);
	}
	iterator insert(const_iterator /* hint */, const value_type& value) {
		// Use ordinary insertion until the tree supports optimized hints.
		return insert(value).first;
	}
	template<typename InputIt,
		typename = typename std::iterator_traits<InputIt>::iterator_category>
	void insert(InputIt first, InputIt last) {
		for (; first != last; ++first)
			insert(*first);
	}
	std::pair<iterator, bool> insert(const Key& key, const Value& value) {
		return tree_.insert(value_type(key, value));
	}
	std::pair<iterator, bool> insert(const Key& key, Value&& value){
		return tree_.insert(value_type(key, std::move(value)));
	}
	template<typename... Args>
	std::pair<iterator, bool> emplace(const Key& key, Args&&... args);
	bool erase(const Key& key) { return tree_.erase(key); }
	iterator erase(iterator pos) { return tree_.erase(pos); }
	iterator erase(iterator first, iterator last) {
		while (first != last)
			first = erase(first);
		return first;
	}
	void clear() { tree_.clear(); }

	Value& operator[](const Key& key) {
		auto it = tree_.find(key);
		if (it != tree_.end())
			return it->second;

		return tree_.insert(value_type(key, Value{})).first->second;
	}
	
	iterator find(const Key& key) { return tree_.find(key); }
	const_iterator find(const Key& key) const { return tree_.find(key); }
	iterator lower_bound(const Key& key) { return tree_.lower_bound(key); }
	const_iterator lower_bound(const Key& key) const { return tree_.lower_bound(key); }
	iterator upper_bound(const Key& key) { return tree_.upper_bound(key); }
	const_iterator upper_bound(const Key& key) const { return tree_.upper_bound(key); }
	std::pair<iterator, iterator> equal_range(const Key& key) {
		return {lower_bound(key), upper_bound(key)};
	}
	std::pair<const_iterator, const_iterator> equal_range(const Key& key) const {
		return {lower_bound(key), upper_bound(key)};
	}
	bool contains(const Key& key) const { return tree_.contains(key); }
	std::size_t count(const Key& key) const { return tree_.contains(key) ? 1 : 0; }
	Value& at(const Key& key);
	const Value& at(const Key& key) const;

	allocator_type get_allocator() const noexcept { return allocator_type{}; }

	key_compare key_comp() const { return tree_.key_comp(); }
	value_compare value_comp() const { return value_compare(key_comp()); }

	bool empty() const noexcept { return tree_.empty(); }
	std::size_t size() const noexcept { return tree_.size(); }
	
	iterator begin() noexcept { return tree_.begin(); }
	iterator end() noexcept { return tree_.end(); }
	const_iterator begin() const noexcept { return tree_.begin(); }
	const_iterator end() const noexcept { return tree_.end(); }
	const_iterator cbegin() const noexcept { return tree_.cbegin(); }
	const_iterator cend() const noexcept { return tree_.cend(); }

	reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
	reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
	const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
	const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
private:
	tree_type tree_;
};

template<typename Key, typename Value, typename Compare>
bool operator==(const map<Key, Value, Compare>& lhs,
                const map<Key, Value, Compare>& rhs) {
	return lhs.size() == rhs.size() &&
		std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename Key, typename Value, typename Compare>
bool operator!=(const map<Key, Value, Compare>& lhs,
                const map<Key, Value, Compare>& rhs) {
	return !(lhs == rhs);
}

template<typename Key, typename Value, typename Compare>
bool operator<(const map<Key, Value, Compare>& lhs,
                const map<Key, Value, Compare>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename Key, typename Value, typename Compare>
bool operator>(const map<Key, Value, Compare>& lhs,
                const map<Key, Value, Compare>& rhs) {
	return rhs < lhs;
}

template<typename Key, typename Value, typename Compare>
bool operator<=(const map<Key, Value, Compare>& lhs,
                const map<Key, Value, Compare>& rhs) {
	return !(rhs < lhs);
}

template<typename Key, typename Value, typename Compare>
bool operator>=(const map<Key, Value, Compare>& lhs,
                const map<Key, Value, Compare>& rhs) {
	return !(lhs < rhs);
}

}
