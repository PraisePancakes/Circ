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
        void authenticate_circ_extension(const std::string& cfg) {
            size_t i = cfg.length();
            
            std::string ext = "";
            while (cfg[i] != '.') {
                --i;
                ext += cfg[i];
                if (i == 0) {
                    throw std::runtime_error("invalid config path");
                }
            }

            if (ext != "cric.") {
                throw std::runtime_error("Wrong extension type : " + ext + "\n");
            }


        };
    public:
       
        CFGLoader(const std::string& cfg) {
            try {
                authenticate_circ_extension(cfg);
                interp = new CircCFGInterp::Interpreter(cfg);
            }
            catch(std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
           
           
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
                std::cout << value.type().name() << std::endl;
                return std::any_cast<WrapperType>(value);
            }
           
            throw std::runtime_error("Key path did not resolve to a valid value.");
        }

       
        ~CFGLoader() {};
    };
}

