#pragma once
#include "interp.hpp"
#include "expressions.hpp"



namespace CircCFGInterp {
	class Env {
	public:
		typedef std::pair<std::string, std::any> PairType;
		typedef std::map<std::string, std::any> MapType;
	private:
		inline static int env_level = 0;


		MapType find_and_recur(int& level, std::initializer_list<std::string> obj_key_path, const MapType& current_env) {
			if (level >= obj_key_path.size()) {
				return current_env;
			}

			std::string k = obj_key_path.begin()[level];
			auto it = current_env.find(k);

			if (it == current_env.end()) {
				throw std::runtime_error("Key '" + k + "' not found in the object.");
			}

			if (it->second.type() != typeid(MapType)) {
				throw std::runtime_error("Key '" + k + "' does not contain an object.");
			}

			return find_and_recur(++level, obj_key_path, std::any_cast<MapType>(it->second));
		}
		MapType env;



	public:
		Env(const Interpreter* interp) {
			for (BaseExpression* node : interp->astree()) {
				std::any e = interp->evaluate(node);
				PairType pair = std::any_cast<PairType>(e);
				env[pair.first] = pair.second;
				//std::cout << pair.second.type().name() << "\n\n" << std::endl;
				//std::cout << pair.first << std::endl;
				//if (pair.second.type() == typeid(double)) {
					//std::cout << std::any_cast<double>(pair.second) << std::endl;
				//}
			}
		};
		std::any Get(const std::string& key) {
			std::any e = env[key];
			return e;
		};

		MapType GetObj(std::initializer_list<std::string> obj_key_path) {
			env_level = 0;
			return find_and_recur(env_level, obj_key_path, env);
		};
		~Env() {};

	};

	
}