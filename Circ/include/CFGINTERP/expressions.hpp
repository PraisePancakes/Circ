#pragma once
#include <any>
#include <iostream>
#include "lexer.hpp"
#include <map>

namespace CircCFGInterp {

	struct Literal;
	struct Declaration;
	struct Binary;
	struct Unary;
	struct Object;
	struct Grouping;
	struct Array;

	/*
		ExpressionVisitor and the different expression types
	*/
	struct ExpressionVisitor {
		virtual std::any visitLiteral(Literal* l) const = 0;
		virtual std::any visitDeclaration(Declaration* a) const = 0;
		virtual std::any visitBinary(Binary* b) const = 0;
		virtual std::any visitUnary(Unary* u) const = 0;
		virtual std::any visitObject(Object* u) const = 0;
		virtual std::any visitGrouping(Grouping* g) const = 0;
		virtual std::any visitArray(Array* a) const = 0;

	};

	struct BaseExpression {
		BaseExpression() {};
		virtual std::any accept(const ExpressionVisitor& v) = 0;
		~BaseExpression() {};
	};



	struct Declaration : public BaseExpression {
		std::string key;
		BaseExpression* value;
		Declaration(const std::string& s, BaseExpression* v) : key(s), value(v) {};
		std::any accept(const ExpressionVisitor& v) override {
			return v.visitDeclaration(this); //look up env key, if this key is already inserted, update the value, else insert
		};
		~Declaration() {};
	};
	struct Literal : public BaseExpression { 
		std::any lit;

		Literal(std::any l) : lit(l) {
		};
		std::any accept(const ExpressionVisitor& v) override {
			return v.visitLiteral(this);
		};
		~Literal() {};
	};


	struct Binary : public BaseExpression {
		BaseExpression* l;
		TokenType op;
		BaseExpression* r;

		Binary(BaseExpression* l, TokenType op, BaseExpression* r) : l(l), op(op), r(r) {};
		std::any accept(const ExpressionVisitor& v) override {
			return v.visitBinary(this);
		};
		~Binary() {};
	};

	struct Unary : public BaseExpression {
		TokenType op;
		BaseExpression* r;
		Unary(TokenType o, BaseExpression* r) : op(o), r(r) {};
		std::any accept(const ExpressionVisitor& v) override {
			return v.visitUnary(this);
		}
		~Unary() {};
	};

	struct Object : public BaseExpression {
		std::map<std::string, BaseExpression*> members;
		Object(std::map<std::string, BaseExpression*> m) : members(m) {};
		std::any accept(const ExpressionVisitor& v) override {
			return v.visitObject(this);
		}
		~Object() {};
	};

	struct Array : public BaseExpression {
		std::vector<BaseExpression*> arr;
		Array(std::vector<BaseExpression*> a) : arr(a) {
			
		};
		std::any accept(const ExpressionVisitor& v) override {
			return v.visitArray(this);
		}
		~Array() {};
	};

	struct Grouping : public BaseExpression {
		BaseExpression* g;
		Grouping(BaseExpression* g) : g(g) {};
		std::any accept(const ExpressionVisitor& v) override {
			return v.visitGrouping(this);
		}
		~Grouping() {};
	};

}