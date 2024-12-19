#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>
#include "CFGINTERP/interp.hpp"
#include "CFGINTERP/env.hpp"



//std::string title = CFGAttr<std::string>("window_title");


namespace Circ {
   
    class CFGLoader {
        CircCFGInterp::Interpreter* interp;
    public:
       
        CFGLoader(const std::string& cfg) {
           interp = new CircCFGInterp::Interpreter(cfg);
            
        };

      
        template<typename WrapperType>
        WrapperType CFGAttr(std::string k) {
            return std::any_cast<WrapperType>(interp->level->resolve(k));
        };

        template<typename WrapperType>
        WrapperType CFGAttr(std::initializer_list<std::string> key_path) {
            CircCFGInterp::Environment* current = interp->level;
            std::any value;

            for (const auto& key : key_path) {
                if (!current) {
                    throw std::runtime_error("Invalid key path: " + key);
                }

                value = current->resolve(key);

                if (value.type() == typeid(CircCFGInterp::Environment*)) {
                    current = std::any_cast<CircCFGInterp::Environment*>(value);
                }
                else {
                    current = nullptr; 
                }
            }

            // Cast the final value to the desired type
            if (value.has_value()) {
                return std::any_cast<WrapperType>(value);
            }

            throw std::runtime_error("Key path did not resolve to a valid value.");
        }

       
        ~CFGLoader() {};
    };
}

