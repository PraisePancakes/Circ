#pragma once
#include <iostream>
#include <any>
#include <vector>
#include <fstream>
#include <string>

namespace CircCFGInterp {
	enum TokenType {
		TOK_ERROR = -1,
		TOK_HASH,
		TOK_SLASH,
		TOK_STRING,
		TOK_VAR,
		TOK_NUM,
		TOK_COL,
		TOK_ENTRY,
		TOK_DOLLA,
		TOK_LCURL,
		TOK_RCURL,
		TOK_LPAREN,
		TOK_RPAREN,
		TOK_COMMA,
		TOK_PLUS,
		TOK_MINUS,
		TOK_DIV,
		TOK_STAR,
		TOK_BANG,
		TOK_SEMI,
		TOK_MOD,
		TOK_EOF
	};

	struct Token {
		std::string word;
		int line;
		int where;
		TokenType t;
		std::any lit;
		Token(const std::string& w, int l, int where, TokenType t, std::any lit) : word(w), line(l), where(where), t(t), lit(lit) {};
		~Token() {};
	};

	class Lexer {
		std::vector<Token> tokens;
		std::string contents;
		int start = 0;
		int end = 0;
		int line = 0;
		int pos = 0;
		friend class Interpreter;

		bool is_end() const {
			return contents[end] == '\0' || end >= contents.length();
		}

		char peek() {
			if (is_end()) {
				return '\0';
			}
			return contents[end];
		};

		char peek_next() {
			if (is_end()) {
				return '\0';
			}
			return contents[end + 1];
		}

		char advance() {
			if (is_end()) {
				return '\0';
			}
			end++;
			return contents[end - 1];
		}



		void add_token(TokenType t, std::any lit) {
			std::string word = this->contents.substr(start, end - start);
			
			tokens.push_back(Token(word, line, pos, t, lit));
		}

		void add_token(TokenType t) {
			add_token(t, nullptr);
		}

		void lex_number() {
			std::string num = "";
			num += this->contents[end - 1];
			while (isdigit(peek()) || peek() == '.') {
				num += advance();
			}

			add_token(TOK_NUM, std::stod(num));
		};
		void lex_string() {
			std::string lit = "";
			lit += advance();

			while (peek() != '\"' && !is_end()) {
				lit += advance();
			}
			advance();

			add_token(TOK_STRING, lit);
		};

		void lex_var() {

			while (!is_end() && (isalnum(peek()) || peek() == '_')) {

				char c = advance();

			};

			add_token(TOK_VAR);

		};


		void lex_token() {
			char c = advance();
			pos++;
			switch (c) {
			case '#':
				add_token(TOK_HASH);
				break;
			case ':':
				if (advance() == ':') {
					add_token(TOK_ENTRY);
					break;
				}
				add_token(TOK_COL);
				break;
			case '$':
				add_token(TOK_DOLLA);
				break;
			case ';':
				add_token(TOK_SEMI);
				break;
			case '+':
				add_token(TOK_PLUS);
				break;
			case '-':
				add_token(TOK_MINUS);
				break;
			case ',':
				add_token(TOK_COMMA);
				break;
			case '!':
				add_token(TOK_BANG);
				break;
			case '*':
				add_token(TOK_STAR);
				break;
			case '{':
				add_token(TOK_LCURL);
				break;
			case '}':
				add_token(TOK_RCURL);
				break;
			case '(':
				add_token(TOK_LPAREN);
				break;
			case ')':
				add_token(TOK_RPAREN);
				break;
			case '%':
				add_token(TOK_MOD);
				break;
			case '\n':
				line++;
				pos = 0;
				break;
			case '/':
				if (peek() == '/') {
					advance();
					while (peek() != '\n' && !is_end()) {

						advance();
					};
				}
				else if (peek() == '*') {
					advance();
					while ((peek() != '*' && peek_next() != '/' && !is_end())) {
						advance();
					}
					advance();
					advance();

					if (is_end()) {
						throw std::runtime_error("multiline comment missing closing symbol");
					}

				}
				else {
					add_token(TOK_DIV);
				}
				break;
			case '\"':
				lex_string();
				break;

			default:
				if (isdigit(c)) {
					//number

					lex_number();

				}
				else if (isalpha(c)) {
					//identifier
					lex_var();
				}
				break;
			};
		}



		void lex() {
			while (!is_end()) {
				try {
					lex_token();
				}
				catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
				}
				
				start = end;
			}

			add_token(TOK_EOF);
		};
	public:
		Lexer(const std::string& path) {

			std::fstream stream;
			stream.open(path);
			if (!stream.is_open()) {
				std::cout << "file failed to open" << std::endl;
			}
			else {
				std::cout << "cfg file opened" << std::endl;
			}
			std::string line;
			while (std::getline(stream, line)) {
				contents += line + '\n';
			};

			lex();



			stream.close();

		};
		~Lexer() {};
	};
};