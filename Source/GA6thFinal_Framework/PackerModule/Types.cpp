#include "pch.h"
#include "Types.h"

bool Packer::IsEven::operator()(const uint32_t value) const
{
    return value % 2 == 0;
}

constexpr uint32_t Packer::ByteSwap::operator()(const uint32_t value) const
{
    return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000) |
           ((value << 24) & 0xFF000000);
}