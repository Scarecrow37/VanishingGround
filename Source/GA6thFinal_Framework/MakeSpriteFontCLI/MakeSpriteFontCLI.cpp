#include "pch.h"
#include "MakeSpriteFontCLI.h"

using namespace System;
using namespace System::Collections::Generic;

array<String ^> ^ ToManagedStringArray(const std::vector<std::wstring>& vec)
{
    array<String ^> ^ arr = gcnew array<String ^>(static_cast<int>(vec.size()));
    for (int i = 0; i < vec.size(); ++i)
    {
        arr[i] = gcnew String(vec[i].c_str());
    }
    return arr;
}

int MakeSpriteFontCLI::MakeSpriteFontCLI::Run(const std::vector<std::wstring>& options)
{
    MakeSpriteFont::Library ^ library = gcnew MakeSpriteFont::Library();
    return library->Make(ToManagedStringArray(options));
}