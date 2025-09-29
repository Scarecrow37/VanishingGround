#include "pch.h"
#include "SDFFont.h"

SDFFont::SDFFont() = default;

SDFFont::~SDFFont() = default;

bool SDFFont::IsValid() const
{
    return _texture->IsValid();
}

void SDFFont::LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback)
{
    // Assume JSON file has the same name but with .json extension
    std::filesystem::path jsonPath = filePath;
    jsonPath.replace_extension(".json");

    if (!ParseJson(jsonPath))
    {
        // Handle error - log or assert
        return;
    }

    // Load the texture atlas
    _texture = Global::resourceManager->LoadResource<Texture>(filePath, callback);
}

const SDF::Glyph* SDFFont::GetGlyph(wchar_t unicode) const
{
    auto iter = _glyphs.find(unicode);

    if (iter != _glyphs.end())
    {
        return &iter->second;
    }

    return nullptr; // Or return a default character glyph
}

D3D12_GPU_DESCRIPTOR_HANDLE SDFFont::GetTextureHandle() const
{
    return _texture->GetGPUHandle();
}

bool SDFFont::ParseJson(const std::filesystem::path& jsonPath)
{
    yyjson_doc* doc = yyjson_read_file(jsonPath.string().c_str(), 0, nullptr, nullptr);

    if (!doc)
    {
        // Failed to read file
        return false;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);

    ParseAtlas(yyjson_obj_get(root, "atlas"));
    ParseMetrics(yyjson_obj_get(root, "metrics"));
    ParseGlyphs(yyjson_obj_get(root, "glyphs"));

    yyjson_doc_free(doc);
    return true;
}

void SDFFont::ParseAtlas(yyjson_val* atlasVal)
{
    if (nullptr == atlasVal)
        return;
    
    _atlas.Type    = yyjson_get_str(yyjson_obj_get(atlasVal, "type"));
    _atlas.YOrigin = yyjson_get_str(yyjson_obj_get(atlasVal, "yOrigin"));
    yyjsonValuteTypeHelper(yyjson_obj_get(atlasVal, "distanceRange"), _atlas.DistanceRange);
    yyjsonValuteTypeHelper(yyjson_obj_get(atlasVal, "size"), _atlas.Size);
    yyjsonValuteTypeHelper(yyjson_obj_get(atlasVal, "width"), _atlas.Width);
    yyjsonValuteTypeHelper(yyjson_obj_get(atlasVal, "height"), _atlas.Height);
}

void SDFFont::ParseMetrics(yyjson_val* metricsVal)
{
    if (nullptr == metricsVal)
        return;

    yyjsonValuteTypeHelper(yyjson_obj_get(metricsVal, "emSize"), _metrics.EmSize);
    yyjsonValuteTypeHelper(yyjson_obj_get(metricsVal, "lineHeight"), _metrics.LineHeight);
    yyjsonValuteTypeHelper(yyjson_obj_get(metricsVal, "ascender"), _metrics.Ascender);
    yyjsonValuteTypeHelper(yyjson_obj_get(metricsVal, "descender"), _metrics.Descender);
    yyjsonValuteTypeHelper(yyjson_obj_get(metricsVal, "underlineY"), _metrics.UnderlineY);
    yyjsonValuteTypeHelper(yyjson_obj_get(metricsVal, "underlineThickness"), _metrics.UnderlineThickness);
}

void SDFFont::ParseGlyphs(yyjson_val* glyphsValue)
{
    if (nullptr == glyphsValue)
        return;

    size_t index, max;
    yyjson_val* value = nullptr;
    yyjson_arr_foreach(glyphsValue, index, max, value)
    {
        SDF::Glyph glyph{};

        glyph.Unicode = yyjson_get_uint(yyjson_obj_get(value, "unicode"));
        yyjsonValuteTypeHelper(yyjson_obj_get(value, "advance"), glyph.Advance);

        yyjson_val* planeBoundsValue = yyjson_obj_get(value, "planeBounds");
        if (planeBoundsValue)
        {
            yyjsonValuteTypeHelper(yyjson_obj_get(planeBoundsValue, "left"), glyph.PlaneBounds.Left);
            yyjsonValuteTypeHelper(yyjson_obj_get(planeBoundsValue, "bottom"), glyph.PlaneBounds.Bottom);
            yyjsonValuteTypeHelper(yyjson_obj_get(planeBoundsValue, "right"), glyph.PlaneBounds.Right);
            yyjsonValuteTypeHelper(yyjson_obj_get(planeBoundsValue, "top"), glyph.PlaneBounds.Top);
        }

        yyjson_val* atlasBoundsValue = yyjson_obj_get(value, "atlasBounds");
        if (atlasBoundsValue)
        {
            yyjsonValuteTypeHelper(yyjson_obj_get(atlasBoundsValue, "left"), glyph.AtlasBounds.Left);
            yyjsonValuteTypeHelper(yyjson_obj_get(atlasBoundsValue, "bottom"), glyph.AtlasBounds.Bottom);
            yyjsonValuteTypeHelper(yyjson_obj_get(atlasBoundsValue, "right"), glyph.AtlasBounds.Right);
            yyjsonValuteTypeHelper(yyjson_obj_get(atlasBoundsValue, "top"), glyph.AtlasBounds.Top);
        }

        _glyphs[static_cast<wchar_t>(glyph.Unicode)] = glyph;
    }
}

void SDFFont::yyjsonValuteTypeHelper(yyjson_val* value, float& data)
{
    data = yyjson_is_real(value) ? yyjson_get_real(value) : (float)yyjson_get_sint(value);
}