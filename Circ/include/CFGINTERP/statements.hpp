#pragma once
#include <any>
#include <vector>
#include <iostream>
#include "expressions.hpp"

namespace CircCFGInterp {
	struct Block;
	struct Decl;
	struct Expression;


	struct StatementVisitor {
		virtual std::any visitBlock(Block* b) = 0;
		virtual std::any visitDecl(Decl* d) = 0;
		virtual std::any visitExpression(Expression* e) = 0;
	};

	struct BaseStatement {
		BaseStatement() {};
		virtual std::any accept(StatementVisitor* v) = 0;
		~BaseStatement() {};
	};

	struct Block : BaseStatement {
		std::vector<BaseStatement*> statements;
		Block(std::vector<BaseStatement*> s) : statements(s) {};
		std::any accept(StatementVisitor* v) override {
			return v->visitBlock(this);
		}
		~Block() {};
	};

	struct Decl : BaseStatement {
		std::string key;
		BaseExpression* value;
		Decl(const std::string& k, BaseExpression* v) : key(k), value(v) {};
		std::any accept(StatementVisitor* v) override {
			return v->visitDecl(this);
		}
		~Decl() {};
	};

	struct Expression : BaseStatement {
		BaseExpression* e;
		Expression(BaseExpression* e) : e(e) {};
		std::any accept(StatementVisitor* v) override {
			return v->visitExpression(this);
		}
		~Expression() {};
	};
}