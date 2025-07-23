#include "pch.h"
#include "MakeSpriteFontCLI.h"

using namespace System;
using namespace System::Collections::Generic;

array<String ^> ^ ToManagedStringArray(const std::vector<std::wstring>& vec)
{
    array<String ^> ^ arr = gcnew array<String ^>(static_cast<int>(vec.size()));
    int index = 0;
    for (const auto& str : vec)
    {
        arr[index++] = gcnew String(str.c_str());
    }
    return arr;
}

int MakeSpriteFontCLI::MakeSpriteFontCLI::Run(const std::vector<std::wstring>& options)
{
    MakeSpriteFont::Library ^ library = gcnew MakeSpriteFont::Library();
    return library->Make(ToManagedStringArray(options));
}