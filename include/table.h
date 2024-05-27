#ifndef __TABLE_H__
#define __TABLE_H__

#include <string>
#include <stdexcept>
#include <initializer_list>
#include <iterator>
#include <vector>

using namespace std;

template<class Key, class Value>
class Table {
	template<class K, class V>
	using TEntry = pair<K, V>;
	using Entry = TEntry<Key, Value>;
public:
	virtual void add(const Entry& elem) = 0;
	virtual void add(initializer_list<Entry> list) = 0;
	virtual void remove(const Key& key) = 0;
	virtual void remove(initializer_list<Key> list) = 0;
	[[nodiscard]] virtual Value* find(const Key& key) const = 0;
	virtual const Value& operator[](const Key& key) const = 0;
	virtual Value& operator[](const Key& key) = 0;
	[[nodiscard]] virtual size_t get_size() const = 0;
	[[nodiscard]] virtual bool contains(const Key& key) const = 0;

protected:
	virtual void print(ostream& os) = 0;
public:
	friend ostream& operator<<(ostream& os, Table& t) {
		t.print(os);
		return os;
	}
	virtual ~Table()  = default;
};

namespace std {
	template <>
	struct hash<string> {
		size_t k = 1;
		size_t operator()(const string& x) const {
			size_t p = 1, res = 0;
			for (auto& ch : x) {
				res += p * ch;
				p += k;
			}
			return res;
		}
	};
}
#endif