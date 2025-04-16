#include "pch.h"

std::array<float, 4> ImGuiHelper::ImVec4ToArray(const ImVec4& vec4)
{
    std::array<float, 4> array{};
    std::memcpy(array.data(), &vec4.x, sizeof(array));
    return array;
}

ImVec4 ImGuiHelper::ArrayToImVec4(const std::array<float, 4>& array)
{
    ImVec4 vec4;
    std::memcpy(&vec4.x, array.data(), sizeof(array));
    return vec4;
}
