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
        Result Initialize();

    private:
        winrt::com_ptr<IXAudio2> _xAudio2;
        IXAudio2MasteringVoice*  _masteringVoice = nullptr;
    };
}