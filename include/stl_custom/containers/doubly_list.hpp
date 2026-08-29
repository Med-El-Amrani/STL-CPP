#pragma once
#include <cstddef>
#include <utility>
#include <stdexcept>
#include "stl_custom/iterators/bidirectional_iterator.hpp"

namespace stl_custom {

template<typename T, typename NodeT = BidirectionalNode<T>>
class doubly_list{
public:

using iterator = stl_custom::bidirectional_iterator<T, NodeT>;
using const_iterator = stl_custom::bidirectional_iterator<const T, NodeT>;
//constructor : empty state
doubly_list() noexcept {
	sentinel_.next = &sentinel_;
	sentinel_.previous = &sentinel_;
}

//destructor
~doubly_list(){
	clear();
}
// copy constructor
doubly_list(const doubly_list& other) : doubly_list() {
	try {
		for(BidirectionalNodeBase* cur = other.sentinel_.next; cur!=&other.sentinel_; cur= cur->next){
			push_back(static_cast<NodeT*>(cur)->value);
		}
	} catch(...){
		clear();
		throw;
	}
}
// move constructor
doubly_list(doubly_list&& other) noexcept : size_(other.size_) {
	if(other.empty()){
		sentinel_.next = &sentinel_;
		sentinel_.previous = &sentinel_;
	} else {
		sentinel_.next = other.sentinel_.next;
		sentinel_.previous = other.sentinel_.previous;
		sentinel_.next->previous = &sentinel_;
		sentinel_.previous->next = &sentinel_;
		
		other.sentinel_.next = &other.sentinel_;
		other.sentinel_.previous = &other.sentinel_;
	}
	other.size_ = 0;
}
// copy assignement operator
doubly_list& operator=(const doubly_list& other){
	if(this != &other){
	doubly_list tmp(other);
	swap(tmp);
	}
	return *this;
}
// move assignement operator
doubly_list& operator=(doubly_list&& other) noexcept {
	if(this!= &other){
		clear();
		size_ = other.size_;
		
		if(!other.empty()){
		sentinel_.next = other.sentinel_.next;
		sentinel_.previous = other.sentinel_.previous;
		sentinel_.next->previous = &sentinel_;
		sentinel_.previous->next = &sentinel_;

		other.sentinel_.next = &other.sentinel_;
		other.sentinel_.previous = &other.sentinel_;
		}
		
		other.size_ = 0;
	}
	return *this;
}

// modifiers
void push_back(const T& value){
	BidirectionalNodeBase* old_last = sentinel_.previous;
	NodeT* node = new NodeT(value, &sentinel_, old_last);
	old_last->next = node;
	sentinel_.previous = node;
	++size_;
}

void push_back(T&& value){
        BidirectionalNodeBase* old_last = sentinel_.previous;
        NodeT* node = new NodeT(std::move(value), &sentinel_, old_last);
        old_last->next = node;
        sentinel_.previous = node;
        ++size_;
}

void push_front(const T& value){
	BidirectionalNodeBase* old_first = sentinel_.next;
        NodeT* node = new NodeT(value, old_first, &sentinel_);
	sentinel_.next = node;
	old_first->previous = node;
        ++size_;
}

void push_front(T&& value){
        BidirectionalNodeBase* old_first = sentinel_.next;
        NodeT* node = new NodeT(std::move(value), old_first, &sentinel_);
        sentinel_.next = node;
        old_first->previous = node;
        ++size_;
}

template<typename... Args>
T& emplace_back(Args&&... args){
        BidirectionalNodeBase* old_last = sentinel_.previous;
        NodeT* node = new NodeT(&sentinel_, old_last, std::forward<Args>(args)...);
        old_last->next = node;
        sentinel_.previous = node;
        ++size_;
	return node->value;
}

template<typename... Args>
T& emplace_front(Args&&... args){
        BidirectionalNodeBase* old_first = sentinel_.next;
        NodeT* node = new NodeT(old_first, &sentinel_, std::forward<Args>(args)...);
        sentinel_.next = node;
        old_first->previous = node;
        ++size_;

	return node->value;
}

void pop_front(){
	if(empty()) throw std::underflow_error("doubly_list::pop_front empty list");
	NodeT* node = static_cast<NodeT*>(sentinel_.next);
	
	BidirectionalNodeBase* next = node->next;
	sentinel_.next = next;
	next->previous = &sentinel_;

	delete node;
	--size_;
}

void pop_back(){
	if(empty()) throw std::underflow_error("doubly_list::pop_back empty list");
	NodeT* node = static_cast<NodeT*>(sentinel_.previous);

	BidirectionalNodeBase* prev = node->previous;
	sentinel_.previous = prev;
	prev->next = &sentinel_;

	delete node;
	--size_;
}

void clear() noexcept {
	BidirectionalNodeBase* cur = sentinel_.next;
	while(cur != &sentinel_){
		BidirectionalNodeBase* next = cur->next;
		delete static_cast<NodeT*>(cur);
		cur = next;
	}
	sentinel_.next = &sentinel_;
	sentinel_.previous = &sentinel_;
	size_ = 0;
}

// swap: a naive std::swap on sentinel_.next/previous would break the
// self-referencing "empty" invariant, since swapping raw addresses
// doesn't know which pointer should become "points to itself" vs
// "points to the other object's sentinel". So we capture old state
// explicitly and rebuild both lists' boundaries from it.
void swap(doubly_list& other) noexcept {
	if (this == &other) return;
 
	bool this_was_empty = empty();
	bool other_was_empty = other.empty();

	BidirectionalNodeBase* this_first = sentinel_.next;
	BidirectionalNodeBase* this_last = sentinel_.previous;
	BidirectionalNodeBase* other_first = other.sentinel_.next;
	BidirectionalNodeBase* other_last = other.sentinel_.previous;
	// *this* now takes on other's old content
	if (other_was_empty) {
		sentinel_.next = &sentinel_;
		sentinel_.previous = &sentinel_;
	} else {
		sentinel_.next = other_first;
		sentinel_.previous = other_last;
		other_first->previous = &sentinel_;
		other_last->next = &sentinel_;
	}

	//other now takes on this's old content
	if (this_was_empty) {
		other.sentinel_.next = &other.sentinel_;
		other.sentinel_.previous = &other.sentinel_;
	} else {
		other.sentinel_.next = this_first;
		other.sentinel_.previous = this_last;
		this_first->previous = &other.sentinel_;
		this_last->next = &other.sentinel_;
	}

	std::swap(size_, other.size_);
}

// accessors

T& back(){ return static_cast<NodeT*>(sentinel_.previous)->value; }
const T& back() const { return static_cast<NodeT*>(sentinel_.previous)->value; }
T& front(){ return static_cast<NodeT*>(sentinel_.next)->value; }
const T& front() const { return static_cast<NodeT*>(sentinel_.next)->value; }

bool empty() const noexcept { return size_ == 0; }
std::size_t size() const noexcept { return size_; }

// iterators
iterator begin(){ return iterator(sentinel_.next); }
iterator end() { return iterator(&sentinel_); }
const_iterator begin() const { return const_iterator(sentinel_.next); }
const_iterator end() const { return const_iterator(const_cast<BidirectionalNodeBase*>(&sentinel_)); }
const_iterator cbegin() const { return const_iterator(sentinel_.next); }
const_iterator cend() const { return const_iterator(const_cast<BidirectionalNodeBase*>(&sentinel_)); }


private:
	BidirectionalNodeBase sentinel_;
	std::size_t size_ = 0;
};

}
