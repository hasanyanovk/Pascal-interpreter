#ifndef __VALUE_H__
#define __VALUE_H__

#include <string>
#include <map>
#include <iostream>
#include <functional>

using byte_t = unsigned char;

namespace interpreter {
	enum class DataType : byte_t {
		Null,
		Reference,
		Double,
		Real,
		Extended, //long double
		Byte, //uchar
		Word, //short
		Integer,
		Boolean,
		String,
		Char,
	};


	enum class TypeGroup : byte_t {
		Numeric,
		String,
		Real,
		Any

	};

	bool in_group(DataType type, TypeGroup group);
}

using namespace interpreter;



class Value {

private:
	[[maybe_unused]] std::string _name;
	 const bool _const = false;
	 DataType _type;
public:
	union {
		int int_ptr;
		long double double_ptr;
		bool bool_ptr;
		std::string* string_ptr;
		char char_ptr;
	} value = {} ;

public:

	Value();
	Value(const Value& v);
	explicit Value(DataType type, const void* p = nullptr, bool is_const = false);
	explicit Value(const std::string& name, DataType type, const void* p = nullptr, bool is_const = false);
	Value(Value&& moved) noexcept;
	~Value();
	operator bool() const noexcept;
	[[nodiscard]] bool is_const() const noexcept;
	[[nodiscard]] DataType type() const noexcept;
	void init_type(const void* ptr);
	[[nodiscard]] int cmp(const Value& other) const noexcept;
	[[nodiscard]] std::string name() const;

	Value operator+(const Value& other) const;
	Value operator-(const Value& other) const;
	Value operator*(const Value& other) const;
	Value operator/(const Value& other) const;
	Value operator%(const Value& other) const;

	Value operator-() const;

	Value& operator=(const Value& other);
	Value& operator=(Value&& other) noexcept;

	bool operator==(const Value& other) const noexcept;
	bool operator!=(const Value& other) const noexcept;

	bool operator< (const Value& other) const noexcept;
	bool operator<=(const Value& other) const noexcept;
	bool operator>=(const Value& other) const noexcept;
	bool operator> (const Value& other) const noexcept;

	friend std::ostream& operator<<(std::ostream& os, const Value& v) {
		switch(v._type) {
			case DataType::Integer:
				os << v.value.int_ptr;
				break;
			case DataType::Double:
			case DataType::Real:
				os << v.value.double_ptr;
				break;
			case DataType::String:
				os << *v.value.string_ptr;
				break;
		}
		return os;
	}

};

namespace  interpreter {

const std::map<std::string, std::function<bool(const Value &, const Value &)>> compares = {
		{"=",  std::equal_to<>{}},
		{"<>", std::not_equal_to<>{}},
		{"<",  std::less<>{}},
		{"<=", std::less_equal<>{}},
		{">=", std::greater_equal<>{}},
		{">",  std::greater<>{}},
};

const std::map<std::string, DataType> types = {
		{"boolean",  DataType::Boolean},
		{"char",     DataType::Char},
		{"integer",  DataType::Integer},
		{"word",     DataType::Word},
		{"byte",     DataType::Byte},
		{"real",     DataType::Real},
		{"double",   DataType::Double},
		{"extended", DataType::Extended},
		{"string",   DataType::String},
};

}

#endif
