#include "pch.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Model.h"
/////////////////////////////////////////////////
void EmitLocator::RandomInitialize() 
{
    _randomGenerator = std::mt19937(_randomizer());
    _randomRange     = std::uniform_real_distribution<float>(-1.f, 1.f);
    _randomVal       = std::bind(_randomRange, _randomGenerator);
}

 MeshSurfaceLocator::~MeshSurfaceLocator() 
 {
     _targetModel = nullptr;
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
    float   locationY     = (_randomVal() + 1) / 2 * _factor.y;
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
    if (_factor.x <= _factor.z)
    {
        _factor.z = _factor.x - 0.1f;
    }
    // 랜덤 값을 [0,1] 범위로 매핑
    auto mapTo01 = [&]() { return (_randomVal() + 1.0f) * 0.5f; };

    // 각도는 균등 분포 [0, 2π]
    float angle = 2.0f * XM_PI * mapTo01();


    float radius = _randomVal();
    radius *= (_factor.x - _factor.z) / 2;
    radius += (_factor.x + _factor.z) / 2;


    float height = _randomVal();
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
    do
    {
        direction = {_randomVal(), _randomVal(), _randomVal()};
    } while (direction.LengthSquared() > 1.0f);
    direction.Normalize();

    // 균등한 반지름 분포 (r³이 균등)
    float u      = (_randomVal() + 1.0f) * 0.5f; // [0,1] 범위로 변환
    float radius = pow(u, 1.0f / 3.0f);

    Vector3 location = direction * radius;
    return {location.x * _factor.x, location.y * _factor.y, location.z * _factor.z};
}

DirectX::SimpleMath::Vector3 MeshSurfaceLocator::EmitLocate() 
{
    if (nullptr != _targetModel)
    {
        float temp  = ((_randomVal() + 1) * 0.5f);
        UINT index = static_cast<UINT>(temp* (_totalVertexCount-1));
        UINT  tempIdx = 0;
        int   meshIdx = 0;
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
        
        Vertex* targetVertex = reinterpret_cast<Vertex*>(vertices);
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

void SpriteModule::Initialize()
{
    
}

void RibbonModule::Initialize() 
{
}


 SpriteModule::~SpriteModule() 
 {
     _albedoTexture = nullptr;
 }

void SpriteModule::SetFrameInfo(Vector4 frameInfo) 
{
    _initialFrameInfo = frameInfo;
    CalculateFrameInfos();
}

void SpriteModule::SetFrameInfo(int widthCount, int heightCount, int startIndex, int totalCount) 
{
    _initialFrameInfo =
        Vector4((float)widthCount, (float)heightCount, (float)startIndex, static_cast<float>(totalCount));
    CalculateFrameInfos();
}


DirectX::SimpleMath::Vector4 SpriteModule::GetInitialFrameInfo() const 
{
    return _initialFrameInfo;
}

Texture* SpriteModule::GetNormalTexture() const 
{
    return _normalTexture.get();
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


RibbonModule::~RibbonModule() 
{
    _albedoTexture = nullptr;
}

void SpriteModule::SetAlbedoTexture(std::shared_ptr<class Texture> texture) 
{
    _albedoTexture = std::move(texture);
}

void RibbonModule::SetAlbedoTexture(std::shared_ptr<class Texture> texture) 
{
    _albedoTexture = std::move(texture);
}

void SpriteModule::ChangeAlbedoTexture(std::wstring_view filePath)
{
    _isAlbedoTextureChanged = true;
    _modelAndTexturePath = filePath;
}


void RibbonModule::ChangeAlbedoTexture(std::wstring_view filePath)
{
    _isAlbedoTextureChanged = true;
    _newAlbedoTexturePath   = filePath;
}

Texture* SpriteModule::GetAlbedoTexture() const
{
    return _albedoTexture.get();
}

Texture* RibbonModule::GetAlbedoTexture() const 
{
    return _albedoTexture.get();
}



/////////////////////////////////////////////////





 ParticleEmitter::~ParticleEmitter() 
 {
     // 1. Particle 객체들 정리
     for (auto particle : _particlePool)
     {
         delete particle;
     }
     _particlePool.clear();

     // 2. EmitLocator 객체 정리
     delete _emitLocator;
     _emitLocator = nullptr;

     // 3. ParticleRenderModule 객체 정리
     delete _particleRenderModule;
     _particleRenderModule = nullptr;

     // 4. 기타 컨테이너 정리 (안전성을 위해)
     while (!_inactiveParticleIndices.empty())
     {
         _inactiveParticleIndices.pop();
     }
 }

 ParticleEmitter::ParticleEmitter(const ParticleEmitter& other) 
         : _particleType(other._particleType), _locationType(other._locationType), _velocityType(other._velocityType),
       _emitterRotationQ(other._emitterRotationQ), _emitterRotationE(other._emitterRotationE),_emitterPosition(other._emitterPosition)
 {
     _emitterLifetime           =other.GetEmitterLifetime();
     _maxParticles              =other.GetMaxParticles();
     _emissionRate              =other.GetEmissionRate();
     _startDelay                =other.GetStartDelay();
     _spawnBurstFlag            =other.GetSpawnBurstFlag();
     _spawnBurstCount           =other.GetSpawnBurstCount();
     _emitterName               =other.GetEmitterName();
     _velocityFactor            =other.GetVelocityFactor();
     _startColor                =other.GetStartColor();
     _startOpacity              =other.GetStartOpacity();
     _endColor                  =other.GetEndColor();
     _endOpacity                =other.GetEndOpacity();
     _startScale                =other.GetStartScale();
     _endScale                  =other.GetEndScale();
     _particleLifetime          =other.GetParticleLifetime();
     _particleMass              =other.GetParticleMass();
     _particleDistributionOffset=other.GetParticleDistributionOffset();
     _dragPoint                 =other.GetDragPoint();
     _dragForce                 =other.GetDragForce();



 }
 void ParticleEmitter::SetLocatorFactor(const Vector3& factor) 
{
    _emitLocator->SetFactor(factor);
}

void ParticleEmitter::SetVelocityType(VelocityScaleType velType) 
{
    _velocityType = velType;
}


void ParticleEmitter::Initialize(SIZE_T maxParticles /*= 100000*/, float emissionRate /*= 500.f*/,
                                 float         emitterLifetime /*= 5.f*/,
                                 LocationShape locatorShape /*= LocationShape::SPHERE*/,
                                 Vector3       locationFactor /*= Vector3(1,1,1)*/,
                                  ParticleType  particleType /*= ParticleType::SPRITE*/,
                                  std::wstring_view  meshspritePath /*= L""*/)
 {
    _emitterLifetime = emitterLifetime;
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

    _particleRenderModule->SetModelAndTexturePath(meshspritePath);
    //_particleRenderModule->Initialize();


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


void ParticleEmitter::Update(float deltaTime) 
{
    if (false == _delayFlag)
    {

        _delayTimer += deltaTime;
        if (_delayTimer >= _startDelay)
        {
            _activeFlag = true;
            _delayFlag  = true;
        }
        else
        {
            _activeFlag = false;
            return;
        }
    }




    _emitterAge += deltaTime;
    if (_emitterAge >= _emitterLifetime-_particleLifetime)
    {
        _endFlag = true;
        //return;
    }
    if (_emitterAge >= _emitterLifetime)
    {
        _emitterAge = 0;
        _activeFlag = false;
        return;
    }

    _translationMatrix = Matrix::CreateTranslation(_emitterPosition);
    _rotationMatrix    = Matrix::CreateFromQuaternion(_emitterRotationQ);
    _worldMatrix       = _rotationMatrix * _translationMatrix * _effectWorldMatrix;
}


void ParticleEmitter::UpdateParticleLifeCycle(float deltaTime) 
{
    // 수명 다한 파티클 비활성화
    for (int i = 0; i < _activeParticleCount; ++i)
    {
        _particlePool[i]->SetAge(_particlePool[i]->GetAge() + deltaTime);
        if (_particlePool[i]->GetAge() >= _particleLifetime)
        {
                _activeParticleCount--;
                std::swap(_particlePool[i], _particlePool[_activeParticleCount]);
                _inactiveParticleIndices.push(_activeParticleCount);
            }
        }
    if (true == _endFlag)
        return;

    // 새 파티클 생성
    size_t newParticles = 0;
    _emissionThreshold += deltaTime * _emissionRate;
    if (true == _spawnBurstFlag && false == _isSpawnBursted)
    {
        _emissionThreshold += _spawnBurstCount;
        _isSpawnBursted = true;
    }
    if (_emissionThreshold >= 1)
    {
        newParticles = static_cast<size_t>(_emissionThreshold);
        _emissionThreshold -= newParticles;
    }
    while (0 < newParticles && false == _inactiveParticleIndices.empty())
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
        AwakeParticle(static_cast<UINT>(index));
        newParticles--;
    }

    if (ParticleType::SPRITE == _particleType)
    {
        SpriteModule* spritemodule = static_cast<SpriteModule*>(_particleRenderModule);
        if (true == spritemodule->GetTextureChangeFlag())
        {
            spritemodule->SetTextureChangeFlag(false);
            spritemodule->SetAlbedoTexture(
                Global::resourceManager->LoadResource<Texture>(spritemodule->GetNewAlbedoTexturePath()));
            Global::particleManager->RefreshEditor();
        }        
    }

    if (ParticleType::RIBBON == _particleType)
    {
        RibbonModule* ribbonmodule = static_cast<RibbonModule*>(_particleRenderModule);
        if (true == ribbonmodule->GetTextureChangeFlag())
        {
            ribbonmodule->SetTextureChangeFlag(false);
            ribbonmodule->SetAlbedoTexture(
                Global::resourceManager->LoadResource<Texture>(ribbonmodule->GetNewAlbedoTexturePath()));
            Global::particleManager->RefreshEditor();

        }
    }


}


void ParticleEmitter::Reset() 
{
    _delayFlag = _activeFlag = false;
    _isSpawnBursted          = false;
    _endFlag                 = false;
    _delayTimer              = 0.f;
    _emitterAge              = 0.f;
    _activeParticleCount     = 0;
    while (false == _inactiveParticleIndices.empty())
    {
        _inactiveParticleIndices.pop();
    }
    for (size_t i = 0; i < _maxParticles; ++i)
    {
        _inactiveParticleIndices.push(i);

    }
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

void ParticleEmitter::AwakeParticle(UINT index) 
{
    Vector3 offset = {_emitLocator->_randomVal(), _emitLocator->_randomVal(), _emitLocator->_randomVal()};

    Vector4 location = {1, 1, 1, 1};
    Vector3 tempPos = _emitLocator->EmitLocate();
    location.x      = tempPos.x + offset.x * _particleDistributionOffset.x;
    location.y      = tempPos.y + offset.y * _particleDistributionOffset.y;
    location.z      = tempPos.z + offset.z * _particleDistributionOffset.z;

    if (_useWorldSpace)
    {
		location = Vector4::Transform(location, _worldMatrix);
	}

    _particlePool[index]->SetPosition(location);
    ScaleVelocity({location.x, location.y, location.z});

    Vector3 finalVelocity = _velocity;
    if (!_useWorldSpace)
    {
        // If simulating in local space, the calculated velocity is also local.
        // Transform it to world space to apply emitter's rotation and scale.
        finalVelocity = Vector3::TransformNormal(_velocity, _worldMatrix);
    }
    _particlePool[index]->SetVelocity(finalVelocity);

    _particlePool[index]->SetAge(0.f);
    _particlePool[index]->SetMass(_particleMass);

    if (ParticleType::SPRITE == _particleType)
    {
        auto    spritemodule = static_cast<SpriteModule*>(_particleRenderModule);
        Vector4 frameInfo    = {spritemodule->GetFrameDuration(), 0, 0, 0};
        _particlePool[index]->SetFrameinfo(frameInfo);
    }

}

void ParticleEmitter::ScaleVelocity(Vector3 pos) 
{
    switch (_velocityType)
    {
    case VelocityScaleType::LINEAR:
        _velocity = _velocityFactor;
        break;
        // scale, radius
    case VelocityScaleType::CONE:

    case VelocityScaleType::POINT:
        ScaleVelFromPoint(pos);
        break;
    case VelocityScaleType::CUSTOM:
        _velocity = _velocityScalingFunciton();
        break;
    default:
        _velocity = _velocityFactor;
        break;
    }

}

void ParticleEmitter::ScaleVelFromPoint(Vector3 pos) 
{
    Vector3 emitterCenter;
    if (_useWorldSpace)
    {
        // In world space simulation, 'pos' is the particle's world position.
        // We need the emitter's world position to calculate the direction.
        emitterCenter = Vector3(_worldMatrix._41, _worldMatrix._42, _worldMatrix._43);
    }
    else
    {
        // In local space simulation, 'pos' is the particle's local position.
        // The emitter's center is the origin of the local space.
        emitterCenter = Vector3::Zero;
    }

    Vector3 direction = pos - emitterCenter;
    direction.Normalize();
    _velocity = direction * _velocityFactor.x;
}

void ParticleEmitter::ScaleVelInCone(Vector3 pos) 
{



}


