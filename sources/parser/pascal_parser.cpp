#include <iostream>
#include "pascal_parser.h"
#include <antlr4-runtime.h>
#include <tree/IterativeParseTreeWalker.h>

#include <PascalLexer.h>
#include <PascalParser.h>
#include "visitor.h"

using namespace antlr4;
using namespace ANTLRPascalParser;

void ::Interpretator::run(const std::string& infix) {
	ANTLRInputStream input(infix);
	PascalLexer lexer(&input);
	CommonTokenStream tokens(&lexer);
	PascalParser parser(&tokens);

	PascalParser::ProgramContext* tree;
	try{
		tree = parser.program();
	} catch(ParseCancellationException& e){
		std::cerr << e.what();
		exit(1);
	}
	Visitor visitor;
	try {
		visitor.visit(tree);
	} catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	std::cout << "================Dumping variables================" << std::endl;
	for(auto& a : visitor.program.vars) {
		if(a.second.is_const()) std::cout << "const ";
		cout << a.first << " = " << a.second << endl;
	}
}
