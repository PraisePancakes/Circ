#pragma once
#include <iostream>
#include <vector>
#include "lexer.hpp"
#include "expressions.hpp"
#include "error_log.hpp"

namespace CircCFGInterp {
	
	class Parser {
		typedef std::size_t TokenIndex;
		std::vector<Token> tokens;
		TokenIndex curr;
		inline static bool is_last = false;

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
		Token parser_peek() const {
			return tokens[curr];
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

			if (match({ TOK_LBRAC })) {
				return array();
			}

			if (match({ TOK_LCURL })) {

				return obj();
			}
			if (match({ TOK_LPAREN })) {
				BaseExpression* expr = term();
				if (!match({ TOK_RPAREN })) {
					ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing ')'");
					sync();
				}
				return new Grouping(expr);
			}
			ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Expected a primary expression.");
			sync();
			
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

		
		TokenType type_at_offset(size_t off) const {
			return tokens[curr + off].t;
		}
	

		BaseExpression* array() {
			std::vector<BaseExpression*> arr;
			while (!match({ TOK_RBRAC })) {
				BaseExpression* el = element();
				arr.push_back(el);
			}
			return new Array(arr);
		}

		BaseExpression* element() {
			BaseExpression* lit = term();
			if (!check(TOK_RBRAC)) {
				if (!match({ TOK_COMMA })) {
					ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing ',' after element.");
					sync();
				}
				return lit;
			}
		}
		BaseExpression* var() {
			if (match({ TOK_DOLLA })) {
				std::string key = advance().word;
				
				if (match({ TOK_COL })) {
					BaseExpression* lit = term();
					if (!check(TOK_RCURL)) {
						if (!match({ TOK_COMMA })) {
							
							ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing ',' after variable declaration.");
							sync();
						}
					}
					return new Assignment(key, lit);
				}
				else {
					ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing ':' in variable declaration.");
					sync();
					
				}
			}
			else {
				
				ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Variable declaration prefix '$' missing.");
				
				sync();
				return var();
			}
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

		bool next_valid_token() {
			if (parser_peek().t == TOK_DOLLA) {
				return true;
			}
			else {
				advance();
				return false;
			}
		};
		static inline bool had_error = false;

		void sync() {
			had_error = true;
			if (!next_valid_token()) {
				sync();
			}
			
			return;
		};

		BaseExpression* parse() {
			BaseExpression* ast = nullptr;
			while (!is_end()) {
				if (match({ TOK_LCURL })) {
					ast = obj();
				}
				else {
					ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing Entry '::' Token.");
					sync();
					
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
					ParseErrorLogger::instance().log(LogType::ENTRY, parser_peek(), "Missing Entry '::' Token.");
					throw std::runtime_error("CFG ENTRY NOT FOUND");
				}

				if (had_error) {
					ParseErrorLogger::instance().print_list();
					throw std::runtime_error("total errors : " + std::to_string(ParseErrorLogger::instance().logsize()));
				}
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				exit(EXIT_FAILURE);
			}
			
		};
		~Parser() {};
	};

}