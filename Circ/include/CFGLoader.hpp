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
            RCURL,
            LBRAC,
            RBRAC
        };

        const inline static std::vector<char> construction_lookup = {
            '$',
            ':',
            ',',
            '\n',
            '\"',
            '{',
            '}',
            '[',
            ']'
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

        struct VarTypeArray {
            [[nodiscard]] static std::string construct_array(std::any value) noexcept {

                std::string ret = ""; 
                ret += construction_lookup[CT::LBRAC];

                std::vector<std::any> arr = std::any_cast<std::vector<std::any>>(value);
                for (size_t i = 0; i < arr.size(); i++) {
                    std::any v = arr[i];

                    if (v.type() == typeid(double)) {
                        double e = std::any_cast<double>(v);
                        ret += std::to_string(e);
                        
                    }
                    else if (v.type() == typeid(std::string)) {
                        std::string e = std::any_cast<std::string>(v);
                        ret += construction_lookup[CT::QUOTE];
                        ret += e;
                        ret += construction_lookup[CT::QUOTE];
                    }
                    else if (v.type() == typeid(std::vector<std::any>)) {
                        ret += construct_array(v);
                       
                    } if (v.type() == typeid(int)) {
                        int e = std::any_cast<int>(v);
                        ret += std::to_string(e);
                     
                    }
                    if (i < arr.size() - 1) {
                        ret += construction_lookup[CT::COMMA];
                    }

                }

                ret += construction_lookup[CT::RBRAC];
                return ret;
            };

        public:
            [[nodiscard]] static std::string construct_serializable(std::string key, std::any value) noexcept {
             
                std::string ret =
                    construction_lookup[CT::DOLLA]
                    + key
                    + construction_lookup[CT::COL];
                   
                ret += construct_array(value);
                ret += construction_lookup[CT::COMMA];
                ret += construction_lookup[CT::NEW_LINE];
                
                return ret;
            };
            [[nodiscard]] static var_info_t construct(std::string key, std::any value) noexcept {
                int byte_size = 0;
                std::string serializable = construct_serializable(key, value);
                byte_size += serializable.length();

                return { byte_size , serializable };
            };
        };

        struct VarTypeObject {
        public:
            [[nodiscard]] static std::string construct_serializable(std::string key, std::any value) noexcept {
                CircCFGInterp::Environment* env = std::any_cast<CircCFGInterp::Environment*>(value);
                std::string ret = 
                    construction_lookup[CT::DOLLA] 
                    + key 
                    + construction_lookup[CT::COL] 
                    + construction_lookup[CT::LCURL]
                    +construction_lookup[CT::NEW_LINE];
                
                for (auto it = env->members.rbegin(); it != env->members.rend(); ++it) {
                    std::string k = it->first;
                    std::any v = it->second;
                  
                    if (v.type() == typeid(double)) {
                       ret += IConstructionPolicy<VarTypeDouble>::construct(k, v).second;
                    }
                    if (v.type() == typeid(std::string)) {
                        ret += IConstructionPolicy<VarTypeString>::construct(k, v).second;
                    }

                    if (v.type() == typeid(CircCFGInterp::Environment*)) {
                        ret += construct_serializable(k, v);
                    }
                };
                ret += construction_lookup[CT::RCURL];
                ret += construction_lookup[CT::COMMA];
                ret += construction_lookup[CT::NEW_LINE];
               
                return ret;
            };
            [[nodiscard]] static var_info_t construct(std::string key, std::any value) noexcept {
                int byte_size = 0;
                std::string serializable = construct_serializable(key, value);
                byte_size += serializable.length();
                
                return { byte_size , serializable };
            };
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
           [[nodiscard]] static var_info_t construct(std::string key, std::any value)  noexcept {
               double v = std::any_cast<double>(value);
               std::string str_lit = std::to_string(v);
               int byte_size = 0;
               std::string serializable = construct_serializable(key, str_lit);
               byte_size += serializable.length();
               return { byte_size , serializable };

           };
          
           
        };

        struct VarTypeString 
        {
        public:
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
        };

        var_info_t construct_variable(std::string key, std::any value) {
            if (value.type() == typeid(double)) {
                return IConstructionPolicy<VarTypeDouble>::construct(key, value);
            }
            else if (value.type() == typeid(std::string)) {
                return IConstructionPolicy<VarTypeString>::construct(key, value);
            }
            else if(value.type() == typeid(CircCFGInterp::Environment*)) {
                //object
                
                return IConstructionPolicy<VarTypeObject>::construct(key, value);
            }
            else if (value.type() == typeid(std::vector<std::any>)) {
                //arrays
                return IConstructionPolicy<VarTypeArray>::construct(key, value);
                
            }
        }

     
 

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
        
        void Serialize() {
            CircCFGInterp::Environment* env = this->interp->glob;
            std::ofstream ofs(cfg_path, std::ios::trunc | std::ios::out);
            std::cout << env->members.size() << " : " << interp->glob->members.size();
            while (env) {
                for (auto it = env->members.rbegin(); it != env->members.rend(); it++) {
                    var_info_t var_info = construct_variable(it->first, it->second);
                    ofs.write(var_info.second.c_str(), var_info.first);
                }
                env = env->outer;

            }
                
          

            ofs.close();
        };

        template<typename AttrType>
        void CFGAttrSet(std::initializer_list<std::string> kp, const std::any& v) {
            try {
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
          
        };
    };
}

