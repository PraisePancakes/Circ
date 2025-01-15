#pragma once
#include <iostream>
#include <vector>
#include "lexer.hpp"
#define MAX_POOL 15
namespace Serialization {



	//convert to bitmask
	enum LogType {
		UNKNOWN = 0,
		WARNING,
		SYNTAX,
		RUNTIME_CAST,
		ENTRY
	};

	inline static std::vector<std::string> tlookup = {
		"UNKNOWN",
		"WARNING",
		"SYNTAX",
		"RUNTIME_CAST",
		"ENTRY"
	};

	class ParseErrorLogger {
	private:
		std::string log_pool[MAX_POOL];
		unsigned short cursor;
		ParseErrorLogger() : cursor(0) {};
		~ParseErrorLogger() {};
	
		

	public:
		static ParseErrorLogger& instance() {
			static ParseErrorLogger logger;
			return logger;
		}
		void print_list() const noexcept {
			for (int i = 0; i < cursor; i++) {
				std::cerr << log_pool[i] << std::endl;
			}
		}

		size_t logsize() const noexcept {
			return cursor;
		}

		void log(LogType log_type, Token t, const std::string& desc) {
			
			std::string type = tlookup[log_type];
			std::string line = std::to_string(t.line);
			std::string where = std::to_string(t.where);
			std::string word = t.word;
			
			std::string message = "__CIRCFG RUNTIME EXCEPTION__ :\n\t type [ " + type + " ]\n\t line : " + line + " , " + where + " at token '" + word + "' " + desc;

			
			log_pool[cursor++] = message;

		};
	};



}

