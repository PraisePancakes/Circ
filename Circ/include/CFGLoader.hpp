#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>
#include "CFGINTERP/interp.hpp"
#include "CFGINTERP/env.hpp"
#include "CFGINTERP/carch.hpp"



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

        CircCFGInterp::CARCH* arch;


    public:
       
        CFGLoader(const std::string& cfg) {
            try {
                authenticate_circ_extension(cfg);
                interp = new CircCFGInterp::Interpreter(cfg);
                arch = new CircCFGInterp::CARCH(interp->glob, cfg);
            }
            catch(std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
           
           
        };
        

        template<typename AttrType>
        void CFGAttrSet(std::initializer_list<std::string> kp, const std::any& v) {
            try {
                AttrType check_type = CFGAttr<AttrType>(kp);
                CircCFGInterp::Environment* current = interp->env;
                std::any value;
                const std::string last_key = kp.begin()[kp.size() - 1];
                for (size_t i = 0; i < kp.size() - 1; i++) {
                    std::string curr_key = kp.begin()[i];
                    if (!current) {
                        throw std::runtime_error("Invalid key path: " + curr_key);
                    }
                    value = current->resolve(curr_key);
                    if (value.type() == typeid(CircCFGInterp::Environment*)) {
                        current = std::any_cast<CircCFGInterp::Environment*>(value);
                    }
                    
                }
                current->assign(last_key, v);
            }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
           
            
        };


        template<typename WrapperType>
        WrapperType CFGAttr(std::string k) {
            return std::any_cast<WrapperType>(interp->env->resolve(k));
        };


        template<typename WrapperType>
        WrapperType CFGAttr(std::initializer_list<std::string> key_path) {
            try {
                CircCFGInterp::Environment* current = interp->env;
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
                    std::string value_type = value.type().name();

                    try {
                        auto v = std::any_cast<WrapperType>(value);
                        return v;
                    }
                    catch (std::exception& e) {
                        std::cerr << "Invalid cast from value type : " << value_type << std::endl;
                    }
                   
                }

                throw std::runtime_error("Key path did not resolve to a valid value.");
            }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
               
           }
            
        }

       
        ~CFGLoader() {};
    };
}

