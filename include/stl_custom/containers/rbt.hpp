#pragma once
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
#include <iterator>

namespace stl_custom {

enum class rb_color : bool { red, black };

template<typename Value>
struct RbNode {
	Value value;
	RbNode* left = nullptr;
	RbNode* right = nullptr;
	RbNode* parent = nullptr;
	rb_color color = rb_color::red; // new nodes are red by convention

	template<typename... Args>
	explicit RbNode(RbNode* parent_, Args&&... args)
		: value(std::forward<Args>(args)...), parent(parent_) {}
};

// Tree-specific bidirectional iterator: moves forward/backward using the in-order
// successor/predecessor algorithm (no stored pointer). 
// Relies on a sentinel "header" node that end()/--end() work correctly.
template<typename Value, typename NodeT = RbNode<Value>>
class rb_tree_iterator {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = std::remove_cv_t<Value>;
	using difference_type = std::ptrdiff_t;
	using pointer = Value*;
	using reference = Value&;

	rb_tree_iterator(): node_(nullptr) {}
	explicit rb_tree_iterator(NodeT* node) : node_(node) {}

	reference operator*() const { return node_->value; }
	pointer operator->() const { return &node_->value; }

	rb_tree_iterator& operator++(){ // successor in-order

		if (node_->right != nullptr) {
        	node_ = node_->right;
        	while (node_->left != nullptr)
            	node_ = node_->left;
    	} else {
			NodeT* parent = node_->parent;
			while (node_ == parent->right) {
				node_ = parent;
				parent = parent->parent;
			}
			if (node_->right != parent)
				node_ = parent;
		}
		return *this;
	}
	rb_tree_iterator operator++(int){
		rb_tree_iterator node = *this;
		++(*this);
		return node;
	}
	rb_tree_iterator& operator--(){ // precedecessor in-order
		// node_ == header_
		if (node_->color == rb_color::red && node_->parent != nullptr && node_->parent->parent == node_) {
			node_ = node_->right; // maximum
		} else if (node_->left != nullptr) {
			node_ = node_->left;

			while (node_->right != nullptr)
				node_ = node_->right;
		} else {
			NodeT* parent = node_->parent;
			while (node_ == parent->left) {
				node_ = parent;
				parent = parent->parent;
			}
			node_ = parent;
		}
		return *this;
	}
	rb_tree_iterator operator--(int){
		rb_tree_iterator node = *this;
		--(*this);
		return node;
	}

	friend bool operator==(const rb_tree_iterator& a, const rb_tree_iterator& b){
		return a.node_ == b.node_;
	}
	
	friend bool operator!=(const rb_tree_iterator& a, const rb_tree_iterator& b){
		return !(a == b);
	}

	NodeT* base() const { return node_; }

private:
	NodeT* node_;
};

// Generic engine: Key = comparaison type, Value = what is stored in each node (T for set, pair<const K, V> for map),
// KeyOfValue extracts the key from Value (identity for set, .first for map), Compare orders the keys.
template<typename Key, typename Value, typename KeyOfValue, typename Compare = std::less<Key>>
class rbt {
public:
	using node_type = RbNode<Value>;
	using iterator = rb_tree_iterator<Value, node_type>;
	using const_iterator = rb_tree_iterator<const Value, const node_type>;

	rbt() noexcept : header_(nullptr) {
    	header_.color = rb_color::red;
   	 	header_.parent = nullptr;
   	 	header_.left = &header_;
    	header_.right = &header_;
	}
	~rbt(){
		clear();
	}
	rbt(const rbt& other) : rbt() {
		comp_ = other.comp_;
		key_of_value_ = other.key_of_value_;

		if (other.root_ != nullptr) {
			root_ = clone_subtree(other.root_, &header_);
		}
		size_ = other.size_;
		refresh_header();
	}
	rbt& operator=(const rbt& other) {
		if (this != &other) {
			rbt temp(other);
			swap(temp);
		}

		return *this;
	}
	rbt(rbt&& other) noexcept : rbt(){
    	swap(other);
	}
	rbt& operator=(rbt&& other) noexcept {
		if (this != &other) {
			clear();
			swap(other);
		}

		return *this;
	}
	void swap(rbt& other) noexcept {
		std::swap(root_, other.root_);
		std::swap(size_, other.size_);
		std::swap(comp_, other.comp_);
		std::swap(key_of_value_, other.key_of_value_);

		refresh_header();
		other.refresh_header();
	}

	// modifiers
	std::pair<iterator, bool> insert(const Value& value){
		return insert_impl(value);
	}
	std::pair<iterator, bool> insert(Value&& value){
		return insert_impl(std::move(value));
	}
	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args){
		Value value(std::forward<Args>(args)...);
    	return insert(std::move(value));
	}
	bool erase(const Key& key) {
		auto it = find(key);

		if (it == end())
			return false;

		node_type* z = it.base();
		node_type* y = z;

		rb_color original_color = y->color;

		node_type* x = nullptr;
		node_type* x_parent = nullptr;
		bool x_is_left = false;

		if (z->left == nullptr){
			x = z->right;
			x_parent = z->parent;

			if (z != root_)
				x_is_left = (z == z->parent->left);

			transplant(z, z->right);
		}else if (z->right == nullptr){
			x = z->left;
			x_parent = z->parent;

			if (z != root_)
				x_is_left = (z == z->parent->left);

			transplant(z, z->left);
		}else{
			y = minimum(z->right);
			original_color = y->color;

			x = y->right;

			if (y->parent == z){
				x_parent = y;
				x_is_left = false;

				if (x != nullptr)
					x->parent = y;
			}else{
				x_parent = y->parent;
				x_is_left = (y == y->parent->left);

				transplant(y, y->right);

				y->right = z->right;
				y->right->parent = y;
			}
			transplant(z, y);
			y->left = z->left;
			y->left->parent = y;

			y->color = z->color;
		}
		delete z;
		--size_;

		if (original_color == rb_color::black) {
			fix_erase(x, x_parent, x_is_left);
		}
		refresh_header();
		return true;
	}
	iterator erase(iterator pos){
		if (pos == end()) return end();

		iterator next = pos;
		++next;

		Key key = key_of_value_(*pos);
		erase(key);
		return next;
	}
	void clear() noexcept {
		destroy_subtree(root_);

    	root_ = nullptr;
    	size_ = 0;
    	refresh_header();
	}

	// lookup
	iterator find(const Key& key){
		node_type* current = root_;
		while( current != nullptr){
			const Key& current_key = key_of_value_(current->value);
			if(comp_(key, current_key)){
				current = current->left;
			} else if(comp_(current_key, key)){
				current = current->right;
			} else {
				return iterator(current);
			}
		}
		return end();
	}
	const_iterator find(const Key& key) const {
		node_type* current = root_;
		while (current != nullptr) {
			const Key& current_key = key_of_value_(current->value);
			if (comp_(key, current_key)) {
				current = current->left;
			} else if (comp_(current_key, key)) {
				current = current->right;
			} else {
				return const_iterator(current);
			}
		}
		return cend();
	}
	bool contains(const Key& key) const {
		return find(key) != cend();
	}

	// accessors
	bool empty() const noexcept { return size_ == 0; }
	std::size_t size() const noexcept { return size_; }

	// iterators (in-order)
	iterator begin() noexcept { return iterator(header_.left); }
	iterator end() noexcept { return iterator(&header_); }
	const_iterator begin() const noexcept { return const_iterator(header_.left); }
	const_iterator end() const noexcept { return const_iterator(&header_); }
	const_iterator cbegin() const noexcept { return const_iterator(header_.left); }
	const_iterator cend() const noexcept { return const_iterator(&header_); }

private:
	static void destroy_subtree(node_type* node) noexcept
	{
		if (node == nullptr)
			return;

		destroy_subtree(node->left);
		destroy_subtree(node->right);

		delete node;
	}
	void rotate_left(node_type* x){
		if(x->right == nullptr) return;
		node_type* y = x->right;

		x->right = y->left;
		if(y->left != nullptr){
			y->left->parent = x;
		}

		y->parent = x->parent;

		if(x == root_){
			root_ = y;
			header_.parent = root_; // sentined node
		}else if(x == x->parent->left){
			x->parent->left = y;
		}else if(x == x->parent->right){
			x->parent->right = y;
		}

		y->left = x;
		x->parent = y;
	}
	void rotate_right(node_type* x){
		if(x->left == nullptr) return;

		node_type* y = x->left;
		x->left = y->right;
		if(y->right!= nullptr){
			y->right->parent = x;
		}

		y->parent = x->parent;
		if(x == root_){
			root_ = y;
			header_.parent = root_;
		}else if(x == x->parent->left){
			x->parent->left = y;
		}else if(x == x->parent->right){
			x->parent->right = y;
		}

		y->right = x;
		x->parent = y;
	}
	void fix_insert(node_type* z){ // restores the red-black tree invariants after insertion
		while(z!= root_ && z->parent->color == rb_color::red){
			node_type* parent = z->parent;
			node_type* grandparent = parent->parent;

			if(parent == grandparent->left){
				node_type* uncle = grandparent->right;
				if(uncle != nullptr && uncle->color == rb_color::red){
					// case 1 : red uncle -> recolor, go up
					parent->color = rb_color::black;
					uncle->color = rb_color::black;
					grandparent->color = rb_color::red;
					z = grandparent;
				} else {
					if(z == parent->right){
						// case 2 : z is right child -> rotate left to get to case 3
						z = parent;
						rotate_left(z);
					}
					// case 3 : z is the left child -> recolor , right rotation
					z->parent->color = rb_color::black;
					z->parent->parent->color = rb_color::red;
					rotate_right(z->parent->parent);
				}
			} else {
				// symmetric (parent == grandparent->right)
				node_type* uncle = grandparent->left;
				if(uncle != nullptr && uncle->color == rb_color::red){
					parent->color = rb_color::black;
					uncle->color = rb_color::black;
					grandparent->color = rb_color::red;
					z = grandparent;
				} else {
					if(z == parent->left){
						z = parent;
						rotate_right(z);
					}
					z->parent->color = rb_color::black;
					z->parent->parent->color = rb_color::red;
					rotate_left(z->parent->parent);
				}
			}
		}
		root_->color = rb_color::black;
	}
	void fix_erase(node_type* x, node_type* x_parent, bool x_is_left){

    auto color_of = [](node_type* node) {
        return node == nullptr ? rb_color::black : node->color;
    };

    while (x != root_ && color_of(x) == rb_color::black) {

        // x is the LEFT child
        if (x_is_left) {

            node_type* sibling = x_parent->right;

            // Case 1:
            // sibling is RED
            //
            //        P(B)                 S(B)
            //       /   \                /
            //      x    S(R)     ->     P(R)
            //
            // Convert the situation so that sibling becomes black.
            if (color_of(sibling) == rb_color::red) {
                sibling->color = rb_color::black;
                x_parent->color = rb_color::red;

                rotate_left(x_parent);

                sibling = x_parent->right;
            }

            // Case 2:
            // sibling is black and both sibling children are black
            //
            // Push the extra black upward.
            if (sibling == nullptr ||
                (color_of(sibling->left) == rb_color::black &&
                 color_of(sibling->right) == rb_color::black))
            {
                if (sibling != nullptr) {
                    sibling->color = rb_color::red;
                }

                x = x_parent;
                x_parent = x_parent->parent;

                if (x_parent != nullptr) {
                    x_is_left = (x == x_parent->left);
                }
            }
            else {

                // Case 3:
                // sibling is black,
                // sibling's far child is black,
                // sibling's near child is red.
                //
                // Turn it into case 4.
                if (color_of(sibling->right) == rb_color::black) {

                    if (sibling->left != nullptr) {
                        sibling->left->color = rb_color::black;
                    }

                    sibling->color = rb_color::red;

                    rotate_right(sibling);

                    sibling = x_parent->right;
                }

                // Case 4:
                // sibling black and far child red.
                //
                // Final rotation removes the double-black.
                sibling->color = x_parent->color;
                x_parent->color = rb_color::black;

                if (sibling->right != nullptr) {
                    sibling->right->color = rb_color::black;
                }

                rotate_left(x_parent);

                // Problem solved
                x = root_;
            }
        }
        // Symmetric case: x is the RIGHT child
        else {

            node_type* sibling = x_parent->left;

            // Case 1: red sibling
            if (color_of(sibling) == rb_color::red) {
                sibling->color = rb_color::black;
                x_parent->color = rb_color::red;

                rotate_right(x_parent);

                sibling = x_parent->left;
            }

            // Case 2: sibling + both children black
            if (sibling == nullptr ||
                (color_of(sibling->left) == rb_color::black &&
                 color_of(sibling->right) == rb_color::black))
            {
                if (sibling != nullptr) {
                    sibling->color = rb_color::red;
                }

                x = x_parent;
                x_parent = x_parent->parent;

                if (x_parent != nullptr) {
                    x_is_left = (x == x_parent->left);
                }
            } else {

                // Case 3:
                // far child (left) is black
                if (color_of(sibling->left) == rb_color::black) {

                    if (sibling->right != nullptr) {
                        sibling->right->color = rb_color::black;
                    }

                    sibling->color = rb_color::red;

                    rotate_left(sibling);

                    sibling = x_parent->left;
                }

                // Case 4
                sibling->color = x_parent->color;
                x_parent->color = rb_color::black;

                if (sibling->left != nullptr) {
                    sibling->left->color = rb_color::black;
                }

                rotate_right(x_parent);

                x = root_;
            }
        }
    }

    if (x != nullptr) {
        x->color = rb_color::black;
    }
	}

	static node_type* minimum(node_type* node) noexcept {
		if (node == nullptr)
			return nullptr;

		while (node->left != nullptr)
			node = node->left;

		return node;
	}

	static node_type* maximum(node_type* node) noexcept {
		if (node == nullptr)
			return nullptr;

		while (node->right != nullptr)
			node = node->right;

		return node;
	}

	void refresh_header() noexcept {
		header_.parent = root_;

		if (root_ == nullptr) {
			header_.left = &header_;
			header_.right = &header_;
			return;
		}

		root_->parent = &header_;
		header_.left = minimum(root_);
		header_.right = maximum(root_);
	}

	template<typename U>
	std::pair<iterator, bool> insert_impl(U&& value){
		const Key& key = key_of_value_(value);

		node_type* parent = &header_;
		node_type* current = root_;

		bool insert_left = true;

		while (current != nullptr) {
			parent = current;
			const Key& current_key = key_of_value_(current->value);

			if (comp_(key, current_key)) {
				current = current->left;
				insert_left = true;
			}
			else if (comp_(current_key, key)) {
				current = current->right;
				insert_left = false;
			}else {
				// Key already exists
				return {iterator(current), false};
			}
		}

		node_type* new_node = new node_type(parent, std::forward<U>(value));

		if (parent == &header_) {
			root_ = new_node;
		} else if (insert_left) {
			parent->left = new_node;
		} else {
			parent->right = new_node;
		}

		++size_;
		fix_insert(new_node);
		refresh_header();

		return {iterator(new_node), true};
	}

	void transplant(node_type* u, node_type* v){
		if (u == root_) {
			root_ = v;
			if (v != nullptr)
				v->parent = &header_;
		}else if (u == u->parent->left) {
			u->parent->left = v;
			if (v != nullptr)
				v->parent = u->parent;
		}else {
			u->parent->right = v;
			if (v != nullptr)
				v->parent = u->parent;
		}
	}

	node_type* clone_subtree( const node_type* source, node_type* parent){
		if (source == nullptr) return nullptr;

		node_type* node = new node_type(parent, source->value);
		node->color = source->color;

		try {
			node->left = clone_subtree(source->left, node);
			node->right = clone_subtree(source->right, node);
		}catch (...) {
			destroy_subtree(node);
			throw;
		}

		return node;
	}

	node_type* root_ = nullptr;
	node_type header_; // sentinel node: left=min, right=max, parent=root_; end() points to this node
	std::size_t size_ = 0;
	Compare comp_{};
	KeyOfValue key_of_value_{};
};


}
