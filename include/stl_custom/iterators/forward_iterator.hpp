#pragma once
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace stl_custom {

template<typename T>
struct ForwardNode {
	T value;
	ForwardNode* next = nullptr;
};

template<typename T, typename NodeT = ForwardNode<T>>
class forward_iterator {
public:
	// iterator_traits conformance to allow usage with STL algorithms
	using iterator_category = std::forward_iterator_tag;
	using value_type = std::remove_cv_t<T>;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;

	forward_iterator() : node_(nullptr) {}
	explicit forward_iterator(NodeT* node) : node_(node) {}

	// to enable iterator -> const_iterator
	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
	forward_iterator(const forward_iterator<U, NodeT>& other) : node_(other.node_) {}

	// dereference operator
	reference operator*() const {
		return node_->value;
	}

	// arrow operator
	pointer operator->() const {
		return &(node_->value);
	}

	//pre-increment operator
	forward_iterator& operator++(){
		node_ = node_->next;
		return *this;
	}

	// post-increment operator
	forward_iterator operator++(int) {
		forward_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	// comparison operators
	friend bool operator==(const forward_iterator& a, const forward_iterator& b){
		return a.node_ == b.node_;
	}

	friend bool operator!=(const forward_iterator& a, const forward_iterator& b){
		return !(a == b);
	}

private:
	
	template<typename, typename>
	friend class forward_iterator;
	
	template<typename>
	friend class singly_list;
	NodeT* node_;
};

} // stl_custom
