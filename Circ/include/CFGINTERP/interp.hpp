#pragma once
#include "expressions.hpp"
#include "env.hpp"
#include "rdparser.hpp"

namespace CircCFGInterp {
	

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

	
		bool is_truthy(std::any obj) const {
			if (!obj.has_value()) {
				return false;
			};
			if (obj.type() == typeid(bool)) {
				return std::any_cast<bool>(obj);
			}
			
			return true;
		}
		
		std::any visitDeclaration(Declaration* a) const override {
			std::pair<std::string, std::any> pair;
			pair.first = a->key;
			pair.second = evaluate(a->value);


			return pair;
		};

		std::any visitObject(Object* a) const override {
			
			std::map<std::string, std::any> members;
			for (const auto& [key, value] : a->members) {
				
				members[key] = evaluate(value);
			}
			Environment* env = new Environment(members);
			env->outer = level;
			level = env;
			return env;
		};

		std::any visitArray(Array * a) const override {
			std::any arr;
			BaseExpression* first = a->arr[0];
			std::any t = evaluate(first);
			if (t.type() == typeid(double)) {
				std::vector<double> arr = {};

				for (BaseExpression* i : a->arr) {
					Literal* literal = (Literal*)i;
					std::any v = evaluate(literal);
					arr.push_back(std::any_cast<double>(v));
				}

				return arr;
			}

			if (t.type() == typeid(std::string)) {
				std::vector<std::string> arr = {};

				for (BaseExpression* i : a->arr) {
					Literal* literal = (Literal*)i;
					std::any v = evaluate(literal);
					arr.push_back(std::any_cast<std::string>(v));
				}

				return arr;
			}

			throw std::runtime_error("Invalid array type");
		}


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
		/*
		!val
		-val
		!(val + val1)
		-(val + val1)
	
		*/
		std::any visitGrouping(Grouping* gr) const override {
			return evaluate(gr->g);
		}

		std::any visitUnary(Unary* u) const override {
			
			std::any r = evaluate(u->r);
			
			if (u->op == TOK_BANG) {
				return !(is_truthy(r));
			}
			if (u->op == TOK_MINUS) {
				
				return std::any_cast<double>(r) * -1;
			}
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
		BaseExpression* ast;
	public:
		inline static Environment* level = nullptr;
		Interpreter(const std::string& cfg_path) {
			Lexer l(cfg_path);
		
			Parser p(l.tokens);
			
			ast = p.ast;
			evaluate(ast);
		};
		BaseExpression* astree() const {
			return ast;
		}
		std::any evaluate(BaseExpression* e) const {
			std::any v = e->accept(*this);
			
			return v;
		
		};


		~Interpreter() {};
	};

	

	
};