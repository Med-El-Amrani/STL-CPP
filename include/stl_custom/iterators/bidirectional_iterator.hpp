#pragma once
#include <cstddef>
#include <iterator>
#include <utility>

namespace stl_custom {

struct BidirectionalNodeBase {
	BidirectionalNodeBase* next = nullptr;
	BidirectionalNodeBase* previous = nullptr;

	BidirectionalNodeBase() = default;
	
	BidirectionalNodeBase(BidirectionalNodeBase* next_, BidirectionalNodeBase* previous_)
			: next(next_), previous(previous_)
	{}
};

template< typename T>
struct BidirectionalNode : BidirectionalNodeBase {
	T value;

	// Pour push_back(const T&), push_front(const T&), copy constructor...
	BidirectionalNode(const T& value_, BidirectionalNodeBase* next_, BidirectionalNodeBase* previous_)
		: BidirectionalNodeBase(next_, previous_), value(value_)
		{}

	// Pour push_back(T&&), push_front(T&&)
	BidirectionalNode(T&& value_, BidirectionalNodeBase* next_, BidirectionalNodeBase* previous_)
		: BidirectionalNodeBase(next_, previous_), value(std::move(value_))
		{}

	// pour emplace
	template<typename... Args>
	BidirectionalNode(BidirectionalNodeBase* next_, BidirectionalNodeBase* previous_, Args&&... args)
		: BidirectionalNodeBase(next_, previous_), value(std::forward<Args>(args)...)
		{}
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
	explicit bidirectional_iterator(BidirectionalNodeBase* node) : node_(node) {}

	// Derefrence operator
	reference operator*() const {
		return static_cast<NodeT*>(node_)->value;
	}

	// arrow operator
	pointer operator->() const {
		return &(static_cast<NodeT*>(node_)->value);
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
	BidirectionalNodeBase* node_;
};

} // stl_custom
