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
			if (match({ TOK_LPAREN })) {
				BaseExpression* expr = term();
				if (!match({ TOK_RPAREN })) {
					
					throw std::runtime_error("Missing ')'");
				}
				return new Grouping(expr);
			}
			
			throw std::runtime_error("Expected a primary expression.");
		};

		BaseExpression* unary() {
			if (match({ TOK_BANG, TOK_MINUS })) {
				TokenType t = parser_previous().t;
				BaseExpression* r = unary();
				return new Unary(t, r);
			}
			return primary();
		};
		//grouping : $window_width : (a + b) - c,


	

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

		Token parser_peek() const {
			return tokens[curr];
		}

		BaseExpression* var() {
			if (parser_peek().t == TOK_DOLLA) {
				while (match({ TOK_DOLLA })) {
					std::string key = advance().word;

					if (!match({ TOK_COL })) {
						throw std::runtime_error("Missing ':'");
					}
					BaseExpression* value = term();
					if (!is_end()) {
						if (!match({ TOK_SEMI })) {

							throw std::runtime_error("Missing ';'");
						}
					}

					return new Assignment(key, value);
				}
			}
			else {
				throw std::runtime_error("Variable declaration prefix '$' missing.");
			}
			
		};



		BaseExpression* parse() {
			BaseExpression* ast = nullptr;
			while (!is_end()) {
				if (match({ TOK_LCURL })) {
					
					ast = obj();
				}
				else {
				
					throw std::runtime_error("Missing Entry '{'");
				}
				

			}
			return ast;
		};

	public:
		BaseExpression* ast;
		Parser(const std::vector<Token>& toks) : tokens(toks), curr() {
			try {
				if (match({ TOK_ENTRY })) {
					ast = parse();

				}
				else {

					throw std::runtime_error("CFG ENTRY NOT FOUND");
				}
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
			}
			

		};
		~Parser() {};
	};

}