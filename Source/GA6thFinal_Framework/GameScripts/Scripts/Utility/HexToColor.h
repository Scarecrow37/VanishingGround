#pragma once

struct HexToColor
{
    Color operator()(const std::string& hex) const;
};