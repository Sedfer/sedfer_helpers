#pragma once

namespace sedfer::test {

template<typename T>
void force_implicit_conversion(T);

template<typename To, typename From>
static constexpr bool is_convertable = requires(To to, From from) {
    To(from);
    to = from;
    force_implicit_conversion<To>(from);
};

}
