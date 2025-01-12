#pragma once
#include <iostream>
#include "circtypes.hpp"
#include "lexer.hpp"

namespace Serialization {
	namespace CircFormat {
		
		// formatter assumes correct syntax in input serialized string
		class CircFormat {
			
			inline std::string do_circ_fmt(TokenVector& ref) {
				std::string fmt = "";
				for (size_t i = 0; i < ref.size(); i++) {
					fmt += ref[i].word;
				}
				
				return fmt;
			}
		public:
			unsigned short depth = 0;
			size_t cursor = 0;

			inline std::string operator()(TokenVector& ref_to_fmt) {
				return do_circ_fmt(ref_to_fmt);
			};
		};

		/*
		*	The formatter will take a serialized representation (string) of the Circ file and simply format it.
		*/
		template<typename FmtT = CircFormat>
		class FormatTypeFunctor {
		public:
			 inline std::string operator()(TokenVector& ref_to_fmt) {
				FmtT t;
				return t(ref_to_fmt);
			};
		};
	}
}