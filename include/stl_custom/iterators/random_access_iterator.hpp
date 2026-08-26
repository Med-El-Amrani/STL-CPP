#pragma once
#include <cstddef>
#include <iterator>
#include <type_traits>
namespace stl_custom {

template<typename T>
class random_access_iterator {
	template<typename>
	friend class random_access_iterator;
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type =std::remove_cv_t<T>;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    random_access_iterator() : ptr_(nullptr) {}
    explicit random_access_iterator(T* ptr) : ptr_(ptr) {}

    // to enable constructing iterator from const_iterator(U = const T)
    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    random_access_iterator(const random_access_iterator<U>& other) : ptr_(other.ptr_) {}

    // dereference operator
    reference operator*() const {
        return *ptr_;
    }
    // arrow operator
    pointer operator->() const {
        return ptr_;
    }

    // pre-increment
    random_access_iterator& operator++(){
        ++ptr_;
        return *this;
    }
    // post-increment
    random_access_iterator operator++(int){
        random_access_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    // pre-decrement
    random_access_iterator& operator--(){
        --ptr_;
        return *this;
    }

    // post-decrement
    random_access_iterator operator--(int){
        random_access_iterator tmp = *this;
        --(*this);
        return tmp;
    }

    // advances the iterator forward by n positions
    random_access_iterator& operator+=(difference_type n){
        ptr_ += n;
        return *this;
    }

    // moves the iterator backward by n positions
    random_access_iterator& operator-=(difference_type n){
        ptr_ -= n;
        return *this;
    }

    // returns a new iterator advanced forward by n
    random_access_iterator operator+(difference_type n) const {
        random_access_iterator tmp = *this;
        tmp += n;
        return tmp;
    }

    // return a new iterator moved backward by n
    random_access_iterator operator-(difference_type n) const {
        random_access_iterator tmp = *this;
        tmp -=n;
        return tmp;
    }

    // distance between two iterators
    difference_type operator-(const random_access_iterator& other) const {
        return ptr_ - other.ptr_;
    }

    // random-access operator
    reference operator[](difference_type n) const {
        return *(*this + n);
    }

    // symmetric form of it + n
    friend random_access_iterator operator+(difference_type n, const random_access_iterator& it){
        return it + n;
    }




    // comparison operators
    friend bool operator==(const random_access_iterator& a, const random_access_iterator& b){
        return a.ptr_ == b.ptr_;
    }
    friend bool operator!=(const random_access_iterator& a, const random_access_iterator& b){
        return !(a==b);
    }

    friend bool operator<(const random_access_iterator& a, const random_access_iterator& b){
        return a.ptr_ < b.ptr_;
    }
    friend bool operator>(const random_access_iterator& a, const random_access_iterator& b){
        return a.ptr_ > b.ptr_;
    }
    friend bool operator<=(const random_access_iterator& a, const random_access_iterator& b){
        return a.ptr_ <= b.ptr_;
    }
    friend bool operator>=(const random_access_iterator& a, const random_access_iterator& b){
        return a.ptr_ >= b.ptr_;
    }

private:
    T* ptr_;
};

} // stl_custom
