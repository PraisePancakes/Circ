#pragma once
#include "expressions.hpp"
#include "env.hpp"
#include "rdparser.hpp"
/*
* @why :
*		The interpreter system is responsible for one thing and one thing only, 
*	it converts the Circ file into usable objects in memory. The main purpose for the interpreter is to interpret values to the archive.
*	The archive then takes these values and are accessed via the CFGLoader API
* 
* 
**/
namespace Serialization {


	

	namespace Internal {
		/*
		*
		* @type :
		*	The policy adapter takes a type in which can be evaluated in any if not all arithmetic manner.
			This policy type then evaluates via its respsective arthimetic policy
		* @params : 
		*	passing the left expression, operator, and the right expression
		**/
		template<typename PolicyType>
		class IBinaryPolicy {
		public:
			typedef PolicyType::Type Type;
			static Type evaluate_binary(std::any l, TokenType op, std::any r) {
				return PolicyType::evaluate(l, op, r);
			}
		
		};

		struct BDouble {
			using Type = double;
		
			static Type evaluate(std::any l, TokenType op, std::any r) {
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
				default:break;
				};
				throw std::runtime_error("Serialization::Internal::BDouble policy operate.");
			};
			
		};

		struct BInt {
			using Type = int;

			static Type evaluate(std::any l, TokenType op, std::any r) {
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
				default:break;

				};
				throw std::runtime_error("Serialization::Internal::BInt policy failed to operate.");
			};
		};

		struct BString {
			using Type = std::string;
			static Type evaluate(std::any l, TokenType op, std::any r) {
				Type left = std::any_cast<Type>(l);
				Type right = std::any_cast<Type>(r);
				
				switch (op) {
				case TOK_PLUS:
					return left + right;
				default:
					throw std::runtime_error("Serialization::Internal::BString policy failed to operate.");
				};

			};
			
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
		bool known_type(std::any v) const {
			if (v.type() == typeid(double) 
				|| v.type() == typeid(std::string) 
				|| v.type() == typeid(std::vector<std::any>)
				|| v.type() == typeid(int)) {
				return true;
			}

			throw std::runtime_error("Uknown array type");
			
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
				if (known_type(v)) {
					if (t.type().hash_code() != v.type().hash_code()) {
						throw std::runtime_error("Array type mismatch.");
					}
					
				}
				arr.push_back(v);
			}

			return arr;
		}

		std::any visitBinary(Binary* b) const override {
			std::any l = evaluate(b->l);
			std::any r = evaluate(b->r);
			TokenType op = b->op;

			if (l.type() == typeid(double) && r.type() == typeid(double)) {
				return Internal::IBinaryPolicy<Internal::BDouble>::evaluate_binary(l, op, r);
			}

			if (l.type() == typeid(std::string) && r.type() == typeid(std::string)) {
				return Internal::IBinaryPolicy<Internal::BString>::evaluate_binary(l, op, r);
			}

			if (l.type() == typeid(int) && r.type() == typeid(int)) {
				return Internal::IBinaryPolicy<Internal::BInt>::evaluate_binary(l, op, r);
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
			if (l->lit.type() == typeid(int)) {
				return std::any_cast<int>(l->lit);
			}


			return nullptr;
		};

		std::any visitObject(Object* o) const override {
			std::map<std::string, std::any> members;
			for (const auto& [key, value] : o->members) {
				std::any v = evaluate(value);
				std::pair<std::string, std::any> kvp(key, v);
				members.insert(kvp);
			}
			Environment* new_env = new Environment(members, env);
			
			return new_env;
		}
		
		std::any visitDecl(Decl* d) override {
			std::any val = nullptr;
			if (d->value) {
				val = evaluate(d->value);
				std::pair<std::string, std::any> kvp(d->key, val);
				env->insert(kvp.first, kvp.second);
			}
			
			return nullptr;
		}

		std::any execute(BaseStatement* s) {
			return s->accept(this);
		}

		std::vector<BaseStatement*> stree;
	public:
		inline static Environment* glob = new Environment();
		inline static Environment* env = nullptr;

		Interpreter(const std::string& cfg_path) {
			env = glob;
			Lexer l(cfg_path);
			Parser p(l.tokens);
			stree = p.statements;
			for (auto& s : stree) {
				execute(s);
			}
		};

		
		std::any evaluate(BaseExpression* e) const {
			std::any v = e->accept(*this);
			return v;
		};


		~Interpreter() {};
	};

	
}

