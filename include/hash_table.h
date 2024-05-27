#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

using namespace std;

#include "table.h"
#include "list.h"
#include "exceptions.h"


template<class K, class V>
class HashTable : public Table<K, V> {
	template<class Key, class Value>
	using TEntry = pair<Key, Value>;
	using Entry = TEntry<K, V>;

	enum properties : size_t { default_capacity = 50, default_hash_coefficient = 1 };

	size_t hash_coefficient;
	std::vector<List<Entry>> data;
	size_t capacity;
	size_t size;
protected:
	void print(ostream& os) override;
	void rehash();
public:
	HashTable();
	explicit HashTable(size_t capacity);
	HashTable(size_t capacity, size_t coef);
	HashTable(const HashTable& table);
	HashTable(HashTable&& table) noexcept;

	[[nodiscard]] inline size_t hash(const K& key) const;

	void add(const Entry& elem) override;
	void add(initializer_list<Entry> list) override;
	void remove(const K& key) override;
	void remove(initializer_list<K> list) override;
	void emplace(const K& key, const V& value);
	[[nodiscard]] V* find(const K& key) const override;
	[[nodiscard]] bool contains(const K& key) const override;

	[[nodiscard]] const V& operator[](const K& key) const override;
	[[nodiscard]] V& operator[](const K& key) override;
	[[nodiscard]] bool operator==(const HashTable& table) const;
	[[nodiscard]] bool operator!=(const HashTable& table) const;
	HashTable<K, V>& operator=(const HashTable& table);
	HashTable<K, V>& operator=(HashTable&& table) noexcept;

	[[nodiscard]] size_t get_size() const override;

	friend void swap(HashTable<K, V>& lhs, HashTable<K, V>& rhs) {
		swap(lhs.hash_coefficient, rhs.hash_coefficient);
		swap(lhs.capacity, rhs.capacity);
		swap(lhs.size, rhs.size);
		swap(lhs.data, rhs.data);
	}

	template<class U>
	class Iterator {
		friend class HashTable;

		HashTable& table;
		typename vector<List<TEntry<U, V>>>::iterator IT;
		typename List<TEntry<U, V>>::template iterator<TEntry<U, V>> it_list;
		typename List<TEntry<U, V>>::template iterator<TEntry<U, V>> it_list_end;

		static typename vector<List<TEntry<U, V>>>::iterator prepare(typename vector<List<TEntry<U, V>>>::iterator IT) {
			while (IT->empty()) IT++;
			return IT;
		}

		Iterator(HashTable<U, V>& t, [[maybe_unused]] bool is_end)
				: table(t)
				, IT(--table.data.end())
				, it_list(IT->end())
				, it_list_end(IT->end()) {}
	public:
		explicit Iterator(HashTable<U, V>& t)
				: table(t)
				, IT(prepare(table.data.begin()))
				, it_list(IT->begin())
				, it_list_end(IT->end()) {}

		Iterator& operator++() {
			++it_list;
			while (it_list == IT->end() && ++IT != table.data.end()) {
				it_list = IT->begin();
				it_list_end = IT->end();
			}
			return *this;
		}

		Iterator operator++(int) {
			Iterator<K> tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator!=(const Iterator& other) const { return it_list != other.it_list; }
		bool operator==(const Iterator& other) const { return it_list == other.it_list; }

		U& operator*() const { return it_list->first; }
		U* operator->() const { return &it_list->first; }
	};
	Iterator<K> begin() { return Iterator<K>(*this); }
	Iterator<K> end() { return Iterator<K>(*this, true); }
	Iterator<const K> begin() const { return Iterator<K>(*this); }
	Iterator<const K> end()  const { return Iterator<K>(*this, true); }
	Iterator<const K> cbegin() const { return Iterator<K>(*this); }
	Iterator<const K> cend() const { return Iterator<K>(*this, true); }

	friend ostream& operator<<(ostream& os, const HashTable<K, V>& t) {
		os << "#\t" << "Key:\t" << "Value:\n";
		size_t i = 1;
		for (auto& a : t) {
			os << i << '\t' << a << '\t' << t[a] << "\n";
			i++;
		}
		return os;
	}
};

template<class K, class V>
void HashTable<K, V>::print(ostream& os) {
	os << "#\t" << "Key:\t" << "Value:" << endl;
	size_t i = 1;
	for (auto& a : data)
		for (auto& d : a) {
			os << i << "\t" << d.first << "\t" << d.second << endl;
			i++;
		}
}

template<class K, class V>
void HashTable<K, V>::rehash() {
	HashTable<K, V> tmp(capacity * 2, hash_coefficient * 2);
	for (auto& a : data)
		for (auto& p : a)
			tmp.add(p);
	*this = std::move(tmp);

}

template<class K, class V>
inline HashTable<K, V>::HashTable() : HashTable(default_capacity, default_hash_coefficient) {}

template<class K, class V>
inline HashTable<K, V>::HashTable(size_t capacity) : HashTable(capacity, default_hash_coefficient) {}

template<class K, class V>
inline HashTable<K, V>::HashTable(size_t capacity, size_t coef) : hash_coefficient(coef), size(0), capacity(capacity) {
	if (capacity <= 0) throw invalid_size(__FILE__, typeid(*this).name(), __LINE__, "size should be greater than zero");
	data.resize(capacity);
	this->capacity = data.capacity();
}

template<class K, class V>
inline HashTable<K, V>::HashTable(const HashTable& table) :
		hash_coefficient(table.hash_coefficient),
		capacity(table.capacity),
		size(table.size), data(table.data) {}

template<class K, class V>
inline HashTable<K, V>::HashTable(HashTable&& table) noexcept
		: size(std::move(table.size))
		, capacity(std::move(table.capacity))
		, hash_coefficient(std::move(table.hash_coefficient))
		, data(std::move(table.data))
{
	table.size = 0;
}

template<class K, class V>
inline size_t HashTable<K, V>::hash(const K& key) const {
	return std::hash<K>{hash_coefficient}(key) % capacity;
}


template<class K, class V>
inline void HashTable<K, V>::add(const Entry& elem) {
	if (find(elem.first)) throw key_error(__FILE__, typeid(*this).name(),
										  __LINE__, "Key already used");
	if (size >= capacity / 2) rehash();
	data[hash(elem.first)].push_back(elem);
	size++;
}

template<class K, class V>
inline void HashTable<K, V>::add(initializer_list<Entry> list) { for (auto& el : list) add(el); }

template<class K, class V>
inline void HashTable<K, V>::remove(const K& key) { //rework
	size_t h = hash(key);
	int i = 0;
	for (auto& a : data[h]) {
		if (a.first == key) {
			data[h].erase(data[h].begin() + i);
			return;
		}
		i++;
	}
	size--;
}

template<class K, class V>
inline void HashTable<K, V>::remove(initializer_list<K> list) { for (auto& el : list) remove(el); }

template<class K, class V>
inline V* HashTable<K, V>::find(const K& key) const {
	size_t id = hash(key);
	if (data[id].empty()) return nullptr;
	for (auto& a : data[id])
		if (a.first == key) return const_cast<V*>(&a.second);
	return nullptr;
}

template<class K, class V>
bool HashTable<K, V>::contains(const K& key) const { return (bool)find(key); }

template<class K, class V>
const V& HashTable<K, V>::operator[](const K& key) const {
	V* val = find(key);
	if (!val) throw key_error(__FILE__, typeid(*this).name(), __LINE__, "No elements with this key");
	return *val;
}

template<class K, class V>
V& HashTable<K, V>::operator[](const K& key) {
	V* val = find(key);
	if (!val) throw key_error(__FILE__, typeid(*this).name(), __LINE__, "No elements with this key");
	return *val;
}

template<class K, class V>
bool HashTable<K, V>::operator==(const HashTable& table) const { return data == table.data; }

template<class K, class V>
bool HashTable<K, V>::operator!=(const HashTable& table) const { return !(*this == table); }

template<class K, class V>
HashTable<K, V>& HashTable<K, V>::operator=(const HashTable& table) {
	hash_coefficient = table.hash_coefficient;
	capacity = table.capacity;
	size = table.size;
	data = table.data;
	return *this;
}

template<class K, class V>
HashTable<K, V>& HashTable<K, V>::operator=(HashTable&& table) noexcept {
	hash_coefficient = move(table.hash_coefficient);
	capacity = move(table.capacity);
	size = move(table.size);
	data = move(table.data);
	table.size = 0;
	return *this;
}

template<class K, class V>
inline size_t HashTable<K, V>::get_size() const { return size; }

template<class K, class V>
void HashTable<K, V>::emplace(const K& key, const V& value) { add(std::make_pair(key, value)); }

#endif 


