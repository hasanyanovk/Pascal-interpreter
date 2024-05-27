#ifndef PASCAL_TMP_H
#define PASCAL_TMP_H

#include "list.h"
#include "exceptions.h"

template<class T>
class Stack {

	List<T> data;
public:
	Stack();
	Stack(const Stack& st);
	Stack(Stack&& st) noexcept;
	~Stack();
	important inline bool empty() const noexcept;
	important size_t size() const noexcept;
	important const T& peek() const;
	void clear();
	void push(const T& val);
	void push(T&& val);
	void push(std::initializer_list<T> list);
	T pop();

	Stack<T>& operator=(const Stack<T>& st);
	Stack<T>& operator=(Stack<T>&& st);
	bool operator==(const Stack<T>& st) const;
	bool operator!=(const Stack<T>& st) const;

	friend std::ostream& operator<<(std::ostream& os, const Stack& st){
		os << st.data;
		return os;
	}

	friend void swap(Stack<T>& lhs, Stack<int>& rhs){
		swap(lhs.data, rhs.data);
	}

};

template <class T>
Stack<T>::~Stack() = default;

template <class T>
Stack<T>::Stack() : data({}) {}

template <class T>
Stack<T>::Stack(const Stack<T>& st) { data = st.data;}

template <class T>
Stack<T>::Stack(Stack&& st) noexcept {
	swap(data, st.data);
}

template <class T>
bool Stack<T>::operator==(const Stack<T>& st) const{ return data == st.data;}

template <class T>
bool Stack<T>::operator!=(const Stack<T>& st) const{ return data != st.data;}

template <class T>
inline bool Stack<T>::empty() const noexcept { return data.empty(); }

template <class T>
size_t Stack<T>::size() const noexcept { return data.size(); }

template <class T>
void Stack<T>::push(const T& val) { data.push_back((val));}

template<class T>
void Stack<T>::push(T &&val) { data.push_back((val));}

template<class T>
void Stack<T>::push(std::initializer_list<T> list) {
	for(const auto& a : list)
		data.push_back((a));
}

template <class T>
T Stack<T>::pop() {
	if (empty()) [[unlikely]]
				throw empty_container(__FILE__, typeid(*this).name(), __LINE__, "Stack is empty");
	T res = data.back();
	data.pop_back();
	return res;
}

template <class T>
const T& Stack<T>::peek() const  { return data.back();}

template <class T>
Stack<T>& Stack<T>::operator=(const Stack& st) {
	data = st.data;
	return *this;
}

template<class T>
Stack<T> &Stack<T>::operator=(Stack<T> &&st) {
	data = std::move(st.data);
	return *this;
}

template<class T>
void Stack<T>::clear() {data.clear();}

#endif
