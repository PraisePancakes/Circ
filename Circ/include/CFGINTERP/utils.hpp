#pragma once
#include <type_traits>

namespace Serialization {
	namespace Utils {
        template <typename T, class UNDERLYING = typename std::underlying_type<T>::type>
        struct BitEnum
        {
            static_assert(std::is_enum_v<T>, "T must be an enum");
            using enum_type = T;
           
            BitEnum() : flags() {};
            BitEnum(T single) : flags(single) {};
            BitEnum(const BitEnum& o) : flags(o.flags) {};
            
            BitEnum operator|(T add) { BitEnum r(*this); r |= add; return r; };
            BitEnum& operator|=(T add) {flags |= add; return *this; }
            BitEnum operator&(T mask) { BitEnum r(*this);r &= mask; return r;}
            BitEnum& operator&=(T mask) {flags &= mask;return *this;}
            explicit operator bool() { return flags != 0; };
        protected:
            UNDERLYING flags;
        };
	}
}