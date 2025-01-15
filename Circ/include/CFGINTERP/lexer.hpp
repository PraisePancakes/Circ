#pragma once
#include <iostream>
#include <any>
#include <vector>
#include <fstream>
#include <string>
#include "common.hpp"


namespace Serialization {
	

		class Lexer {
			std::vector<Token> tokens;
			std::string contents;
			int start = 0;
			int end = 0;
			int line = 1;
			int pos = 0;
			friend class Interpreter;

			bool is_end() const {
				return contents[end] == '\0' || end >= contents.length();
			}

			char prev() {
				if (is_end()) {
					return '\0';
				}
				return contents[end - 1];
			};

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
				bool is_double = false;

				while (isdigit(peek())) {
					num += advance();
				}

				if (peek() == '.') {
					is_double = true;
					num += advance();
					if (!isdigit(peek()) && is_double) {
						num += '0';
					}
					while (isdigit(peek())) {
						num += advance();
					}
				}


				if (is_double) {
					add_token(TOK_NUM, std::stod(num));
				}
				else {
					add_token(TOK_NUM, std::stoi(num));
				}
			};
			void lex_string() {
				std::string lit = "";
				lit += advance();
				while (peek() != '\"' && !is_end()) {

					lit += advance();
				}

				if (is_end()) {

					throw std::runtime_error("Open string");
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
				case '[':
					add_token(TOK_LBRAC);
					break;
				case ']':
					add_token(TOK_RBRAC);
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
						line++;
					}
					else if (peek() == '*') {
						advance();
						while ((peek() != '*' && peek_next() != '/' && !is_end())) {
							advance();
							if (peek() == '\n') {
								line++;
							}
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
					std::cerr << "file failed to open" << std::endl;
				}
				else {
					std::cerr << "cfg file opened" << std::endl;
				}
				std::string line;
				while (std::getline(stream, line)) {
					contents += line + '\n';
				};

				lex();



				stream.close();

			};

			Lexer() = default;

			[[nodiscard]] std::vector<Token> lex_contents(const std::string& str) {
				contents = str;
				lex();
				return tokens;
			}

			 
			~Lexer() {};
		};
	};

