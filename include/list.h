#ifndef __LIST_H__
#define __LIST_H__

#define important [[nodiscard]]

#include <initializer_list>
#include <iostream>

template <class Type>
class List{
	struct Node {
	Type data;
	Node* next;
	};
private:
	Node* first, *last;
	size_t _size;
public:
	List();
	List(std::initializer_list<Type> list);
	List(const List& other);
	List(List&& moved) noexcept;
	~List();

	void push_back(const Type& value);
	void push_back(Type&& value);
	void push_back(std::initializer_list<Type> list);
	void push_front(const Type& value);
	void push_front(Type&& value);
	void push_front(std::initializer_list<Type> list);

	void pop_front();
	void pop_back();

	important const Type& front() const;
	important const Type& back() const;
	important inline size_t size() const noexcept;
	important inline bool empty() const noexcept;
	important inline bool contains(const Type& value);
	void clear();

	important bool operator==(const List& other) const;
	important bool operator!=(const List& other) const;
	List<Type>& operator=(const List& other);
	List<Type>& operator=(List&& moved) noexcept;
	important Type& operator[](int id);
	important const Type& operator[](int id) const;

	template<class T>
	class iterator {
		Node* cur;
		explicit iterator(Node* node);
		friend class List<Type>;
	public:

		iterator& operator++();
		const iterator operator++(int);
		//iterator& operator--();
		//iterator operator--(int);
		iterator operator+(int i);
		//iterator operator-(int i);

		bool operator==(const iterator& it) const;
		bool operator!=(const iterator& it) const;
		operator bool() const;

		T& operator*() const;
		T* operator->() const;
	};

	iterator<Type> begin();
	iterator<Type> end();
	iterator<const Type> begin() const;
	iterator<const Type> end() const;
	iterator<const Type> cbegin() const;
	iterator<const Type> cend() const;
	iterator<Type> find(const Type& value);

	void insert(iterator<Type> it, const Type& value);
	void insert(iterator<Type> start, std::initializer_list<Type> list);
	void erase(iterator<Type> it);
	void erase(iterator<Type> start, iterator<Type> finish);
	void remove(const Type& value);
	void remove(Type&& value);

	//---------------friend-functions--------------------
	friend void swap(List<Type>& lhs,  List<Type>& rhs) {
		std::swap(lhs.first, rhs.first);
		std::swap(lhs.last, rhs.last);
		std::swap(lhs._size, rhs._size);
	}

	friend std::ostream& operator<<(std::ostream& os, const List<Type>& list){
		if(list.empty())
			os << "{}";
		else {
			os << '{';
			auto it = list.begin();
			os << *it;
			while (++it) {
				os << " -> " << *it;
			}
			os << '}';
		}
		return os;
	}
};

//---------------iterators--------------------
template<class Type>
template<class T>
List<Type>::iterator<T>::iterator(List::Node *node) : cur(node) {}

template<class Type>
template<class T>
typename List<Type>::template iterator<T> &List<Type>::iterator<T>::operator++() {
	cur = cur->next;
	return *this;
}

template<class Type>
template<class T>
const typename List<Type>::template iterator<T> List<Type>::iterator<T>::operator++(int) {
	iterator<T> tmp{*this};
	++(*this);
	return tmp;
}

template<class Type>
template<class T>
T& List<Type>::iterator<T>::operator*() const { return cur->data;}

template<class Type>
template<class T>
T* List<Type>::iterator<T>::operator->() const { return &cur->data;}

template<class Type>
template<class T>
bool List<Type>::iterator<T>::operator==(const List<Type>::iterator<T> &it) const {return cur == it.cur;}

template<class Type>
template<class T>
bool List<Type>::iterator<T>::operator!=(const List<Type>::iterator<T> &it) const {return cur != it.cur;}

template<class Type>
template<class T>
List<Type>::iterator<T>::operator bool() const { return cur != nullptr;}

template<class Type>
template<class T>
typename List<Type>::template iterator<T> List<Type>::iterator<T>::operator+(int i) {
	auto res = iterator<Type>{cur};
	for(int k = 0; res && k < i; k++, ++res);
	return res;
}


//---------------constructors--------------------
template<class Type>
List<Type>::List() : first(nullptr), last(nullptr), _size(0) {}

template<class Type>
List<Type>::List(std::initializer_list<Type> list) : first(nullptr), last(nullptr), _size(0) {
	for(const auto& val : list) push_back(val);
}

template<class Type>
List<Type>::List(const List &other) : _size(other._size) {
	if(this == &other) [[unlikely]] return;
	for(auto& val : other) push_back(val);
}

template<class Type>
List<Type>::List(List &&moved) noexcept : _size(moved._size), first(moved.first), last(moved.last) {
	moved.first = nullptr;
	moved.last = nullptr;
}

template<class Type>
List<Type>::~List() {this->clear();}

//---------------push/pop--------------------
template<class Type>
void List<Type>::clear() {
	while(_size > 0) pop_front();
}

template<class Type>
void List<Type>::push_back(const Type &value) {
	Node* node = new Node{value, nullptr};
	if(_size == 0) first = last = node;
	else {
		last->next = node;
		last = node;
	}
	_size++;
}

template<class Type>
void List<Type>::push_back(Type &&value) {
	Node* node = new Node{value, nullptr};
	if(_size == 0) first = last = node;
	else {
		last->next = node;
		last = node;
	}
	_size++;
}

template<class Type>
void List<Type>::push_back(std::initializer_list<Type> list) {
for(auto& val : list) push_back(val);
}

template<class Type>
void List<Type>::push_front(const Type &value) {
	Node* node = new Node{value, this->first};
	first = node;
	if(_size == 0) last = node;
	_size++;
}

template<class Type>
void List<Type>::push_front(Type &&value) {
	Node* node = new Node{value, this->first};
	first = node;
	if(_size == 0) last = node;
	_size++;
}

template<class Type>
void List<Type>::push_front(std::initializer_list<Type> list) {
	for(auto& val : list) push_front(val);
}

template<class Type>
void List<Type>::pop_back() {
	if(_size == 0) return;
	else if(_size == 1){
		delete last;
		first = last = nullptr;
		_size--;
		return;
	}
	Node* tmp = first;
	while(tmp->next->next)
		tmp = tmp->next;
	delete last;
	tmp->next = nullptr;
	last = tmp;
	_size--;
}

template<class Type>
void List<Type>::pop_front() {
	Node* tmp = first;
	first = tmp->next;
	delete tmp;
	_size--;
}

//---------------getters--------------------
template<class Type>
size_t List<Type>::size() const noexcept { return _size;}

template<class Type>
bool List<Type>::empty() const noexcept { return _size == 0;}

template<class Type>
const Type &List<Type>::front() const { return first->data;}

template<class Type>
const Type &List<Type>::back() const { return last->data;}

template<class Type>
bool List<Type>::contains(const Type &value) { return (bool)find(value);}

//---------------operators--------------------
template<class Type>
bool List<Type>::operator==(const List &other) const {
	if(this == &other) return true;
	if(_size != other._size) return false;
	Node* lhs{first}, *rhs{other.first};
	while(lhs) {
		if(lhs->data != rhs->data)
			return false;
		lhs = lhs->next;
		rhs = rhs->next;
	}
	return true;
}

template<class Type>
bool List<Type>::operator!=(const List &other) const {return !(other == *this);}

template<class Type>
List<Type> &List<Type>::operator=(const List& other) {
	if(this == &other) return *this;
	clear();
	for(auto val : other)
		push_back(val);
	return *this;
}

template<class Type>
List<Type> &List<Type>::operator=(List&& moved) noexcept {
	first = moved.first;
	last = moved.last;
	_size = std::move(moved._size);
	moved.last = nullptr;
	moved.first = nullptr;
	return *this;
}

template<class Type>
Type &List<Type>::operator[](int id) {
	if(id < 0 || id > _size) [[unlikely]] throw std::out_of_range("list index out of range");
	auto it = begin();
	return *(it + id);
}

template<class Type>
const Type &List<Type>::operator[](int id) const {
	if(id < 0 || id > _size) [[unlikely]] throw std::out_of_range("list index out of range");
	auto it = begin();
	return *(it + id);
}


//---------------data--------------------
template<class Type>
typename List<Type>::template iterator<Type> List<Type>::begin() { return iterator<Type>{first}; }

template<class Type>
typename List<Type>::template iterator<Type> List<Type>::end() { return iterator<Type>{nullptr}; }

template<class Type>
typename List<Type>::template iterator<const Type> List<Type>::begin() const { return iterator<const Type>{first}; }

template<class Type>
typename List<Type>::template iterator<const Type> List<Type>::end() const { return iterator<const Type>{nullptr}; }

template<class Type>
typename List<Type>::template iterator<const Type> List<Type>::cbegin() const {return iterator<const Type>{first};}

template<class Type>
typename List<Type>::template iterator<const Type> List<Type>::cend() const { return iterator<const Type>{nullptr}; }

template<class Type>
typename List<Type>::template iterator<Type> List<Type>::find(const Type &value) {
	auto it = iterator<Type>{first};
	while(it != iterator<Type>{nullptr} && *it != value)
		++it;
	return it;
}

//-------------------insert/erase-------------
template<class Type>
void List<Type>::insert(List::iterator<Type> it, const Type &value) {
	Node* node = new Node{value,it.cur->next};
	it.cur->next = node;
	_size++;
}

template<class Type>
void List<Type>::insert(List::iterator<Type> start, std::initializer_list<Type> list) {
	for(auto& val : list){
		insert(start, val);
		++start;
	}
}

template<class Type>
void List<Type>::erase(List::iterator<Type> it) {
	if(_size == 0 || !it.cur) return;
	else if(it.cur == first){
		pop_front();
		return;
	}
	else if(it.cur == last){
		pop_back();
		return;
	}
	auto left = begin();
	while(left.cur->next != it.cur) ++left;
	left.cur->next = it.cur->next;
	delete it.cur;
	_size--;
}

template<class Type>
void List<Type>::erase(List::iterator<Type> start, List::iterator<Type> finish) {
	if(!start.cur) return;
	while(start != finish) {
		auto tmp = start;
		++start;
		this->erase(tmp);
	}
}

template<class Type>
void List<Type>::remove(const Type &value) {
	for(auto it = begin(); it;){
		if(*it == value) {
			auto tmp = it;
			++it;
			this->erase(tmp);
			continue;
		}
		++it;
	}
}

template<class Type>
void List<Type>::remove(Type&& value) {
	for(auto it = begin(); it;){
		if(*it == value) {
			auto tmp = it;
			++it;
			this->erase(tmp);
			continue;
		}
		++it;
	}
}

//------------------------end----------------------------------

#endif
