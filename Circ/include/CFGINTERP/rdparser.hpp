#pragma once
#include <iostream>
#include <vector>
#include "lexer.hpp"
#include "expressions.hpp"

namespace CircCFGInterp {

	class Parser {
		typedef std::size_t TokenIndex;
		std::vector<Token> tokens;
		TokenIndex curr;

		bool is_end() const {
			return tokens[curr].t == TOK_EOF;
		}


		bool check(TokenType t) {
			if (is_end())
				return false;

			return tokens[curr].t == t;
		}

		Token advance() {
			return tokens[curr++];
		}



		bool match(std::initializer_list<TokenType> t) {
			for (TokenType a : t) {
				if (check(a)) {
					advance();
					return true;
				}

			}
			return false;
		};

		Token parser_previous() {
			return tokens[curr - 1];
		}

		BaseExpression* primary() {
			if (match({ TOK_STRING, TOK_NUM })) {
				return new Literal(parser_previous().lit);
			}

			if (match({ TOK_LCURL })) {

				return obj();
			}

			throw std::runtime_error("Expected a primary expression.");
		};

		BaseExpression* unary() {

			if (match({ TOK_BANG, TOK_MINUS })) {

				TokenType t = parser_previous().t;
				BaseExpression* r = primary();
				return new Unary(t, r);
			}
			return primary();
		};
		//grouping : $window_width : (a + b) - c #


		BaseExpression* obj() {
			std::map<std::string, BaseExpression*> members;

			while (!match({ TOK_RCURL })) {
				BaseExpression* v = var();
				Assignment* a = (Assignment*)v;
				std::string key = a->key;

				Literal* l = (Literal*)a->value;

				members[key] = l;

			}

			return new Object(members);

		}

		BaseExpression* factor() {
			BaseExpression* left = unary();
			while (match({ TOK_DIV, TOK_STAR, TOK_MOD })) {

				TokenType t = parser_previous().t;
				BaseExpression* r = unary();

				left = new Binary(left, t, r);
			}

			return left;
		};

		BaseExpression* term() {

			BaseExpression* left = factor();
			while (match({ TOK_PLUS, TOK_MINUS })) {
				TokenType t = parser_previous().t;
				BaseExpression* r = factor();
				left = new Binary(left, t, r);
			}

			return left;
		}

		BaseExpression* var() {
			while (match({ TOK_DOLLA })) {
				std::string key = advance().word;

				if (!match({ TOK_COL })) {
					throw std::runtime_error("Missing ':'");
				}
				BaseExpression* value = term();
				if (!is_end()) {
					if (!match({ TOK_COMMA })) {
						throw std::runtime_error("Missing ','");
					}
				}

				return new Assignment(key, value);
			}
		};

		std::vector<BaseExpression*> parse() {
			std::vector<BaseExpression*> ast;
			while (!is_end()) {
				ast.push_back(var());

			}
			return ast;
		};

	public:
		std::vector<BaseExpression*> ast;
		Parser(const std::vector<Token>& toks) : tokens(toks), curr() {

			if (match({ TOK_ENTRY })) {
				ast = parse();

			}
			else {
				throw std::runtime_error("CFG ENTRY NOT FOUND");
			}

		};
		~Parser() {};
	};

}