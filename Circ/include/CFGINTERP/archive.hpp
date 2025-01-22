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
        typedef std::initializer_list<std::string> Path;
    public:
        Archive(Interpreter* interp, const std::string& cfg) : interp(interp), cfg_path(cfg) {};

        /**
        * @method
        *   Create
        * @what
        *   We have some set of keys {"k1", "k2"} 
        * @pseudo
        *  E current env
        *   if key exists get key
                if key is E 
                    cast to E and set current to inner E
                    continue
                else
                    assign k, v to current env
            else
                if not on last key
                    create a new environment
                    insert new environment to current env
                    set current env equal to new environment
                else
                    insert kvp into current environment
        *   
        * 
        */
        void Create(Path kp, const std::any& v) {
            Environment* current = interp->env;
            //last_key is where we are creating
            const std::string last_key = kp.begin()[kp.size() - 1]; 
            for (size_t i = 0; i < kp.size(); i++) {
                std::string curr_key = kp.begin()[i];
                if (current->find(curr_key)) {
                    std::any val = current->resolve(curr_key);
                    if (val.type() == typeid(Environment*)) {
                        Environment* env = std::any_cast<Environment*>(val);
                        current = env;
                        continue;
                    }
                   
                    current->assign(curr_key, v);

                }
                else {
                  
                    if (i < kp.size() - 1) {
                        Environment* new_env = new Environment();
                        current->insert(curr_key, new_env);
                        current = new_env;
                    }
                    else {
                        current->insert(curr_key, v);
                    }
                }
               
            }
        };


        /*
            @exception :
                throw if key path is invalid, return from function after caught, that way assignment is discarded.
        */
        void Set(Path kp, const std::any& v) {
            Environment* current = interp->env;
            const std::string last_key = kp.begin()[kp.size() - 1];

            for (size_t i = 0; i < kp.size() - 1; i++) {
                std::string curr_key = kp.begin()[i];
                if (!current) {
                    throw std::runtime_error("Invalid key path: " + curr_key);
                    return;
                }
                std::any value = current->resolve(curr_key);
                if (value.type() == typeid(Environment*)) {
                    /*
                      this should never throw
                    */
                    current = std::any_cast<Environment*>(value);
                }
            }
            current->assign(last_key, v);
        };

        /*
            @exception :
                if thrown from here, program should exit, otherwise client will use default constructed type
        */
        template<typename Ty>
        Ty Get(Path kp) {
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

        void Serialize() {
           Environment* glob = this->interp->glob;
           std::ofstream ofs(cfg_path, std::ios::trunc | std::ios::out);
           ofs.exceptions(std::ofstream::badbit | std::ofstream::failbit);
           Lexer l;
           std::string initial_layout_state = "";
           for (auto it = glob->members.rbegin(); it != glob->members.rend(); it++) {
               var_info_t var_info = construct_variable(it->first, it->second);
               initial_layout_state += var_info.second;
           }
           CircFormat::FormatTypeFunctor formatter;
           TokenVector content = l.lex_contents(initial_layout_state);
           std::string out = formatter(content);
           ofs.write(out.c_str(), out.size());
           ofs.close();
        };


    };
};