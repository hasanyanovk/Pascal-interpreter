#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include <exception>
#include <string>

using namespace std;

class base_exception : public std::exception {
protected:
    string info;

public:
	explicit base_exception(const string& fileName = "Undeclared file"
			, const string& className = "Undeclared  class"
			, int numLine = -1
			, const string& erInfo = "Undeclared error");

    [[nodiscard]] char const* what() const override;

    ~base_exception() override = default;
};

class invalid_size : public base_exception {
public:
	invalid_size(const string& fileName, const string& className, int numLine,
				 const string& erInfo = "Invalid size");
};

class empty_container : public base_exception {
public:
	empty_container(const string& fileName, const string& className, int numLine,
					const string& erInfo = "Container is empty");
};

class bad_allocate : public base_exception {
public:
	bad_allocate(const string& fileName, const string& className, int numLine,
				 const string& erInfo = "Can`t allocate memory");
};

class key_error : public base_exception {
public:
	key_error(const string& fileName, const string& className, int numLine,
			  const string& erInfo = "Key error");
};

class parse_error :  public base_exception{
public:
	parse_error(const string& fileName, const string& className, int numLine,
				const string& erInfo = "Parse error");
};

#endif