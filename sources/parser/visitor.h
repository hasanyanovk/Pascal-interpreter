#ifndef __VISITOR_H__
#define __VISITOR_H__

#include "value.h"
#include "exceptions.h"
#include <PascalParserBaseVisitor.h>


using namespace ANTLRPascalParser;

struct Program{
	std::string program_name;
	std::stack<Value> stack;
	map<std::string, Value> vars;
};

class Visitor : public PascalParserBaseVisitor {

	map<std::string, std::function<void(std::vector<Value>&)>> functions;
public:
	Program program;
	Visitor();
	~Visitor() override = default;

	std::any call(const std::string& name, PascalParser::ParameterListContext *paramsCtx);

	std::any visitProgram(PascalParser::ProgramContext *ctx) override;
	std::any visitProgramHeading(PascalParser::ProgramHeadingContext *ctx) override;
	std::any visitBlock(PascalParser::BlockContext *ctx) override;

	std::any visitCompoundStatement(PascalParser::CompoundStatementContext *ctx) override;
	std::any visitStatements(PascalParser::StatementsContext *ctx) override;
	std::any visitStatement(PascalParser::StatementContext *ctx) override;
	std::any visitUnlabelledStatement(PascalParser::UnlabelledStatementContext *ctx) override;
	std::any visitAssignmentStatement(PascalParser::AssignmentStatementContext *ctx) override;

	std::any visitExpression(PascalParser::ExpressionContext *ctx) override;
	std::any visitSimpleExpression(PascalParser::SimpleExpressionContext *ctx) override;

	std::any visitTerm(PascalParser::TermContext *ctx) override;
	std::any visitFactor(PascalParser::FactorContext *ctx) override;
	std::any visitSignedFactor(PascalParser::SignedFactorContext *ctx) override;

	std::any visitType_(PascalParser::Type_Context *ctx) override;
	std::any visitUnsignedInteger(PascalParser::UnsignedIntegerContext *ctx) override;
	std::any visitUnsignedReal(PascalParser::UnsignedRealContext *ctx) override;
	std::any visitString(PascalParser::StringContext *ctx) override;
	std::any visitVariable(PascalParser::VariableContext *ctx) override;

	std::any visitIfStatement(PascalParser::IfStatementContext *ctx) override;
	std::any visitWhileStatement(PascalParser::WhileStatementContext *ctx) override;
	std::any visitForStatement(PascalParser::ForStatementContext *ctx) override;

	std::any visitProcedureStatement(PascalParser::ProcedureStatementContext *ctx) override;
	std::any visitFunctionDesignator(PascalParser::FunctionDesignatorContext *ctx) override;
	std::any visitParameterList(PascalParser::ParameterListContext *ctx) override;

};


#endif