#pragma once

#include "helpers/packed.h"

static_assert(sizeof(sedfer::u16packed) == 2);
static_assert(sizeof(sedfer::u32packed) == 4);
static_assert(sizeof(sedfer::u64packed) == 8);
static_assert(sizeof(sedfer::u128packed) == 16);

static_assert(sizeof(sedfer::i16packed) == 2);
static_assert(sizeof(sedfer::i32packed) == 4);
static_assert(sizeof(sedfer::i64packed) == 8);
static_assert(sizeof(sedfer::i128packed) == 16);

static_assert(sizeof(sedfer::f16packed) == 2);
static_assert(sizeof(sedfer::f32packed) == 4);
static_assert(sizeof(sedfer::f64packed) == 8);
static_assert(sizeof(sedfer::f128packed) == 16);

static_assert(alignof(sedfer::u16packed) == 1);
static_assert(alignof(sedfer::u32packed) == 1);
static_assert(alignof(sedfer::u64packed) == 1);
static_assert(alignof(sedfer::u128packed) == 1);

static_assert(alignof(sedfer::i16packed) == 1);
static_assert(alignof(sedfer::i32packed) == 1);
static_assert(alignof(sedfer::i64packed) == 1);
static_assert(alignof(sedfer::i128packed) == 1);

static_assert(alignof(sedfer::f16packed) == 1);
static_assert(alignof(sedfer::f32packed) == 1);
static_assert(alignof(sedfer::f64packed) == 1);
static_assert(alignof(sedfer::f128packed) == 1);
