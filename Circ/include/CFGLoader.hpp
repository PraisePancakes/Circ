#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>
#include "CFGINTERP/archive.hpp"
#include "CFGINTERP/env.hpp"
#include <sstream>
#include "debug.hpp"

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

        void print_env_ast(Environment* curr) {
            std::clog << "[ Environment ]" << std::endl;
            for (auto& kvp : curr->members) {
                std::clog << kvp.first << " , ";
                if (kvp.second.type() == typeid(Environment*)) {
                    print_env_ast(std::any_cast<Environment*>(kvp.second));
                    std::clog << "[ END ]" << std::endl;
                }
            }
            std::clog << std::endl;
        }
    public:
        Serialization::Archive* arc;
        std::string cfg_path;
        CFGLoader(const std::string& cfg) : cfg_path(cfg), arc(nullptr) {
            try {
                authenticate_circ_extension(cfg_path);
                interp = new Serialization::Interpreter(cfg_path);
                arc = new Serialization::Archive(interp, cfg_path);
#if defined(__CIRC__DEBUG__) && __CIRC__DEBUG__
                
                    print_env_ast(interp->glob);
#endif
              
            }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
                exit(EXIT_FAILURE);
            }
            catch (std::ofstream::failure& e) {
                std::cerr << e.what() << std::endl;
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
            catch (std::ofstream::failure& e) {
                std::cerr << e.what() << std::endl;
            }
        };
        
        void CFGAttrSet(std::initializer_list<std::string> kp, const std::any& v) {
            try {
                arc->Set(kp, v);
            }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
                std::cerr << "[ WARN ] Failed to assign at : " << std::endl;
                for (auto s : kp) {
                    std::cerr << s << " => ";
                }
                std::cerr << "NULL" << std::endl;
                std::cerr << "attempted to assign type : " << v.type().name() << " to key => " << kp.begin()[kp.size() - 1] << std::endl;
                std::cerr << "assignment attempt has been discarded" << std::endl;
            }
        };

        template<typename WrapperType>
        WrapperType CFGAttr(std::initializer_list<std::string> key_path) {
            WrapperType ret;
            try {
                ret = arc->Get<WrapperType>(key_path);
                return ret;
            }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
            catch (std::bad_any_cast& e) {
                std::cerr << e.what() << std::endl;
            }
           
        }

        ~CFGLoader() {
          
        };
    };
}

