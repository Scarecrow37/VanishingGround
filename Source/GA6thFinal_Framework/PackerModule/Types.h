#pragma once

namespace Packer
{
    struct IsEven
    {
        bool operator()(uint32_t value) const;
    };

    struct IsBigEndian
    {
        constexpr bool operator()() const { return std::endian::native == std::endian::big; }
    };

    struct ByteSwap
    {
        constexpr uint32_t operator()(uint32_t value) const;
    };

    using Fourcc = uint32_t;

    template <std::endian Endian>
    constexpr Fourcc MakeFourcc(const char a, const char b, const char c, const char d) noexcept
    {
        if constexpr (Endian == std::endian::little)
        {
            return static_cast<Fourcc>(static_cast<uint8_t>(a)) | (static_cast<Fourcc>(static_cast<uint8_t>(b)) << 8) |
                   (static_cast<Fourcc>(static_cast<uint8_t>(c)) << 16) |
                   (static_cast<Fourcc>(static_cast<uint8_t>(d)) << 24);
        }
        else
        {
            return static_cast<Fourcc>(static_cast<uint8_t>(d)) | (static_cast<Fourcc>(static_cast<uint8_t>(c)) << 8) |
                   (static_cast<Fourcc>(static_cast<uint8_t>(b)) << 16) |
                   (static_cast<Fourcc>(static_cast<uint8_t>(a)) << 24);
        }
    };

    constexpr Fourcc FOURCC_RIFF = MakeFourcc<std::endian::little>('R', 'I', 'F', 'F');
    constexpr Fourcc FOURCC_PACK = MakeFourcc<std::endian::little>('P', 'A', 'C', 'K');
    constexpr Fourcc FOURCC_FILE = MakeFourcc<std::endian::little>('f', 'i', 'l', 'e');
    constexpr Fourcc FOURCC_PATH = MakeFourcc<std::endian::little>('p', 'a', 't', 'h');
    constexpr Fourcc FOURCC_DATA = MakeFourcc<std::endian::little>('d', 'a', 't', 'a');

    constexpr uint32_t FOURCC_COUNT = sizeof(Fourcc);
    constexpr uint32_t SIZE_COUNT   = sizeof(uint32_t);
}