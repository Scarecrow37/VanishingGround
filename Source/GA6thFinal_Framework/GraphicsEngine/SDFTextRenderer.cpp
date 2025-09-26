#include "pch.h"
#include "SDFTextRenderer.h"
#include "SDFFont.h"

SDFTextRenderer::SDFTextRenderer()
    : _color(DirectX::Colors::White)
{
    _worldMatrix = XMMatrixIdentity();
    _size        = Vector2::Zero;
}

SDFTextRenderer::~SDFTextRenderer() = default;

bool SDFTextRenderer::IsActive() const
{
    bool isActive = _isActive ? *_isActive : false;
    return isActive && !_text.empty() && _font && _font->IsValid();
}

D3D12_GPU_DESCRIPTOR_HANDLE SDFTextRenderer::GetFontTextureHandle() const
{
    return _font->GetTextureHandle();
}

const Matrix& SDFTextRenderer::GetWorldMatrix() const
{
    return _worldMatrix;
}

void SDFTextRenderer::SetActive(const bool* isActive)
{
    _isActive = isActive;
}

void SDFTextRenderer::SetFont(std::shared_ptr<SDFFont> font)
{
    _font = font;
}

void SDFTextRenderer::SetText(const wchar_t* text)
{
    _text = text;
    _dirty = true;
}

void SDFTextRenderer::SetPosition(const Vector3& position)
{
    _position = position;
    _dirty    = true;
}

void SDFTextRenderer::SetScale(const float scale)
{
    _scale = scale;
    _dirty = true;
}

void SDFTextRenderer::SetColor(const Vector4& color)
{
    _color = color;
}

void SDFTextRenderer::SetRotation(const float rotation)
{
    _rotation = rotation;
}

void SDFTextRenderer::Release()
{
    for (auto& isDestroy : _isDestroyeds)
    {
        *isDestroy = true;
    }
    _isDestroyeds.clear();

    delete this;
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

    hr = device->CreateCommittedResource(&heapPropertyUPLOAD, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_vertexUploadBuffer));
    FAILED_CHECK_MESSAGE(hr, L"d3dUtil::CreateDefaultBuffer Failed");

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
}

void SDFTextRenderer::Update(ID3D12GraphicsCommandList* commandList)
{
    if (_dirty)
    {
        _worldMatrix = Matrix::CreateRotationZ(_rotation) * Matrix::CreateTranslation(_position);

        const auto& atlasInfo   = _font->GetAtlasInfo();
        const auto& metricsInfo = _font->GetMetricsInfo();

        float cursorX = _position.x;
        float cursorY = _position.y;

        int charCount = 0;

        // [추가] 이 프레임에서 계산할 로컬 경계 상자 변수
        float calculatedBounds[4] = { FLT_MAX, -FLT_MAX, -FLT_MAX, FLT_MAX }; // Left, Top, Right, Bottom

        for (wchar_t wc : _text)
        {
            if (charCount >= MAX_CHARS)
                break;

            if (wc == L'\n')
            {
                cursorX = _position.x;
                cursorY += metricsInfo.LineHeight * _scale;
                continue;
            }

            const SDF::Glyph* glyph = _font->GetGlyph(wc);
            if (!glyph)
            {
                glyph = _font->GetGlyph(L'?'); // Use a default character
                if (!glyph)
                    continue;
            }

            float u0 = glyph->AtlasBounds.Left / atlasInfo.Width;
            float v0 = glyph->AtlasBounds.Bottom / atlasInfo.Height;
            float u1 = glyph->AtlasBounds.Right / atlasInfo.Width;
            float v1 = glyph->AtlasBounds.Top / atlasInfo.Height;

            float planeLeft   = glyph->PlaneBounds.Left * _scale;
            float planeBottom = glyph->PlaneBounds.Bottom * _scale;
            float planeRight  = glyph->PlaneBounds.Right * _scale;
            float planeTop    = glyph->PlaneBounds.Top * _scale;

            float quadLeft   = cursorX + planeLeft;
            float quadBottom = cursorY - planeBottom;
            float quadRight  = cursorX + planeRight;
            float quadTop    = cursorY - planeTop;

            // [추가] 전체 경계 상자를 현재 글리프의 경계에 맞게 확장
            calculatedBounds[0] = std::min(calculatedBounds[0], quadLeft);
            calculatedBounds[1] = std::max(calculatedBounds[1], quadTop);       // Y축 방향에 따라 min/max가 달라질 수 있습니다.
            calculatedBounds[2] = std::max(calculatedBounds[2], quadRight);
            calculatedBounds[3] = std::min(calculatedBounds[3], quadBottom); // 현재 코드의 정점 생성 방식에 맞춤

            _vertices[charCount * 4 + 0] = Vertex{{quadLeft, quadTop, 0.0f, 0.f}, {u0, v1}};
            _vertices[charCount * 4 + 1] = Vertex{{quadRight, quadTop, 0.0f, 0.f}, {u1, v1}};
            _vertices[charCount * 4 + 2] = Vertex{{quadRight, quadBottom, 0.0f, 0.f}, {u1, v0}};
            _vertices[charCount * 4 + 3] = Vertex{{quadLeft, quadBottom, 0.0f, 0.f}, {u0, v0}};

            cursorX += glyph->Advance * _scale;
            charCount++;
        }

        // [추가] 계산된 경계 상자를 멤버 변수에 저장
        if (charCount > 0)
        {
            _size = Vector2(calculatedBounds[2] - calculatedBounds[0], calculatedBounds[1] - calculatedBounds[3]);
        }
        else
        {
            _size = Vector2::Zero;
        }

        if (charCount == 0)
        {
			_dirty = false; // [수정] 텍스트가 비었을 경우에도 dirty 플래그를 해제
            return;
        }

        void* data = nullptr;
        _vertexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&data));
        memcpy(data, _vertices.data(), sizeof(Vertex) * charCount * 4);
        _vertexUploadBuffer->Unmap(0, nullptr);

        auto br = CD3DX12_RESOURCE_BARRIER::Transition(_vertexBuffer.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);

        commandList->ResourceBarrier(1, &br);
        commandList->CopyBufferRegion(_vertexBuffer.Get(), 0, _vertexUploadBuffer.Get(), 0, sizeof(Vertex) * charCount * 4);

        br = CD3DX12_RESOURCE_BARRIER::Transition(_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);

        commandList->ResourceBarrier(1, &br);

        _dirty = false;
    }
}

void SDFTextRenderer::Render(ID3D12GraphicsCommandList* commandList)
{
    if (!_font || _text.empty())
        return;

    commandList->IASetIndexBuffer(&_indexBufferView);
    commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawIndexedInstanced(static_cast<UINT>(_text.size()) * 6, 1, 0, 0, 0);
}