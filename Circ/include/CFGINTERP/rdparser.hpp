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
			if (!is_end()) curr++;
			return parser_previous();
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

			if (parser_peek().t == TOK_LBRAC) {
				return array();
			}

			if (parser_peek().t == TOK_LCURL ) {

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
	

		

		BaseExpression* element() {
			BaseExpression* lit = term();
			if (!check(TOK_RBRAC)) {
				if (!match({ TOK_COMMA })) {
					ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing ',' after element.");
					sync();
				}
			}
			return lit;
		}
		BaseExpression* decl() {
			if (match({ TOK_DOLLA }) ) {
				std::string key = advance().word;
				
				if (match({ TOK_COL })) {
					BaseExpression* lit = term();
					if (!check(TOK_RCURL)) {
						if (!match({ TOK_COMMA })) {
							
							ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing ',' after variable declaration.");
							sync();
						}
					}
					return new Declaration(key, lit);
				}
				else {
					ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing ':' in variable declaration.");
					sync();
				}
			}
			else {
				
				ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Variable declaration prefix '$' missing.");
				sync();
			}
			
			return decl();
		}


		

		bool next_valid_token() {
			
			if (parser_peek().t == TOK_DOLLA ) {
				return true;
			}
			else {
				advance();
				return false;
			}
		};
		

		void sync() {
			
			if (is_end()) return;
			had_error = true;
			if (!next_valid_token()) {
				sync();
			}
		};

		BaseExpression* array() {
			std::vector<BaseExpression*> arr;
			if (match({ TOK_LBRAC })) {
				while (!match({ TOK_RBRAC })) {
					BaseExpression* el = element();
					Literal* l = (Literal*)el;
					arr.push_back(el);
				}
			}
			else {
				ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing ']'.");
				sync();
			}
			
			
			return new Array(arr);
		}

	static inline bool had_error = false;
		BaseExpression* obj() {
			std::map<std::string, BaseExpression*> members;
			
			if (match({ TOK_LCURL })) {
				while (!match({ TOK_RCURL })) {
					BaseExpression* v = decl();
					Declaration* a = (Declaration*)v;
					std::string key = a->key;
					
					Literal* l = (Literal*)a->value;
					members[key] = l;
				}
				
			}
			else {
				ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing '{'.");
				sync();

			}
			return new Object(members);

		}

		BaseExpression* parse() {
			BaseExpression* ast = nullptr;
			while (!is_end()) {
				
					ast = obj();
					
			}
			return ast;
		};

	public:
		BaseExpression* ast;
		Parser(const std::vector<Token>& toks) : tokens(toks), curr() {
			try {
				ast = parse();
				
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