#pragma once
#include <cstddef>
#include <utility>
#include <functional>
#include <stdexcept>
#include <vector>

namespace stl_custom{

template<typename T>
struct BstNode {
	T value;
	BstNode* left = nullptr;
	BstNode* right = nullptr;
	BstNode* parent = nullptr;

	explicit BstNode(const T& value_, BstNode* parent_ = nullptr)
		: value(value_), parent(parent_) {}
	explicit BstNode(T&& value_, BstNode* parent_ = nullptr)
		: value(std::move(value_)), parent(parent_) {}
	// for emplace
	template<typename... Args>
	explicit BstNode(BstNode* parent_, Args&&... args)
		: value(std::forward<Args>(args)...), parent(parent_) {}
};

template<typename T, typename Compare = std::less<T>>
class bst {
public:
	bst() noexcept = default;
	~bst() {
		clear();
	}

	bst(const bst& other){
		if(other.root_ == nullptr) return;
		root_ = new BstNode<T>(other.root_->value , nullptr);
		++size_;
		
		// stack entries: {source node whose children need cloning, its cloned counterpart}
		std::vector<std::pair<BstNode<T>*, BstNode<T>*>> stack;
		stack.push_back({other.root_, root_});
		try{

		while(!stack.empty()){
			auto [src, dst] = stack.back();
			stack.pop_back();
			
			if(src->left != nullptr){
				dst->left = new BstNode<T>(src->left->value , dst);
				++size_;
				stack.push_back({src->left, dst->left});
			}
			if(src->right != nullptr){
				dst->right = new BstNode<T>(src->right->value, dst);
				++size_;
				stack.push_back({src->right, dst->right});
			}
		}

		} catch(...){
			clear(); // unwind what was build
			throw;
		}
	}
	bst& operator=(const bst& other) {
		bst tmp(other);
		swap(tmp);
		return *this;
	}
	bst(bst&& other) noexcept
		: root_(other.root_), size_(other.size_) {
		other.root_ = nullptr;
		other.size_ = 0;
	}
	bst& operator=(bst&& other) noexcept {
		if(this != &other){
			clear();
			
			root_ = other.root_;
			size_ = other.size_;

			other.root_ = nullptr;
			other.size_ = 0;
		}
		return *this;
	}

	void swap(bst& other) noexcept {
		std::swap(root_, other.root_);
		std::swap(size_, other.size_);
	}

	// modifiers
	bool insert(const T& value) {
		
		if(empty()){
			root_ = new BstNode<T>(value, nullptr);
			++size_;
			return true;
		}
		BstNode<T>* cur = root_;
		while(true){
			if(comp_(value, cur->value)){
				if(cur->left == nullptr){
					cur->left = new BstNode<T>(value, cur);
					break;
				}
				cur = cur->left;
			} else if(comp_(cur->value ,value)) {
				if(cur->right == nullptr){
					cur->right = new BstNode<T>(value, cur);
					break;
				}
				cur = cur->right;
			} else {
				return false; // equal
			}
		}
		++size_;
		return true;
	}
	bool insert(T&& value){
                if(empty()){
                        root_ = new BstNode<T>(std::move(value), nullptr);
                        ++size_;
                        return true;
                }
                BstNode<T>* cur = root_;
                while(true){
                        if(comp_(value, cur->value)){
                                if(cur->left == nullptr){
                                        cur->left = new BstNode<T>(std::move(value), cur);
                                        break;
                                }
                                cur = cur->left;
                        } else if(comp_(cur->value ,value)) {
                                if(cur->right == nullptr){
                                        cur->right = new BstNode<T>(std::move(value), cur);
                                        break;
                                }
                                cur = cur->right;
                        } else {
                                return false; // equal
                        }
                }
                ++size_;
                return true;
	}
	template<typename... Args>
	T& emplace(Args&&... args){
		BstNode<T*> node = new BstNode<T>(nullptr, std::forward<Args>(args)...);
                if(empty()){
                        root_ = node;
                        ++size_;
                        return root_->value;
                }
                BstNode<T>* cur = root_;
                while(true){
                        if(comp_(node->value, cur->value)){
                                if(cur->left == nullptr){
                                        cur->left = node;
					node->parent = cur;
                                        ++size_;
					return cur->left->value;
                                }
                                cur = cur->left;
                        } else if(comp_(cur->value ,node->value)) {
                                if(cur->right == nullptr){
                                	cur->right = node;
					node->parent = cur;
                                	++size_;
					return cur->right->value;
                                }
                                cur = cur->right;
                        } else {
				delete node;
                                return cur->value; // equal
                        }
                }

	}
	bool erase(const T& value){
		BstNode<T>* cur = root_;
		while(cur!=nullptr){
			if(comp_(value, cur->value)){
				cur = cur->left;
			}else if(comp_(cur->value, value)){
				cur = cur->right;
			}else{
				erase_node(cur);
				--size_;
				return true;
			}
		}
		return false;
	}
	void clear() noexcept {
		BstNode<T>* node = root_;
		while(node!= nullptr){
		if(node->left == nullptr){
			BstNode<T>* tmp = node;
			node = node->right;
			delete tmp;
		}else{
			BstNode<T>* left = node->left;
			node->left = left->right;
			left->right = node;
			node = left;
		}
		}
		root_ = nullptr;
		size_ = 0;
	}

	// lookup
	bool contains(const T& value) const {
		BstNode<T>* cur = root_;
		while(cur != nullptr){
			if(comp_(value, cur->value)){
				cur = cur->left;
			} else if(comp_(cur->value, value)){
				cur = cur->right;
			} else {
				return true; // equal
			}
		}
		return false;
	}
	T* find(const T& value){
		BstNode<T>* cur = root_;
		while(cur != nullptr){
			if(comp_(value, cur->value)){
				cur = cur->left;
			} else if(comp_(cur->value, value)){
				cur = cur->right
			}else{
				return &cur->value;
			}
		}
		return nullptr;
	}

	const T* find(const T& value) const {
   		BstNode<T>* cur = root_;
    		while(cur != nullptr){
        		if(comp_(value, cur->value)){
            		cur = cur->left;
       	 		} else if(comp_(cur->value, value)){
            		cur = cur->right;
        		} else {
            		return &cur->value;
        		}
    		}
    		return nullptr;
	}

	// accessors
	bool empty() const noexcept { return size_ == 0; }
	std::size_t size() const noexcept { return size_; }
	const T& min() const {
		if(empty()) throw std::underflow_error("bst::min empty tree");
		BstNode<T>* cur = root_;
		while(cur->left != nullptr){
		cur = cur->left;
		}
		return cur->value;
	}
	const T& max() const {
		if(empty()) throw std::underflow_error("bst::max empty tree");
		BstNode<T>* cur = root_;
		while(cur->right != nullptr){
			cur = cur->right;
		}
		return cur->value;
	}
private:
	void erase_node(BstNode<T>* node){
		// case: two children - replace value with in-order successor,
		//		then remove the successor node instead which has <= 1 child (right one)
		if(node->left != nullptr && node->right != nullptr){
			BstNode<T>* successor = node->right;
			while(successor->left != nullptr){
			successor = successor->left;
			}
			node->value = successor->value;
			erase_node(successor); // successor has at most a right child
			return;
		}
		// case: at most one child - splice it up into node's position.
		BstNode<T>* child = (node->left != nullptr) ? node->left : node->right;
		if(child != nullptr){
			child->parent = node->parent; 
		}

		if(node->parent == nullptr){
			root_ = child;
		} else if(node->parent->left == node){
			node->parent->left = child;
		} else {
			node->parent->right = child;
		}
		delete node;
	}

	
	BstNode<T>* root_ = nullptr;
	std::size_t size_ = 0;
	Compare comp_{};
};

}
