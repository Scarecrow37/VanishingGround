#pragma once

namespace Audio
{
    class Handle
    {
        friend class EManager;

    public:
        Handle();

    protected:
        Handle(WaveFormatHash hash, Index index, Generation generation);

    private:
        WaveFormatHash _hash;
        Index          _index;
        Generation     _generation;
    };
} // namespace Audio