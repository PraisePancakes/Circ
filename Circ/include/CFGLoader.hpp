#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>
#include "CFGINTERP/interp.hpp"
#include "CFGINTERP/env.hpp"



//std::string title = CFGAttr<std::string>("window_title");


namespace Circ {
   
    class CFGLoader {
    public:
       
        CircCFGInterp::Env* env;
        CFGLoader(const std::string& cfg) {
            CircCFGInterp::Interpreter* interp = new CircCFGInterp::Interpreter(cfg);
            env = new CircCFGInterp::Env(interp);
        };

        void CircInitWindow() {
            auto w = CFGAttr<double>("window_width");
            auto h = CFGAttr<double>("window_height");
            auto t = CFGAttr<std::string>("window_title");
            InitWindow(w, h, t.data());
        };

        template<typename WrapperType>
        WrapperType CFGAttr(const std::string& key) {
            return std::any_cast<WrapperType>(env->Get(key));
        };

        CircCFGInterp::Env::MapType CFGObj(std::initializer_list<std::string> obj_path) {
            return CircCFGInterp::Env::MapType(env->GetObj(obj_path));
        }
        ~CFGLoader() {};
    };
}

