#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>
#include "CFGINTERP/interp.hpp"






//std::string title = CFGAttr<std::string>("window_title");


namespace Circ {
   
    class CFGLoader {
    public:
        CircCFGInterp::Interpreter* interp;
        CFGLoader(const std::string& cfg) {
            interp = new CircCFGInterp::Interpreter(cfg);
            CircCFGInterp::Interpreter::MapType m = CFGObj({ "object" });

        };

        void CircInitWindow() {
            auto w = CFGAttr<double>("window_width");
            auto h = CFGAttr<double>("window_height");
            auto t = CFGAttr<std::string>("window_title");
            InitWindow(w, h, t.data());
        };

        template<typename WrapperType>
        WrapperType CFGAttr(const std::string& key) {
            return std::any_cast<WrapperType>(interp->Get(key));
        };

        CircCFGInterp::Interpreter::MapType CFGObj(std::initializer_list<std::string> obj_path) {
            return CircCFGInterp::Interpreter::MapType(interp->GetObj(obj_path));
        }
        ~CFGLoader() {};
    };
}

