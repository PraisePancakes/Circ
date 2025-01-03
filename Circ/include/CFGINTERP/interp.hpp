#pragma once
#include "expressions.hpp"
#include "env.hpp"
#include "rdparser.hpp"
#include "carch.hpp"

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

	

	class Interpreter : public ExpressionVisitor, public StatementVisitor {


		bool is_truthy(std::any obj) const {
			if (!obj.has_value()) {
				return false;
			};
			if (obj.type() == typeid(bool)) {
				return std::any_cast<bool>(obj);
			}

			return true;
		}


		std::any visitArray(Array* a) const override {
			std::vector<std::any> arr;
			if (a->arr.size() == 0) {
				return arr;
			}

			std::any t = evaluate(a->arr[0]);

			for (BaseExpression* i : a->arr) {
				Literal* literal = (Literal*)i;
				std::any v = evaluate(literal);
				if (t.type().hash_code() != v.type().hash_code()) {
					throw std::runtime_error("Array type mismatch.");
				}
				arr.push_back(v);
			}

			return arr;
		}


		std::any visitAssignment(Assignment* a)const override {
			std::any r = evaluate(a->v);
			env->assign(a->k, r);
			return r;
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

		std::any visitObject(Object* o) const override {
			std::map<std::string, std::any> members;
			for (const auto& [key, value] : o->members) {
				std::any v = evaluate(value);
				members[key] = v;
			}

			Environment* new_env = new Environment(members, env);
			env = new_env;
			
			return new_env;
		}

		

		std::any visitVariable(Variable* v) const override {
			return env->resolve(v->name);
		}

		
		std::any visitDecl(Decl* d) override {
			std::any val = nullptr;
			
			if (d->value) {
				val = evaluate(d->value);
			}
			env->members.insert(std::pair<std::string, std::any>(d->key, val));
			return nullptr;
		}

		std::any execute(BaseStatement* s) {
			return s->accept(this);
		}

		std::vector<BaseStatement*> stree;
	public:
		inline static Environment* glob = new Environment();
		inline static Environment* env;

		Interpreter(const std::string& cfg_path) {
			
			env = glob;
			Lexer l(cfg_path);
			Parser p(l.tokens);

			stree = p.statements;
			try {
				for (auto& s : stree) {
					execute(s);
				}
				
				
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
			}


			
		};

		
		std::any evaluate(BaseExpression* e) const {
			std::any v = e->accept(*this);
			return v;
		};


		~Interpreter() {};
	};

	

	
};