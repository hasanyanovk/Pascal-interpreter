#include "exceptions.h"

base_exception::base_exception(const string &fileName, const string &className, int numLine, const string &erInfo)
		: info("File: " + fileName + "\n"
			   + "Class: " + className + "\n"
			   + "Line: " + to_string(numLine) + "\n"
			   + "Error: " + erInfo + "\n") {}

char const *base_exception::what() const { return info.c_str(); }


invalid_size::invalid_size(const string &fileName, const string &className, int numLine, const string &erInfo)
		: base_exception(fileName, className, numLine, erInfo) {}

empty_container::empty_container(const string &fileName, const string &className, int numLine, const string &erInfo)
		: base_exception(fileName, className, numLine, erInfo) {}

bad_allocate::bad_allocate(const string &fileName, const string &className, int numLine, const string &erInfo)
		: base_exception(fileName, className, numLine, erInfo) {}

key_error::key_error(const string &fileName, const string &className, int numLine, const string &erInfo)
		: base_exception(fileName, className, numLine, erInfo) {}

parse_error::parse_error(const string &fileName, const string &className, int numLine, const string &erInfo)
		: base_exception(fileName, className, numLine, erInfo) {}
