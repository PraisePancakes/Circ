#pragma once
#include <iostream>
#include <vector>
#include "lexer.hpp"
#define MAX_POOL 15
namespace Serialization {
	//convert to bitmask for bitwise or 
	enum LogType : uint32_t {
		UNKNOWN = 1 << 0,
		WARNING = 1 << 1,
		SYNTAX = 1 << 2,
		RUNTIME_CAST = 1 << 3,
		VITAL = 1 << 4,
		ENTRY = 1 << 5
	};

	constexpr enum LogType operator|(const enum LogType e, const enum LogType i) {
		return (enum LogType)(uint32_t(e) | uint32_t(i));
	}
	constexpr enum LogType operator|=(const enum LogType e, const enum LogType i) {
		return (enum LogType)(e |= i);
	}
	constexpr enum LogType operator&(const enum LogType e, const enum LogType i) {
		return (enum LogType)(uint32_t(e) & uint32_t(i));
	}
	constexpr enum LogType operator&=(const enum LogType e, const enum LogType i) {
		return (enum LogType)(e &= i);
	}

	inline static std::vector<std::string> tlookup = {
		"UNKNOWN",
		"WARNING",
		"SYNTAX",
		"RUNTIME_CAST",
		"VITAL",
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

		std::string proccess_log_flags(LogType flags) {
			std::string r = "";
			for (size_t i = 0; i < tlookup.size(); i++) {
				LogType set_bit = (LogType)(1 << i);
				if (flags & set_bit) {
					r += (tlookup[i]);
					r += ", ";
				}
				
			}
			return r;
		}

		void log(LogType flags, Token t, const std::string& desc) {
			if (cursor >= MAX_POOL) cursor = 0;
			std::string log_type = proccess_log_flags(flags);
			std::string line = std::to_string(t.line);
			std::string where = std::to_string(t.where);
			std::string word = t.word;
			std::string message = "__CIRCFG RUNTIME EXCEPTION__ :\n\t type [ " + log_type + " ]\n\t line : " + line + " , " + where + " at token '" + word + "' " + desc;
			log_pool[cursor++] = message;

		};
	};



}

