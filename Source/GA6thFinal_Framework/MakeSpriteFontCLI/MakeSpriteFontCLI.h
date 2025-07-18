#pragma once

using namespace System;

namespace MakeSpriteFontCLI
{
public
    ref class MakeSpriteFontCLI
    {
    public:
        static int Run(const std::vector<std::string>& options);
    };

    extern "C" inline __declspec(dllexport) int MakeSpriteFont(const std::vector<std::string>& options)
    {
        return MakeSpriteFontCLI::Run(options);
    }
} // namespace MakeSpriteFontCLI