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

        enum CT {
            DOLLA,
            COL,
            COMMA,
            NEW_LINE,
            QUOTE,
            LCURL,
            RCURL
        };

        const inline static std::vector<char> construction_lookup = {
            '$',
            ':',
            ',',
            '\n',
            '\"',
            '{',
            '}'
        };

        typedef std::pair<int, std::string> var_info_t;

        template<typename VarType>
        struct IConstructionPolicy {
            typedef VarType type;
            static var_info_t construct(std::string k, std::any v) {
                return VarType::construct(k, v);
            }
            ~IConstructionPolicy() {};
        };

        struct VarTypeDouble 
        {
        public:
            [[nodiscard]] static std::string construct_serializable(std::string k, std::string str_lit) noexcept {
                return (construction_lookup[CT::DOLLA]
                    + k
                    + construction_lookup[CT::COL]
                    + str_lit
                    + construction_lookup[CT::COMMA]
                    + construction_lookup[CT::NEW_LINE]);
            };
            VarTypeDouble() {};
           [[nodiscard]] static var_info_t construct(std::string key, std::any value)  noexcept {
               double v = std::any_cast<double>(value);
               std::string str_lit = std::to_string(v);
               int byte_size = 0;
               std::string serializable = construct_serializable(key, str_lit);
               byte_size += serializable.length();
               return { byte_size , serializable };

           };
           ~VarTypeDouble() {};
           
        };

        struct VarTypeString 
        {
      
          
        public:
            VarTypeString() {};
            [[nodiscard]] static std::string construct_serializable(std::string k, std::string str_lit)  noexcept {
                return (construction_lookup[CT::DOLLA]
                    + k
                    + construction_lookup[CT::COL]
                    + construction_lookup[CT::QUOTE]
                    + str_lit
                    + construction_lookup[CT::QUOTE]
                    + construction_lookup[CT::COMMA]
                    + construction_lookup[CT::NEW_LINE]);
            };

            [[nodiscard]] static var_info_t construct(std::string key, std::any value)  noexcept {
                std::string str_lit = std::any_cast<std::string>(value);
                int byte_size = 0;
                std::string serializable = construct_serializable(key, str_lit);
                byte_size += serializable.length();
                
                return { byte_size  , serializable };

            };
            ~VarTypeString() {};
        };

        var_info_t construct_variable(std::string key, std::any value) {
            if (value.type() == typeid(double)) {
                return IConstructionPolicy<VarTypeDouble>::construct(key, value);
            }
            else {
                return IConstructionPolicy<VarTypeString>::construct(key, value);
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

