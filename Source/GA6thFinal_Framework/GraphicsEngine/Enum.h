#pragma once

enum class ShaderType { Vertex, Pixel, Geometry, Compute };
enum class TextureType { Diffuse, Normal, RMA, Emission, Opacity, End };

namespace MeshType
{
	enum Type { Static, Skeletal, SkyBox, End };
}

namespace RenderType
{
	enum Type { NoneAlpha, Alpha, End };
}

constexpr float SHADOW_WIDTH = 8192.f;
constexpr float SHADOW_HEIGHT = 8192.f;
constexpr unsigned int MAX_BONE_MATRIX = 512;
constexpr unsigned int MAX_DRAW_OBJECT = 2048;
constexpr unsigned int MAX_DIRECTIONAL_LIGHT = 4;
constexpr unsigned int MAX_POINT_LIGHT = 32;
constexpr float COLOR_ZERO[4]{ 0.f, 0.f, 0.f, 1.f };