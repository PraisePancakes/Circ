#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>
#include "CFGINTERP/archive.hpp"
#include "CFGINTERP/env.hpp"
#include <sstream>

/*
* @why :
*		The CFGLoader is a wrapper API around the main components of this api and their systemic logic. The interpreter
* composed of a lexer, parser, and environments, converts the parse tree into an object in memory that is then passed to the Archive which handles
* lookups, entries, removals, etc..
* Think about what an interpreter does in real life. It translates a language so that another person (in this case the Archive) can respond with their known language
* (Archive knows what an object in memory is, interpreter knows what the config file structure is and what the object in memory is).
*
**/


namespace Circ {
    using namespace Serialization;
    class CFGLoader {
        Serialization::Interpreter* interp;
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

        void print_env(Environment* curr) {
            std::cout << "[ Environment ]" << std::endl;
            for (auto& kvp : curr->members) {
                std::cout << kvp.first << " , ";
                if (kvp.second.type() == typeid(Environment*)) {
                    print_env(std::any_cast<Environment*>(kvp.second));
                    std::cout << "[ END ]" << std::endl;
                }
            }
            std::cout << std::endl;
        }

    public:
        Serialization::Archive* arc;
        std::string cfg_path;
        CFGLoader(const std::string& cfg) : cfg_path(cfg), arc(nullptr) {
            try {
                authenticate_circ_extension(cfg_path);
                interp = new Serialization::Interpreter(cfg_path);
                arc = new Serialization::Archive(interp, cfg_path);
                print_env(interp->glob);
            }
            catch (std::exception& e) {
                exit(EXIT_FAILURE);
            }
        };

        void Serialize() {
            try {
                arc->Serialize();
            }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
               
            }
            
        };
        
        void CFGAttrSet(std::initializer_list<std::string> kp, const std::any& v) {
            try {
                arc->Set(kp, v);
            }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
                std::cerr << "Failed to assign at : " << std::endl;
                for (auto s : kp) {
                    std::cout << s << " => ";
                }
                std::cout << "NULL" << std::endl;
                std::cout << "attempted to assign type : " << v.type().name() << " to key => " << kp.begin()[kp.size() - 1] << std::endl;
                throw;
            }
           
            
        };

        template<typename WrapperType>
        WrapperType CFGAttr(std::string k) {
            return arc->Get<WrapperType>(k);
        };


        template<typename WrapperType>
        WrapperType CFGAttr(std::initializer_list<std::string> key_path) {
            return arc->Get<WrapperType>(key_path);
        }

       
        ~CFGLoader() {
          
        };
    };
}

