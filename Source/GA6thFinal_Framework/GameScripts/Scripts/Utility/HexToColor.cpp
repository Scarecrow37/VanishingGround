#include "pchScripts.h"
#include "HexToColor.h"

Color HexToColor::operator()(const std::string& hex) const
{
    static const std::regex HEX_COLOR_REGEX(R"(^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{8})$)");

    if (!std::regex_match(hex, HEX_COLOR_REGEX))
        return {0.0f, 0.0f, 0.0f, 1.0f}; // Invalid hex format

    std::string hexColor = hex;
    hexColor             = hexColor.substr(1);

    unsigned int      r = 0, g = 0, b = 0, a = 255;
    std::stringstream ss;
    ss << std::hex << hexColor.substr(0, 2);
    ss >> r;
    ss.clear();
    ss << std::hex << hexColor.substr(2, 2);
    ss >> g;
    ss.clear();
    ss << std::hex << hexColor.substr(4, 2);
    ss >> b;

    if (hexColor.length() == 8)
    {
        ss.clear();
        ss << std::hex << hexColor.substr(6, 2);
        ss >> a;
    }

    float red   = static_cast<float>(r) / 255.0f;
    float green = static_cast<float>(g) / 255.0f;
    float blue  = static_cast<float>(b) / 255.0f;
    float alpha = static_cast<float>(a) / 255.0f;

    return {red, green, blue, alpha};
}