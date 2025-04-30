#include "pch.h"
#include "ParticleEmitter.h"

void EmitLocator::RandomInitialize() 
{
    _randomGenerator = std::mt19937(_randomizer());
    _randomRange     = std::uniform_real_distribution<float>(-1.f, 1.f);
    _randomVal       = std::bind(_randomRange, _randomGenerator);
}

DirectX::SimpleMath::Vector3 SphereLocator::EmitLocate()
{
  
    Vector3 location = {_randomVal(), _randomVal(), _randomVal()};
    while (location.Length() > 1)
    {
        location = {_randomVal(), _randomVal(), _randomVal()};
    }
    return {location.x * _factor.x, location.y * _factor.y, location.z * _factor.z};
}

DirectX::SimpleMath::Vector3 CubeLocator::EmitLocate() 
{
    return {_randomVal() * _factor.x, _randomVal() * _factor.y, _randomVal() * _factor.z};
}

DirectX::SimpleMath::Vector3 CylinderLocator::EmitLocate() 
{
    Vector2 location = {_randomVal(), _randomVal()};
    while (location.Length() > 1)
    {
        location = {_randomVal(), _randomVal()};
    }
    return {location.x * _factor.x, _randomVal() * _factor.y / 2, location.y * _factor.z};
}

DirectX::SimpleMath::Vector3 ConeLocator::EmitLocate() 
{
    float   locationY     = (_randomVal() + 2 / 2) * _factor.y;
    float   sectionRadius = locationY * std::tan(_factor.x);
    Vector3 location      = Vector3(_randomVal(), 0, _randomVal());
    while (location.Length() > 1)
    {
        location = {_randomVal(), 0, _randomVal()};
    }
    location *= sectionRadius;
    location.y = locationY;
    return location;
}

DirectX::SimpleMath::Vector3 TorusLocator::EmitLocate() 
{
    Vector3 location = Vector3(_randomVal(), 0, _randomVal()) * _factor.x;
    while (location.Length() > _factor.x || location.Length() < _factor.z)
    {
        location = Vector3(_randomVal(), 0, _randomVal()) * _factor.x;
    }
    float length = location.Length();
    float range  = std::sqrtf(_factor.x * (_factor.x - 2 * _factor.z) - length * (length - 2 * _factor.z));
    location.y   = _randomVal() * range * _factor.y;
    return location;


}

DirectX::SimpleMath::Vector3 MeshSurfaceLocator::EmitLocate() 
{
    Vector3 offset = {_randomVal() * _factor.x, _randomVal() * _factor.y, _randomVal() * _factor.z};
    if (false == _vertices.empty())
        return _vertices[static_cast<UINT>(_randomVal() * (_vertices.size() - 1))] + offset;

    else
        return {0, 0, 0};
}

void ParticleEmitter::Initialize(SIZE_T maxParticles /*= 100000*/, float emissionRate /*= 500.f*/,
                                 float         emitterLifetime /*= 5.f */,
                                 LocationShape locatorShape /*= LocationShape::SPHERE*/,
                                 Vector3       locationFactor /* = Vector3(1, 1, 1)*/)
{
    _maxParticles = maxParticles;
    _emissionRate = emissionRate;


}

void ParticleEmitter::Update(float deltaTime) {}

void ParticleEmitter::SetLocatorFactor(const Vector3& factor) 
{
    _emitLocator->SetFactor(factor);
}

void ParticleEmitter::InitializeLocator(LocationShape locatorShape , Vector3 factor) 
{
    switch (locatorShape)
    {
    case LocationShape::SPHERE:
        _emitLocator = new SphereLocator();
        break;
    case LocationShape::CUBE:
        _emitLocator = new CubeLocator();
        break;
    case LocationShape::CYLINDER:
        _emitLocator = new CylinderLocator();
        break;
    case LocationShape::CONE:
        _emitLocator = new ConeLocator();
        break;
    case LocationShape::TORUS:
        _emitLocator = new TorusLocator();
        break;
    case LocationShape::MESH_SURFACE:
        _emitLocator = new MeshSurfaceLocator();
        break;

    }
    _emitLocator->SetFactor(factor);
    _emitLocator->RandomInitialize();
}