#pragma once
#include <iostream>

namespace Serialization {
	namespace CircFormat {
		/*
		*	The formatter will simply take a serialized representation (string) of the Circ file and simply format it.
		*/
		class Formatter {
			std::string src;
		public:
			Formatter(const std::string& s) : src(s) {};
			std::string fmt() {};
			~Formatter() {};
		};
	}
}