#include "helpers/all.h"

#include "tests/test.h"
#include "tests/is_convertable.h"

#include "bits/stdc++.h"

namespace sedfer::test {

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

template<typename T>
static void expect_same(const T & t, ConstBuffer buffer) {
    EXPECT(buffer.size == sizeof(T), "size " << buffer.size);
    const u8 * ptr = reinterpret_cast<const u8 *>(&t);
    EXPECT(std::equal(buffer.data, buffer.data + buffer.size, ptr), "");
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

    f16 f16v = 16.0;
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

template<typename T>
static void fill_random(T * data, usize size) {
    for(usize i = 0; i < size; ++i) {
        data[i] = rand();
    }
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
    S4 s4 = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536., -373839, 404142, -43445};
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
    S4 s4 = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536., -373839, 404142, -43445};
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
    S4 s4[11] = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536., -373839, 404142, -43445};
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
    S4 s4[11] = {123, 456, 789, 101112, 131415, 161718, -19, -2021, -232425, -262728, -293031, -323334, 3536., -373839, 404142, -43445};
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
}

void test_const_buffer() {
    constructor_from_parts();
    default_constructor();
    copy_constructor();
    move_constructor();
    operator_assign();

    implicit_cast_from_basic_types();
    implicit_cast_from_packed_types();
    implicit_cast_from_basic_arrays();
    implicit_cast_from_packed_struct();
    implicit_cast_from_unpacked_struct();
    implicit_cast_from_packed_struct_array();
    implicit_cast_from_unpacked_struct_array();

    static_is_convertable();

    // TODO: from enum
    // TODO: unions
    // TODO: SEDFER_CONST_BUFFER
}

}
