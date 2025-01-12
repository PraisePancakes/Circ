#pragma once
namespace Serialization {

		class Environment {
		public:
			std::map<std::string, std::any> members;
			Environment* outer;
			Environment() {
				outer = nullptr;
			}
			Environment(std::map<std::string, std::any> members, Environment* outer = nullptr)
				: members(members), outer(outer) {

			}



			std::any resolve(const std::string& name) {
				if (members.find(name) != members.end()) {
					return members[name];
				}
				else if (outer) {
					return outer->resolve(name);
				}

				throw std::runtime_error("Variable not found: " + name);
			}
			void insert(const std::string& k, const std::any& v) {
				std::cout << k << std::endl;
				if (members.find(k) == members.end()) {
					members.insert(std::pair<std::string, const std::any&>(k, v));
				}
				else {
					assign(k, v);
				}
			}

			void assign(const std::string& name, const std::any& value) {
				if (members.find(name) != members.end()) {
					auto prev = members[name];
					if (prev.type() != value.type()) {
						throw std::runtime_error("Mismatched value types");
					}
					members[name] = value;
				}
				else if (outer) {
					outer->assign(name, value);
				}
				else {
					throw std::runtime_error("Undefined variable: " + name);
				}
			}
		};


	typedef Environment CircObject;
}
