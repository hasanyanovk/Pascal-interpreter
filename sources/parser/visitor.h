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
	map<std::string, std::function<void(std::vector<Value>&)>> functions = {
			{"Writeln", [](std::vector<Value>& args) {
				for(auto& a : args)
					if(a.type() == DataType::String)
						std::cout << a.value.string_ptr->substr(1,a.value.string_ptr->size()-2) << " ";
					else std::cout << a << " ";
				std::cout << std::endl;
			}},

			{"Write", [](std::vector<Value>& args) {
				for(auto& a : args)
					if(a.type() == DataType::String)
						std::cout << a.value.string_ptr->substr(1,a.value.string_ptr->size()-2) << " ";
					else std::cout << a << " ";
			}},

			{"Read", [&](std::vector<Value>& args) {
				for(auto& a : args){
					if(program.vars.find(a.name()) == program.vars.end())
						throw parse_error(__FILE__, typeid(*this).name(), __LINE__, "Undeclared identifier: " + a.name());

					std::string tmp;
					getline(std::cin, tmp);
					if(a.type() ==  DataType::Integer) {
						int v1 = std::stoi(tmp);
						program.vars[a.name()].init_type(&v1);
					} else if(a.type() == DataType::Double || a.type() == DataType::Real) {
						double v2 = std::stod(tmp);
						program.vars[a.name()].init_type(&v2);
					}
					else if(a.type() == DataType::String)
						program.vars[a.name()].init_type(&tmp);

				}

			}},
	};
public:
	Program program;

	std::any call(const std::string& name, PascalParser::ParameterListContext *paramsCtx) {
		const auto it = functions.find(name);
		if(it == functions.end())
			throw parse_error(__FILE__, typeid(*this).name(), __LINE__, "Unknown function");

		const auto& func = it->second;
		//std::cerr << visitParameterList(paramsCtx).type().name();
		auto args = std::any_cast<std::vector<Value>>(visitParameterList(paramsCtx));

		func(args);
		return{};
	}

	std::any visitProgram(PascalParser::ProgramContext *ctx) override{
		visitProgramHeading(ctx->programHeading());
		visitBlock(ctx->block());
		return {};
	}

	std::any visitProgramHeading(PascalParser::ProgramHeadingContext *ctx) override {
		program.program_name = ctx->identifier()->getText();
		return {};
	}

	std::any visitBlock(PascalParser::BlockContext *ctx) override {
		for (const auto& part : ctx->constantDefinitionPart()) {
			for (const auto& def : part->constantDefinition()) {
				const auto& name = def->identifier()->getText();
				auto val = def->constant()->getText();
				auto type = std::any_cast<DataType>(visitType_(def->type_()));
				auto res = Value{ type,nullptr, true};
				if (in_group(type, TypeGroup::Numeric)) {
					int v = std::stoi(val);
					res.init_type(&v);
				}
				else if (in_group(type, TypeGroup::Real)) {
					double v = std::stod(val);
					res.init_type(&v);
				}
				else if (in_group(type, TypeGroup::String)) {
					res.init_type(&val);
				}
				else {
					throw std::runtime_error("Unknown type");
				}
				program.vars.emplace(name, res);
			}
		}

		for (const auto& part : ctx->variableDeclarationPart()) {
			for (const auto& decl : part->variableDeclaration()) {
				const auto type = std::any_cast<DataType>(visitType_(decl->type_()));
				for (const auto& identifier : decl->identifierList()->identifier()) {
					const auto& name = identifier->getText();
					program.vars.emplace(name, Value{name, type, nullptr, false});
				}
			}
		}
		visitCompoundStatement(ctx->compoundStatement());
		return {};
	}

	std::any visitType_(PascalParser::Type_Context *ctx) override {
		auto type = types.find(ctx->simpleType()->getText());
		if (type == types.end()) throw parse_error(__FILE__, typeid(*this).name(), __LINE__, "Unsupported type");
		return type->second;

	}

	std::any visitCompoundStatement(PascalParser::CompoundStatementContext *ctx) override{
		visitStatements(ctx->statements());
		return {};
	}

	std::any visitStatements(PascalParser::StatementsContext *ctx) override {
		for(const auto& stat : ctx->statement())
			visitStatement(stat);
		return {};
	}

	std::any visitStatement(PascalParser::StatementContext *ctx) override {
		visitUnlabelledStatement(ctx->unlabelledStatement());
		return {};
	}

	std::any visitUnlabelledStatement(PascalParser::UnlabelledStatementContext *ctx) override {
		visitChildren(ctx);
		return{};
	}

	std::any visitAssignmentStatement(PascalParser::AssignmentStatementContext *ctx) override {
		const auto& var = ctx->variable()->getText();
		const auto expr = ctx->expression();

		const auto& val = std::any_cast<Value>(visitExpression(expr));

		auto& res = program.vars[var];
		if (res.type() != val.type()) {
			if (res.type() == DataType::Integer && in_group(val.type(), TypeGroup::Real)) {
				const auto v = static_cast<int>(val.value.double_ptr);
				res.value.int_ptr = v;
			} else if (in_group(res.type(), TypeGroup::Real) && val.type() == DataType::Integer) {
				const auto v = static_cast<double>(val.value.int_ptr);
				res.value.double_ptr = v;
			} else  throw std::runtime_error("Error assignment types");
		}
		if(res.is_const()) throw parse_error(__FILE__, typeid(*this).name(), __LINE__, "Can't modify constant");
			res = val;
		return {};
	}

	std::any visitExpression(PascalParser::ExpressionContext *ctx) override {
		if (!ctx->relationaloperator()) return visitSimpleExpression(ctx->simpleExpression());

		const auto& lhs = std::any_cast<Value>(visitSimpleExpression(ctx->simpleExpression()));
		const auto& rhs = std::any_cast<Value>(visitExpression(ctx->expression()));

		const auto op = compares.find(ctx->relationaloperator()->getText());
		if (op == compares.end())  throw runtime_error("Unknown relational operator");
		const bool val = op->second(lhs, rhs);
		auto res = Value{DataType::Boolean, &val};

		return res;
	}

	std::any visitSimpleExpression(PascalParser::SimpleExpressionContext *ctx) override {
		if (!ctx->additiveoperator())  return visitTerm(ctx->term());

		const auto& lhs = std::any_cast<Value>(visitTerm(ctx->term()));
		const auto& rhs = std::any_cast<Value>(visitSimpleExpression(ctx->simpleExpression()));

		Value res;
		auto* op = ctx->additiveoperator();
		if (op->PLUS())
			res = lhs + rhs;
		else if (op->MINUS())
			res = lhs - rhs;
		else throw std::runtime_error("Unsupported binary operator");

		return res;
	}

	std::any visitTerm(PascalParser::TermContext *ctx) override {
		if (ctx->children.size() == 1) {
			return visitSignedFactor(ctx->signedFactor());
		}
		const auto& lhs = std::any_cast<Value>(visitSignedFactor(ctx->signedFactor()));
		const auto& rhs = std::any_cast<Value>(visitTerm(ctx->term()));

		Value res;

		auto* op = ctx->multiplicativeoperator();
		if ( op->STAR())
			res = lhs * rhs;
		else if (op->SLASH())
			res = lhs / rhs;
		else if (op->DIV()) {
			res = lhs / rhs;
			if (res.type() != DataType::Integer)
				res = Value{DataType::Integer, &res.value.double_ptr, false};
		}
		else if (op->MOD())
			res = lhs % rhs;
		else if (op->AND())  throw std::runtime_error("&&");

		return res;
	}

	std::any visitFactor(PascalParser::FactorContext *ctx) override {
		if (auto* expr = ctx->expression()) {
			return visitExpression(expr);
		}
		return visitChildren(ctx);
	}

	std::any visitSignedFactor(PascalParser::SignedFactorContext *ctx) override {
		const auto& val = std::any_cast<Value>(visitFactor(ctx->factor()));

		if (ctx->MINUS()) {
			auto res = -val;
			return res;
		}

		return val;
	}

	std::any visitUnsignedInteger(PascalParser::UnsignedIntegerContext *ctx) override {
		const auto val = std::stoi(ctx->getText());
		return Value{DataType::Integer, &val};
	}

	std::any visitUnsignedReal(PascalParser::UnsignedRealContext *ctx) override {
		const auto val = std::stod(ctx->getText());
		return Value{DataType::Double, &val};
	}

	std::any visitString(PascalParser::StringContext *ctx) override {
		const auto val = ctx->getText();
		return Value{DataType::String, &val};
	}

	std::any visitVariable(PascalParser::VariableContext *ctx) override {
		return program.vars[ctx->identifier(0)->getText()];
	}

	std::any visitIfStatement(PascalParser::IfStatementContext *ctx) override {
		const bool result = std::any_cast<Value>(visitExpression(ctx->expression())).value.bool_ptr;

		if (result) {
			visitStatement(ctx->statement(0));
		} else {
			visitStatement(ctx->statement(1));
		}
		return {};
	}

	std::any visitWhileStatement(PascalParser::WhileStatementContext *ctx) override {
		while (true) {
			const bool result = std::any_cast<Value>(visitExpression(ctx->expression())).value.bool_ptr;
			if (!result) {
				break;
			}
			visitStatement(ctx->statement());
		}
		return {};
	}

	std::any visitForStatement(PascalParser::ForStatementContext *ctx) override {
		auto& var = program.vars[ctx->identifier()->getText()];

		if (!in_group(var.type(), TypeGroup::Numeric)) {
			throw std::runtime_error("Only integers applicable as variables in for loop");
		}

		var.value.int_ptr  = std::any_cast<Value>(visitExpression(ctx->forList()->initialValue()->expression())).value.int_ptr;
		const int final_value = std::any_cast<Value>(visitExpression(ctx->forList()->finalValue()->expression())).value.int_ptr;

		while (var.value.int_ptr < final_value) {
			visitStatement(ctx->statement());
			var.value.int_ptr++;
		}

		return{};
	}

	std::any visitProcedureStatement(PascalParser::ProcedureStatementContext *ctx) override {

		call(ctx->identifier()->getText(), ctx->parameterList());
		return {};
	}

	std::any visitFunctionDesignator(PascalParser::FunctionDesignatorContext *ctx) override {
		return call(ctx->identifier()->getText(), ctx->parameterList());
	}

	std::any visitParameterList(PascalParser::ParameterListContext *ctx) override {
		std::vector<Value> params;
		for (const auto& param : ctx->actualParameter()) {
			params.push_back(std::any_cast<Value>(visitActualParameter(param)));
		}
		return params;
	}

};


#endif