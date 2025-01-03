#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>
#include "CFGINTERP/interp.hpp"
#include "CFGINTERP/env.hpp"
#include <sstream>


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
        
        template<typename VarType>
        struct ConstructionPolicy {
            typedef VarType type;
            std::any v;
            std::string k;
            type vt;
            ConstructionPolicy(std::string k, std::any v) {
                this->v = v;
                this->k = k;
             }
            std::pair<int, std::string> construct() {
                return vt.construct(k, v);
            }
            ~ConstructionPolicy() {};
        };



        struct VarTypeDouble 
        {
            const std::string dolla = "$";
            const std::string col = ":";
            const std::string com = ",";
           
            VarTypeDouble() {};
           [[nodiscard]] std::pair<int, std::string> construct(std::string key, std::any value) const {
               double v = std::any_cast<double>(value);
               std::string str_lit = std::to_string(v);
               int byte_size = 0;
               byte_size += col.length();
               byte_size += key.length();
               byte_size += str_lit.length();
               byte_size += com.length();
               
               std::string serializable = dolla + key + col + str_lit + com;
               return { byte_size + 1, serializable };

           };
           ~VarTypeDouble() {};
           
        };

        struct VarTypeString 
        {
          
             const std::string dolla = "$";
             const std::string col = ":";
             const std::string com = ",";
             const std::string quote = "\"";
            
            VarTypeString() {};
            [[nodiscard]] std::pair<int, std::string> construct(std::string key, std::any value) const {
                std::string str_lit = std::any_cast<std::string>(value);
                int byte_size = 0;
                byte_size += key.length();
                byte_size += str_lit.length();
                byte_size += quote.length() * 2;
                byte_size += com.length();
               
                std::string serializable = dolla + key + col + quote + str_lit + quote + com;
                return { byte_size + 1, serializable };

            };
            ~VarTypeString() {};
        };


        std::pair<int, std::string> construct_variable(std::string key, std::any value) {
         
            std::pair<int, std::string> var_info = {};
            
            if (value.type() == typeid(double)) {
                ConstructionPolicy<VarTypeDouble> vcp(key, value);
                return vcp.construct();
            }
            else {
                ConstructionPolicy<VarTypeString> vcp(key, value);
                return vcp.construct();

            }
          

           
           
        }

        void serialize() {
            CircCFGInterp::Environment* global = this->interp->glob;
            std::ofstream ofs(cfg_path, std::ios::trunc);

            for (auto it = global->members.rbegin(); it != global->members.rend(); it++) {
                //first = byte_size
                //second = serializable text
                std::pair<int, std::string> var_info = construct_variable(it->first, it->second);
                ofs.write(var_info.second.c_str(), var_info.first);
            }
            ofs.close();
        };
 

    public:
        std::string cfg_path;
        CFGLoader(const std::string& cfg) : cfg_path(cfg) {
            try {
                authenticate_circ_extension(cfg);
                interp = new CircCFGInterp::Interpreter(cfg);
           
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

       
        ~CFGLoader() {
            serialize();
        };
    };
}

