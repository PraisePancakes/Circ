#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <any>
#include <map>

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
		TOK_PLUS,
		TOK_MINUS,
		TOK_DIV,
		TOK_STAR,
		TOK_BANG,
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
			std::string var = "";
			var += advance();
			while (!is_end() && (isalnum(peek()) || peek() == '_')) {
				var += advance();
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
			case '$' :
				add_token(TOK_DOLLA);
				break;
			case '+' :
				add_token(TOK_PLUS);
				break;
			case '-':
				add_token(TOK_MINUS);
				break;
			case '!':
				add_token(TOK_BANG);
				break;
			case '*':
				add_token(TOK_STAR);
				break;
			case '{' :
				add_token(TOK_LCURL);
				break;
			case '}' :
				add_token(TOK_RCURL);
				break;
			case '(' :
				add_token(TOK_LPAREN);
				break;
			case ')' :
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
					while (peek() != '*' && peek_next() != '\\' && !is_end()) {
						advance();
					}
					if (is_end()) {
						throw std::runtime_error("multiline comment missing closing symbol");
					}
				}
				else {
					add_token(TOK_DIV);
				}
				break;
			case '\"' :
				lex_string();
				break;

			default:
				 if (isdigit(c)) {
					//number
					
					lex_number();

				}
				else if(isalpha(c)) {
					//identifier
					lex_var();
				}
				 break;
			};
		}



		void lex() {
			while (!is_end()) {
				lex_token();
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

	struct Literal;
	struct Assignment; 
	struct Binary;
	struct Unary;
	struct ExpressionVisitor;
	

	struct BaseExpression {
		BaseExpression() {};
		virtual std::any accept(const ExpressionVisitor& v) = 0;
		~BaseExpression() {};
	};

	

	/*
		ExpressionVisitor and the different expression types
	*/
	struct ExpressionVisitor {
		virtual std::any visitLiteral(Literal* l) const = 0;
		virtual std::any visitAssignment(Assignment* a) const = 0;
		virtual std::any visitBinary(Binary* b) const = 0;
		virtual std::any visitUnary(Unary* u) const = 0;

	};
	struct Assignment : public BaseExpression {
		std::string key;
		BaseExpression* value;
		Assignment(const std::string& s, BaseExpression* v) : key(s), value(v) {};
		std::any accept(const ExpressionVisitor& v) override {
			return v.visitAssignment(this); //look up env key, if this key is already inserted, update the value, else insert
		};
		~Assignment() {};
	};
	struct Literal : public BaseExpression {
		std::any lit;

		Literal(std::any l) : lit(l) {};
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


	class Parser {
		typedef std::size_t TokenIndex;
		std::vector<Token> tokens;
		TokenIndex curr;
		
		bool is_end() const {
			return curr > tokens.size();
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
			std::string key = advance().word;
			while (match({ TOK_COL })) {
				BaseExpression* value = term();
				if (!match({ TOK_HASH })) {
					throw std::runtime_error("Missing #");
				}
				return new Assignment(key, value);
			}
			throw std::runtime_error("Missing Colon");
		};

		std::vector<BaseExpression*> parse() {
			std::vector<BaseExpression*> ast;
			while (match({ TOK_DOLLA })) {
				
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

	namespace Internal {
		template<typename PolicyType>
		class IBinaryPolicy {
			PolicyType a;
			typedef PolicyType::Type Type;
		public:
			IBinaryPolicy() {

			};
			Type evaluate_binary(std::any l, TokenType op, std::any r) {
				return a.evaluate(l, op, r);
			}
			~IBinaryPolicy() {};
		};

		struct BDouble {
			using Type = double;
			BDouble() {};
			Type evaluate(std::any l, TokenType op, std::any r) {
				Type left = std::any_cast<Type>(l);
				Type right = std::any_cast<Type>(r);
				switch (op) {
				case TOK_PLUS:
					return left + right;
				case TOK_MINUS:
					return left - right;
				case TOK_DIV:
					return left / right;
				case TOK_STAR:
					return left * right;
				case TOK_MOD:
					return (int)left % (int)right;

				};
			};
			~BDouble() {};
		};

		struct BString {
			using Type = std::string;
			BString() = default;
			Type evaluate(std::any l, TokenType op, std::any r) {
				Type left = std::any_cast<Type>(l);
				Type right = std::any_cast<Type>(r);

				switch (op) {
				case TOK_PLUS:
					return left + right;
				default:
					throw std::runtime_error("invalid binary operation.");
				};

			};
			~BString() = default;
		};
	}

	

	class Interpreter : public ExpressionVisitor {
		typedef std::pair<std::string, std::any> PairType;

		bool is_truthy(std::any obj) {
			if (!obj.has_value()) {
				if (obj.type() == typeid(bool)) {
					return std::any_cast<bool>(obj);
				}
			};
			return false;
		}
	public:
		std::map<std::string, std::any> env;
		Interpreter(const std::string& cfg_path) {
			Lexer l(cfg_path);
			Parser p(l.tokens);
			
			for (auto& node : p.ast) {
				auto e = evaluate(node);
				PairType pair = std::any_cast<PairType>(e);
				env[pair.first] = pair.second;
				//std::cout << pair.first << std::endl;
				//if (pair.second.type() == typeid(double)) {
					//std::cout << std::any_cast<double>(pair.second) << std::endl;
				//}
			}
			
		
		};

		std::any evaluate(BaseExpression* e) const {
			return e->accept(*this);
		};

		
		std::any Get(const std::string& key) {
			std::any e = env[key];
			return e;
		};
		

		std::any visitAssignment(Assignment* a) const override {
			std::pair<std::string, std::any> pair;
			pair.first = a->key;
			pair.second = evaluate(a->value);
			
			
			return pair;
		};


		std::any visitBinary(Binary* b) const override {
			std::any l = evaluate(b->l);
			std::any r = evaluate(b->r);
			TokenType op = b->op;
			
				if (l.type() == typeid(double) && r.type() == typeid(double)) {
					Internal::IBinaryPolicy<Internal::BDouble> d;
					return d.evaluate_binary(l, op, r);
				}

				if (l.type() == typeid(std::string) && r.type() == typeid(std::string)) {
					Internal::IBinaryPolicy<Internal::BString> s;
					return s.evaluate_binary(l, op, r);
				
				}

			

			return nullptr;
		}

		std::any visitUnary(Unary* u) const override {
			
			std::any r = evaluate(u->r);
			


			return nullptr;
		}

		std::any visitLiteral(Literal* l) const override {
			if (l->lit.type() == typeid(double)) {
				return std::any_cast<double>(l->lit);
			}
			if (l->lit.type() == typeid(std::string)) {
				return std::any_cast<std::string>(l->lit);
			}
			return nullptr;
		};


		~Interpreter() {};
	};


	
};