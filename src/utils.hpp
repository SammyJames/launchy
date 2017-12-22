#pragma once

#include "types.hpp"

template <class To, class From>
constexpr unique_ptr<To> unique_cast(unique_ptr<From>&& src)
{
    return unique_ptr<To>(static_cast<To*>(src.release()));
}

template <class To, class From>
constexpr unique_ptr<To> unique_cast(const unique_ptr<From>& src)
{
    return unique_ptr<To>(static_cast<To*>(src.get()));
}