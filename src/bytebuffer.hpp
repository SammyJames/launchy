#pragma once

#include "types.hpp"

#include <sstream>

using std::decay_t;
using std::forward;

class ByteBuffer
{
public:
    template <class T>
    void Read(T& out);

    template <class T>
    void Write(const T& value);

    template <class T>
    void Write(T&& value);

    string ToString() const;

protected:
    u8* GetMemory(size_t space_requested);
    void ReadMemory(u8* dest, size_t len);

private:
    u8* m_data        = nullptr;
    size_t m_size     = 0;
    size_t m_capacity = 0;
    size_t m_seek     = 0;
};

template <class T>
inline void ByteBuffer::Read(T& out)
{
    union {
        decay_t<T> value;
        u8 data[ sizeof(T) ];
    } coerce;
    ReadMemory(coerce.data, sizeof(T));
    out = coerce.value;
}

template <>
inline void ByteBuffer::Read<string>(string& out)
{
    u32 len;
    Read(len);

    if (len > 0)
    {
        out.resize((size_t)len);
        ReadMemory((u8*)out.data(), (size_t)len);
    }
}

template <class T>
inline void ByteBuffer::Write(const T& value)
{
    union {
        decay_t<T> value;
        u8 data[ sizeof(T) ];
    } coerce;
    coerce.value = value;
    if (auto mem = GetMemory(sizeof(T)))
    {
        memcpy(mem, coerce.data, sizeof(T));
    }
}

template <>
inline void ByteBuffer::Write<string>(const string& value)
{
    Write<u32>(value.length());
    if (auto mem = GetMemory(value.length()))
    {
        memcpy(mem, value.data(), value.length());
    }
}

template <class T>
inline void ByteBuffer::Write(T&& value)
{
    union {
        decay_t<T> value;
        char data[ sizeof(T) ];
    } coerce;
    coerce.value = forward<T>(value);
    if (auto mem = GetMemory(sizeof(T)))
    {
        memcpy(mem, coerce.data, sizeof(T));
    }
}

template <>
inline void ByteBuffer::Write<string>(string&& value)
{
    auto len = forward<string>(value).size();
    Write<u32>(len);
    if (auto mem = GetMemory(len))
    {
        memcpy(mem, forward<string>(value).data(), len);
    }
}
