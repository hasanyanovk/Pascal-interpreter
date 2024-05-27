#include <iostream>
#include <tree/IterativeParseTreeWalker.h>

#include "pascal_parser.h"

using std::cout;
using std::cin;
using std::endl;

int main(){
	std::string def = "../examples/", path;
	std::cout << "Enter path:  ";
	getline(std::cin, path);
	std::stringstream ss;
	std::ifstream fin(def + path);
	while(!fin.is_open()) {
		std::cout << "No such file" << std::endl;
		std::cout << "Enter path:  ";
		getline(std::cin, path);
		fin.open(def + path);
	}
	ss << fin.rdbuf();
	fin.close();
	std::cout << "================Input================" << std::endl << ss.str() << std::endl;
	std::cout << "================Output===============" << std::endl;
	Runtime::run(ss.str());

    return 0;
}
