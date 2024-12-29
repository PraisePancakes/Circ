#pragma once
#include <iostream>
#include <vector>
#include "lexer.hpp"
#include "expressions.hpp"
#include "error_log.hpp"
#include "statements.hpp"

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

			if (match({ TOK_LPAREN })) {
				BaseExpression* expr = term();
				if (!match({ TOK_RPAREN })) {
					ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing ')'");
					throw std::runtime_error("");
				}
				return new Grouping(expr);
			}

			if (match({ TOK_LCURL })) {
				BaseExpression* object = obj();
				if (!match({ TOK_RCURL })) {
					ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing '}'");
					throw std::runtime_error("");
				}
				Object* o = (Object*)object;
				return o;
			}

			

			ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Expected a primary expression.");
			throw std::runtime_error("");
			
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
	

		void sync() {
			advance();
			while (!is_end())
			{
				// parser ignores until the next statement is found, if and only if the previous statement throws an exception. Panic mode recovery allows for the parser to catch up to the next correct rule discarding all previous false rules.
				if (parser_previous().t == TOK_COMMA)
				{
					return;
				}
				switch (parser_peek().t)
				{
				case TOK_DOLLA:
					return;
				}
				advance();
			}
		};


		BaseExpression* assignment() {
			BaseExpression* e = term();
			if (match({ TOK_COL })) {
				Token eq = parser_previous();
				BaseExpression* r = assignment();
				if (Variable* v = (Variable*)e) {
					std::string l = v->name;
					return new Assignment(l, r);
				}

				ParseErrorLogger::instance().log(LogType::SYNTAX, eq, "Invalid assignment target");
			}
			return e;
		}
		BaseExpression* obj() {
			std::map<std::string, BaseExpression*> members;
			while (parser_peek().t != TOK_RCURL && !is_end()) {
				Decl* d = (Decl*)statement();
				members[d->key] = d->value;
			}
			
			return new Object(members);
		}

		BaseExpression* expression() {
			return assignment();
		}
		

		

		BaseStatement* var_decl() {
			std::string key = advance().word;
			BaseExpression* init = nullptr;
			if (match({ TOK_COL })) {
				init = expression();
			}
			else {
				
				ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), " Missing ':' Circ variables must be initialized.");
				throw std::runtime_error("");
			}
			if (parser_peek().t != TOK_EOF && parser_peek().t != TOK_RCURL) {
				if (!match({ TOK_COMMA })) {
					ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Expected ',' after expression");
					throw std::runtime_error("");
				}
			}
			
			return new Decl(key, init);
		}

		BaseStatement* statement() {
			if (match({ TOK_DOLLA })) {
				return var_decl();
			}
			else {
				ParseErrorLogger::instance().log(LogType::SYNTAX, parser_peek(), "Missing '$', Circ variables must be prefixed with '$'.");
				throw std::runtime_error("");
			}
		}

		BaseStatement* decl() {
			try {
				
				return statement();
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				sync();
			}
		}

	public:
		std::vector<BaseStatement*> statements;
		Parser(const std::vector<Token>& toks) : tokens(toks), curr() {
			
				while (!is_end()) {
					statements.push_back(decl());
				}
			
			ParseErrorLogger::instance().print_list();
			
		};
		~Parser() {};
	};

}