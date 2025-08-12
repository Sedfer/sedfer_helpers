#pragma once

#include "types.h"

#include <concepts>

namespace sedfer {

/// \brief Concept checks if a type can be used in packed<T>.
template<typename T>
concept packable = std::is_trivially_copyable_v<T> &&
                   std::is_trivially_destructible_v<T> &&
                   (alignof(T) > 1);

/// \brief Helper struct to work with Buffer::interpret, because it requires packed types.
template<packable T>
struct packed {
    T value;

    [[gnu::always_inline]] inline packed() = default;
    [[gnu::always_inline]] inline packed(const packed &) = default;
    [[gnu::always_inline]] inline packed(packed &&) = default;
    [[gnu::always_inline]] inline packed & operator=(const packed &) = default;
    [[gnu::always_inline]] inline packed & operator=(packed &&) = default;

    // NOLINTNEXTLINE(google-explicit-constructor)
    [[gnu::always_inline]] inline constexpr packed(const T & _value)
        : value(_value)
    { }

    [[gnu::always_inline]] inline constexpr packed & operator=(const T & _value) {
        value = _value;

        return *this;
    }

    // NOLINTNEXTLINE(google-explicit-constructor)
    [[gnu::always_inline]] inline constexpr operator T() const {
        return value;
    }
} __attribute__((packed));

using u16packed = packed<u16>;
using u32packed = packed<u32>;
using u64packed = packed<u64>;
using u128packed = packed<u128>;

using i16packed = packed<i16>;
using i32packed = packed<i32>;
using i64packed = packed<i64>;
using i128packed = packed<i128>;

using f16packed = packed<f16>;
using f32packed = packed<f32>;
using f64packed = packed<f64>;
using f128packed = packed<f128>;

}
