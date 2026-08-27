#pragma once
#include <cstddef>
#include <initializer_list>
#include <utility>
#include "stl_custom/iterators/forward_iterator.hpp"

namespace stl_custom {

template<typename T>
class singly_list {
private:
	struct Node;
public:
	using iterator = stl_custom::forward_iterator<T, Node>;
	using const_iterator = stl_custom::forward_iterator<const T, Node>;
	// constructor
	singly_list() noexcept = default;
	singly_list(std::initializer_list<T> list) {
		Node *previous_node = nullptr;
		for(const auto& e: list) {
			if(data_== nullptr){
				data_ = new Node(e, nullptr); 
				previous_node = data_;
			}else{
				previous_node->next = new Node(e, nullptr);
				previous_node = previous_node->next;
			}
			++size_;
		}
	}

	// copy constructor
	singly_list(const singly_list& other) {
	try{
		Node* source = other.data_;
		Node* tail = nullptr;
		
		while(source != nullptr) {
			Node* new_node = new Node{ source->value, nullptr};
			if(data_ == nullptr) data_ = new_node;
			else tail->next = new_node;

			tail = new_node;
			source = source->next;
			++size_;
		}
	} catch(...) {
		clear();
		throw;
	}

	}

	singly_list& operator=(const singly_list& other){
		singly_list tmp(other);
		swap(tmp);
		return *this;
	}
	
	singly_list(singly_list&& other) noexcept 
		: data_(other.data_), size_(other.size_) {
		other.data_ = nullptr;
		other.size_ = 0;
	}
	
	singly_list& operator=(singly_list&& other) noexcept {
		if(this != &other){
		clear();
		data_ = other.data_;
		size_ = other.size_;
		other.data_ = nullptr;
		other.size_ = 0;
		}
		return *this;
	}

	void push_front(const T& value){
		data_ = new Node(value, data_);
		++size_;
	}
	void push_front(T&& value){
		data_ = new Node{std::move(value), data_};
		++size_;
	}

	template<typename... Args>
	T& emplace_front(Args&&... args){
		data_ = new Node{T(std::forward<Args>(args)...), data_};
		++size_;
		return data_->value;
	}

	//Precondition: pos must refer to a valid element of this list.
	iterator insert_after(const_iterator pos, const T& value){
		Node* node = pos.node_;
		node->next = new Node{value, node->next};
		++size_;
		return iterator(node->next);
	}
	//Precondition: pos must refer to a valid element of this list.
	iterator insert_after(const_iterator pos, T&& value){
		Node* node = pos.node_;
		node->next = new Node(std::move(value) , node->next);
		++size_;
		return iterator(node->next);
	}
	//Precondition: pos must refer to a valid element of this list.
	template<typename... Args>
	iterator emplace_after(const_iterator pos, Args&&... args){
		Node* node = pos.node_;

		node->next = new Node{T(std::forward<Args>(args)...), node->next};
		++size_;
	
		return iterator(node->next);
	}

	void pop_front(){
		if(data_ == nullptr) return;
		Node * tmp = data_;
		data_ = data_->next;
		delete tmp;
		--size_;
	}
	
	iterator erase_after(const_iterator pos){
		Node* node = pos.node_;
		if(node == nullptr || node->next == nullptr) return end();
	
		Node* victim = node->next;
	
		node->next = victim->next;
		delete victim;
		--size_;

		return iterator(node->next);
	}

	void remove(const T& value) {
		Node* current = data_;
		Node* previous = nullptr;
		while(current!=nullptr){
			if(current->value == value){
				Node* to_delete = current;
				if(previous==nullptr){
					data_ = current->next;
					current = data_;
				} else {
					previous->next = current->next;
					current = current->next;
				}
				delete to_delete;
				--size_;
				
			} else {
				previous = current;
				current = current->next;
			}
		}
	}

	void reverse() noexcept {
		if(size_ < 2) return;
		Node* current = data_;
		Node* previous = nullptr;
		Node* next = nullptr;
		while(current != nullptr){
		next = current->next;
		current->next = previous;
		previous = current;

		current = next;
		}
		data_ = previous;
	}

	T& front() {
		return data_->value;
	}
	const T& front() const {
		return data_->value;
	}

	void clear() noexcept {
		while(data_!= nullptr) {
			Node* tmp = data_;
			data_ = data_->next;
			delete tmp;
		}
		size_ = 0;
	}

	void swap(singly_list& other) noexcept {
		std::swap(data_, other.data_);
		std::swap(size_, other.size_);
	}

	iterator begin() noexcept { return iterator(data_); }
	iterator end() noexcept { return iterator(nullptr); }
	const_iterator begin() const noexcept { return const_iterator(data_); }
	const_iterator end() const noexcept { return const_iterator(nullptr); }
	const_iterator cbegin() const noexcept { return const_iterator(data_); }
	const_iterator cend() const noexcept { return const_iterator(nullptr); }

	std::size_t size() const noexcept { return size_; }
	bool empty() const noexcept { return size_ == 0; }
	
	~singly_list() {
		clear();
	}
	
private:

	struct Node{
		T value;
		Node* next;
	};
	Node* data_ = nullptr;
	std::size_t size_ = 0;
};

}
