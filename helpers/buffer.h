#pragma once

#include "helpers/types.h"
#include <algorithm>

namespace sedfer {

struct ConstBuffer;
struct MutableBuffer;

template<typename>
struct is_buffer : std::false_type {};

template<>
struct is_buffer<ConstBuffer> : std::true_type {};

template<>
struct is_buffer<MutableBuffer> : std::true_type {};

template<typename T>
static constexpr bool is_buffer_v = is_buffer<T>::value;

/**
 * \brief Concept checks if a type should be implicitly cast to ConstBuffer or MutableBuffer.
 *
 * 1. not is_buffer:
 *     Disable casting ConstBuffer to MutableBuffer.
 * 2. is_trivially_copyable (default copy, move, assign and destructor):
 *     Non-default copy/move/destructor usually means non-POD type, possibly with pointers.
 * 3. is_standard_layout (no virtual functions, no inheritance, all-public or all-private members):
 *     None of these should be interpreted as byte arrays.
 *
 * \note This is not 100% bulletproof, but should prevent many potential bugs.
 * \note Some false-positives are possible. Use SEDFER_CONST_BUFFER() and SEDFER_MUTABLE_BUFFER() helpers (or fix layout).
 */
template<typename T>
concept implicit_cast_to_buffer = (not is_buffer_v<T>) &&
                                  std::is_trivially_copyable_v<T> &&
                                  std::is_standard_layout_v<T>;

/// \brief Concept checks if a type can be interpreted from unaligned bytes without causing UB.
template<typename T>
concept interpretable_from_unaligned = (alignof(T) == 1);

/**
 * \brief ConstBuffer is a light-weight, non-owning view of immutable bytes, similar to span\<u8, dynamic_extent\>.
 *
 * 1. Pack together pointer + size.
 *    Fewer arguments (never worry about order).
 *    Easier to return from a function.
 * \code
 * // ugly
 * void set_bytes(const u8 * data, usize size);
 * void get_bytes(const u8 ** data_out, usize * size_out);
 *
 * // nice
 * void set_bytes(ConstBuffer buffer);
 * ConstBuffer get_bytes();
 * \endcode
 *
 * 2. Implicitly construct from any trivial type.
 * \code
 * struct A {
 *     u32 a;
 *     u8 b;
 *     u16 c;
 * } __attrubute__((packed));
 *
 * void foo(ConstBuffer);
 *
 * int main() {
 *     u64 one = 1;
 *     f32 two = 2.0;
 *     A a_struct = {3, 4, 5};
 *     std::array<int, 4> array = {6, 7, 8, 9};
 *
 *     foo(one);
 *     foo(two);
 *     foo(a_struct);
 *     foo(array);
 *     foo((i16)-1);
 * }
 * \endcode
 * \warn Implicit cast always does { reinterpret_cast<const u8 *>(x), sizeof(x) }, this may not always be what you want.
 * \code
 * void print(ConstBuffer);
 *
 * void cast_string_to_buffer() {
 *     std::string s = "abc";
 *     // Most definitely a bug, buffer would point to (ptr, size, capacity), NOT to "abc"!
 *     print(s); // (does not compile)
 *     print({(const u8 *)s.data(), s.size()}); // OK, 3 bytes {'a', 'b', 'c'}
 *     print("abc"); // OK, 4 bytes {'a', 'b', 'c', '\0'}
 * }
 *
 * void use c_style_array(u32 array_argument[10]) {
 *     print(array_argument); // ouch, 8 bytes (!)
 *
 *     u32 local_array[10] = {};
 *     print(local_array); // OK, 40 bytes
 * }
 * \endcode
 *
 * 3. Read from buffer using built-in stream interface.
 * \code
 * struct Header {
 *     u8 version;
 *     u32 count;
 * } __attribute__((packed));
 *
 * enum class Type : u16 {
 *     // ...
 * };
 *
 * bool try_parse(ConstBuffer packet) {
 *     auto header = packet.interpret<Header>();
 *     if(header == nullptr) return false;
 *
 *     if(header->version != 1) return false;
 *
 *     for(usize i = 0; i < header->count; ++i) {
 *         Type type;
 *         if(not packet.pop(type)) return false; // type is implicitly cast to MutableBuffer
 *
 *         u16 size;
 *         if(not packet.pop(size)) return false; // size is implicitly cast to MutableBuffer
 *
 *         ConstBuffer data = packet.pop_buffer(size);
 *         if(data.data == nullptr) return false;
 *
 *         // use (type, size, data)
 *     }
 *
 *     if(packet.size != 0) return false;
 *
 *     return true;
 * }
 * \endcode
 * \note ALL access methods return bool or nullptr and are marked [[nodiscard]] to enforce range-checks everywhere.
 * \note Read methods (except peek) consume bytes from the buffer.
 *       This is designed to parse binary protocols without an explicit offset variable.
 */
struct ConstBuffer {
    const u8 * data = nullptr;
    usize size = 0;

    [[gnu::always_inline]] inline ConstBuffer() = default;
    [[gnu::always_inline]] inline ConstBuffer(const ConstBuffer &) = default;
    [[gnu::always_inline]] inline ConstBuffer(ConstBuffer &&) = default;
    [[gnu::always_inline]] inline ConstBuffer & operator=(const ConstBuffer &) = default;
    [[gnu::always_inline]] inline ConstBuffer & operator=(ConstBuffer &&) = default;

    [[gnu::always_inline]] inline ConstBuffer(const u8 * _data, usize _size)
        : data(_data),
          size(_size)
    { }

    template<implicit_cast_to_buffer T>
    // NOLINTNEXTLINE(google-explicit-constructor)
    [[gnu::always_inline]] inline ConstBuffer(const T & t)
        : data(reinterpret_cast<const u8 *>(&t)),
          size(sizeof(T))
    { }

    /**
     * \brief Copy first mutable_buffer.size bytes into mutable_buffer.data. Current buffer is unaffected.
     * \return true if OK, false if mutable_buffer.size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool peek(MutableBuffer mutable_buffer) const;

    /**
     * \brief Copy last mutable_buffer.size bytes into mutable_buffer.data. Current buffer is unaffected.
     * \return true if OK, false if mutable_buffer.size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool peek_back(MutableBuffer mutable_buffer) const;

    /**
     * \brief Get first _size bytes as sub-buffer. Current buffer is unaffected.
     * \return Valid buffer if OK, {nullptr, 0} if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline ConstBuffer peek_buffer(usize _size) const {
        if(size < _size) {
            return {};
        }
        return {data, _size};
    }

    /**
     * \brief Get last _size bytes as sub-buffer. Current buffer is unaffected.
     * \return Valid buffer if OK, {nullptr, 0} if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline ConstBuffer peek_buffer_back(usize _size) const {
        if(size < _size) {
            return {};
        }
        return {data + size - _size, _size};
    }

    /**
     * \brief Interpret first sizeof(T) bytes as (const T*). Current buffer is unaffected.
     * \return Valid pointer if OK, nullptr if sizeof(T) > this.size.
     * \note alignof(T) must be 1. Use __attribute__((packed)) for structs, sedfer::packed\<T> for trivial types.
     */
    template<interpretable_from_unaligned T>
    [[nodiscard, gnu::always_inline]] inline const T * peek_interpret() const {
        if(size < sizeof(T)) {
            return nullptr;
        }
        return reinterpret_cast<const T *>(data);
    }

    /**
     * \brief Interpret last sizeof(T) bytes as (const T*). Current buffer is unaffected.
     * \return Valid pointer if OK, nullptr if sizeof(T) > this.size.
     * \note alignof(T) must be 1. Use __attribute__((packed)) for structs, sedfer::packed\<T> for trivial types.
     */
    template<interpretable_from_unaligned T>
    [[nodiscard, gnu::always_inline]] inline const T * peek_interpret_back() const {
        if(size < sizeof(T)) {
            return nullptr;
        }
        return reinterpret_cast<const T *>(data + size - sizeof(T));
    }

    /**
     * \brief Skip first _size bytes (.data and .size are adjusted).
     * \return true if OK, false if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool skip(usize _size) {
        if(size < _size) {
            return false;
        }
        data += _size;
        size -= _size;
        return true;
    }

    /**
     * \brief Skip last _size bytes (.data and .size are adjusted).
     * \return true if OK, false if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool skip_back(usize _size) {
        if(size < _size) {
            return false;
        }
        size -= _size;
        return true;
    }

    /**
     * \brief Copy first mutable_buffer.size bytes into mutable_buffer.data. Read bytes are consumed (.data and .size are adjusted).
     * \return true if OK, false if mutable_buffer.size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool pop(MutableBuffer mutable_buffer);

    /**
     * \brief Copy last mutable_buffer.size bytes into mutable_buffer.data. Read bytes are consumed (.data and .size are adjusted).
     * \return true if OK, false if mutable_buffer.size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool pop_back(MutableBuffer mutable_buffer);

    /**
     * \brief Get first _size bytes as sub-buffer. Read bytes are consumed (.data and .size are adjusted).
     * \return Valid buffer if OK, {nullptr, 0} if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline ConstBuffer pop_buffer(usize _size) {
        const ConstBuffer ret = peek_buffer(_size);
        (void)skip(_size);
        return ret;
    }

    /**
     * \brief Get last _size bytes as sub-buffer. Read bytes are consumed (.data and .size are adjusted).
     * \return Valid buffer if OK, {nullptr, 0} if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline ConstBuffer pop_buffer_back(usize _size) {
        const ConstBuffer ret = peek_buffer_back(_size);
        (void)skip_back(_size);
        return ret;
    }

    /**
     * \brief Interpret first sizeof(T) bytes as (const T*). Read bytes are consumed (.data and .size are adjusted).
     * \return Valid pointer if OK, nullptr if sizeof(T) > this.size.
     * \note alignof(T) must be 1. Use __attribute__((packed)) for structs, sedfer::packed\<T> for trivial types.
     */
    template<interpretable_from_unaligned T>
    [[nodiscard, gnu::always_inline]] inline const T * interpret() {
        const T * const ret = peek_interpret<T>();
        (void)skip(sizeof(T));
        return ret;
    }

    /**
     * \brief Interpret last sizeof(T) bytes as (const T*). Read bytes are consumed (.data and .size are adjusted).
     * \return Valid pointer if OK, nullptr if sizeof(T) > this.size.
     * \note alignof(T) must be 1. Use __attribute__((packed)) for structs, sedfer::packed\<T> for trivial types.
     */
    template<interpretable_from_unaligned T>
    [[nodiscard, gnu::always_inline]] inline const T * interpret_back() {
        const T * const ret = peek_interpret_back<T>();
        (void)skip_back(sizeof(T));
        return ret;
    }
};

/**
 * \brief MutableBuffer is a light-weight, non-owning view of mutable bytes, similar to span\<u8, dynamic_extent\>.
 *
 * 1. Pack together pointer + size.
 *    Fewer arguments (never worry about order).
 *    Easier to return from a function.
 * \code
 * // ugly
 * void set_bytes(u8 * data, usize size);
 * void get_bytes(u8 ** data_out, usize * size_out);
 *
 * // nice
 * void set_bytes(MutableBuffer buffer);
 * MutableBuffer get_bytes();
 * \endcode
 *
 * 2. Implicitly construct from any trivial type.
 * \code
 * struct A {
 *     u32 a;
 *     u8 b;
 *     u16 c;
 * } __attrubute__((packed));
 *
 * void foo(MutableBuffer);
 *
 * int main() {
 *     u64 one = 1;
 *     f32 two = 2.0;
 *     A a_struct = {3, 4, 5};
 *     std::array<int, 4> array = {6, 7, 8, 9};
 *
 *     foo(one);
 *     foo(two);
 *     foo(a_struct);
 *     foo(array);
 *     foo((i16)-1);
 * }
 * \endcode
 * \warn Implicit cast always does { reinterpret_cast<u8 *>(x), sizeof(x) }, this may not always be what you want.
 * \code
 * void zero_fill(MutableBuffer);
 *
 * void zero_fill_vector() {
 *     std::vector<u32> v(3);
 *     // Most definitely a bug, buffer would point to (ptr, size, capacity), NOT to v.data()!
 *     zero_fill(v); // (does not compile)
 *     zero_fill({v.data(), v.size()}); // OK
 * }
 *
 * void use c_style_array(u32 array_argument[10]) {
 *     zero_fill(array_argument); // ouch, 8 bytes (!)
 *
 *     u32 local_array[10] = {};
 *     zero_fill(local_array); // OK, 40 bytes
 * }
 * \endcode
 *
 * 3. Read from buffer using built-in stream interface.
 * \code
 * struct Header {
 *     u8 version;
 *     u32 count;
 * } __attribute__((packed));
 *
 * enum class Type : u16 {
 *     // ...
 * };
 *
 * bool try_parse(ConstBuffer packet) {
 *     auto header = packet.interpret<Header>();
 *     if(header == nullptr) return false;
 *
 *     if(header->version != 1) return false;
 *
 *     for(usize i = 0; i < header->count; ++i) {
 *         Type type;
 *         if(not packet.pop(type)) return false; // type is implicitly cast to MutableBuffer
 *
 *         u16 size;
 *         if(not packet.pop(size)) return false; // size is implicitly cast to MutableBuffer
 *
 *         ConstBuffer data = packet.pop_buffer(size);
 *         if(data.data == nullptr) return false;
 *
 *         // use (type, size, data)
 *     }
 *
 *     if(packet.size != 0) return false;
 *
 *     return true;
 * }
 * \endcode
 * \note ALL access methods return bool or nullptr and are marked [[nodiscard]] to enforce range-checks everywhere.
 * \note Read methods (except peek) consume bytes from the buffer.
 *       This is designed to parse binary protocols without an explicit offset variable.
 */
struct MutableBuffer {
    u8 * data = nullptr;
    usize size = 0;

    [[gnu::always_inline]] inline MutableBuffer() = default;
    [[gnu::always_inline]] inline MutableBuffer(const MutableBuffer &) = default;
    [[gnu::always_inline]] inline MutableBuffer(MutableBuffer &) = default;
    [[gnu::always_inline]] inline MutableBuffer(MutableBuffer &&) = default;
    [[gnu::always_inline]] inline MutableBuffer & operator=(const MutableBuffer &) = default;
    [[gnu::always_inline]] inline MutableBuffer & operator=(MutableBuffer &) = default;
    [[gnu::always_inline]] inline MutableBuffer & operator=(MutableBuffer &&) = default;

    [[gnu::always_inline]] inline MutableBuffer(u8 * _data, usize _size)
        : data(_data),
          size(_size)
    { }

    template<implicit_cast_to_buffer T>
    // NOLINTNEXTLINE(google-explicit-constructor)
    [[gnu::always_inline]] inline MutableBuffer(T & t)
        : data(reinterpret_cast<u8 *>(&t)),
          size(sizeof(T))
    { }

    // NOLINTNEXTLINE(google-explicit-constructor)
    [[gnu::always_inline]] inline operator ConstBuffer() const {
        return {data, size};
    }

    /**
     * \brief Copy first mutable_buffer.size bytes into mutable_buffer.data. Current buffer is unaffected.
     * \return true if OK, false if mutable_buffer.size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool peek(MutableBuffer mutable_buffer) const {
        if(size < mutable_buffer.size) {
            return false;
        }
        std::copy_n(data, mutable_buffer.size, mutable_buffer.data);
        return true;
    }

    /**
     * \brief Copy last mutable_buffer.size bytes into mutable_buffer.data. Current buffer is unaffected.
     * \return true if OK, false if mutable_buffer.size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool peek_back(MutableBuffer mutable_buffer) const {
        if(size < mutable_buffer.size) {
            return false;
        }
        std::copy_n(data + size - mutable_buffer.size, mutable_buffer.size, mutable_buffer.data);
        return true;
    }

    /**
     * \brief Get first _size bytes as sub-buffer. Current buffer is unaffected.
     * \return Valid buffer if OK, {nullptr, 0} if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline MutableBuffer peek_buffer(usize _size) const {
        if(size < _size) {
            return {};
        }
        return {data, _size};
    }

    /**
     * \brief Get last _size bytes as sub-buffer. Current buffer is unaffected.
     * \return Valid buffer if OK, {nullptr, 0} if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline MutableBuffer peek_buffer_back(usize _size) const {
        if(size < _size) {
            return {};
        }
        return {data + size - _size, _size};
    }

    /**
     * \brief Interpret first sizeof(T) bytes as T*. Current buffer is unaffected.
     * \return Valid pointer if OK, nullptr if sizeof(T) > this.size.
     * \note alignof(T) must be 1. Use __attribute__((packed)) for structs, sedfer::packed\<T> for trivial types.
     */
    template<interpretable_from_unaligned T>
    [[nodiscard, gnu::always_inline]] inline T * peek_interpret() {
        if(size < sizeof(T)) {
            return nullptr;
        }
        return reinterpret_cast<T *>(data);
    }

    /**
     * \brief Interpret last sizeof(T) bytes as T*. Current buffer is unaffected.
     * \return Valid pointer if OK, nullptr if sizeof(T) > this.size.
     * \note alignof(T) must be 1. Use __attribute__((packed)) for structs, sedfer::packed\<T> for trivial types.
     */
    template<interpretable_from_unaligned T>
    [[nodiscard, gnu::always_inline]] inline T * peek_interpret_back() {
        if(size < sizeof(T)) {
            return nullptr;
        }
        return reinterpret_cast<T *>(data + size - sizeof(T));
    }

    /**
     * \brief Skip first _size bytes (.data and .size are adjusted).
     * \return true if OK, false if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool skip(usize _size) {
        if(size < _size) {
            return false;
        }
        data += _size;
        size -= _size;
        return true;
    }

    /**
     * \brief Skip last _size bytes (.data and .size are adjusted).
     * \return true if OK, false if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool skip_back(usize _size) {
        if(size < _size) {
            return false;
        }
        size -= _size;
        return true;
    }

    /**
     * \brief Copy first mutable_buffer.size bytes into mutable_buffer.data. Read bytes are consumed (.data and .size are adjusted).
     * \return true if OK, false if mutable_buffer.size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool pop(MutableBuffer mutable_buffer) {
        if(not peek(mutable_buffer)) {
            return false;
        }
        return skip(mutable_buffer.size);
    }

    /**
     * \brief Copy last mutable_buffer.size bytes into mutable_buffer.data. Read bytes are consumed (.data and .size are adjusted).
     * \return true if OK, false if mutable_buffer.size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool pop_back(MutableBuffer mutable_buffer) {
        if(not peek_back(mutable_buffer)) {
            return false;
        }
        return skip_back(mutable_buffer.size);
    }

    /**
     * \brief Get first _size bytes as sub-buffer. Read bytes are consumed (.data and .size are adjusted).
     * \return Valid buffer if OK, {nullptr, 0} if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline MutableBuffer pop_buffer(usize _size) {
        MutableBuffer ret = peek_buffer(_size);
        (void)skip(_size);
        return ret;
    }

    /**
     * \brief Get last _size bytes as sub-buffer. Read bytes are consumed (.data and .size are adjusted).
     * \return Valid buffer if OK, {nullptr, 0} if _size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline MutableBuffer pop_buffer_back(usize _size) {
        MutableBuffer ret = peek_buffer_back(_size);
        (void)skip_back(_size);
        return ret;
    }

    /**
     * \brief Interpret first sizeof(T) bytes as T*. Read bytes are consumed (.data and .size are adjusted).
     * \return Valid pointer if OK, nullptr if sizeof(T) > this.size.
     * \note alignof(T) must be 1. Use __attribute__((packed)) for structs, sedfer::packed\<T> for trivial types.
     */
    template<interpretable_from_unaligned T>
    [[nodiscard, gnu::always_inline]] inline T * interpret() {
        T * const ret = peek_interpret<T>();
        (void)skip(sizeof(T));
        return ret;
    }

    /**
     * \brief Interpret last sizeof(T) bytes as T*. Read bytes are consumed (.data and .size are adjusted).
     * \return Valid pointer if OK, nullptr if sizeof(T) > this.size.
     * \note alignof(T) must be 1. Use __attribute__((packed)) for structs, sedfer::packed\<T> for trivial types.
     */
    template<interpretable_from_unaligned T>
    [[nodiscard, gnu::always_inline]] inline T * interpret_back() {
        T * const ret = peek_interpret_back<T>();
        (void)skip_back(sizeof(T));
        return ret;
    }

    /**
     * \brief Copy mutable_buffer.size bytes from mutable_buffer.data into first (this) bytes. Read bytes are consumed (.data and .size are adjusted).
     * \return true if OK, false if mutable_buffer.size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool push(ConstBuffer const_buffer) {
        if(size < const_buffer.size) {
            return false;
        }
        std::copy_n(const_buffer.data, const_buffer.size, data);
        return skip(const_buffer.size);
    }

    /**
     * \brief Copy mutable_buffer.size bytes from mutable_buffer.data into last (this) bytes. Read bytes are consumed (.data and .size are adjusted).
     * \return true if OK, false if mutable_buffer.size > this.size.
     */
    [[nodiscard, gnu::always_inline]] inline bool push_back(ConstBuffer const_buffer) {
        if(size < const_buffer.size) {
            return false;
        }
        std::copy_n(const_buffer.data, const_buffer.size, data + size - const_buffer.size);
        return skip_back(const_buffer.size);
    }
};

[[nodiscard, gnu::always_inline]] inline bool ConstBuffer::peek(MutableBuffer mutable_buffer) const {
    if(size < mutable_buffer.size) {
        return false;
    }
    std::copy_n(data, mutable_buffer.size, mutable_buffer.data);
    return true;
}

[[nodiscard, gnu::always_inline]] inline bool ConstBuffer::peek_back(MutableBuffer mutable_buffer) const {
    if(size < mutable_buffer.size) {
        return false;
    }
    std::copy_n(data + size - mutable_buffer.size, mutable_buffer.size, mutable_buffer.data);
    return true;
}

[[nodiscard, gnu::always_inline]] inline bool ConstBuffer::pop(MutableBuffer mutable_buffer) {
    if(not peek(mutable_buffer)) {
        return false;
    }
    return skip(mutable_buffer.size);
}

[[nodiscard, gnu::always_inline]] inline bool ConstBuffer::pop_back(MutableBuffer mutable_buffer) {
    if(not peek_back(mutable_buffer)) {
        return false;
    }
    return skip_back(mutable_buffer.size);
}

/**
 * \brief Compare two buffers byte-wise.
 * \return true if left == right, false otherwise.
 */
[[nodiscard, gnu::always_inline]] inline bool equal(ConstBuffer left, ConstBuffer right) {
    return std::equal(left.data, left.data + left.size, right.data, right.data + right.size);
}

}

/**
 * \brief Helper to cast objects to ConstBuffer without having to type reinterpret_cast and sizeof.
 * \note Most type can be implicitly cast to ConstBuffer, this is only useful if implicit cast fails.
 */
#define SEDFER_CONST_BUFFER(x) sedfer::ConstBuffer{reinterpret_cast<const sedfer::u8 *>(&(x)), sedfer::usize(sizeof(x))}

/**
 * \brief Helper to cast objects to MutableBuffer without having to type reinterpret_cast and sizeof.
 * \note Most type can be implicitly cast to MutableBuffer, this is only useful if implicit cast fails.
 * \note One useful case it casting packed members, because implicit cast tries to take reference (not allowed).
 *
 * \example
 * \code
 * struct Packed {
 *     u8 a;
 *     u32 b;
 * } __attribute__((packed));
 *
 * void foo(MutableBuffer);
 *
 * int main() {
 *     Packed p;
 *
 *     foo(p.b); // error: cannot bind packed field 'p.b' to 'u32&'
 *     foo(SEDFER_MUTABLE_BUFFER(p.b)); // OK
 * }
 * \endcode
 */
#define SEDFER_MUTABLE_BUFFER(x) sedfer::MutableBuffer{reinterpret_cast<sedfer::u8 *>(&(x)), sedfer::usize(sizeof(x))}
