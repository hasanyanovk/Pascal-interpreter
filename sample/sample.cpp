#include <iostream>

#include <antlr4-runtime.h>
#include <tree/IterativeParseTreeWalker.h>

#include "pascal_parser.h"

using std::cout;
using std::cin;
using std::endl;

int main(){


	std::string def = "../sample/", path;
	std::cout << "Enter path:  ";
	getline(std::cin, path);
	std::stringstream ss;
	std::ifstream fin(def + path);
	ss << fin.rdbuf();
	fin.close();
	std::cout << "================Input================" << std::endl << ss.str() << std::endl;
	std::cout << "================Output================" << std::endl;
	Interpretator::run(ss.str());

    return 0;
}
