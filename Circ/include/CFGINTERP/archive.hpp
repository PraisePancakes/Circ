#pragma once
#include <iostream>
#include <vector>
#include "interp.hpp"
#include "env.hpp"
#include "circfmt.hpp"
#include "common.hpp"
#include "type_schema.hpp"

namespace Serialization {
    
   



    class Archive {
        Interpreter* interp;
        std::string cfg_path;
    public:
        Archive(Interpreter* interp, const std::string& cfg) : interp(interp), cfg_path(cfg) {};
        void Set(std::initializer_list<std::string> kp, const std::any& v) {
            Environment* current = interp->env;
            std::any value;
            const std::string last_key = kp.begin()[kp.size() - 1];
            for (size_t i = 0; i < kp.size() - 1; i++) {
                std::string curr_key = kp.begin()[i];
                if (!current) {
                    throw std::runtime_error("Invalid key path: " + curr_key);
                }
                value = current->resolve(curr_key);
                if (value.type() == typeid(Environment*)) {
                    current = std::any_cast<Environment*>(value);
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
                Environment* current = interp->env;
                std::any value;

                for (const auto& key : kp) {
                    if (!current) {

                        throw std::runtime_error("Invalid key path: " + key);
                    }
                    value = current->resolve(key);

                    if (value.type() == typeid(Environment*)) {

                        current = std::any_cast<Environment*>(value);
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


           Environment* glob = this->interp->glob;
           std::ofstream ofs(cfg_path, std::ios::trunc | std::ios::out);
           ofs.exceptions(std::ofstream::badbit | std::ofstream::failbit);

           std::string initial_layout_state = "";
           for (auto it = glob->members.rbegin(); it != glob->members.rend(); it++) {
               var_info_t var_info = construct_variable(it->first, it->second);
               initial_layout_state += var_info.second;
           }
           try {
               CircFormat::FormatTypeFunctor formatter;
               Lexer l;

               std::string out = formatter(l.lex_contents(initial_layout_state));
               ofs.write(out.c_str(), out.size());
               ofs.close();
           }
           catch (std::ofstream::failure& e) {
               std::cout << e.what() << std::endl;
           }
           catch (std::exception& e) {
               std::cout << e.what() << std::endl;
           }
            
        };


    };
};