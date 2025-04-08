#include "pch.h"
#include "LineRenderer.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "PixelShader.h"
#include "Texture.h"

void LineRenderer::Release()
{
    SafeRelease(_pVertexBuffer);
	delete this;
}

void LineRenderer::SetSourcePosition(float* pArray)
{
    _pSrcPosition = reinterpret_cast<Vector3*>(pArray);
}

void LineRenderer::SetDestinationPosition(float* pArray)
{
    _pDstPosition = reinterpret_cast<Vector3*>(pArray);
}

void LineRenderer::SetSegment(unsigned int segment)
{
    _numSegments = 2;

    std::vector<Line> lineVertices;
    lineVertices.resize(_numSegments);

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = lineVertices.size() * sizeof(Line);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = lineVertices.data();

    ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();
    pDevice->CreateBuffer(&bufferDesc, &initData, &_pVertexBuffer);

    /*_numSegments = segment;

    std::vector<Line> lineVertices;
    lineVertices.resize(_numSegments + 1);

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = lineVertices.size() * sizeof(Line);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = lineVertices.data();

    ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();
    pDevice->CreateBuffer(&bufferDesc, &initData, &_pVertexBuffer);*/
}

void LineRenderer::SetBaseTexture(const wchar_t* filePath)
{
    _textures[Base] = g_pResourceMgr->LoadResource<Texture>(filePath);
}

void LineRenderer::SetMaskTexture(const wchar_t* filePath)
{
    _textures[Mask] = g_pResourceMgr->LoadResource<Texture>(filePath);
}

void LineRenderer::SetGradientTexture(const wchar_t* filePath)
{
    _textures[Gradient] = g_pResourceMgr->LoadResource<Texture>(filePath);
}

void LineRenderer::Initialize()
{
    _pDeviceContext = g_pGraphicDevice->GetDeviceContext();
    
    _stride = sizeof(Line);

    _vs = g_pResourceMgr->LoadResource<VertexShader>(L"Assets/Shaders/LineWaveVS.cso");
    _gs = g_pResourceMgr->LoadResource<GeometryShader>(L"Assets/Shaders/LineWaveGS.cso");
    _ps = g_pResourceMgr->LoadResource<PixelShader>(L"Assets/Shaders/LineWavePS.cso");
    
    _waveData.waveAmplitude = 0.5f;
    _waveData.waveFrequency = 2.f;
}

void LineRenderer::Update(float deltaTime)
{
    ID3D11DeviceContext* pDeviceContext = g_pGraphicDevice->GetDeviceContext();
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = pDeviceContext->Map(_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    Line* vertices = reinterpret_cast<Line*>(mappedResource.pData);
    vertices[0].position = Vector4(*_pSrcPosition);
    vertices[0].position.w = 1.f;

    vertices[1].position = Vector4(*_pDstPosition);
    vertices[1].position.w = 1.f;

    /*for (unsigned int i = 0; i <= _numSegments; ++i)
    {
        float t = (float)i / _numSegments;
        vertices[i].position = XMVectorLerp(*_pSrcPosition, *_pDstPosition, t);
        vertices[i].position.w = 1.f;
        vertices[i].waveData = Vector2(t, i % 2 == 0 ? 1.0f : -1.0f);
    }*/

    pDeviceContext->Unmap(_pVertexBuffer, 0);

    _waveData.time += deltaTime;

    if (_waveData.time >= 1.f)
    {
        _waveData.time -= 1.f;
    }
}

void LineRenderer::Render()
{    
    _vs->SetVertexShader();
    _gs->SetGeometryShader();
    _ps->SetPixelShader();
    
    g_pConstantBuffer->UpdateConstantBuffer(L"WaveData", &_waveData);
    _pDeviceContext->PSSetShaderResources(0, 1, _textures[Base]->Get());
    _pDeviceContext->PSSetShaderResources(1, 1, _textures[Mask]->Get());
    _pDeviceContext->PSSetShaderResources(2, 1, _textures[Gradient]->Get());
    _pDeviceContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &_stride, &_offset);
    _pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    _pDeviceContext->Draw(2, 0);
    _pDeviceContext->GSSetShader(nullptr, nullptr, 0);
}

void LineRenderer::SetPostEffectLayer(unsigned int layer)
{
    UnlitRenderer::SetPostEffectLayer(layer);
}
