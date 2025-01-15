#pragma once
#include <iostream>
#include <vector>
#include "lexer.hpp"
#include "expressions.hpp"
#include "error_log.hpp"
#include "statements.hpp"
namespace Serialization {



	
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

		static void report(LogType t, Token tok, const std::string& s) {
			ParseErrorLogger::instance().log(t, tok, s);
			throw std::runtime_error("err");
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
			if (match({ TOK_NUM, TOK_STRING })) {
				return new Literal(parser_previous().lit);
			}

			if (match({ TOK_LPAREN })) {
				BaseExpression* expr = expression();
				if (match({ TOK_RPAREN })) {
					return new Grouping(expr);
				}
				report(LogType::SYNTAX, parser_peek(), "Missing closing ')'");
				
			}
			if (match({ TOK_LBRAC })) {
				BaseExpression* arr = array();
				if (!match({ TOK_RBRAC })) {
					report(LogType::SYNTAX, parser_peek(), "Missing ']'");
					throw std::runtime_error("");
				}
				Array* a = (Array*)arr;
				return a;
			}

			if (match({ TOK_LCURL })) {
				Object* object = (Object*)obj();
				if (match({ TOK_RCURL })) {
					return object;
				}
				report(LogType::SYNTAX, parser_peek(), "Missing closing '}'");
			}

		};

		BaseExpression* unary() {
			if (match({ TOK_MINUS, TOK_BANG })) {
				TokenType op = parser_previous().t;
				BaseExpression* r = unary();
				return new Unary(op, r);
			}
			return primary();
		};
		//grouping : $window_width : (a + b) - c,
		BaseExpression* array() {
			std::vector<BaseExpression*> elems;
			while (parser_peek().t != TOK_RBRAC && !is_end()) {
				BaseExpression* e = term();
				if (parser_peek().t != TOK_RBRAC) {
					if (!match({ TOK_COMMA })) {
						report(LogType::SYNTAX, parser_peek(), "Missing , in array");
					
					}
				}

				elems.push_back(e);
			}
			if (parser_peek().t == TOK_RBRAC && parser_previous().t == TOK_COMMA) {
				report(LogType::SYNTAX, parser_peek(), "trailing comma");
				
			}

			return new Array(elems);
		}

		

		BaseExpression* factor() {
			BaseExpression* l = unary();
			while (match({ TOK_DIV, TOK_STAR, TOK_MOD })) {
				TokenType op = parser_previous().t;
				BaseExpression* r = unary();
				l = new Binary(l, op, r);
			}
			return l;
		};

		BaseExpression* term() {
			BaseExpression* l = factor();
			while (match({ TOK_PLUS, TOK_MINUS })) {
				TokenType op = parser_previous().t;
				BaseExpression* r = factor();
				l = new Binary(l, op, r);
			}
			return l;
			
		}

		
		TokenType type_at_offset(size_t off) const {
			return tokens[curr + off].t;
		}
	

		void sync() {
			had_error = true;
			advance();
			while (!is_end()) {
				if (parser_peek().t == TOK_DOLLA) return;
				advance();
			}

		};


		
		BaseExpression* obj() {
			std::map<std::string, BaseExpression*> members;
			while (parser_peek().t != TOK_RCURL && !is_end()) {
				Decl* d = (Decl*)decl();
				if (d == nullptr) {
					continue;
				}
				std::pair<std::string, BaseExpression*> kvp(d->key, d->value);
				members.insert(kvp);
			}
			return new Object(members);
		};

		BaseExpression* expression() {
			return term();
		}

		BaseStatement* decl() {
			try {
				if (match({ TOK_DOLLA })) {
					std::string key = parser_peek().word;
					if (match({ TOK_VAR })) {
						if (match({ TOK_COL })) {
							BaseExpression* v = expression();
							if (!is_end() && !check(TOK_RCURL)) {
								if (!match({ TOK_COMMA })) {
									report(LogType::SYNTAX, parser_peek(), "Missing ',' ");
								}
							}
							return new Decl(key, v);
						}
						report(LogType::SYNTAX, parser_peek(), "Missing ':' initializer");
					}
					report(LogType::SYNTAX, parser_peek(), "Invalid identifier");
				}
				report(LogType::SYNTAX, parser_peek(), "Missing '$'");
			}
			catch (std::exception& e) {
				sync();
				
			}
		
		}

	public:
		bool had_error = false;
		std::vector<BaseStatement*> statements;
		Parser(const std::vector<Token>& toks) : tokens(toks), curr() {
			while (!is_end()) {
				statements.push_back(decl());
			}
			if (had_error) {
				//guard such that file may not be serialized if not parsed correctly.
				ParseErrorLogger::instance().print_list();
				throw std::runtime_error("");
				
			}
		};
		~Parser() {};
	};

}
