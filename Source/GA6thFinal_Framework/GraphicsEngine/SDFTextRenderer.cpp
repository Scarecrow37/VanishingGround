#include "pch.h"
#include "SDFTextRenderer.h"
#include "SDFFont.h"

SDFTextRenderer::SDFTextRenderer()
    : UIRenderer(UIRenderer::Type::TEXT)
    , _color(DirectX::Colors::White)
{
    _size        = Vector2::Zero;
    _fontSize    = 16.f;
    _fontWeight  = 0.f;
    _uiType      = UIType::MODE_2D;
}

SDFTextRenderer::~SDFTextRenderer() = default;

bool SDFTextRenderer::IsActive() const
{
    return GraphicsBase::IsActive() && !_text.empty() && _font && _font->IsValid();
}

Vector2 SDFTextRenderer::GetStringSize() const
{    
    return _size * _fontSize;
}

D3D12_GPU_DESCRIPTOR_HANDLE SDFTextRenderer::GetFontTextureHandle() const
{
    return _font->GetTextureHandle();
}

void SDFTextRenderer::SetActive(const bool* isActive)
{
    GraphicsBase::SetActive(isActive);
}

void SDFTextRenderer::SetFont(std::shared_ptr<SDFFont> font)
{
    _font = font;
    _dirty = true;

    MeasureString();
}

void SDFTextRenderer::SetText(const wchar_t* text)
{
    _text = text;

    std::wstring::size_type pos = 0;
    while ((pos = _text.find(L"\\n", pos)) != std::wstring::npos)
    {
        _text.replace(pos, lstrlen(L"\\n"), L"\n");
        pos += lstrlen(L"\n");
    }

    _dirty = true;

    MeasureString();
}

void SDFTextRenderer::SetFontSize(const float fontSize)
{
    _fontSize = fontSize;
}

void SDFTextRenderer::SetRotation(const Vector3& rotation)
{
    _rotation = rotation;
}

void SDFTextRenderer::SetPosition(const Vector3& position)
{
    _position = position;
}

void SDFTextRenderer::SetColor(const Vector4& color)
{
    _color = color;
}

void SDFTextRenderer::SetFontWeight(const float fontWeight)
{
    float convert = std::clamp(fontWeight, 0.f, 1.f);
    _fontWeight = convert - 0.5f;
}

void SDFTextRenderer::SetFontOutline(const GE::FontOutline& outline)
{
    UINT flag = _fontFlags;
    flag      = outline.Enabled ? flag | OUTLINE : 
                                  flag & ~OUTLINE;

    _fontFlags = static_cast<FontFlags>(flag);

    _fontOutline = outline;
}

void SDFTextRenderer::AddReference()
{
    GraphicsBase::AddReference();
}

void SDFTextRenderer::Release()
{
    GraphicsBase::Release();
}

void SDFTextRenderer::Initialize()
{
    _vertices.resize(MAX_CHARS * 4);
    auto device = Global::device->GetDevice();
    size_t byteSize = sizeof(Vertex) * MAX_CHARS * 4;

    CD3DX12_HEAP_PROPERTIES heapPropertyDefault(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_HEAP_PROPERTIES heapPropertyUPLOAD(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC   defaultBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    CD3DX12_RESOURCE_DESC   uploadBufferDesc  = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    HRESULT hr = S_OK;
    hr         = device->CreateCommittedResource(&heapPropertyDefault, D3D12_HEAP_FLAG_NONE, &defaultBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_vertexBuffer));
    FAILED_CHECK_MESSAGE(hr, L"d3dUtil::CreateDefaultBuffer Failed");
    _vertexBuffer->SetName(L"SDF_Default");

    hr = device->CreateCommittedResource(&heapPropertyUPLOAD, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_vertexUploadBuffer));
    FAILED_CHECK_MESSAGE(hr, L"d3dUtil::CreateDefaultBuffer Failed");
    _vertexUploadBuffer->SetName(L"SDF_Upload");

    std::vector<UINT> indices(MAX_CHARS * 6);
    for (int i = 0; i < MAX_CHARS; ++i)
    {
        indices[i * 6 + 0] = i * 4 + 0;
        indices[i * 6 + 1] = i * 4 + 1;
        indices[i * 6 + 2] = i * 4 + 2;
        indices[i * 6 + 3] = i * 4 + 0;
        indices[i * 6 + 4] = i * 4 + 2;
        indices[i * 6 + 5] = i * 4 + 3;
    }
    Global::device->CreateIndexBuffer(indices.data(), sizeof(UINT) * MAX_CHARS * 6, DXGI_FORMAT_R32_UINT, _indexBuffer, _indexBufferView);

    _vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
    _vertexBufferView.StrideInBytes  = sizeof(Vertex);
    _vertexBufferView.SizeInBytes    = static_cast<UINT>(byteSize);
}

void SDFTextRenderer::Update(ID3D12GraphicsCommandList* commandList)
{
    if (_dirty)
    {
        const auto& atlasInfo   = _font->GetAtlasInfo();
        const auto& metricsInfo = _font->GetMetricsInfo();

        float cursorX = 0;
        float cursorY = metricsInfo.Ascender;
        _charCount    = 0;

        for (wchar_t wc : _text)
        {
            if (_charCount >= MAX_CHARS)
                break;

            if (wc == L'\n')
            {
                cursorX = 0;
                cursorY += metricsInfo.LineHeight;
                continue;
            }

            const SDF::Glyph* glyph = _font->GetGlyph(wc);
            if (!glyph)
            {
                glyph = _font->GetGlyph(L'□');
                if (!glyph)
                    continue;
            }

            float u0 = glyph->AtlasBounds.Left / atlasInfo.Width;
            float u1 = glyph->AtlasBounds.Right / atlasInfo.Width;
            float v0 = 1.0f - glyph->AtlasBounds.Top / atlasInfo.Height;
            float v1 = 1.0f - glyph->AtlasBounds.Bottom / atlasInfo.Height;

            float planeLeft   = glyph->PlaneBounds.Left;
            float planeBottom = glyph->PlaneBounds.Bottom;
            float planeRight  = glyph->PlaneBounds.Right;
            float planeTop    = glyph->PlaneBounds.Top;

            // Calculate quad expansion for outline
            float quadExpansion = 0.0f;
            float uvExpansion = 0.0f;
            if (_fontFlags & OUTLINE)
            {
                // Expand in plane space (em units)
                quadExpansion = _fontOutline.Width / atlasInfo.DistanceRange * metricsInfo.EmSize;
                
                // Calculate UV expansion
                // OutlineWidth is in distance field units, convert to atlas texture space
                float glyphWidthInAtlas = glyph->AtlasBounds.Right - glyph->AtlasBounds.Left;
                float glyphHeightInAtlas = glyph->AtlasBounds.Top - glyph->AtlasBounds.Bottom;
                float glyphWidthInPlane = planeRight - planeLeft;
                float glyphHeightInPlane = planeTop - planeBottom;
                
                // Ratio of plane expansion to original plane size
                float expansionRatioX = (glyphWidthInPlane > 0) ? quadExpansion / glyphWidthInPlane : 0;
                float expansionRatioY = (glyphHeightInPlane > 0) ? quadExpansion / glyphHeightInPlane : 0;
                
                // Apply same ratio to UV space
                float uvExpandX = (u1 - u0) * expansionRatioX;
                float uvExpandY = (v1 - v0) * expansionRatioY;
                
                // Expand UV coordinates
                u0 -= uvExpandX;
                u1 += uvExpandX;
                v0 -= uvExpandY;
                v1 += uvExpandY;
            }

            float quadLeft   = cursorX + planeLeft - quadExpansion;
            float quadBottom = cursorY - planeBottom - quadExpansion;
            float quadRight  = cursorX + planeRight + quadExpansion;
            float quadTop    = cursorY - planeTop + quadExpansion;

            _vertices[_charCount * 4 + 0] = Vertex{{quadLeft, quadTop, 0.f, 1.f}, {u0, v0}};
            _vertices[_charCount * 4 + 1] = Vertex{{quadRight, quadTop, 0.f, 1.f}, {u1, v0}};
            _vertices[_charCount * 4 + 2] = Vertex{{quadRight, quadBottom, 0.f, 1.f}, {u1, v1}};
            _vertices[_charCount * 4 + 3] = Vertex{{quadLeft, quadBottom, 0.f, 1.f}, {u0, v1}};

            cursorX += glyph->Advance;
            _charCount++;
        }

        if (0 == _charCount)
        {
			_dirty = false;
            return;
        }

        void* data = nullptr;
        _vertexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&data));
        memcpy(data, _vertices.data(), sizeof(Vertex) * _charCount * 4);
        _vertexUploadBuffer->Unmap(0, nullptr);

        auto br = CD3DX12_RESOURCE_BARRIER::Transition(_vertexBuffer.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);

        commandList->ResourceBarrier(1, &br);
        commandList->CopyBufferRegion(_vertexBuffer.Get(), 0, _vertexUploadBuffer.Get(), 0, sizeof(Vertex) * _charCount * 4);

        br = CD3DX12_RESOURCE_BARRIER::Transition(_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);

        commandList->ResourceBarrier(1, &br);

        _dirty = false;
    }
}

void SDFTextRenderer::Render(ID3D12GraphicsCommandList* commandList)
{
    if (!_font || _text.empty() || 0 == _charCount)
        return;

    commandList->IASetIndexBuffer(&_indexBufferView);
    commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawIndexedInstanced(static_cast<UINT>(_charCount) * 6, 1, 0, 0, 0);
}

void SDFTextRenderer::MeasureString()
{
    if (!_font || _text.empty())
    {
        _size = Vector2::Zero;
        return;
    }

    const auto& metricsInfo         = _font->GetMetricsInfo();
    float       calculatedBounds[4] = {FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX};
    float       cursorX             = 0;
    float       cursorY             = metricsInfo.Ascender;
    int         charCount           = 0;

    for (wchar_t wc : _text)
    {
        if (charCount >= MAX_CHARS)
            break;

        if (wc == L'\n')
        {
            cursorX = 0;
            cursorY += metricsInfo.LineHeight;
            continue;
        }

        const SDF::Glyph* glyph = _font->GetGlyph(wc);
        if (!glyph)
        {
            glyph = _font->GetGlyph(L'□'); // Use a default character
            if (!glyph)
                continue;
        }

        float planeLeft   = glyph->PlaneBounds.Left;
        float planeBottom = glyph->PlaneBounds.Bottom;
        float planeRight  = glyph->PlaneBounds.Right;
        float planeTop    = glyph->PlaneBounds.Top;

        // Expand quad if outline is enabled (same as in Update function)
        float quadExpansion = 0.0f;
        if (_fontFlags & OUTLINE)
        {
            // Calculate expansion in em-space units
            // OutlineWidth is in SDF distance units, convert to plane space
            quadExpansion = _fontOutline.Width / _font->GetAtlasInfo().DistanceRange * metricsInfo.EmSize;
        }

        float quadLeft   = cursorX + planeLeft - quadExpansion;
        float quadBottom = cursorY - planeBottom - quadExpansion;
        float quadRight  = cursorX + planeRight + quadExpansion;
        float quadTop    = cursorY - planeTop + quadExpansion;

        calculatedBounds[0] = std::min(calculatedBounds[0], quadLeft);
        calculatedBounds[1] = std::min(calculatedBounds[1], quadTop);
        calculatedBounds[2] = std::max(calculatedBounds[2], quadRight);
        calculatedBounds[3] = std::max(calculatedBounds[3], quadBottom);

        cursorX += glyph->Advance;
        charCount++;
    }

    if (charCount > 0)
    {
        _size = Vector2(calculatedBounds[2] - calculatedBounds[0], calculatedBounds[3] - calculatedBounds[1]);
    }
    else
    {
        _size = Vector2::Zero;
    }
}
