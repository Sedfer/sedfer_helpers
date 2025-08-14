#include "helpers/all.h"

#include "tests/test.h"
#include "tests/is_convertable.h"

#include "bits/stdc++.h"

namespace sedfer::test {

template<typename T>
static void expect_same(const T & t, ConstBuffer buffer) {
    EXPECT(buffer.size == sizeof(T), "size " << buffer.size);
    const u8 * ptr = reinterpret_cast<const u8 *>(&t);
    EXPECT(std::equal(buffer.data, buffer.data + buffer.size, ptr), "");
}

template<typename T>
static void fill_random(T * data, usize size) {
    for(usize i = 0; i < size; ++i) {
        data[i] = rand();
    }
}

static void constructor_from_parts() {
    ConstBuffer random((const u8 *)0x12345678, 0x9abcdef0);
    EXPECT(random.data == (const u8 *)0x12345678, "");
    EXPECT(random.size == 0x9abcdef0, "");

    u8 bytes[6] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    ConstBuffer from_bytes(bytes, 6);
    EXPECT(from_bytes.data == bytes, "");
    EXPECT(from_bytes.size == 6, "");
    EXPECT(from_bytes.data[0] == 0x01, "");
    EXPECT(from_bytes.data[1] == 0x02, "");
    EXPECT(from_bytes.data[2] == 0x03, "");
    EXPECT(from_bytes.data[3] == 0x04, "");
    EXPECT(from_bytes.data[4] == 0x05, "");
    EXPECT(from_bytes.data[5] == 0x06, "");

    bytes[0] = 0x12;
    bytes[1] = 0x34;
    bytes[2] = 0x56;
    bytes[3] = 0x78;
    bytes[4] = 0x9A;
    bytes[5] = 0xBC;

    EXPECT(from_bytes.data[0] == 0x12, "");
    EXPECT(from_bytes.data[1] == 0x34, "");
    EXPECT(from_bytes.data[2] == 0x56, "");
    EXPECT(from_bytes.data[3] == 0x78, "");
    EXPECT(from_bytes.data[4] == 0x9A, "");
    EXPECT(from_bytes.data[5] == 0xBC, "");
}

static void default_constructor() {
    ConstBuffer uninitialized;
    ConstBuffer default_initialized{};
    ConstBuffer assign_initialized = {};
    ConstBuffer placement_new {(const u8 *)1, 2};
    new (&placement_new) ConstBuffer;
    ConstBuffer * new_allocated = new ConstBuffer;

    EXPECT(uninitialized.data == nullptr, "");
    EXPECT(uninitialized.size == 0, "size " << uninitialized.size);

    EXPECT(default_initialized.data == nullptr, "");
    EXPECT(default_initialized.size == 0, "size " << default_initialized.size);

    EXPECT(assign_initialized.data == nullptr, "");
    EXPECT(assign_initialized.size == 0, "size " << assign_initialized.size);

    EXPECT(placement_new.data == nullptr, "");
    EXPECT(placement_new.size == 0, "size " << placement_new.size);

    EXPECT(new_allocated->data == nullptr, "");
    EXPECT(new_allocated->size == 0, "size " << new_allocated->size);
}

static void copy_constructor() {
    u8 bytes[6] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    ConstBuffer buffer(bytes, 6);

    ConstBuffer copy_constructed(buffer);
    ConstBuffer copy_assigned = buffer;

    EXPECT(copy_constructed.data == bytes, "");
    EXPECT(copy_constructed.size == 6, "");

    EXPECT(copy_assigned.data == bytes, "");
    EXPECT(copy_assigned.size == 6, "");

    bytes[0] = 0x12;
    bytes[1] = 0x34;
    bytes[2] = 0x56;
    bytes[3] = 0x78;
    bytes[4] = 0x9A;
    bytes[5] = 0xBC;

    EXPECT(copy_constructed.data[0] == 0x12, "");
    EXPECT(copy_constructed.data[1] == 0x34, "");
    EXPECT(copy_constructed.data[2] == 0x56, "");
    EXPECT(copy_constructed.data[3] == 0x78, "");
    EXPECT(copy_constructed.data[4] == 0x9A, "");
    EXPECT(copy_constructed.data[5] == 0xBC, "");

    EXPECT(copy_assigned.data[0] == 0x12, "");
    EXPECT(copy_assigned.data[1] == 0x34, "");
    EXPECT(copy_assigned.data[2] == 0x56, "");
    EXPECT(copy_assigned.data[3] == 0x78, "");
    EXPECT(copy_assigned.data[4] == 0x9A, "");
    EXPECT(copy_assigned.data[5] == 0xBC, "");
}

static void move_constructor() {
    u8 bytes[6] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    ConstBuffer buffer(bytes, 6);

    ConstBuffer move_constructed(std::move(buffer));
    ConstBuffer move_assigned = std::move(buffer);

    EXPECT(move_constructed.data == bytes, "");
    EXPECT(move_constructed.size == 6, "");

    EXPECT(move_assigned.data == bytes, "");
    EXPECT(move_assigned.size == 6, "");

    bytes[0] = 0x12;
    bytes[1] = 0x34;
    bytes[2] = 0x56;
    bytes[3] = 0x78;
    bytes[4] = 0x9A;
    bytes[5] = 0xBC;

    EXPECT(move_constructed.data[0] == 0x12, "");
    EXPECT(move_constructed.data[1] == 0x34, "");
    EXPECT(move_constructed.data[2] == 0x56, "");
    EXPECT(move_constructed.data[3] == 0x78, "");
    EXPECT(move_constructed.data[4] == 0x9A, "");
    EXPECT(move_constructed.data[5] == 0xBC, "");

    EXPECT(move_assigned.data[0] == 0x12, "");
    EXPECT(move_assigned.data[1] == 0x34, "");
    EXPECT(move_assigned.data[2] == 0x56, "");
    EXPECT(move_assigned.data[3] == 0x78, "");
    EXPECT(move_assigned.data[4] == 0x9A, "");
    EXPECT(move_assigned.data[5] == 0xBC, "");
}

static void operator_assign() {
    u8 bytes[6] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    ConstBuffer buffer(bytes, 6);

    ConstBuffer assigned((const u8 *)0x12345678, 0x9abcdef0);
    assigned = buffer;

    EXPECT(assigned.data == bytes, "");
    EXPECT(assigned.size == 6, "");

    bytes[0] = 0x12;
    bytes[1] = 0x34;
    bytes[2] = 0x56;
    bytes[3] = 0x78;
    bytes[4] = 0x9A;
    bytes[5] = 0xBC;

    EXPECT(assigned.data[0] == 0x12, "");
    EXPECT(assigned.data[1] == 0x34, "");
    EXPECT(assigned.data[2] == 0x56, "");
    EXPECT(assigned.data[3] == 0x78, "");
    EXPECT(assigned.data[4] == 0x9A, "");
    EXPECT(assigned.data[5] == 0xBC, "");
}

static void static_is_convertable() {
    // basic types
    static_assert(is_convertable<ConstBuffer, u8>);
    static_assert(is_convertable<ConstBuffer, u16>);
    static_assert(is_convertable<ConstBuffer, u32>);
    static_assert(is_convertable<ConstBuffer, u64>);
    static_assert(is_convertable<ConstBuffer, u128>);
    static_assert(is_convertable<ConstBuffer, usize>);
    static_assert(is_convertable<ConstBuffer, i8>);
    static_assert(is_convertable<ConstBuffer, i16>);
    static_assert(is_convertable<ConstBuffer, i32>);
    static_assert(is_convertable<ConstBuffer, i64>);
    static_assert(is_convertable<ConstBuffer, i128>);
    static_assert(is_convertable<ConstBuffer, isize>);
    static_assert(is_convertable<ConstBuffer, f16>);
    static_assert(is_convertable<ConstBuffer, f32>);
    static_assert(is_convertable<ConstBuffer, f64>);
    static_assert(is_convertable<ConstBuffer, f128>);

    // packed types
    static_assert(is_convertable<ConstBuffer, u16packed>);
    static_assert(is_convertable<ConstBuffer, u32packed>);
    static_assert(is_convertable<ConstBuffer, u64packed>);
    static_assert(is_convertable<ConstBuffer, u128packed>);
    static_assert(is_convertable<ConstBuffer, i16packed>);
    static_assert(is_convertable<ConstBuffer, i32packed>);
    static_assert(is_convertable<ConstBuffer, i64packed>);
    static_assert(is_convertable<ConstBuffer, i128packed>);

    // OK
    struct S1 {
        u32 a;
        f64 b;
    };
    static_assert(is_convertable<ConstBuffer, S1>);

    // user default constructor OK
    struct S2 {
        u32 a;
        f64 b;

        S2() : a{}, b{} {}
    };
    static_assert(is_convertable<ConstBuffer, S2>);

    // default initializers OK
    struct S3 {
        u32 a = 1;
        f64 b = 2;
    };
    static_assert(is_convertable<ConstBuffer, S3>);

    // user copy constructor NOT OK
    struct S4 {
        u32 a;
        f64 b;

        S4(const S4 &);
    };
    static_assert(not is_convertable<ConstBuffer, S4>);

    // mixed public/private members NOT OK
    struct S5 {
    public:
        u32 a;
    private:
        f64 b;
    };
    static_assert(not is_convertable<ConstBuffer, S5>);

    // all private members OK
    struct S6 {
    private:
        u32 a;
        f64 b;
    };
    static_assert(is_convertable<ConstBuffer, S6>);

    struct Empty {};

    // OK, only one type class has members
    struct S7 : private Empty {
    private:
        u8 c;
    };
    static_assert(is_convertable<ConstBuffer, S7>);

    // inheritance
    struct S8 : private S1 {
    private:
        u8 c;
    };
    static_assert(not is_convertable<ConstBuffer, S8>);

    // virtual function
    struct S9 {
        u32 a;
        f64 b;

        virtual void foo();
    };
    static_assert(not is_convertable<ConstBuffer, S9>);

    // array-like OK
    static_assert(is_convertable<ConstBuffer, u8[10]>);
    static_assert(is_convertable<ConstBuffer, std::array<u32, 15>>);
    static_assert(is_convertable<ConstBuffer, decltype("abc")>);

    // OK? TODO: maybe do not allow
    static_assert(is_convertable<ConstBuffer, std::optional<i32>>);
    static_assert(is_convertable<ConstBuffer, std::variant<i32, u32, u64>>);

    // most std containers NOT OK
    static_assert(not is_convertable<ConstBuffer, std::pair<u8, u8>>);
    static_assert(not is_convertable<ConstBuffer, std::tuple<u8, u16, u32>>);
    static_assert(not is_convertable<ConstBuffer, std::string>);
    static_assert(not is_convertable<ConstBuffer, std::vector<i32>>);
    static_assert(not is_convertable<ConstBuffer, std::set<i32>>);
    static_assert(not is_convertable<ConstBuffer, std::unordered_set<i32>>);
    static_assert(not is_convertable<ConstBuffer, std::multiset<i32>>);
    static_assert(not is_convertable<ConstBuffer, std::unordered_multiset<i32>>);
    static_assert(not is_convertable<ConstBuffer, std::map<i32, u32>>);
    static_assert(not is_convertable<ConstBuffer, std::unordered_map<i32, u32>>);
    static_assert(not is_convertable<ConstBuffer, std::multimap<i32, u32>>);
    static_assert(not is_convertable<ConstBuffer, std::unordered_multimap<i32, u32>>);
    static_assert(not is_convertable<ConstBuffer, std::list<i32>>);
    static_assert(not is_convertable<ConstBuffer, std::forward_list<i32>>);
    static_assert(not is_convertable<ConstBuffer, std::deque<i32>>);
    static_assert(not is_convertable<ConstBuffer, std::expected<i32, u32>>);
    static_assert(not is_convertable<ConstBuffer, std::any>);

    // from buffers
    static_assert(is_convertable<ConstBuffer, ConstBuffer>);
    static_assert(is_convertable<ConstBuffer, MutableBuffer>);
}

static void implicit_cast_from_basic_types() {
    u8 u8v = 8;
    u16 u16v = 16;
    u32 u32v = 32;
    u64 u64v = 64;
    u128 u128v = 128;
    usize usizev = 1000;

    i8 i8v = -8;
    i16 i16v = -16;
    i32 i32v = -32;
    i64 i64v = -64;
    i128 i128v = -128;
    isize isizev = -1000;

    f16 f16v = 16.0f16;
    f32 f32v = 32.0;
    f64 f64v = 64.0;
    f128 f128v = 128.0;

    expect_same(u8v, u8v);
    expect_same(u16v, u16v);
    expect_same(u32v, u32v);
    expect_same(u64v, u64v);
    expect_same(u128v, u128v);
    expect_same(usizev, usizev);

    expect_same(i8v, i8v);
    expect_same(i16v, i16v);
    expect_same(i32v, i32v);
    expect_same(i64v, i64v);
    expect_same(i128v, i128v);
    expect_same(isizev, isizev);

    expect_same(f16v, f16v);
    expect_same(f32v, f32v);
    expect_same(f64v, f64v);
    expect_same(f128v, f128v);
}

static void implicit_cast_from_packed_types() {
    u16packed u16v = 16;
    u32packed u32v = 32;
    u64packed u64v = 64;
    u128packed u128v = 128;
    i16packed i16v = -16;
    i32packed i32v = -32;
    i64packed i64v = -64;
    i128packed i128v = -128;

    expect_same(u16v, u16v);
    expect_same(u32v, u32v);
    expect_same(u64v, u64v);
    expect_same(u128v, u128v);

    expect_same(i16v, i16v);
    expect_same(i32v, i32v);
    expect_same(i64v, i64v);
    expect_same(i128v, i128v);
}

static void implicit_cast_from_enum() {
    enum class E {};
    enum class Eu8 : u8 {};
    enum class Eu16 : u16 {};
    enum class Eu32 : u32 {};
    enum class Eu64 : u64 {};
    enum class Eu128 : u128 {};
    enum class Eusize : usize {};
    enum class Ei8 : i8 {};
    enum class Ei16 : i16 {};
    enum class Ei32 : i32 {};
    enum class Ei64 : i64 {};
    enum class Ei128 : i128 {};
    enum class Eisize : isize {};

    E e = (E)1;

    Eu8 u8v = (Eu8)8;
    Eu16 u16v = (Eu16)16;
    Eu32 u32v = (Eu32)32;
    Eu64 u64v = (Eu64)64;
    Eu128 u128v = (Eu128)128;
    Eusize usizev = (Eusize)1000;

    Ei8 i8v = (Ei8)8;
    Ei16 i16v = (Ei16)16;
    Ei32 i32v = (Ei32)32;
    Ei64 i64v = (Ei64)64;
    Ei128 i128v = (Ei128)128;
    Eisize isizev = (Eisize)1000;

    expect_same(e, e);

    expect_same(u8v, u8v);
    expect_same(u16v, u16v);
    expect_same(u32v, u32v);
    expect_same(u64v, u64v);
    expect_same(u128v, u128v);
    expect_same(usizev, usizev);

    expect_same(i8v, i8v);
    expect_same(i16v, i16v);
    expect_same(i32v, i32v);
    expect_same(i64v, i64v);
    expect_same(i128v, i128v);
    expect_same(isizev, isizev);

    static_assert(is_convertable<ConstBuffer, E>);
    static_assert(is_convertable<ConstBuffer, Eu8>);
    static_assert(is_convertable<ConstBuffer, Eu16>);
    static_assert(is_convertable<ConstBuffer, Eu32>);
    static_assert(is_convertable<ConstBuffer, Eu64>);
    static_assert(is_convertable<ConstBuffer, Eu128>);
    static_assert(is_convertable<ConstBuffer, Eusize>);
    static_assert(is_convertable<ConstBuffer, Ei8>);
    static_assert(is_convertable<ConstBuffer, Ei16>);
    static_assert(is_convertable<ConstBuffer, Ei32>);
    static_assert(is_convertable<ConstBuffer, Ei64>);
    static_assert(is_convertable<ConstBuffer, Ei128>);
    static_assert(is_convertable<ConstBuffer, Eisize>);
}

static void implicit_cast_from_basic_arrays() {
    u8 u8v[2] = {1, 2};
    u16 u16v[3] = {4, 5, 6};
    u32 u32v[7] = {8, 9, 10, 11, 12, 13, 14};
    u64 u64v[15] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    u128 u128v[31]; fill_random(u128v, 31);
    usize usizev[997]; fill_random(usizev, 997);

    i8 i8v[10]; fill_random(i8v, 10);
    i16 i16v[21]; fill_random(i16v, 21);
    i32 i32v[33]; fill_random(i32v, 33);
    i64 i64v[47]; fill_random(i64v, 47);
    i128 i128v[55]; fill_random(i128v, 55);
    isize isizev[99]; fill_random(isizev, 99);

    f16 f16v[7]; fill_random(f16v, 7);
    f32 f32v[17]; fill_random(f32v, 17);
    f64 f64v[137]; fill_random(f64v, 137);
    f128 f128v[1337]; fill_random(f128v, 1337);

    expect_same(u8v, u8v);
    expect_same(u16v, u16v);
    expect_same(u32v, u32v);
    expect_same(u64v, u64v);
    expect_same(u128v, u128v);
    expect_same(usizev, usizev);

    expect_same(i8v, i8v);
    expect_same(i16v, i16v);
    expect_same(i32v, i32v);
    expect_same(i64v, i64v);
    expect_same(i128v, i128v);
    expect_same(isizev, isizev);

    expect_same(f16v, f16v);
    expect_same(f32v, f32v);
    expect_same(f64v, f64v);
    expect_same(f128v, f128v);
}

static void implicit_cast_from_packed_struct() {
    struct S1 {
        u32 a;
        u32 b;
    } __attribute__((packed));
    S1 s1 = {123, 456};
    expect_same(s1, s1);

    struct S2 {
        u8 a;
        u32 b;
    } __attribute__((packed));
    S2 s2 = {123, 456};
    expect_same(s2, s2);

    struct S3 {
        f64 a;
        i16 b;
    } __attribute__((packed));
    S3 s3 = {123, -456};
    expect_same(s3, s3);

    struct S4 {
        u8 a;
        u16 b;
        u32 c;
        u64 d;
        u128 e;
        usize f;
        i8 g;
        i16 h;
        i32 i;
        i64 j;
        i128 k;
        isize l;
        f16 m;
        f32 n;
        f64 o;
        f128 p;
    } __attribute__((packed));
    S4 s4 = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536.f16, -373839, 404142, -43445};
    expect_same(s4, s4);

    struct S5 {
        S1 a;
        S2 b;
        S3 c;
        S4 d;
    } __attribute__((packed));
    S5 s5 = {s1, s2, s3, s4};
    expect_same(s5, s5);
}

static void implicit_cast_from_unpacked_struct() {
    struct S1 {
        u32 a;
        u32 b;
    };
    S1 s1 = {123, 456};
    expect_same(s1, s1);

    struct S2 {
        u8 a;
        u32 b;
    };
    S2 s2 = {123, 456};
    expect_same(s2, s2);

    struct S3 {
        f64 a;
        i16 b;
    };
    S3 s3 = {123, -456};
    expect_same(s3, s3);

    struct S4 {
        u8 a;
        u16 b;
        u32 c;
        u64 d;
        u128 e;
        usize f;
        i8 g;
        i16 h;
        i32 i;
        i64 j;
        i128 k;
        isize l;
        f16 m;
        f32 n;
        f64 o;
        f128 p;
    };
    S4 s4 = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536.f16, -373839, 404142, -43445};
    expect_same(s4, s4);

    struct S5 {
        S1 a;
        S2 b;
        S3 c;
        S4 d;
    };
    S5 s5 = {s1, s2, s3, s4};
    expect_same(s5, s5);
}

static void implicit_cast_from_union() {
    union S1 {
        u32 a;
        i32 b;
    };
    S1 s1 = {.b = 123};
    expect_same(s1, s1);

    union S2 {
        u8 a;
        u32 b;
    };
    S2 s2 = {.b = 456};
    expect_same(s2, s2);

    union S3 {
        f64 a;
        i16 b;
    };
    S3 s3 = {.b = -456};
    expect_same(s3, s3);

    union S4 {
        u8 a;
        u16 b;
        u32 c;
        u64 d;
        u128 e;
        usize f;
        i8 g;
        i16 h;
        i32 i;
        i64 j;
        i128 k;
        isize l;
        f16 m;
        f32 n;
        f64 o;
        f128 p;
    };
    S4 s4 = {.e = 7777};
    expect_same(s4, s4);

    struct S5 {
        S1 a;
        S2 b;
        S3 c;
        S4 d;
    };
    S5 s5 = {s1, s2, s3, s4};
    expect_same(s5, s5);
}

static void implicit_cast_from_packed_struct_array() {
    struct S1 {
        u32 a;
        u32 b;
    } __attribute__((packed));
    S1 s1[3] = {123, 456, 789, 101112, 131415, 161718};
    expect_same(s1, s1);

    struct S2 {
        u8 a;
        u32 b;
    } __attribute__((packed));
    S2 s2[4] = {123, 456, 78, 101112, 131, 161718, 255, 77777};
    expect_same(s2, s2);

    struct S3 {
        f64 a;
        i16 b;
    } __attribute__((packed));
    S3 s3[5] = {123, -456, 789, 11112, 131415, 16118, 255, 7777, 7777, 255};
    expect_same(s3, s3);

    struct S4 {
        u8 a;
        u16 b;
        u32 c;
        u64 d;
        u128 e;
        usize f;
        i8 g;
        i16 h;
        i32 i;
        i64 j;
        i128 k;
        isize l;
        f16 m;
        f32 n;
        f64 o;
        f128 p;
    } __attribute__((packed));
    S4 s4[11] = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536.f16, -373839, 404142, -43445};
    expect_same(s4, s4);

    struct S5 {
        S1 a[3];
        S2 b[4];
        S3 c[5];
        S4 d[11];
    } __attribute__((packed));
    S5 s5[17];
    expect_same(s5, s5);
}

static void implicit_cast_from_unpacked_struct_array() {
    struct S1 {
        u32 a;
        u32 b;
    };
    S1 s1[3] = {123, 456, 789, 101112, 131415, 161718};
    expect_same(s1, s1);

    struct S2 {
        u8 a;
        u32 b;
    };
    S2 s2[4] = {123, 456, 78, 101112, 131, 161718, 255, 77777};
    expect_same(s2, s2);

    struct S3 {
        f64 a;
        i16 b;
    };
    S3 s3[5] = {123, -456, 789, 11112, 131415, 16118, 255, 7777, 7777, 255};
    expect_same(s3, s3);

    struct S4 {
        u8 a;
        u16 b;
        u32 c;
        u64 d;
        u128 e;
        usize f;
        i8 g;
        i16 h;
        i32 i;
        i64 j;
        i128 k;
        isize l;
        f16 m;
        f32 n;
        f64 o;
        f128 p;
    };
    S4 s4[11] = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536.f16, -373839, 404142, -43445};
    expect_same(s4, s4);

    struct S5 {
        S1 a[3];
        S2 b[4];
        S3 c[5];
        S4 d[11];
    };
    S5 s5[17];
    expect_same(s5, s5);
}

static void implicit_cast_from_mutable_buffer() {
    u8 bytes[] = {1, 2, 3, 4};
    MutableBuffer mutable_buffer(bytes);
    EXPECT(mutable_buffer.data == bytes, "");
    EXPECT(mutable_buffer.size == sizeof(bytes), "size " << mutable_buffer.size);

    ConstBuffer copy_constructor(mutable_buffer);
    EXPECT(copy_constructor.data == mutable_buffer.data, "");
    EXPECT(copy_constructor.size == mutable_buffer.size, "size " << copy_constructor.size);

    ConstBuffer move_constructor(std::move(mutable_buffer));
    EXPECT(move_constructor.data == mutable_buffer.data, "");
    EXPECT(move_constructor.size == mutable_buffer.size, "size " << move_constructor.size);

    ConstBuffer assign = mutable_buffer;
    EXPECT(assign.data == mutable_buffer.data, "");
    EXPECT(assign.size == mutable_buffer.size, "size " << assign.size);
}

static void macro_cast_from_basic_types() {
    u8 u8v = 8;
    u16 u16v = 16;
    u32 u32v = 32;
    u64 u64v = 64;
    u128 u128v = 128;
    usize usizev = 1000;

    i8 i8v = -8;
    i16 i16v = -16;
    i32 i32v = -32;
    i64 i64v = -64;
    i128 i128v = -128;
    isize isizev = -1000;

    f16 f16v = 16.0f16;
    f32 f32v = 32.0;
    f64 f64v = 64.0;
    f128 f128v = 128.0;

    expect_same(u8v, SEDFER_CONST_BUFFER(u8v));
    expect_same(u16v, SEDFER_CONST_BUFFER(u16v));
    expect_same(u32v, SEDFER_CONST_BUFFER(u32v));
    expect_same(u64v, SEDFER_CONST_BUFFER(u64v));
    expect_same(u128v, SEDFER_CONST_BUFFER(u128v));
    expect_same(usizev, SEDFER_CONST_BUFFER(usizev));

    expect_same(i8v, SEDFER_CONST_BUFFER(i8v));
    expect_same(i16v, SEDFER_CONST_BUFFER(i16v));
    expect_same(i32v, SEDFER_CONST_BUFFER(i32v));
    expect_same(i64v, SEDFER_CONST_BUFFER(i64v));
    expect_same(i128v, SEDFER_CONST_BUFFER(i128v));
    expect_same(isizev, SEDFER_CONST_BUFFER(isizev));

    expect_same(f16v, SEDFER_CONST_BUFFER(f16v));
    expect_same(f32v, SEDFER_CONST_BUFFER(f32v));
    expect_same(f64v, SEDFER_CONST_BUFFER(f64v));
    expect_same(f128v, SEDFER_CONST_BUFFER(f128v));
}

static void macro_cast_from_packed_types() {
    u16packed u16v = 16;
    u32packed u32v = 32;
    u64packed u64v = 64;
    u128packed u128v = 128;
    i16packed i16v = -16;
    i32packed i32v = -32;
    i64packed i64v = -64;
    i128packed i128v = -128;

    expect_same(u16v, SEDFER_CONST_BUFFER(u16v));
    expect_same(u32v, SEDFER_CONST_BUFFER(u32v));
    expect_same(u64v, SEDFER_CONST_BUFFER(u64v));
    expect_same(u128v, SEDFER_CONST_BUFFER(u128v));

    expect_same(i16v, SEDFER_CONST_BUFFER(i16v));
    expect_same(i32v, SEDFER_CONST_BUFFER(i32v));
    expect_same(i64v, SEDFER_CONST_BUFFER(i64v));
    expect_same(i128v, SEDFER_CONST_BUFFER(i128v));
}

static void macro_cast_from_enum() {
    enum class E {};
    enum class Eu8 : u8 {};
    enum class Eu16 : u16 {};
    enum class Eu32 : u32 {};
    enum class Eu64 : u64 {};
    enum class Eu128 : u128 {};
    enum class Eusize : usize {};
    enum class Ei8 : i8 {};
    enum class Ei16 : i16 {};
    enum class Ei32 : i32 {};
    enum class Ei64 : i64 {};
    enum class Ei128 : i128 {};
    enum class Eisize : isize {};

    E e = (E)1;

    Eu8 u8v = (Eu8)8;
    Eu16 u16v = (Eu16)16;
    Eu32 u32v = (Eu32)32;
    Eu64 u64v = (Eu64)64;
    Eu128 u128v = (Eu128)128;
    Eusize usizev = (Eusize)1000;

    Ei8 i8v = (Ei8)8;
    Ei16 i16v = (Ei16)16;
    Ei32 i32v = (Ei32)32;
    Ei64 i64v = (Ei64)64;
    Ei128 i128v = (Ei128)128;
    Eisize isizev = (Eisize)1000;

    expect_same(e, SEDFER_CONST_BUFFER(e));

    expect_same(u8v, SEDFER_CONST_BUFFER(u8v));
    expect_same(u16v, SEDFER_CONST_BUFFER(u16v));
    expect_same(u32v, SEDFER_CONST_BUFFER(u32v));
    expect_same(u64v, SEDFER_CONST_BUFFER(u64v));
    expect_same(u128v, SEDFER_CONST_BUFFER(u128v));
    expect_same(usizev, SEDFER_CONST_BUFFER(usizev));

    expect_same(i8v, SEDFER_CONST_BUFFER(i8v));
    expect_same(i16v, SEDFER_CONST_BUFFER(i16v));
    expect_same(i32v, SEDFER_CONST_BUFFER(i32v));
    expect_same(i64v, SEDFER_CONST_BUFFER(i64v));
    expect_same(i128v, SEDFER_CONST_BUFFER(i128v));
    expect_same(isizev, SEDFER_CONST_BUFFER(isizev));

    static_assert(is_convertable<ConstBuffer, E>);
    static_assert(is_convertable<ConstBuffer, Eu8>);
    static_assert(is_convertable<ConstBuffer, Eu16>);
    static_assert(is_convertable<ConstBuffer, Eu32>);
    static_assert(is_convertable<ConstBuffer, Eu64>);
    static_assert(is_convertable<ConstBuffer, Eu128>);
    static_assert(is_convertable<ConstBuffer, Eusize>);
    static_assert(is_convertable<ConstBuffer, Ei8>);
    static_assert(is_convertable<ConstBuffer, Ei16>);
    static_assert(is_convertable<ConstBuffer, Ei32>);
    static_assert(is_convertable<ConstBuffer, Ei64>);
    static_assert(is_convertable<ConstBuffer, Ei128>);
    static_assert(is_convertable<ConstBuffer, Eisize>);
}

static void macro_cast_from_basic_arrays() {
    u8 u8v[2] = {1, 2};
    u16 u16v[3] = {4, 5, 6};
    u32 u32v[7] = {8, 9, 10, 11, 12, 13, 14};
    u64 u64v[15] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    u128 u128v[31]; fill_random(u128v, 31);
    usize usizev[997]; fill_random(usizev, 997);

    i8 i8v[10]; fill_random(i8v, 10);
    i16 i16v[21]; fill_random(i16v, 21);
    i32 i32v[33]; fill_random(i32v, 33);
    i64 i64v[47]; fill_random(i64v, 47);
    i128 i128v[55]; fill_random(i128v, 55);
    isize isizev[99]; fill_random(isizev, 99);

    f16 f16v[7]; fill_random(f16v, 7);
    f32 f32v[17]; fill_random(f32v, 17);
    f64 f64v[137]; fill_random(f64v, 137);
    f128 f128v[1337]; fill_random(f128v, 1337);

    expect_same(u8v, SEDFER_CONST_BUFFER(u8v));
    expect_same(u16v, SEDFER_CONST_BUFFER(u16v));
    expect_same(u32v, SEDFER_CONST_BUFFER(u32v));
    expect_same(u64v, SEDFER_CONST_BUFFER(u64v));
    expect_same(u128v, SEDFER_CONST_BUFFER(u128v));
    expect_same(usizev, SEDFER_CONST_BUFFER(usizev));

    expect_same(i8v, SEDFER_CONST_BUFFER(i8v));
    expect_same(i16v, SEDFER_CONST_BUFFER(i16v));
    expect_same(i32v, SEDFER_CONST_BUFFER(i32v));
    expect_same(i64v, SEDFER_CONST_BUFFER(i64v));
    expect_same(i128v, SEDFER_CONST_BUFFER(i128v));
    expect_same(isizev, SEDFER_CONST_BUFFER(isizev));

    expect_same(f16v, SEDFER_CONST_BUFFER(f16v));
    expect_same(f32v, SEDFER_CONST_BUFFER(f32v));
    expect_same(f64v, SEDFER_CONST_BUFFER(f64v));
    expect_same(f128v, SEDFER_CONST_BUFFER(f128v));
}

static void macro_cast_from_packed_struct() {
    struct S1 {
        u32 a;
        u32 b;
    } __attribute__((packed));
    S1 s1 = {123, 456};
    expect_same(s1, SEDFER_CONST_BUFFER(s1));

    struct S2 {
        u8 a;
        u32 b;
    } __attribute__((packed));
    S2 s2 = {123, 456};
    expect_same(s2, SEDFER_CONST_BUFFER(s2));

    struct S3 {
        f64 a;
        i16 b;
    } __attribute__((packed));
    S3 s3 = {123, -456};
    expect_same(s3, SEDFER_CONST_BUFFER(s3));

    struct S4 {
        u8 a;
        u16 b;
        u32 c;
        u64 d;
        u128 e;
        usize f;
        i8 g;
        i16 h;
        i32 i;
        i64 j;
        i128 k;
        isize l;
        f16 m;
        f32 n;
        f64 o;
        f128 p;
    } __attribute__((packed));
    S4 s4 = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536.f16, -373839, 404142, -43445};
    expect_same(s4, SEDFER_CONST_BUFFER(s4));

    struct S5 {
        S1 a;
        S2 b;
        S3 c;
        S4 d;
    } __attribute__((packed));
    S5 s5 = {s1, s2, s3, s4};
    expect_same(s5, SEDFER_CONST_BUFFER(s5));
}

static void macro_cast_from_unpacked_struct() {
    struct S1 {
        u32 a;
        u32 b;
    };
    S1 s1 = {123, 456};
    expect_same(s1, SEDFER_CONST_BUFFER(s1));

    struct S2 {
        u8 a;
        u32 b;
    };
    S2 s2 = {123, 456};
    expect_same(s2, SEDFER_CONST_BUFFER(s2));

    struct S3 {
        f64 a;
        i16 b;
    };
    S3 s3 = {123, -456};
    expect_same(s3, SEDFER_CONST_BUFFER(s3));

    struct S4 {
        u8 a;
        u16 b;
        u32 c;
        u64 d;
        u128 e;
        usize f;
        i8 g;
        i16 h;
        i32 i;
        i64 j;
        i128 k;
        isize l;
        f16 m;
        f32 n;
        f64 o;
        f128 p;
    };
    S4 s4 = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536.f16, -373839, 404142, -43445};
    expect_same(s4, SEDFER_CONST_BUFFER(s4));

    struct S5 {
        S1 a;
        S2 b;
        S3 c;
        S4 d;
    };
    S5 s5 = {s1, s2, s3, s4};
    expect_same(s5, SEDFER_CONST_BUFFER(s5));
}

static void macro_cast_from_union() {
    union S1 {
        u32 a;
        i32 b;
    };
    S1 s1 = {.b = 123};
    expect_same(s1, SEDFER_CONST_BUFFER(s1));

    union S2 {
        u8 a;
        u32 b;
    };
    S2 s2 = {.b = 456};
    expect_same(s2, SEDFER_CONST_BUFFER(s2));

    union S3 {
        f64 a;
        i16 b;
    };
    S3 s3 = {.b = -456};
    expect_same(s3, SEDFER_CONST_BUFFER(s3));

    union S4 {
        u8 a;
        u16 b;
        u32 c;
        u64 d;
        u128 e;
        usize f;
        i8 g;
        i16 h;
        i32 i;
        i64 j;
        i128 k;
        isize l;
        f16 m;
        f32 n;
        f64 o;
        f128 p;
    };
    S4 s4 = {.e = 7777};
    expect_same(s4, SEDFER_CONST_BUFFER(s4));

    struct S5 {
        S1 a;
        S2 b;
        S3 c;
        S4 d;
    };
    S5 s5 = {s1, s2, s3, s4};
    expect_same(s5, SEDFER_CONST_BUFFER(s5));
}

static void macro_cast_from_packed_struct_array() {
    struct S1 {
        u32 a;
        u32 b;
    } __attribute__((packed));
    S1 s1[3] = {123, 456, 789, 101112, 131415, 161718};
    expect_same(s1, SEDFER_CONST_BUFFER(s1));

    struct S2 {
        u8 a;
        u32 b;
    } __attribute__((packed));
    S2 s2[4] = {123, 456, 78, 101112, 131, 161718, 255, 77777};
    expect_same(s2, SEDFER_CONST_BUFFER(s2));

    struct S3 {
        f64 a;
        i16 b;
    } __attribute__((packed));
    S3 s3[5] = {123, -456, 789, 11112, 131415, 16118, 255, 7777, 7777, 255};
    expect_same(s3, SEDFER_CONST_BUFFER(s3));

    struct S4 {
        u8 a;
        u16 b;
        u32 c;
        u64 d;
        u128 e;
        usize f;
        i8 g;
        i16 h;
        i32 i;
        i64 j;
        i128 k;
        isize l;
        f16 m;
        f32 n;
        f64 o;
        f128 p;
    } __attribute__((packed));
    S4 s4[11] = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536.f16, -373839, 404142, -43445};
    expect_same(s4, SEDFER_CONST_BUFFER(s4));

    struct S5 {
        S1 a[3];
        S2 b[4];
        S3 c[5];
        S4 d[11];
    } __attribute__((packed));
    S5 s5[17];
    expect_same(s5, SEDFER_CONST_BUFFER(s5));
}

static void macro_cast_from_unpacked_struct_array() {
    struct S1 {
        u32 a;
        u32 b;
    };
    S1 s1[3] = {123, 456, 789, 101112, 131415, 161718};
    expect_same(s1, SEDFER_CONST_BUFFER(s1));

    struct S2 {
        u8 a;
        u32 b;
    };
    S2 s2[4] = {123, 456, 78, 101112, 131, 161718, 255, 77777};
    expect_same(s2, SEDFER_CONST_BUFFER(s2));

    struct S3 {
        f64 a;
        i16 b;
    };
    S3 s3[5] = {123, -456, 789, 11112, 131415, 16118, 255, 7777, 7777, 255};
    expect_same(s3, SEDFER_CONST_BUFFER(s3));

    struct S4 {
        u8 a;
        u16 b;
        u32 c;
        u64 d;
        u128 e;
        usize f;
        i8 g;
        i16 h;
        i32 i;
        i64 j;
        i128 k;
        isize l;
        f16 m;
        f32 n;
        f64 o;
        f128 p;
    };
    S4 s4[11] = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536.f16, -373839, 404142, -43445};
    expect_same(s4, SEDFER_CONST_BUFFER(s4));

    struct S5 {
        S1 a[3];
        S2 b[4];
        S3 c[5];
        S4 d[11];
    };
    S5 s5[17];
    expect_same(s5, SEDFER_CONST_BUFFER(s5));
}

static void peek() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    ConstBuffer buffer = bytes;
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);

    u8 u8v = -1;
    u16 u16v = -1;
    u32 u32v = -1;
    u64 u64v = -1;
    u128 u128v = -1;

    EXPECT(buffer.peek(u8v), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
    EXPECT(u8v == 0x01, "");

    EXPECT(buffer.peek(u16v), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
    EXPECT(u16v == 0x0201, "");

    EXPECT(buffer.peek(u32v), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
    EXPECT(u32v == 0x04030201, "");

    EXPECT(buffer.peek(u64v), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
    EXPECT(u64v == 0x0807060504030201, "");

    EXPECT(not buffer.peek(u128v), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
    EXPECT(u128v == -1, "");
}

static void peek_back() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    ConstBuffer buffer = bytes;
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);

    u8 u8v = -1;
    u16 u16v = -1;
    u32 u32v = -1;
    u64 u64v = -1;
    u128 u128v = -1;

    EXPECT(buffer.peek_back(u8v), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
    EXPECT(u8v == 0x09, "");

    EXPECT(buffer.peek_back(u16v), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
    EXPECT(u16v == 0x0908, "");

    EXPECT(buffer.peek_back(u32v), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
    EXPECT(u32v == 0x09080706, "");

    EXPECT(buffer.peek_back(u64v), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
    EXPECT(u64v == 0x0908070605040302, "");

    EXPECT(not buffer.peek_back(u128v), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
    EXPECT(u128v == -1, "");
}

static void peek_buffer() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    ConstBuffer buffer = bytes;
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);

    for(usize i = 0; i <= 9; ++i) {
        ConstBuffer head = buffer.peek_buffer(i);
        EXPECT(head.data == bytes, "");
        EXPECT(head.size == i, "size " << head.size);
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }

    ConstBuffer head = buffer.peek_buffer(10);
    EXPECT(head.data == nullptr, "");
    EXPECT(head.size == 0, "size " << head.size);
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
}

static void peek_buffer_back() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    ConstBuffer buffer = bytes;
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);

    for(usize i = 0; i <= 9; ++i) {
        ConstBuffer tail = buffer.peek_buffer_back(i);
        EXPECT(tail.data == bytes + 9 - i, "");
        EXPECT(tail.size == i, "size " << tail.size);
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }

    ConstBuffer tail = buffer.peek_buffer_back(10);
    EXPECT(tail.data == nullptr, "");
    EXPECT(tail.size == 0, "size " << tail.size);
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
}

static void peek_interpret() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.peek_interpret<u8>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x01, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.peek_interpret<u16packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x0201, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.peek_interpret<u32packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x04030201, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.peek_interpret<u64packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x0807060504030201, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.peek_interpret<u128packed>();
        EXPECT(ptr == nullptr, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }
}

static void peek_interpret_back() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.peek_interpret_back<u8>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x09, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.peek_interpret_back<u16packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x0908, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.peek_interpret_back<u32packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x09080706, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.peek_interpret_back<u64packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x0908070605040302, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.peek_interpret_back<u128packed>();
        EXPECT(ptr == nullptr, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }
}

static void pop() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    
    u8 u8v = -1;
    u16 u16v = -1;
    u32 u32v = -1;
    u64 u64v = -1;
    u128 u128v = -1;

    {
        ConstBuffer buffer = bytes;
        EXPECT(buffer.pop(u8v), "");
        EXPECT(buffer.data == bytes + 1, "");
        EXPECT(buffer.size == 8, "size " << buffer.size);
        EXPECT(u8v == 0x01, "");
    }

    {
        ConstBuffer buffer = bytes;
        EXPECT(buffer.pop(u16v), "");
        EXPECT(buffer.data == bytes + 2, "");
        EXPECT(buffer.size == 7, "size " << buffer.size);
        EXPECT(u16v == 0x0201, "");
    }

    {
        ConstBuffer buffer = bytes;
        EXPECT(buffer.pop(u32v), "");
        EXPECT(buffer.data == bytes + 4, "");
        EXPECT(buffer.size == 5, "size " << buffer.size);
        EXPECT(u32v == 0x04030201, "");
    }

    {
        ConstBuffer buffer = bytes;
        EXPECT(buffer.pop(u64v), "");
        EXPECT(buffer.data == bytes + 8, "");
        EXPECT(buffer.size == 1, "size " << buffer.size);
        EXPECT(u64v == 0x0807060504030201, "");
    }

    {
        ConstBuffer buffer = bytes;
        EXPECT(not buffer.pop(u128v), "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
        EXPECT(u128v == -1, "");
    }
}

static void pop_back() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    u8 u8v = -1;
    u16 u16v = -1;
    u32 u32v = -1;
    u64 u64v = -1;
    u128 u128v = -1;

    {
        ConstBuffer buffer = bytes;
        EXPECT(buffer.pop_back(u8v), "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 8, "size " << buffer.size);
        EXPECT(u8v == 0x09, "");
    }

    {
        ConstBuffer buffer = bytes;
        EXPECT(buffer.pop_back(u16v), "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 7, "size " << buffer.size);
        EXPECT(u16v == 0x0908, "");
    }

    {
        ConstBuffer buffer = bytes;
        EXPECT(buffer.pop_back(u32v), "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 5, "size " << buffer.size);
        EXPECT(u32v == 0x09080706, "");
    }

    {
        ConstBuffer buffer = bytes;
        EXPECT(buffer.pop_back(u64v), "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 1, "size " << buffer.size);
        EXPECT(u64v == 0x0908070605040302, "");
    }

    {
        ConstBuffer buffer = bytes;
        EXPECT(not buffer.pop_back(u128v), "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
        EXPECT(u128v == -1, "");
    }
}

static void pop_buffer() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    for(usize i = 0; i <= 9; ++i) {
        ConstBuffer buffer = bytes;
        ConstBuffer head = buffer.pop_buffer(i);
        EXPECT(head.data == bytes, "");
        EXPECT(head.size == i, "size " << head.size);
        EXPECT(buffer.data == bytes + i, "");
        EXPECT(buffer.size == 9 - i, "size " << buffer.size);
    }

    ConstBuffer buffer = bytes;
    ConstBuffer head = buffer.pop_buffer(10);
    EXPECT(head.data == nullptr, "");
    EXPECT(head.size == 0, "size " << head.size);
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
}

static void pop_buffer_back() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    for(usize i = 0; i <= 9; ++i) {
        ConstBuffer buffer = bytes;
        ConstBuffer tail = buffer.pop_buffer_back(i);
        EXPECT(tail.data == bytes + 9 - i, "");
        EXPECT(tail.size == i, "size " << tail.size);
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9 - i, "size " << buffer.size);
    }

    ConstBuffer buffer = bytes;
    ConstBuffer tail = buffer.pop_buffer_back(10);
    EXPECT(tail.data == nullptr, "");
    EXPECT(tail.size == 0, "size " << tail.size);
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
}

static void interpret() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.interpret<u8>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x01, "");
        EXPECT(buffer.data == bytes + 1, "");
        EXPECT(buffer.size == 8, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.interpret<u16packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x0201, "");
        EXPECT(buffer.data == bytes + 2, "");
        EXPECT(buffer.size == 7, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.interpret<u32packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x04030201, "");
        EXPECT(buffer.data == bytes + 4, "");
        EXPECT(buffer.size == 5, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.interpret<u64packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x0807060504030201, "");
        EXPECT(buffer.data == bytes + 8, "");
        EXPECT(buffer.size == 1, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.interpret<u128packed>();
        EXPECT(ptr == nullptr, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }
}

void interpret_back() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.interpret_back<u8>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x09, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 8, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.interpret_back<u16packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x0908, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 7, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.interpret_back<u32packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x09080706, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 5, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.interpret_back<u64packed>();
        EXPECT(ptr != nullptr, "");
        if(ptr) EXPECT(*ptr == 0x0908070605040302, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 1, "size " << buffer.size);
    }

    {
        ConstBuffer buffer = bytes;
        auto * ptr = buffer.interpret_back<u128packed>();
        EXPECT(ptr == nullptr, "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9, "size " << buffer.size);
    }
}

static void skip() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    for(usize i = 0; i <= 9; ++i) {
        ConstBuffer buffer = bytes;
        EXPECT(buffer.skip(i), "");
        EXPECT(buffer.data == bytes + i, "");
        EXPECT(buffer.size == 9 - i, "size " << buffer.size);
    }

    ConstBuffer buffer = bytes;
    EXPECT(not buffer.skip(10), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
}

static void skip_back() {
    const u8 bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    for(usize i = 0; i <= 9; ++i) {
        ConstBuffer buffer = bytes;
        EXPECT(buffer.skip_back(i), "");
        EXPECT(buffer.data == bytes, "");
        EXPECT(buffer.size == 9 - i, "size " << buffer.size);
    }

    ConstBuffer buffer = bytes;
    EXPECT(not buffer.skip_back(10), "");
    EXPECT(buffer.data == bytes, "");
    EXPECT(buffer.size == 9, "size " << buffer.size);
}

void test_const_buffer() {
    constructor_from_parts();
    default_constructor();
    copy_constructor();
    move_constructor();
    operator_assign();

    static_is_convertable();

    implicit_cast_from_basic_types();
    implicit_cast_from_packed_types();
    implicit_cast_from_enum();
    implicit_cast_from_basic_arrays();
    implicit_cast_from_packed_struct();
    implicit_cast_from_unpacked_struct();
    implicit_cast_from_union();
    implicit_cast_from_packed_struct_array();
    implicit_cast_from_unpacked_struct_array();
    implicit_cast_from_mutable_buffer();

    macro_cast_from_basic_types();
    macro_cast_from_packed_types();
    macro_cast_from_enum();
    macro_cast_from_basic_arrays();
    macro_cast_from_packed_struct();
    macro_cast_from_unpacked_struct();
    macro_cast_from_union();
    macro_cast_from_packed_struct_array();
    macro_cast_from_unpacked_struct_array();

    peek();
    peek_back();
    peek_buffer();
    peek_buffer_back();
    peek_interpret();
    peek_interpret_back();

    pop();
    pop_back();
    pop_buffer();
    pop_buffer_back();
    interpret();
    interpret_back();

    skip();
    skip_back();
}

}
