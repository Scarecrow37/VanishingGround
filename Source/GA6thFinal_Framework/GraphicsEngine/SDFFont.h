#pragma once
#include "Resource.h"

namespace SDF
{
    struct Atlas
    {
        std::string Type;
        std::string YOrigin;
        float       DistanceRange;
        float       Size;
        float       Width;
        float       Height;
    };

    struct Metrics
    {
        float EmSize;
        float LineHeight;
        float Ascender;
        float Descender;
        float UnderlineY;
        float UnderlineThickness;
    };

    struct PlaneBounds
    {
        float Left;
        float Bottom;
        float Right;
        float Top;
    };

    struct AtlasBounds
    {
        float Left;
        float Bottom;
        float Right;
        float Top;
    };

    struct Glyph
    {
        PlaneBounds  PlaneBounds;
        AtlasBounds  AtlasBounds;
        unsigned int Unicode;
        float        Advance;
    };
}

class SDFFont : public Resource
{
public:
    SDFFont();
    virtual ~SDFFont();

public:
    bool                        IsValid() const override;
    const SDF::Atlas&           GetAtlasInfo() const { return _atlas; }
    const SDF::Metrics&         GetMetricsInfo() const { return _metrics; }
    const SDF::Glyph*           GetGlyph(wchar_t unicode) const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle() const;

public:
    void LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback = nullptr) override;

private:
    bool ParseJson(const std::filesystem::path& jsonPath);
    void ParseAtlas(yyjson_val* atlasValue);
    void ParseMetrics(yyjson_val* metricsValue);
    void ParseGlyphs(yyjson_val* glyphsValue);
    void yyjsonValueTypeHelper(yyjson_val* value, float& data);

private:
    SDF::Atlas                    _atlas;
    SDF::Metrics                  _metrics;
    std::map<wchar_t, SDF::Glyph> _glyphs;
    std::shared_ptr<Texture>      _texture;
};
