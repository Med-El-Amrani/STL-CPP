#pragma once
#include <cstddef>
#include <iterator>

namespace stl_custom {
template< typename T>
struct BidirectionalNode {
	T value;
	BidirectionalNode* next = nullptr;
	BidirectionalNode* previous = nullptr;
};

template<typename T, typename NodeT = BidirectionalNode<T>> 
class bidirectional_iterator {
public:
	// iterator_traits conformance to allow usage with stl algorithms
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;
	
	// constrcutors
	bidirectional_iterator() : node_(nullptr) {}
	explicit bidirectional_iterator(NodeT* node) : node_(node) {}

	// Derefrence operator
	reference operator*() const {
		return node_->value;
	}

	// arrow operator
	pointer operator->() const {
		return &(node_->value);
	}
	
	// pre-increment
	bidirectional_iterator& operator++(){
		node_ = node_->next;
		return *this;
	}
	// post-increment
	bidirectional_iterator operator++(int){
		bidirectional_iterator tmp = *this;
		++(*this);
		return tmp;
	}
	// pre-decrement
	bidirectional_iterator& operator--(){
		node_ = node_->previous;
		return *this;
	}
	// post-decrement
	bidirectional_iterator operator--(int){
		bidirectional_iterator tmp = *this;
		--(*this);
		return tmp;
	}

	// comparison operators
	friend bool operator==(const bidirectional_iterator& a, const bidirectional_iterator& b){
		return b.node_ == a.node_;
	}
	
	friend bool operator!=(const bidirectional_iterator& a, const bidirectional_iterator& b){
		return !(a==b);
	}
	
private:
	NodeT* node_;
};

} // stl_custom
