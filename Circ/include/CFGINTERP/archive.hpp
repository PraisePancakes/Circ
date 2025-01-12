#pragma once
#include <iostream>
#include <vector>
#include "interp.hpp"
#include "env.hpp"
#include "circfmt.hpp"
#include "circtypes.hpp"

namespace Serialization {
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
                );
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

    struct VarTypeInt {
        [[nodiscard]] static std::string construct_serializable(std::string k, std::string str_lit) noexcept {
            return (construction_lookup[CT::DOLLA]
                + k
                + construction_lookup[CT::COL]
                + str_lit
                + construction_lookup[CT::COMMA]
                );
        };
        [[nodiscard]] static var_info_t construct(std::string key, std::any value)  noexcept {
            int v = std::any_cast<int>(value);
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
                );
        };

        [[nodiscard]] static var_info_t construct(std::string key, std::any value)  noexcept {
            std::string str_lit = std::any_cast<std::string>(value);
            int byte_size = 0;
            std::string serializable = construct_serializable(key, str_lit);
            byte_size += serializable.length();
            return { byte_size  , serializable };

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
                + construction_lookup[CT::LCURL];
                

            for (auto it = env->members.begin(); it != env->members.end(); ++it) {
                std::string k = it->first;
                std::any v = it->second;

                if (v.type() == typeid(double)) {
                    ret += IConstructionPolicy<VarTypeDouble>::construct(k, v).second;
                }
                if (v.type() == typeid(std::string)) {
                    ret += IConstructionPolicy<VarTypeString>::construct(k, v).second;
                }
                if (v.type() == typeid(int)) {
                    ret += IConstructionPolicy<VarTypeInt>::construct(k, v).second;
                }

                if (v.type() == typeid(std::vector<std::any>)) {
                    ret += IConstructionPolicy<VarTypeArray>::construct(k, v).second;
                }

                if (v.type() == typeid(CircCFGInterp::Environment*)) {
                    ret += construct_serializable(k, v);
                }
            };
            ret += construction_lookup[CT::RCURL];
            ret += construction_lookup[CT::COMMA];
            

            return ret;
        };
        [[nodiscard]] static var_info_t construct(std::string key, std::any value) noexcept {
            int byte_size = 0;
            std::string serializable = construct_serializable(key, value);
            byte_size += serializable.length();

            return { byte_size , serializable };
        };
    };

    [[nodiscard]] static var_info_t construct_variable(std::string key, std::any value) {
        if (value.type() == typeid(double)) {

            return IConstructionPolicy<VarTypeDouble>::construct(key, value);
        }
        else if (value.type() == typeid(std::string)) {
            return IConstructionPolicy<VarTypeString>::construct(key, value);
        }
        else if (value.type() == typeid(CircCFGInterp::Environment*)) {
            //object

            return IConstructionPolicy<VarTypeObject>::construct(key, value);
        }
        else if (value.type() == typeid(std::vector<std::any>)) {
            //arrays
            return IConstructionPolicy<VarTypeArray>::construct(key, value);

        }
        else if (value.type() == typeid(int)) {

            return IConstructionPolicy<VarTypeInt>::construct(key, value);
        }
    }



	class Archive {
        CircCFGInterp::Interpreter* interp;
        std::string cfg_path;
    public:
        Archive(CircCFGInterp::Interpreter* interp, const std::string& cfg) : interp(interp), cfg_path(cfg) {};
        void Set(std::initializer_list<std::string> kp, const std::any& v) {
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
            
        };

        template<typename Ty>
        Ty Get(std::string k) {
            return std::any_cast<Ty>(interp->env->resolve(k));
        };

        template<typename Ty>
        Ty Get(std::initializer_list<std::string> kp) {
            try {
                CircCFGInterp::Environment* current = interp->env;
                std::any value;

                for (const auto& key : kp) {
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
                    auto v = std::any_cast<Ty>(value);
                    return v;
                }

                throw std::runtime_error("Key path did not resolve to a valid value.");
            }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        }

        void Serialize() {
            
            CircCFGInterp::Environment* env = this->interp->glob;
            std::ofstream ofs(cfg_path, std::ios::trunc | std::ios::out);
            std::string initial_layout_state = "";
            while (env) {
                for (auto it = env->members.rbegin(); it != env->members.rend(); it++) {
                    var_info_t var_info = construct_variable(it->first, it->second);
                    initial_layout_state += var_info.second;
                }
                env = env->outer;

            }
            CircFormat::FormatTypeFunctor formatter;
            CircCFGInterp::Lexer l;
            
            std::string out = formatter(l.lex_contents(initial_layout_state));
            ofs.write(out.c_str(), out.size());
            ofs.close();
        };


	};
};