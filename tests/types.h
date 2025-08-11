#pragma once

#include "helpers/types.h"

#include <concepts>

static_assert(sizeof(sedfer::u8) == 1);
static_assert(sizeof(sedfer::u16) == 2);
static_assert(sizeof(sedfer::u32) == 4);
static_assert(sizeof(sedfer::u64) == 8);
static_assert(sizeof(sedfer::u128) == 16);
static_assert(sizeof(sedfer::usize) >= sizeof(sedfer::u32));

static_assert(sizeof(sedfer::i8) == 1);
static_assert(sizeof(sedfer::i16) == 2);
static_assert(sizeof(sedfer::i32) == 4);
static_assert(sizeof(sedfer::i64) == 8);
static_assert(sizeof(sedfer::i128) == 16);
static_assert(sizeof(sedfer::isize) >= sizeof(sedfer::u32));

static_assert(sizeof(sedfer::f16) == 2);
static_assert(sizeof(sedfer::f32) == 4);
static_assert(sizeof(sedfer::f64) == 8);
static_assert(sizeof(sedfer::f128) == 16);

static_assert(std::unsigned_integral<sedfer::u8>);
static_assert(std::unsigned_integral<sedfer::u16>);
static_assert(std::unsigned_integral<sedfer::u32>);
static_assert(std::unsigned_integral<sedfer::u64>);
static_assert(std::unsigned_integral<sedfer::u128>);
static_assert(std::unsigned_integral<sedfer::usize>);

static_assert(std::signed_integral<sedfer::i8>);
static_assert(std::signed_integral<sedfer::i16>);
static_assert(std::signed_integral<sedfer::i32>);
static_assert(std::signed_integral<sedfer::i64>);
static_assert(std::signed_integral<sedfer::i128>);
static_assert(std::signed_integral<sedfer::isize>);

static_assert(std::floating_point<sedfer::f16>);
static_assert(std::floating_point<sedfer::f32>);
static_assert(std::floating_point<sedfer::f64>);
static_assert(std::floating_point<sedfer::f128>);
