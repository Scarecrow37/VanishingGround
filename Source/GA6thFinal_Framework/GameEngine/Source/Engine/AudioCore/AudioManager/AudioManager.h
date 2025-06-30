#pragma once

namespace Audio
{
    class EManager
    {
        friend class EngineCores;
    private:
        EManager();
        ~EManager();

    public:

    private:
        winrt::com_ptr<IXAudio2> _xAudio2;
    };
}