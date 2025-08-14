## Sedfer Helpers

Small collection of helpers for C++:

* Basic type aliases (u8, u32, etc.)
* Packed types
* ConstBuffer, MutableBuffer

## Note
This was designed for personal use. Don't expect anything to meet your expectations.

I have run some tests, so it's not completely broken, but probably not 100% bug-free.

Feel free to use/modify/share in any way you like (free MIT license).

## Buffers overview

ConstBuffer and MutableBuffer are light-weight non-owning views of bytes.
```c++
struct ConstBuffer {
    const u8 * data;
    usize size;
}

struct MutableBuffer {
    u8 * data;
    usize size;
};
```

It is designed to simplify the use of common (pointer + size) pair, with following in mind:
1. Efficiency.
Buffers are 16 bytes on 64-bit (or 8 on 32-bit) platform, very cheap to copy and create on stack.
All functions are inlined and allow compiler optimizations. In most cases buffer objects will not exist in the optimized binary.

2. Safety.
Most common mistake when working with (pointer + size) pairs is accessing data outside buffer range.
Buffers have built-in read/write interface that always does range checks and returns [[nodiscard]] result.

3. Simplicity.
Buffers have implicit constructors from all basic types and POD structs. This allows to write very clean code without ugly `reinterpret_cast` everywhere.
Built-in read/write operations automatically adjust pointer (consume buffer) to avoid explicit (often bug-prone) offset calculation.

Example (parsing binary TLV-struct):
```c++
#include "helpers/buffer.h"

using namespace sedfer;

struct Header {
    u8 version;
    u32 count;
} __attribute__((packed));

enum class Type : u16 {
    // ...
};

bool try_parse(ConstBuffer packet) {
    const Header * const header = packet.interpret<Header>();
    if(header == nullptr) return false;
    if(header->version != 1) return false;

    for(usize i = 0; i < header->count; ++i) {
        Type type;
        if(not packet.pop(type)) return false;

        u16 size;
        if(not packet.pop(size)) return false;

        ConstBuffer data = packet.pop_buffer(size);
        if(data.data == nullptr) return false;

        // use (type, size, data)
    }

    if(packet.size != 0) return false;

    return true;
}
```

Example (creating message):
```c++
#include "helpers/buffer.h"

using namespace sedfer;

static u8 bytes[1024];

static constexpr u8 VERSION_1 = 1;

enum class Type : u16 {
    AA = 0x0001,
    BB = 0x0002,
};

struct AA {
    // ...
} __attribute__((packed));

struct BB {
    // ...
} __attribute__((packed));

// u8 version | u32 size | u16 type | <...> | u16 type | <...> | ...
ConstBuffer make_message() {
    MutableBuffer buffer = bytes;

    ASSERT(buffer.push(VERSION_1));

    u32packed * const size = buffer.interpret<u32packed>(); // reserve space (fill at the end)
    ASSERT(size);

    ASSERT(buffer.push(Type::AA));
    auto aa = buffer.interpret<AA>();
    ASSERT(aa);
    // fill aa

    ASSERT(buffer.push(Type::BB));
    auto bb = buffer.interpret<BB>();
    ASSERT(bb);
    // fill bb

    const usize total_size = sizeof(bytes) - buffer.size;
    *size = total_size;

    return {bytes, total_size};
}
```
