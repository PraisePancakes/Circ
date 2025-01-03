#pragma once

namespace CircCFGInterp {
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

		void assign(const std::string& name, const std::any& value) {
			if (members.find(name) != members.end()) {
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

	

	
}

typedef CircCFGInterp::Environment CircObject;