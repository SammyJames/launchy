#pragma once

#include <cstdint>
#include <cstdlib>
#include <experimental/any>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>

using u8  = unsigned char;
using i8  = signed char;
using u16 = unsigned short;
using i16 = signed short;
using u32 = unsigned int;
using i32 = signed int;
using u64 = unsigned long long;
using i64 = signed long long;
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