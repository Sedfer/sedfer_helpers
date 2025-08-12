#include "helpers/all.h"

#include "tests/test.h"

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
void expect_same(const T & t, ConstBuffer buffer) {
    EXPECT(buffer.size == sizeof(T), "size " << buffer.size);
    const u8 * ptr = reinterpret_cast<const u8 *>(&t);
    EXPECT(std::equal(buffer.data, buffer.data + buffer.size, ptr), "");

    for(usize i = 0; i < buffer.size; ++i) {
        EXPECT(buffer.data[i] == ptr[i], std::dec << i << ": " << std::hex << (u32)buffer.data[i] << " " << (u32)ptr[i] << std::dec);
    }
}

void implicit_cast_from_basic_types() {
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

void test_const_buffer() {
    constructor_from_parts();
    default_constructor();
    copy_constructor();
    move_constructor();
    operator_assign();

    implicit_cast_from_basic_types();
}

}
