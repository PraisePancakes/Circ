#pragma once
#include <iostream>
#include "common.hpp"
#include <vector>
#include <any>

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
            Environment* env = std::any_cast<Environment*>(value);
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

                if (v.type() == typeid(Environment*)) {
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
        else if (value.type() == typeid(Environment*)) {
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
}