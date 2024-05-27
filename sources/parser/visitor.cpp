#include "visitor.h"


Visitor::Visitor() : functions({
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
	}}},
	})
	, program({}) { }


//-------------------------program------------------------------------

std::any Visitor::visitProgram(PascalParser::ProgramContext *ctx) {
	visitProgramHeading(ctx->programHeading());
	visitBlock(ctx->block());
	return {};
}

std::any Visitor::visitProgramHeading(PascalParser::ProgramHeadingContext *ctx) {
	program.program_name = ctx->identifier()->getText();
	return {};
}

std::any Visitor::visitBlock(PascalParser::BlockContext *ctx) {
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

//-------------------------statements------------------------------------

std::any Visitor::visitCompoundStatement(PascalParser::CompoundStatementContext *ctx) {
	visitStatements(ctx->statements());
	return {};
}

std::any Visitor::visitStatements(PascalParser::StatementsContext *ctx) {
	for(const auto& stat : ctx->statement())
		visitStatement(stat);
	return {};
}

std::any Visitor::visitStatement(PascalParser::StatementContext *ctx) {
	visitUnlabelledStatement(ctx->unlabelledStatement());
	return {};
}

std::any Visitor::visitUnlabelledStatement(PascalParser::UnlabelledStatementContext *ctx) {
	visitChildren(ctx);
	return{};
}

std::any Visitor::visitAssignmentStatement(PascalParser::AssignmentStatementContext *ctx) {
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

//-------------------------expressions------------------------------------

std::any Visitor::visitExpression(PascalParser::ExpressionContext *ctx) {
	if (!ctx->relationaloperator()) return visitSimpleExpression(ctx->simpleExpression());

	const auto& lhs = std::any_cast<Value>(visitSimpleExpression(ctx->simpleExpression()));
	const auto& rhs = std::any_cast<Value>(visitExpression(ctx->expression()));

	const auto op = compares.find(ctx->relationaloperator()->getText());
	if (op == compares.end())  throw runtime_error("Unknown relational operator");
	const bool val = op->second(lhs, rhs);
	auto res = Value{DataType::Boolean, &val};

	return res;
}

std::any Visitor::visitSimpleExpression(PascalParser::SimpleExpressionContext *ctx) {
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

//----------------------------terms------------------------------------

std::any Visitor::visitTerm(PascalParser::TermContext *ctx) {
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

std::any Visitor::visitFactor(PascalParser::FactorContext *ctx) {
	if (auto* expr = ctx->expression()) {
		return visitExpression(expr);
	}
	return visitChildren(ctx);
}

std::any Visitor::visitSignedFactor(PascalParser::SignedFactorContext *ctx) {
	const auto& val = std::any_cast<Value>(visitFactor(ctx->factor()));

	if (ctx->MINUS()) {
		auto res = -val;
		return res;
	}

	return val;
}

//-------------------------------------types--------------------------------------

std::any Visitor::visitType_(PascalParser::Type_Context *ctx) {
	auto type = types.find(ctx->simpleType()->getText());
	if (type == types.end()) throw parse_error(__FILE__, typeid(*this).name(), __LINE__, "Unsupported type");
	return type->second;

}

std::any Visitor::visitUnsignedInteger(PascalParser::UnsignedIntegerContext *ctx) {
	const auto val = std::stoi(ctx->getText());
	return Value{DataType::Integer, &val};
}

std::any Visitor::visitUnsignedReal(PascalParser::UnsignedRealContext *ctx) {
	const auto val = std::stod(ctx->getText());
	return Value{DataType::Double, &val};
}

std::any Visitor::visitString(PascalParser::StringContext *ctx) {
	const auto val = ctx->getText();
	return Value{DataType::String, &val};
}

std::any Visitor::visitVariable(PascalParser::VariableContext *ctx) {
	return program.vars[ctx->identifier(0)->getText()];
}

//--------------------------------loops---------------------------------------

std::any Visitor::visitIfStatement(PascalParser::IfStatementContext *ctx){
	const bool result = std::any_cast<Value>(visitExpression(ctx->expression())).value.bool_ptr;

	if (result)
		visitStatement(ctx->statement(0));
	else
		visitStatement(ctx->statement(1));
	return {};
}

std::any Visitor::visitWhileStatement(PascalParser::WhileStatementContext *ctx) {
		while (true) {
			const bool result = std::any_cast<Value>(visitExpression(ctx->expression())).value.bool_ptr;
			if (!result)
				break;
			visitStatement(ctx->statement());
		}
		return {};
}

std::any Visitor::visitForStatement(PascalParser::ForStatementContext *ctx) {
	auto& var = program.vars[ctx->identifier()->getText()];

	if (!in_group(var.type(), TypeGroup::Numeric))
		throw std::runtime_error("Only integers applicable as variables in for loop");

	var.value.int_ptr  = std::any_cast<Value>(visitExpression(ctx->forList()->initialValue()->expression())).value.int_ptr;
	const int final_value = std::any_cast<Value>(visitExpression(ctx->forList()->finalValue()->expression())).value.int_ptr;

	while (var.value.int_ptr <= final_value) {
		visitStatement(ctx->statement());
		++var.value.int_ptr;
	}
	return{};
}

//--------------------------------functions---------------------------------------

std::any Visitor::call(const string &name, PascalParser::ParameterListContext *paramsCtx)  {
	const auto it = functions.find(name);
	if(it == functions.end())
		throw parse_error(__FILE__, typeid(*this).name(), __LINE__, "Unknown function");

	const auto& func = it->second;
	auto args = std::any_cast<std::vector<Value>>(visitParameterList(paramsCtx));

	func(args);
	return{};
}

std::any Visitor::visitProcedureStatement(PascalParser::ProcedureStatementContext *ctx) {
	call(ctx->identifier()->getText(), ctx->parameterList());
	return {};
}

std::any Visitor::visitFunctionDesignator(PascalParser::FunctionDesignatorContext *ctx) {
	return call(ctx->identifier()->getText(), ctx->parameterList());
}

std::any Visitor::visitParameterList(PascalParser::ParameterListContext *ctx) {
	std::vector<Value> params;
	for (const auto& param : ctx->actualParameter()) {
		params.push_back(std::any_cast<Value>(visitActualParameter(param)));
	}
	return params;
}
























