#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>
#include "CFGINTERP/interp.hpp"


template<typename WrapperType>
WrapperType CFGAttr(const std::string& key, CircCFGInterp::Interpreter* interp) {
    return std::any_cast<WrapperType>(interp->Get(key));
};

void CircInitWindow(CircCFGInterp::Interpreter* interp) {
    auto w = CFGAttr<double>("window_width", interp);
    auto h = CFGAttr<double>("window_height", interp);
    auto t = CFGAttr<std::string>("window_title", interp);
    InitWindow(w, h, t.data());
};

//std::string title = CFGAttr<std::string>("window_title");


namespace Circ {
   
    class CFGLoader {
    public:
        CircCFGInterp::Interpreter* interp;
        CFGLoader(const std::string& cfg) {
            interp = new CircCFGInterp::Interpreter(cfg);
        };


        ~CFGLoader() {};
    };
}

