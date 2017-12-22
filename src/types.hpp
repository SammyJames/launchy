#pragma once

#include <cstdint>
#include <cstdlib>
#include <experimental/any>
#include <functional>
#include <memory>
#include <string>
#include <tuple>

using u8  = uint8_t;
using i8  = int8_t;
using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

using std::experimental::any;
using std::experimental::any_cast;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::string;
using std::tuple;
using std::unique_ptr;