#include "bytebuffer.hpp"

using std::make_tuple;
using std::tie;

namespace Detail
{
    tuple<u8*, size_t> AllocCopy(u8* prev_addr, size_t prev_size, size_t needed)
    {
        auto new_cap = prev_size > 0 ? prev_size + (needed * 2) : needed * 2;
        auto new_mem = malloc(new_cap);
        memcpy(new_mem, prev_addr, prev_size);
        free(prev_addr);
        return make_tuple((u8*)new_mem, new_cap);
    }
} // namespace Detail

string ByteBuffer::ToString() const
{
    string result;
    return result;
}

u8* ByteBuffer::GetMemory(size_t space_requested)
{
    if (m_size + space_requested >= m_capacity)
    {
        auto delta              = m_capacity - m_size;
        auto needed             = space_requested - delta;
        tie(m_data, m_capacity) = Detail::AllocCopy(m_data, m_capacity, needed);
    }

    auto result = m_data + m_size;
    m_size += space_requested;
    return result;
}

void ByteBuffer::ReadMemory(u8* dest, size_t len)
{
    if (m_seek + len > m_size)
    {
        memset(dest, 0x0, len);
        return;
    }

    memcpy(dest, m_data + m_seek, len);
    m_seek += len;
}