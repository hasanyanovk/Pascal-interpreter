#include "value.h"
#include <stdexcept>

Value::Value() : Value(DataType::Null, nullptr, false) {}

Value::Value(DataType type, const void* p, bool is_const) : _type(type), _const(is_const)  {
	if (type == DataType::String)
		value.string_ptr = new std::string();
	else if (type == DataType::Null || type == DataType::Reference || !p)  return;
	init_type(p);
}

Value::Value(const std::string& name, DataType type, const void* p, bool is_const) :_name(name), _type(type), _const(is_const)  {
	if (type == DataType::String)
		value.string_ptr = new std::string();
	else if (type == DataType::Null || type == DataType::Reference || !p)  return;
	init_type(p);
}


Value::Value(const Value &v) : _name(v._name),_type(v._type), value(v.value), _const(v._const) {
	if(_type == DataType::String)
		value.string_ptr = new std::string(*v.value.string_ptr);
}

Value::Value(Value &&moved) noexcept{
	std::swap(_type, moved._type);
	std::swap(value, moved.value);
	std::swap(_name, moved._name);
}

Value::~Value() {
	if (_type == DataType::String)
		delete value.string_ptr;
}

DataType Value::type() const noexcept { return _type; }

Value::operator bool() const noexcept { return _type == DataType::Null; }

void Value::init_type(const void *val) {
	switch(_type) {
		case DataType::Integer:
			 value.int_ptr =  *static_cast<const int *>(val) ;
			break;
		case DataType::Boolean:
			value.bool_ptr =  *static_cast<const bool *>(val);
			break;
		case DataType::Double:
		case DataType::Real:
			value.double_ptr =*static_cast<const double *>(val) ;
			break;
		case DataType::String:
			*value.string_ptr = val ? *static_cast<const std::string *>(val) : " ";
			break;
		case DataType::Null:
			throw std::runtime_error("can't identificate type");
	}
}

[[nodiscard]] bool interpreter::in_group(DataType type, TypeGroup group) {
	switch(group) {
		case TypeGroup::Any: return true;
		case TypeGroup::Numeric:
			return type == DataType::Char
			|| type == DataType::Integer
			|| type == DataType::Word
			|| type == DataType::Byte;
		case TypeGroup::Real:
			return type == DataType::Real
			|| type == DataType::Double
			|| type == DataType::Extended;
		case TypeGroup::String:
			return type == DataType::String;
		default: throw std::runtime_error("Unknown type");
	}
}

Value& Value::operator=(Value&& other) noexcept {
	std::swap(_type, other._type);
	std::swap(value, other.value);
	return *this;
}

Value& Value::operator=(const Value& other)  {
	_type = other._type;
	if(_type == DataType::String){
		*value.string_ptr = *other.value.string_ptr;
	}
	value = other.value;
	return *this;
}

bool Value::is_const() const noexcept { return _const;}


int Value::cmp(const Value& other) const noexcept {
	return std::memcmp(&value, &other.value, sizeof(value));
}

template<typename C>
[[nodiscard]] bool xcmp(const Value& lhs, const Value& rhs, C cfn) noexcept {
	if (lhs.type() != rhs.type()) {
		return false;
	}

	if (lhs.type() == DataType::String) {
		return cfn(*lhs.value.string_ptr, *rhs.value.string_ptr);
	}

	return cfn(lhs.cmp(rhs), 0);
}

bool Value::operator==(const Value& other) const noexcept {
	return xcmp(*this, other, std::equal_to{});
}
bool Value::operator!=(const Value& other) const noexcept {
	return xcmp(*this, other, std::not_equal_to{});
}
bool Value::operator<(const Value& other) const noexcept {
	return xcmp(*this, other, std::less{});
}
bool Value::operator<=(const Value& other) const noexcept {
	return xcmp(*this, other, std::less_equal{});
}
bool Value::operator>=(const Value& other) const noexcept {
	return xcmp(*this, other, std::greater_equal{});
}
bool Value::operator>(const Value& other) const noexcept {
	return xcmp(*this, other, std::greater{});
}


template<typename BinaryOp>
[[nodiscard]] Value binop(const Value& lhs, const Value& rhs, BinaryOp op, bool to_real = false) {

	if (in_group(lhs.type(), TypeGroup::Numeric) && in_group(rhs.type(), TypeGroup::Numeric)) {
		if (to_real) {
			Value res(DataType::Double);
			res.value.double_ptr = op(lhs.value.int_ptr, rhs.value.int_ptr);
			return res;
		}
		Value res(DataType::Integer);
		res.value.int_ptr = op(lhs.value.int_ptr, rhs.value.int_ptr);
		return res;
	}

	Value res(DataType::Double);
	if (in_group(lhs.type(), TypeGroup::Real) && in_group(rhs.type(), TypeGroup::Real)) {
		res.value.double_ptr = op(lhs.value.double_ptr, rhs.value.double_ptr);
	}
	else if (in_group(lhs.type(), TypeGroup::Numeric) && in_group(rhs.type(), TypeGroup::Real)) {
		res.value.double_ptr = op(lhs.value.int_ptr, rhs.value.double_ptr);
	}
	else if (in_group(lhs.type(), TypeGroup::Real) && in_group(rhs.type(), TypeGroup::Numeric)) {
		res.value.double_ptr = op(lhs.value.double_ptr, rhs.value.int_ptr);
	}
	else {
		throw std::runtime_error("TODO");
	}
	return res;
}

double divide_real(int lhs, int rhs) {
	return static_cast<double>(lhs) / rhs;
}

Value Value::operator+(const Value& other) const {
	if (_type == DataType::String) {
		Value res(_type);
		res.value.string_ptr = new std::string(*value.string_ptr + *other.value.string_ptr);
		return res;
	}
	return binop(*this, other, std::plus<>{});
}

Value Value::operator-(const Value& other) const {
	return binop(*this, other, std::minus<>{});
}

Value Value::operator*(const Value& other) const {
	if (_type == DataType::String) {
		throw std::runtime_error("This operation is applicable to numeric values only");
	}
	return binop(*this, other, std::multiplies<>{});
}

Value Value::operator/(const Value& other) const {
	if (_type == DataType::String) {
		throw std::runtime_error("This operation is applicable to numeric values only");
	}
	if (other.value.double_ptr == 0) {
		throw std::runtime_error("Division by zero");
	}
	if(in_group(_type, TypeGroup::Real)) return binop(*this, other, divide_real, true);
	else return binop(*this, other, divide_real, false);
}

Value Value::operator%(const Value& other) const {
	if (_type != DataType::Integer) {
		throw std::runtime_error("This operation is applicable to integers only");
	}
	Value res(_type);
	res.value.int_ptr = value.int_ptr % other.value.int_ptr;
	return res;
}

Value Value::operator-() const {
	if (_type == DataType::String) {
		throw std::runtime_error("This operation is applicable only to numeric values");
	}

	return binop(Value(_type, nullptr), *this, std::minus<>{});
}

std::string Value::name() const  { return _name;}


