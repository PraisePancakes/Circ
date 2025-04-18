#pragma once
#include <vector>
#include <iostream>

#define CLU_SIZE 9
namespace Serialization {
	enum TokenType {
		TOK_ERROR = -1,
		TOK_HASH,
		TOK_SLASH,
		TOK_STRING,
		TOK_VAR,
		TOK_NUM,
		TOK_COL,
		TOK_ENTRY,
		TOK_DOLLA,
		TOK_LCURL,
		TOK_RCURL,
		TOK_LPAREN,
		TOK_RPAREN,
		TOK_LBRAC,
		TOK_RBRAC,
		TOK_COMMA,
		TOK_PLUS,
		TOK_MINUS,
		TOK_DIV,
		TOK_STAR,
		TOK_BANG,
		TOK_SEMI,
		TOK_MOD,
		TOK_EOF
	};


	struct Token {
		std::string word;
		size_t line;
		size_t where;
		TokenType t;
		std::any lit;
		Token(const std::string& w, size_t l, size_t where, TokenType t, std::any lit) : word(w), line(l), where(where), t(t), lit(lit) {};
		~Token() {};
	};

    typedef std::vector<Token> TokenVector;
    typedef std::pair<size_t, std::string> var_info_t;

	


    enum CT {
        DOLLA,
        COL,
        COMMA,
        NEW_LINE,
        QUOTE,
        LCURL,
        RCURL,
        LBRAC,
        RBRAC
    };

    const inline static char construction_lookup[CLU_SIZE] = {
        '$',
        ':',
        ',',
        '\n',
        '\"',
        '{',
        '}',
        '[',
        ']'
    };
    
}