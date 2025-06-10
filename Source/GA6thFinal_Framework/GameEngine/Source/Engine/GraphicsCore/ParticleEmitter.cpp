#include "pch.h"
#include "Quad.h"
#include "Texture.h"
#include "Particle.h"
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
    Vector3 location = Vector3(_randomVal() , 0, _randomVal());
    location *= _factor.x;
    while (location.Length() > _factor.x || location.Length() < _factor.z)
    {
        location = Vector3(_randomVal(), 0, _randomVal());
        location *= _factor.x;
    }
    //float length = location.Length();
    //float range  = std::sqrtf(_factor.x * (_factor.x - 2 * _factor.z) - length * (length - 2 * _factor.z));
    //location.y   = _randomVal() * range * _factor.y;
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

void SpriteModule::SetFrameInfo(Vector4 frameInfo) 
{
    _initialFrameInfo = frameInfo;
    CalculateFrameInfos();
}

void SpriteModule::SetFrameInfo(int widthCount, int heightCount, int startIndex, int totalCount) 
{
    _initialFrameInfo = Vector4(widthCount, heightCount, startIndex, totalCount);
    CalculateFrameInfos();
}

void SpriteModule::LoadAlbedoTexture(std::wstring filePath) 
{
    _albedoTexture = UmResourceManager.LoadResource<Texture>(filePath);
}

void SpriteModule::LoadNormalTexture(std::wstring filePath) 
{
    _albedoTexture = UmResourceManager.LoadResource<Texture>(filePath);
}

DirectX::SimpleMath::Vector4 SpriteModule::GetInitialFrameInfo() const 
{
    return _initialFrameInfo;
}

std::shared_ptr<class Texture> SpriteModule::GetAlbedoTexture() const
{
    return _albedoTexture;
}

std::shared_ptr<Texture> SpriteModule::GetNormalTexture() const 
{
    return _normalTexture;
}

void SpriteModule::CalculateFrameInfos() 
{
    _preCalculatedFrameInfos.clear();
    Vector2 offset = {1.f / _initialFrameInfo.x, 1 / _initialFrameInfo.y};
    for (int i = 0;i < _initialFrameInfo.w;++i)
    {

        Vector4 newFrame = {0, 0, 1, 1};
        UINT    x        = i % (UINT)_initialFrameInfo.x;
        UINT    y        = i / (UINT)_initialFrameInfo.x;

        newFrame.x = x * offset.x;
        newFrame.y = y * offset.y;
        newFrame.z = newFrame.x + offset.x;
        newFrame.w = newFrame.y + offset.y;
        _preCalculatedFrameInfos.push_back(newFrame);
    }
}

void ParticleEmitter::Initialize(SIZE_T maxParticles /*= 100000*/, float emissionRate /*= 500.f*/,
                                 float         emitterLifetime /*= 5.f*/,
                                 LocationShape locatorShape /*= LocationShape::SPHERE*/,
                                 Vector3       locationFactor /*= Vector3(1,1,1)*/,
                                 ParticleType  particleType /*= ParticleType::SPRITE*/)
{
    _particleType = particleType;
    switch (particleType)
    {
    case ParticleType::SPRITE:
        _particleRenderModule = new SpriteModule();
        break;
    case ParticleType::MESH:
        _particleRenderModule = new MeshModule();
        break;
    case ParticleType::RIBBON:
        _particleRenderModule = new RibbonModule();
        break;
    default:
        _particleRenderModule = new SpriteModule();
        break;

    }
    _locationType = locatorShape;
    switch (_locationType)
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
    case LocationShape::TORUS:
        _emitLocator = new TorusLocator();
        break;

    case LocationShape::CONE:
        _emitLocator = new ConeLocator();
        break;
    case LocationShape::MESH_SURFACE:
        _emitLocator = new MeshSurfaceLocator();
        break;
    default:
        _emitLocator = new SphereLocator();
        break;
    }

    _emitLocator->RandomInitialize();
    _emitLocator->SetFactor(locationFactor);
    _maxParticles = maxParticles;
    _emissionRate = emissionRate;
    _particlePool.resize(_maxParticles);
    for (size_t i = 0; i < maxParticles; ++i)
    {
        _particlePool[i] = new Particle();
        _inactiveParticleIndices.push(i);
    }
}


void ParticleEmitter::AwakeParticle(UINT index) 
{
    Vector3 offset = {_emitLocator->_randomVal(), _emitLocator->_randomVal(), _emitLocator->_randomVal()};

    Vector4 location = {1, 1, 1, 1};
    location.x       = _emitLocator->EmitLocate().x + _emitLocator->_randomVal() * _particleDistributionOffset;
    location.y       = _emitLocator->EmitLocate().y + _emitLocator->_randomVal() * _particleDistributionOffset;
    location.z       = _emitLocator->EmitLocate().z + _emitLocator->_randomVal() * _particleDistributionOffset;
    _particlePool[index]->SetPosition(location);
    _particlePool[index]->SetVelocity(_velocity);
    _particlePool[index]->SetStartColor(_startColor);
    _particlePool[index]->SetStartOpacity(_startOpacity);
    _particlePool[index]->SetEndColor(_endColor);
    _particlePool[index]->SetEndOpacity(_endOpacity);
    _particlePool[index]->SetStartScale(_startScale);
    _particlePool[index]->SetEndScale(_endScale);
    _particlePool[index]->SetAge(0.f);
    _particlePool[index]->SetLifetime(_particleLifetime);
    _particlePool[index]->SetMass(_particleMass);
    auto spritemodule = static_cast<SpriteModule*>(_particleRenderModule);
    Vector4 frameInfo    = {spritemodule->GetFrameDuration(), 0, 0, 0};
    _particlePool[index]->SetFrameinfo(frameInfo);

}

void ParticleEmitter::UpdateParticleLifeCycle(float deltaTime) 
{
    // 수명 다한 파티클 비활성화
    for (int i = 0; i < _activeParticleCount; ++i)
    {
        _particlePool[i]->SetAge(_particlePool[i]->GetAge() + deltaTime);
        if (_particlePool[i]->GetAge() >= _particlePool[i]->GetLifetime())
        {
            _activeParticleCount--;
            std::swap(_particlePool[i], _particlePool[_activeParticleCount]);
            _inactiveParticleIndices.push(_activeParticleCount);
        }
    }

    // 새 파티클 생성
    size_t newParticles = 0;
    _emissionThreshold += deltaTime * _emissionRate;
    if (_emissionThreshold >= 1)
    {
        newParticles = static_cast<size_t>(_emissionThreshold);
        _emissionThreshold -= newParticles;
    }
    while (0 < newParticles && !_inactiveParticleIndices.empty())
    {
        size_t index = _inactiveParticleIndices.top();
        _inactiveParticleIndices.pop();
        if (index >= _activeParticleCount)
        {
            // SwapVectors(index, m_activeCount);
            std::swap(_particlePool[index], _particlePool[_activeParticleCount]);
            index = _activeParticleCount;
        }
        _activeParticleCount++;
        AwakeParticle(index);
        newParticles--;
    }
}


void ParticleEmitter::Update(float deltaTime) 
{
    _emitterAge += deltaTime;
    if (_emitterAge >= _emitterLifetime)
    {
        _emitterAge = 0;
        _activeFlag = false;
        return;
    }
    _translationMatrix = Matrix::CreateTranslation(_emitterPosition);
    _rotationMatrix    = Matrix::CreateFromQuaternion(_emitterRotation);
    _worldMatrix       = _rotationMatrix * _translationMatrix * _effectWorldMatrix;
    for (int i = 0; i < _activeParticleCount; ++i)
    {
        // particle update code.
    }
}

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

