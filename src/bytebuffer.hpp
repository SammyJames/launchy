#pragma once
#include "types.hpp"
#include <sstream>

class ByteBuffer : public std::stringbuf
{
public:
    template <class T>
    size_t Read(T& out)
    {
        union {
            T value;
            u8 data[ sizeof(T) ];
        } coerce;
        auto result = xsgetn(coerce.data, sizeof(T));
        out         = coerce.value;
        return result;
    }

    template <class T>
    size_t Write(const T& in)
    {
        union {
            T value;
            u8 data[ sizeof(T) ];
        } coerce;
        coerce.value = in;
        return xsputn(coerce.data, sizeof(T));
    }

    template <class T>
    size_t Write(T&& in)
    {
        union {
            T value;
            u8 data[ sizeof(T) ];
        } coerce;
        coerce.value = std::forward<T>(in);
        return xsputn(coerce.data, sizeof(T));
    }
};