#include "pch.h"
#include "Model.h"
#include "EmitLocator.h"
void EmitLocator::RandomInitialize()
{
    _randomGenerator = std::mt19937(_randomizer());
    _randomRange     = std::uniform_real_distribution<float>(-1.f, 1.f);
    RandomVal        = std::bind(_randomRange, _randomGenerator);
    RandomVal01      = [&]() { return (RandomVal() + 1.0f) * 0.5f; };
}

MeshSurfaceLocator::~MeshSurfaceLocator() = default;

DirectX::SimpleMath::Vector3 CubeLocator::EmitLocate()
{
    return {RandomVal() * _factor.x, RandomVal() * _factor.y, RandomVal() * _factor.z};
}

DirectX::SimpleMath::Vector3 CylinderLocator::EmitLocate()
{
    Vector2 location = {RandomVal(), RandomVal()};
    while (location.Length() > 1)
    {
        location = {RandomVal(), RandomVal()};
    }
    return {location.x * _factor.x, RandomVal() * _factor.y / 2, location.y * _factor.z};
}

DirectX::SimpleMath::Vector3 ConeLocator::EmitLocate()
{
    float   locationY     = (RandomVal() + 1) / 2 * _factor.y;
    float   sectionRadius = locationY * std::tan(_factor.x);
    Vector3 location      = Vector3(RandomVal(), 0, RandomVal());
    while (location.Length() > 1)
    {
        location = {RandomVal(), 0, RandomVal()};
    }
    location *= sectionRadius;
    location.y = locationY;
    return location;
}

DirectX::SimpleMath::Vector3 TorusLocator::EmitLocate()
{
    if (_factor.x <= _factor.z)
    {
        _factor.z = _factor.x - 0.1f;
    }
    // 랜덤 값을 [0,1] 범위로 매핑

    // 각도는 균등 분포 [0, 2π]
    float angle = 2.0f * XM_PI * RandomVal01();

    float radius = RandomVal();
    radius *= (_factor.x - _factor.z) / 2;
    radius += (_factor.x + _factor.z) / 2;

    float height = RandomVal();
    float temp   = std::sqrt((_factor.x - radius) * (radius - _factor.z));
    height *= temp;
    height *= _factor.y;

    // 극좌표를 직교좌표로 변환
    Vector3 location;
    location.x = radius * std::cos(angle);
    location.y = height;
    location.z = radius * std::sin(angle);

    // 스케일 적용
    return location;
}

DirectX::SimpleMath::Vector3 SphereLocator::EmitLocate()
{

    // 방향 벡터 생성 (단위 구면에서 균등 분포)
    Vector3 direction;
    direction = {RandomVal(), RandomVal(), RandomVal()};
    direction.Normalize();
    direction *= (RandomVal() + 1.0f) / 2.0f;
    return {direction.x * _factor.x, direction.y * _factor.y, direction.z * _factor.z};
}

DirectX::SimpleMath::Vector3 MeshSurfaceLocator::EmitLocate()
{
    if (_totalVertexCount == 0)
    {
        LoadVerticesFromModel(Global::resourceManager->LoadResource<Model>(_targetModelPath));
    }
    if (_targetModel && _targetModel->IsValid())
    {
        float temp         = ((RandomVal() + 1) * 0.5f);
        UINT  index        = static_cast<UINT>(temp * (_totalVertexCount - 1));
        UINT  tempIdx      = 0;
        int   meshIdx      = 0;
        int   vertexoffset = 0;
        for (auto count : _vertexCountPerMesh)
        {
            if (index >= tempIdx && index < tempIdx + count)
            {
                vertexoffset = index - tempIdx;
                break;
            }
            tempIdx += count;
            meshIdx++;
        }

        char* vertices;
        UINT  stride, size;
        auto& mesh = _targetModel->GetMeshes()[meshIdx];
        mesh->GetVertexInfo(vertices, stride, size);

        StaticMeshVertex* targetVertex = reinterpret_cast<StaticMeshVertex*>(vertices);
        targetVertex += vertexoffset;

        return Vector3(targetVertex->Position.x * _factor.x, targetVertex->Position.y * _factor.y,
                       targetVertex->Position.z * _factor.z);
    }

    else
        return {0, 0, 0};
}

void MeshSurfaceLocator::SetModelPath(std::wstring_view filepath)
{
    _targetModelPath = filepath;
}


void MeshSurfaceLocator::LoadVerticesFromModel(std::shared_ptr<class Model> model)
{
    _targetModel = std::move(model);
    _vertexCountPerMesh.clear();
    _totalVertexCount = 0;
    for (auto& mesh : _targetModel->GetMeshes())
    {
        char* tempvertex;
        UINT  stride;
        UINT  size;
        mesh->GetVertexInfo(tempvertex, stride, size);
        _vertexCountPerMesh.push_back(size);
        _totalVertexCount += size;
    }
}


