#pragma once
#include <cstddef>
#include <type_traits>
#include <iterator>

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
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;

	forward_iterator() : node_(nullptr) {}
	explicit forward_iterator(NodeT* node) : node_(node) {}

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
		if(node_) {
			node_ = node_->next;
			return *this;
		}
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
	NodeT* node_;
};

} // stl_custom
