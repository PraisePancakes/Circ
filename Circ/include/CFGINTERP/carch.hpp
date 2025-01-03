#pragma once
#include "env.hpp"
namespace CircCFGInterp {

	class CARCH {
		Environment* old_global;
		std::string cfg;
	public:
		CARCH(Environment* glob, const std::string& cfgwritable) : old_global(glob), cfg(cfgwritable) {};
		
		void set(const std::string& k, const std::any& v) {
			old_global->assign(k, v);
		}
		
	};
	

}