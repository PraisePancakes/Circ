#pragma once
#include <vector>
#include <iostream>
#define CLU_SIZE 9
namespace Serialization {
    typedef const std::vector<CircCFGInterp::Token> TokenVector;
    typedef std::pair<int, std::string> var_info_t;

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