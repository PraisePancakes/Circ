#pragma once
#include <any>
#include <vector>
#include <iostream>
#include "expressions.hpp"

namespace CircCFGInterp {
	struct Block;
	struct Decl;
	


	struct StatementVisitor {
		virtual std::any visitDecl(Decl* d) = 0;
		
	};

	struct BaseStatement {
		BaseStatement() {};
		virtual std::any accept(StatementVisitor* v) = 0;
		~BaseStatement() {};
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

	
}