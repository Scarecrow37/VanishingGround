#include "pch.h"
#include "ParticleEffectSerializer.h"

 ParticleEffectSerializer::ParticleEffectSerializer() 
 {
     RegisterDeserializers();
 }

void ParticleEffectSerializer::OnFileRegistered(const File::Path& path) {}

void ParticleEffectSerializer::OnFileUnregistered(const File::Path& path) {}

void ParticleEffectSerializer::OnFileModified(const File::Path& path) {}

void ParticleEffectSerializer::OnFileRemoved(const File::Path& path) {}

void ParticleEffectSerializer::OnFileRenamed(const File::Path& oldPath, const File::Path& newPath) {}

void ParticleEffectSerializer::OnFileMoved(const File::Path& oldPath, const File::Path& newPath) {}

void ParticleEffectSerializer::OnRequestedSave() {}

void ParticleEffectSerializer::OnPostRequestedSave() {}

void ParticleEffectSerializer::OnRequestedLoad() {}

void ParticleEffectSerializer::OnPostRequestedLoad() {}

void ParticleEffectSerializer::OnRequestedInspect(const File::Path& path) {}

void ParticleEffectSerializer::OnRequestedOpen(const File::Path& path) {}

void ParticleEffectSerializer::OnRequestedCopy(const File::Path& path) {}

void ParticleEffectSerializer::OnRequestedPaste(const File::Path& path) {}

void ParticleEffectSerializer::RegisterDeserializers()
{
    // 1.0
    {
        _serializers[{1, 0}] = [this](std::ofstream& os, ParticleEffect* effect, File::Path destPath) {
            this->Serialize_1_0(os, effect, destPath);
        };
        _deserializers[{1, 0}] = [this](EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                        std::string_view sceneName) -> ParticleEffect* {
            return this->Deserialize_1_0(id, keyString, is, isEditor, sceneName);
        };
        _preDeserializers[{1, 0}] = [this](std::ifstream& is) { this->PreDeserialize_1_0(is); };
    }
    // 1.1
    {
        _serializers[{1, 1}] = [this](std::ofstream& os, ParticleEffect* effect, File::Path destPath) {
            this->Serialize_1_1(os, effect, destPath);
        };
        _deserializers[{1, 1}] = [this](EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                        std::string_view sceneName) -> ParticleEffect* {
            return this->Deserialize_1_1(id, keyString, is, isEditor, sceneName);
        };
        _preDeserializers[{1, 1}] = [this](std::ifstream& is) { this->PreDeserialize_1_1(is); };
    }

    // 1.2
    //  texture serialize - path -> guid
    {
        _serializers[{1, 2}] = [this](std::ofstream& os, ParticleEffect* effect, File::Path destPath) {
            this->Serialize_1_2(os, effect, destPath);
        };
        _deserializers[{1, 2}] = [this](EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                        std::string_view sceneName) -> ParticleEffect* {
            return this->Deserialize_1_2(id, keyString, is, isEditor, sceneName);
        };
        _preDeserializers[{1, 2}] = [this](std::ifstream& is) { this->PreDeserialize_1_2(is); };
    }

    // 1.3
    // scale by velocity added
    {
        _serializers[{1, 3}] = [this](std::ofstream& os, ParticleEffect* effect, File::Path destPath) {
            this->Serialize_1_3(os, effect, destPath);
        };
        _deserializers[{1, 3}] = [this](EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                        std::string_view sceneName) -> ParticleEffect* {
            return this->Deserialize_1_3(id, keyString, is, isEditor, sceneName);
        };
        _preDeserializers[{1, 3}] = [this](std::ifstream& is) { this->PreDeserialize_1_3(is); };
    }

    // 1.4
    // light prop added
    {
        _serializers[{1, 4}] = [this](std::ofstream& os, ParticleEffect* effect, File::Path destPath) {
            this->Serialize_1_4(os, effect, destPath);
        };
        _deserializers[{1, 4}] = [this](EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                        std::string_view sceneName) -> ParticleEffect* {
            return this->Deserialize_1_4(id, keyString, is, isEditor, sceneName);
        };
        _preDeserializers[{1, 4}] = [this](std::ifstream& is) { this->PreDeserialize_1_4(is); };
    }

        // 1.5
    // billboard off added
    {
        _serializers[{1, 5}] = [this](std::ofstream& os, ParticleEffect* effect, File::Path destPath) {
            this->Serialize_1_5(os, effect, destPath);
        };
        _deserializers[{1, 5}] = [this](EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                        std::string_view sceneName) -> ParticleEffect* {
            return this->Deserialize_1_5(id, keyString, is, isEditor, sceneName);
        };
        _preDeserializers[{1, 5}] = [this](std::ifstream& is) { this->PreDeserialize_1_5(is); };
    }

}

void ParticleEffectSerializer::Serialize(ParticleEffect* effect, File::Path destPath)
{
    std::ofstream os(destPath.string(), std::ios::binary);
    if (false == os.is_open())
    {
        return;
    }
    os.write(MAGIC_NUMBER, 4);
    uint32_t majorVersion = MAJOR_VERSION;
    uint32_t minorVersion = MINOR_VERSION;
    os.write(reinterpret_cast<const char*>(&majorVersion), sizeof(majorVersion));
    os.write(reinterpret_cast<const char*>(&minorVersion), sizeof(minorVersion));
    _serializers[{majorVersion, minorVersion}](os, effect, destPath);
}
ParticleEffect* ParticleEffectSerializer::Deserialize(EffectID id, const std::string& keyString, File::Path filepath,
                                                      bool isEditor, std::string_view sceneName)
{
    std::ifstream is(filepath.string(), std::ios::binary);
    if (!is.is_open())
        return nullptr;
    char magic[4];
    is.read(magic, 4);
    if (4 == is.gcount() && 0 == strncmp(magic, MAGIC_NUMBER, 4))
    {
        uint32_t majorVersion, minorVersion;
        is.read(reinterpret_cast<char*>(&majorVersion), sizeof(majorVersion));
        is.read(reinterpret_cast<char*>(&minorVersion), sizeof(minorVersion));
        auto it = _deserializers.find({majorVersion, minorVersion});
        if (it != _deserializers.end())
        {
            return (*it).second(id, keyString, is, isEditor, sceneName);
        }
    }
    return nullptr;
}
void ParticleEffectSerializer::PreDeserialize(File::Path filepath)
{

    std::ifstream is(filepath.string(), std::ios::binary);
    if (!is.is_open())
        return;
    char magic[4];
    is.read(magic, 4);
    if (4 == is.gcount() && 0 == strncmp(magic, MAGIC_NUMBER, 4))
    {
        uint32_t majorVersion, minorVersion;
        is.read(reinterpret_cast<char*>(&majorVersion), sizeof(majorVersion));
        is.read(reinterpret_cast<char*>(&minorVersion), sizeof(minorVersion));
        auto it = _preDeserializers.find({majorVersion, minorVersion});
        if (it != _preDeserializers.end())
        {
            (*it).second(is);
        }
    }
}

void ParticleEffectSerializer::Serialize_1_0(std::ofstream& os, ParticleEffect* effect, File::Path destPath)
{

    const std::string effectname = effect->GetEffectName();
    uint32_t          nameLen    = static_cast<uint32_t>(effectname.size());
    os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    os.write(effectname.c_str(), nameLen);

    // lifetime
    float lifetime = effect->GetLifetime();
    os.write(reinterpret_cast<const char*>(&lifetime), sizeof(lifetime));

    uint32_t count = static_cast<uint32_t>(effect->GetEmitterList().size());
    os.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (auto& emitter : effect->GetEmitterList())
    {
        // name length, name
        const std::string emittername = emitter->GetEmitterName();
        uint32_t          nameLen     = static_cast<uint32_t>(emittername.size());
        os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        os.write(emittername.c_str(), nameLen);

        // worldspaceflag
        {
            auto temp = emitter->GetUseWorldSpace();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter position
        {
            auto temp = emitter->GetEmitterPosition();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation euler
        {
            auto temp = emitter->GetEmitterRotationE();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation quaternion
        {
            auto temp = emitter->GetEmitterRotationQ();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // location type
        {
            auto temp = emitter->_locationType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            if (LocationShape::MESH_SURFACE == emitter->_locationType)
            {
                auto       meshLocator = emitter->_emitLocator->AsMeshSurfaceLocator();
                File::Path modelPath   = meshLocator->GetModelPath();
                SIZE_T     nameLen     = modelPath.string().size();
                os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
                os.write(modelPath.string().c_str(), nameLen);
            }
        }
        // location factor
        {
            auto temp = emitter->_emitLocator->GetFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity type
        {
            auto temp = emitter->_velocityType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity factor
        {
            auto temp = emitter->GetVelocityFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter lifetime
        {
            float emitterlifetime = emitter->GetEmitterLifetime();
            os.write(reinterpret_cast<const char*>(&emitterlifetime), sizeof(emitterlifetime));
        }

        // particle lifetime
        {
            float particlelifetime = emitter->GetParticleLifetime();
            os.write(reinterpret_cast<const char*>(&particlelifetime), sizeof(particlelifetime));
        }

        // max particles
        {
            float maxParticles = static_cast<float>(emitter->GetMaxParticles());
            os.write(reinterpret_cast<const char*>(&maxParticles), sizeof(maxParticles));
        }

        // emission rate
        {
            float emissionrate = emitter->GetEmissionRate();
            os.write(reinterpret_cast<const char*>(&emissionrate), sizeof(emissionrate));
        }

        // start delay
        {
            float startdelay = emitter->GetStartDelay();
            os.write(reinterpret_cast<const char*>(&startdelay), sizeof(startdelay));
        }

        // spawn burst flag
        {
            float spawnBurst = emitter->GetSpawnBurstFlag();
            os.write(reinterpret_cast<const char*>(&spawnBurst), sizeof(spawnBurst));
        }

        // spawn burst count
        {
            float spawnBurstCount = emitter->GetSpawnBurstCount();
            os.write(reinterpret_cast<const char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        }

        // start color
        {
            Vector3 startcolor = emitter->GetStartColor();
            os.write(reinterpret_cast<const char*>(&startcolor), sizeof(startcolor));
        }

        // start alpha
        {
            float startopacity = emitter->GetStartOpacity();
            os.write(reinterpret_cast<const char*>(&startopacity), sizeof(startopacity));
        }

        // end color
        {
            Vector3 endcolor = emitter->GetEndColor();
            os.write(reinterpret_cast<const char*>(&endcolor), sizeof(endcolor));
        }

        // end alpha
        {
            float endopacity = emitter->GetEndOpacity();
            os.write(reinterpret_cast<const char*>(&endopacity), sizeof(endopacity));
        }

        // start scale
        {
            Vector4 startscale = emitter->GetStartScale();
            os.write(reinterpret_cast<const char*>(&startscale), sizeof(startscale));
        }

        // end scale
        {
            Vector4 endscale = emitter->GetEndScale();
            os.write(reinterpret_cast<const char*>(&endscale), sizeof(endscale));
        }

        // particle mass
        {
            float mass = emitter->GetParticleMass();
            os.write(reinterpret_cast<const char*>(&mass), sizeof(mass));
        }

        // distribution offset
        {
            Vector3 offset = emitter->GetParticleStartDistributionOffset();
            os.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        }

        // drag point
        {
            Vector4 dragpoint = emitter->GetDragPoint();
            os.write(reinterpret_cast<const char*>(&dragpoint), sizeof(dragpoint));
        }

        // drag force
        {
            Vector4 dragforce = emitter->GetDragForce();
            os.write(reinterpret_cast<const char*>(&dragforce), sizeof(dragforce));
        }
        // drag force
        {
            Vector4 vortex = emitter->GetVortexForce();
            os.write(reinterpret_cast<const char*>(&vortex), sizeof(vortex));
        }

        // render type
        {
            auto rendertype = emitter->_particleType;
            os.write(reinterpret_cast<const char*>(&rendertype), sizeof(rendertype));
        }

        // render module file path
        {
            const std::wstring_view modelTexturepath = emitter->_particleRenderModule->GetModelAndTexturePath();
            int                     sizeNeeded =
                WideCharToMultiByte(CP_UTF8, 0, modelTexturepath.data(), static_cast<int>(modelTexturepath.size()),
                                    nullptr, 0, nullptr, nullptr);
            std::string result(sizeNeeded, 0);
            WideCharToMultiByte(CP_UTF8, 0, modelTexturepath.data(), static_cast<int>(modelTexturepath.size()),
                                result.data(), sizeNeeded, nullptr, nullptr);

            uint32_t pathNameLen = static_cast<uint32_t>(result.size());
            os.write(reinterpret_cast<const char*>(&pathNameLen), sizeof(pathNameLen));
            os.write(result.c_str(), pathNameLen);
        }

        if (ParticleType::SPRITE == emitter->_particleType)
        {
            if (auto spriteModule = emitter->_particleRenderModule->AsSprite())
            {
                Vector4 frameinfo = spriteModule->GetFrameInfo();
                os.write(reinterpret_cast<const char*>(&frameinfo), sizeof(frameinfo));
            }
        }
        else if (ParticleType::RIBBON == emitter->_particleType)
        {
            auto    ribbonModule = emitter->_particleRenderModule->AsRibbon();
            Vector4 startNormal  = ribbonModule->GetStartNormal();
            Vector4 endNormal    = ribbonModule->GetEndNormal();
            os.write(reinterpret_cast<const char*>(&startNormal), sizeof(startNormal));
            os.write(reinterpret_cast<const char*>(&endNormal), sizeof(endNormal));
        }
    }
    os.close();
}
ParticleEffect* ParticleEffectSerializer::Deserialize_1_0(EffectID id, const std::string& keyString, std::ifstream& is,
                                                          bool isEditor, std::string_view sceneName)
{
    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    ParticleEffect* newEffect = nullptr;
    if (true == isEditor)
        newEffect = UmParticleManager->RegisterEffectOnEditor();
    else
    {
        auto scenename = std::string(sceneName);
        newEffect      = UmParticleManager->RegisterEffect(id, keyString, scenename);
    }
    newEffect->SetLifetime(lifetime);
    newEffect->SetEffectName(effectname);

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           distributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        bool              useWorldSpace;
        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {
            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
        }
        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&distributionOffset), sizeof(distributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        uint32_t pathNameLen = 0;
        is.read(reinterpret_cast<char*>(&pathNameLen), sizeof(pathNameLen));
        std::string utf8Path(pathNameLen, '\0');
        is.read(&utf8Path[0], pathNameLen);
        int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), nullptr, 0);
        std::wstring modelTexturePath(wideSize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), modelTexturePath.data(),
                            wideSize);

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
        }

        {
            auto emitter = UmParticleManager->RegisterEmitter(
                newEffect, static_cast<SIZE_T>(maxParticles), emissionRate, emitterLifetime, locationType,
                locatorFactor, particleType, modelTexturePath);
            if (isEditor)
            {
                File::Path absolutePath = emitter->_particleRenderModule->GetModelAndTexturePath();
                absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                UmGraphics.LoadTextureResource(std::wstring_view(absolutePath.wstring()), emitter);
                if (LocationShape::MESH_SURFACE == emitter->_locationType)
                {
                    auto       meshLocator  = emitter->_emitLocator->AsMeshSurfaceLocator();
                    File::Path absolutePath = meshLocator->GetModelPath();
                    absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                    UmGraphics.LoadModelResource(std::wstring_view(absolutePath.wstring()), emitter);
                    if (auto meshSurfaceLocator = emitter->_emitLocator->AsMeshSurfaceLocator())
                    {
                        meshSurfaceLocator->SetModelPath(absolutePath.wstring());
                    }
                }
            }
            emitter->SetEmitterName(emitterName);
            emitter->SetUseWorldSpace(useWorldSpace);
            emitter->SetEmitterPosition(emitterPosition);
            emitter->SetEmitterRotationE(emitterRotationE);
            emitter->SetEmitterRotationQ(emitterRotationQ);
            emitter->SetVelocityType(velocityType);
            emitter->SetVelocityFactor(velocityFactor);
            emitter->SetParticleLifetime(particleLifetime);
            emitter->SetStartDelay(startDelay);
            emitter->SetSpawnBurstFlag(spawnBurstFlag);
            emitter->SetSpawnBurstCount(spawnBurstCount);
            emitter->SetStartColor(startColor);
            emitter->SetStartOpacity(startOpacity);
            emitter->SetEndColor(endColor);
            emitter->SetEndOpacity(endOpacity);
            emitter->SetStartScale(startScale);
            emitter->SetEndScale(endScale);
            emitter->SetParticleMass(particleMass);
            emitter->SetParticleStartDistributionOffset(distributionOffset);
            emitter->SetDragPoint(dragPoint);
            emitter->SetDragForce(dragForce);
            emitter->SetVortexForce(vortexForce);
            if (particleType == ParticleType::RIBBON)
            {
                if (auto ribbonModule = emitter->_particleRenderModule->AsRibbon())
                {
                    ribbonModule->SetStartNormal(startNormal);
                    ribbonModule->SetEndNormal(endNormal);
                }
            }
        }
    }

    is.close();
    return newEffect;
}
void ParticleEffectSerializer::PreDeserialize_1_0(std::ifstream& is)
{
    UsedTexturePaths.clear();
    UsedModelPaths.clear();

    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           distributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        bool              useWorldSpace;

        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {
            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
            UsedModelPaths.insert(File::Path(modelPath));
        }

        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&distributionOffset), sizeof(distributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        uint32_t pathNameLen = 0;
        is.read(reinterpret_cast<char*>(&pathNameLen), sizeof(pathNameLen));
        std::string utf8Path(pathNameLen, '\0');
        is.read(&utf8Path[0], pathNameLen);

        int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), nullptr, 0);
        std::wstring modelTexturePath(wideSize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), modelTexturePath.data(),
                            wideSize);

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
        }

        UsedTexturePaths.insert(utf8Path);
    }

    is.close();
}

void ParticleEffectSerializer::Serialize_1_1(std::ofstream& os, ParticleEffect* effect, File::Path destPath)
{

    const std::string effectname = effect->GetEffectName();
    uint32_t          nameLen    = static_cast<uint32_t>(effectname.size());
    os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    os.write(effectname.c_str(), nameLen);

    // lifetime
    float lifetime = effect->GetLifetime();
    os.write(reinterpret_cast<const char*>(&lifetime), sizeof(lifetime));

    uint32_t count = static_cast<uint32_t>(effect->GetEmitterList().size());
    os.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (auto& emitter : effect->GetEmitterList())
    {
        // name length, name
        const std::string emittername = emitter->GetEmitterName();
        uint32_t          nameLen     = static_cast<uint32_t>(emittername.size());
        os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        os.write(emittername.c_str(), nameLen);

        // worldspaceflag
        {
            auto temp = emitter->GetUseWorldSpace();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter position
        {
            auto temp = emitter->GetEmitterPosition();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation euler
        {
            auto temp = emitter->GetEmitterRotationE();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation quaternion
        {
            auto temp = emitter->GetEmitterRotationQ();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // location type
        {
            auto temp = emitter->_locationType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            if (LocationShape::MESH_SURFACE == emitter->_locationType)
            {
                auto       meshLocator = emitter->_emitLocator->AsMeshSurfaceLocator();
                File::Path modelPath   = meshLocator->GetModelPath();
                SIZE_T     nameLen     = modelPath.string().size();
                os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
                os.write(modelPath.string().c_str(), nameLen);
            }
        }
        // location factor
        {
            auto temp = emitter->_emitLocator->GetFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity type
        {
            auto temp = emitter->_velocityType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity factor
        {
            auto temp = emitter->GetVelocityFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter lifetime
        {
            float emitterlifetime = emitter->GetEmitterLifetime();
            os.write(reinterpret_cast<const char*>(&emitterlifetime), sizeof(emitterlifetime));
        }

        // particle lifetime
        {
            float particlelifetime = emitter->GetParticleLifetime();
            os.write(reinterpret_cast<const char*>(&particlelifetime), sizeof(particlelifetime));
        }

        // max particles
        {
            float maxParticles = static_cast<float>(emitter->GetMaxParticles());
            os.write(reinterpret_cast<const char*>(&maxParticles), sizeof(maxParticles));
        }

        // emission rate
        {
            float emissionrate = emitter->GetEmissionRate();
            os.write(reinterpret_cast<const char*>(&emissionrate), sizeof(emissionrate));
        }

        // start delay
        {
            float startdelay = emitter->GetStartDelay();
            os.write(reinterpret_cast<const char*>(&startdelay), sizeof(startdelay));
        }

        // spawn burst flag
        {
            float spawnBurst = emitter->GetSpawnBurstFlag();
            os.write(reinterpret_cast<const char*>(&spawnBurst), sizeof(spawnBurst));
        }

        // spawn burst count
        {
            float spawnBurstCount = emitter->GetSpawnBurstCount();
            os.write(reinterpret_cast<const char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        }

        // start color
        {
            Vector3 startcolor = emitter->GetStartColor();
            os.write(reinterpret_cast<const char*>(&startcolor), sizeof(startcolor));
        }

        // start alpha
        {
            float startopacity = emitter->GetStartOpacity();
            os.write(reinterpret_cast<const char*>(&startopacity), sizeof(startopacity));
        }

        // end color
        {
            Vector3 endcolor = emitter->GetEndColor();
            os.write(reinterpret_cast<const char*>(&endcolor), sizeof(endcolor));
        }

        // end alpha
        {
            float endopacity = emitter->GetEndOpacity();
            os.write(reinterpret_cast<const char*>(&endopacity), sizeof(endopacity));
        }

        // start scale
        {
            Vector4 startscale = emitter->GetStartScale();
            os.write(reinterpret_cast<const char*>(&startscale), sizeof(startscale));
        }

        // end scale
        {
            Vector4 endscale = emitter->GetEndScale();
            os.write(reinterpret_cast<const char*>(&endscale), sizeof(endscale));
        }

        // particle mass
        {
            float mass = emitter->GetParticleMass();
            os.write(reinterpret_cast<const char*>(&mass), sizeof(mass));
        }

        // distribution offset
        {
            Vector3 offset = emitter->GetParticleStartDistributionOffset();
            os.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        }

        // drag point
        {
            Vector4 dragpoint = emitter->GetDragPoint();
            os.write(reinterpret_cast<const char*>(&dragpoint), sizeof(dragpoint));
        }

        // drag force
        {
            Vector4 dragforce = emitter->GetDragForce();
            os.write(reinterpret_cast<const char*>(&dragforce), sizeof(dragforce));
        }
        // drag force
        {
            Vector4 vortex = emitter->GetVortexForce();
            os.write(reinterpret_cast<const char*>(&vortex), sizeof(vortex));
        }

        // render type
        {
            auto rendertype = emitter->_particleType;
            os.write(reinterpret_cast<const char*>(&rendertype), sizeof(rendertype));
        }

        // render module file path
        {
            const std::wstring_view modelTexturepath = emitter->_particleRenderModule->GetModelAndTexturePath();
            int                     sizeNeeded =
                WideCharToMultiByte(CP_UTF8, 0, modelTexturepath.data(), static_cast<int>(modelTexturepath.size()),
                                    nullptr, 0, nullptr, nullptr);
            std::string result(sizeNeeded, 0);
            WideCharToMultiByte(CP_UTF8, 0, modelTexturepath.data(), static_cast<int>(modelTexturepath.size()),
                                result.data(), sizeNeeded, nullptr, nullptr);

            uint32_t pathNameLen = static_cast<uint32_t>(result.size());
            os.write(reinterpret_cast<const char*>(&pathNameLen), sizeof(pathNameLen));
            os.write(result.c_str(), pathNameLen);
        }

        if (ParticleType::SPRITE == emitter->_particleType)
        {
            if (auto spriteModule = emitter->_particleRenderModule->AsSprite())
            {
                Vector4 frameinfo = spriteModule->GetFrameInfo();
                os.write(reinterpret_cast<const char*>(&frameinfo), sizeof(frameinfo));
            }
        }
        else if (ParticleType::RIBBON == emitter->_particleType)
        {
            if (auto ribbonModule = emitter->_particleRenderModule->AsRibbon())
            {
                Vector4 startNormal  = ribbonModule->GetStartNormal();
                Vector4 endNormal    = ribbonModule->GetEndNormal();
                Vector4 ribbonVector = ribbonModule->GetRibbonVector();
                os.write(reinterpret_cast<const char*>(&startNormal), sizeof(startNormal));
                os.write(reinterpret_cast<const char*>(&endNormal), sizeof(endNormal));
                os.write(reinterpret_cast<const char*>(&ribbonVector), sizeof(ribbonVector));
            }
        }
    }
    os.close();
}
ParticleEffect* ParticleEffectSerializer::Deserialize_1_1(EffectID id, const std::string& keyString, std::ifstream& is,
                                                          bool isEditor, std::string_view sceneName)
{
    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    ParticleEffect* newEffect = nullptr;
    if (true == isEditor)
        newEffect = UmParticleManager->RegisterEffectOnEditor();
    else
    {
        auto scenename = std::string(sceneName);
        newEffect      = UmParticleManager->RegisterEffect(id, keyString, scenename);
    }
    newEffect->SetLifetime(lifetime);
    newEffect->SetEffectName(effectname);

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           distributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        Vector4           ribbonvector;
        bool              useWorldSpace;
        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {

            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
        }

        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&distributionOffset), sizeof(distributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        uint32_t pathNameLen = 0;
        is.read(reinterpret_cast<char*>(&pathNameLen), sizeof(pathNameLen));
        std::string utf8Path(pathNameLen, '\0');
        is.read(&utf8Path[0], pathNameLen);
        int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), nullptr, 0);
        std::wstring modelTexturePath(wideSize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), modelTexturePath.data(),
                            wideSize);

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
            is.read(reinterpret_cast<char*>(&ribbonvector), sizeof(ribbonvector));
        }

        {
            auto emitter = UmParticleManager->RegisterEmitter(
                newEffect, static_cast<SIZE_T>(maxParticles), emissionRate, emitterLifetime, locationType,
                                                              locatorFactor, particleType, modelTexturePath);
            {
                File::Path absolutePath = emitter->_particleRenderModule->GetModelAndTexturePath();
                absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                UmGraphics.LoadTextureResource(std::wstring_view(absolutePath.wstring()), emitter);
                if (LocationShape::MESH_SURFACE == emitter->_locationType)
                {
                    File::Path absolutePath = modelPath;
                    absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                    UmGraphics.LoadModelResource(std::wstring_view(absolutePath.wstring()), emitter);
                    if (auto meshSurfaceLocator = emitter->_emitLocator->AsMeshSurfaceLocator())
                    {
                        meshSurfaceLocator->SetModelPath(absolutePath.wstring());
                    }
                }
            }
            emitter->SetEmitterName(emitterName);
            emitter->SetUseWorldSpace(useWorldSpace);
            emitter->SetEmitterPosition(emitterPosition);
            emitter->SetEmitterRotationE(emitterRotationE);
            emitter->SetEmitterRotationQ(emitterRotationQ);
            emitter->SetVelocityType(velocityType);
            emitter->SetVelocityFactor(velocityFactor);
            emitter->SetParticleLifetime(particleLifetime);
            emitter->SetStartDelay(startDelay);
            emitter->SetSpawnBurstFlag(spawnBurstFlag);
            emitter->SetSpawnBurstCount(spawnBurstCount);
            emitter->SetStartColor(startColor);
            emitter->SetStartOpacity(startOpacity);
            emitter->SetEndColor(endColor);
            emitter->SetEndOpacity(endOpacity);
            emitter->SetStartScale(startScale);
            emitter->SetEndScale(endScale);
            emitter->SetParticleMass(particleMass);
            emitter->SetParticleStartDistributionOffset(distributionOffset);
            emitter->SetDragPoint(dragPoint);
            emitter->SetDragForce(dragForce);
            emitter->SetVortexForce(vortexForce);
            if (particleType == ParticleType::RIBBON)
            {
                if (auto ribbonModule = emitter->_particleRenderModule->AsRibbon())
                {
                    ribbonModule->SetStartNormal(startNormal);
                    ribbonModule->SetEndNormal(endNormal);
                    ribbonModule->SetRibbonVector(ribbonvector);
                }
            }
            else
            {
                if (auto spriteModule = emitter->_particleRenderModule->AsSprite())
                {
                    spriteModule->SetFrameInfo(frameInfo);
                    spriteModule->CalculateFrameInfos();
                }
            }
        }
    }

    is.close();
    return newEffect;
}
void ParticleEffectSerializer::PreDeserialize_1_1(std::ifstream& is)
{
    UsedTexturePaths.clear();
    UsedModelPaths.clear();

    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           distributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        Vector4           ribbonvector;
        bool              useWorldSpace;

        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {
            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
            UsedModelPaths.insert(File::Path(modelPath));
        }

        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&distributionOffset), sizeof(distributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        uint32_t pathNameLen = 0;
        is.read(reinterpret_cast<char*>(&pathNameLen), sizeof(pathNameLen));
        std::string utf8Path(pathNameLen, '\0');
        is.read(&utf8Path[0], pathNameLen);

        int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), nullptr, 0);
        std::wstring modelTexturePath(wideSize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), modelTexturePath.data(),
                            wideSize);

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
            is.read(reinterpret_cast<char*>(&ribbonvector), sizeof(ribbonvector));
        }

        UsedTexturePaths.insert(utf8Path);
    }

    is.close();
}

void ParticleEffectSerializer::Serialize_1_2(std::ofstream& os, ParticleEffect* effect, File::Path destPath)
{

    const std::string effectname = effect->GetEffectName();
    uint32_t          nameLen    = static_cast<uint32_t>(effectname.size());
    os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    os.write(effectname.c_str(), nameLen);

    // lifetime
    float lifetime = effect->GetLifetime();
    os.write(reinterpret_cast<const char*>(&lifetime), sizeof(lifetime));

    uint32_t count = static_cast<uint32_t>(effect->GetEmitterList().size());
    os.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (auto& emitter : effect->GetEmitterList())
    {
        // name length, name
        const std::string emittername = emitter->GetEmitterName();
        uint32_t          nameLen     = static_cast<uint32_t>(emittername.size());
        os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        os.write(emittername.c_str(), nameLen);

        // worldspaceflag
        {
            auto temp = emitter->GetUseWorldSpace();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter position
        {
            auto temp = emitter->GetEmitterPosition();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation euler
        {
            auto temp = emitter->GetEmitterRotationE();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation quaternion
        {
            auto temp = emitter->GetEmitterRotationQ();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // location type
        {
            auto temp = emitter->_locationType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            if (LocationShape::MESH_SURFACE == emitter->_locationType)
            {
                auto       meshLocator = emitter->_emitLocator->AsMeshSurfaceLocator();
                File::Path path        = meshLocator->GetModelPath();
                path                   = std::filesystem::absolute(path).generic_string();
                const File::Guid&  guid       = path.ToGuid();
                std::string guidstring = guid.string();

                SIZE_T nameLen = guidstring.length();
                os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
                os.write(guidstring.c_str(), nameLen);
            }
        }
        // location factor
        {
            auto temp = emitter->_emitLocator->GetFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity type
        {
            auto temp = emitter->_velocityType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity factor
        {
            auto temp = emitter->GetVelocityFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter lifetime
        {
            float emitterlifetime = emitter->GetEmitterLifetime();
            os.write(reinterpret_cast<const char*>(&emitterlifetime), sizeof(emitterlifetime));
        }

        // particle lifetime
        {
            float particlelifetime = emitter->GetParticleLifetime();
            os.write(reinterpret_cast<const char*>(&particlelifetime), sizeof(particlelifetime));
        }

        // max particles
        {
            float maxParticles = static_cast<float>(emitter->GetMaxParticles());
            os.write(reinterpret_cast<const char*>(&maxParticles), sizeof(maxParticles));
        }

        // emission rate
        {
            float emissionrate = emitter->GetEmissionRate();
            os.write(reinterpret_cast<const char*>(&emissionrate), sizeof(emissionrate));
        }

        // start delay
        {
            float startdelay = emitter->GetStartDelay();
            os.write(reinterpret_cast<const char*>(&startdelay), sizeof(startdelay));
        }

        // spawn burst flag
        {
            float spawnBurst = emitter->GetSpawnBurstFlag();
            os.write(reinterpret_cast<const char*>(&spawnBurst), sizeof(spawnBurst));
        }

        // spawn burst count
        {
            float spawnBurstCount = emitter->GetSpawnBurstCount();
            os.write(reinterpret_cast<const char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        }

        // start color
        {
            Vector3 startcolor = emitter->GetStartColor();
            os.write(reinterpret_cast<const char*>(&startcolor), sizeof(startcolor));
        }

        // start alpha
        {
            float startopacity = emitter->GetStartOpacity();
            os.write(reinterpret_cast<const char*>(&startopacity), sizeof(startopacity));
        }

        // end color
        {
            Vector3 endcolor = emitter->GetEndColor();
            os.write(reinterpret_cast<const char*>(&endcolor), sizeof(endcolor));
        }

        // end alpha
        {
            float endopacity = emitter->GetEndOpacity();
            os.write(reinterpret_cast<const char*>(&endopacity), sizeof(endopacity));
        }

        // start scale
        {
            Vector4 startscale = emitter->GetStartScale();
            os.write(reinterpret_cast<const char*>(&startscale), sizeof(startscale));
        }

        // end scale
        {
            Vector4 endscale = emitter->GetEndScale();
            os.write(reinterpret_cast<const char*>(&endscale), sizeof(endscale));
        }

        // particle mass
        {
            float mass = emitter->GetParticleMass();
            os.write(reinterpret_cast<const char*>(&mass), sizeof(mass));
        }

        // distribution offset
        {
            Vector3 offset = emitter->GetParticleStartDistributionOffset();
            os.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        }

        // drag point
        {
            Vector4 dragpoint = emitter->GetDragPoint();
            os.write(reinterpret_cast<const char*>(&dragpoint), sizeof(dragpoint));
        }

        // drag force
        {
            Vector4 dragforce = emitter->GetDragForce();
            os.write(reinterpret_cast<const char*>(&dragforce), sizeof(dragforce));
        }
        // drag force
        {
            Vector4 vortex = emitter->GetVortexForce();
            os.write(reinterpret_cast<const char*>(&vortex), sizeof(vortex));
        }

        // render type
        {
            auto rendertype = emitter->_particleType;
            os.write(reinterpret_cast<const char*>(&rendertype), sizeof(rendertype));
        }

        // render module file path
        {
            File::Path modelTexturepath = emitter->_particleRenderModule->GetModelAndTexturePath();
            modelTexturepath            = std::filesystem::absolute(modelTexturepath).generic_string();
            const File::Guid&  guid            = modelTexturepath.ToGuid();
            std::string guidstring      = guid.string();
            SIZE_T      size            = guidstring.length();
            os.write(reinterpret_cast<const char*>(&size), sizeof(size));
            os.write(guidstring.c_str(), size);
        }

        if (ParticleType::SPRITE == emitter->_particleType)
        {
            if (auto spriteModule = emitter->_particleRenderModule->AsSprite())
            {
                Vector4 frameinfo = spriteModule->GetFrameInfo();
                os.write(reinterpret_cast<const char*>(&frameinfo), sizeof(frameinfo));
            }
        }
        else if (ParticleType::RIBBON == emitter->_particleType)
        {
            if (auto ribbonModule = emitter->_particleRenderModule->AsRibbon())
            {
                Vector4 startNormal  = ribbonModule->GetStartNormal();
                Vector4 endNormal    = ribbonModule->GetEndNormal();
                Vector4 ribbonVector = ribbonModule->GetRibbonVector();
                os.write(reinterpret_cast<const char*>(&startNormal), sizeof(startNormal));
                os.write(reinterpret_cast<const char*>(&endNormal), sizeof(endNormal));
                os.write(reinterpret_cast<const char*>(&ribbonVector), sizeof(ribbonVector));
            }
        }
    }
    os.close();
}
ParticleEffect* ParticleEffectSerializer::Deserialize_1_2(EffectID id, const std::string& keyString, std::ifstream& is,
                                                          bool isEditor, std::string_view sceneName)
{
    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    ParticleEffect* newEffect = nullptr;
    if (true == isEditor)
        newEffect = UmParticleManager->RegisterEffectOnEditor();
    else
    {
        auto scenename = std::string(sceneName);
        newEffect      = UmParticleManager->RegisterEffect(id, keyString, scenename);
    }
    newEffect->SetLifetime(lifetime);
    newEffect->SetEffectName(effectname);

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           distributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        Vector4           ribbonvector;
        bool              useWorldSpace;
        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {

            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
        }

        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&distributionOffset), sizeof(distributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        SIZE_T size = 0;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::string texturepath(size, '\0');
        is.read(&texturepath[0], size);
        const File::Guid& guid             = texturepath;
        File::Path modelTexturePath = guid.ToPath();

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
            is.read(reinterpret_cast<char*>(&ribbonvector), sizeof(ribbonvector));
        }

        {
            auto emitter = UmParticleManager->RegisterEmitter(newEffect, static_cast<SIZE_T>(maxParticles),
                                                              emissionRate, emitterLifetime, locationType,
                                                              locatorFactor, particleType, modelTexturePath.wstring());
            {
                File::Path absolutePath = emitter->_particleRenderModule->GetModelAndTexturePath();
                absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                UmGraphics.LoadTextureResource(std::wstring_view(absolutePath.wstring()), emitter);
                if (LocationShape::MESH_SURFACE == emitter->_locationType)
                {
                    const File::Guid& guid         = modelPath;
                    File::Path absolutePath = guid.ToPath();
                    absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                    UmGraphics.LoadModelResource(std::wstring_view(absolutePath.wstring()), emitter);
                    if (auto meshSurfaceLocator = emitter->_emitLocator->AsMeshSurfaceLocator())
                    {
                        meshSurfaceLocator->SetModelPath(absolutePath.wstring());
                    }
                }
            }
            emitter->SetEmitterName(emitterName);
            emitter->SetUseWorldSpace(useWorldSpace);
            emitter->SetEmitterPosition(emitterPosition);
            emitter->SetEmitterRotationE(emitterRotationE);
            emitter->SetEmitterRotationQ(emitterRotationQ);
            emitter->SetVelocityType(velocityType);
            emitter->SetVelocityFactor(velocityFactor);
            emitter->SetParticleLifetime(particleLifetime);
            emitter->SetStartDelay(startDelay);
            emitter->SetSpawnBurstFlag(spawnBurstFlag);
            emitter->SetSpawnBurstCount(spawnBurstCount);
            emitter->SetStartColor(startColor);
            emitter->SetStartOpacity(startOpacity);
            emitter->SetEndColor(endColor);
            emitter->SetEndOpacity(endOpacity);
            emitter->SetStartScale(startScale);
            emitter->SetEndScale(endScale);
            emitter->SetParticleMass(particleMass);
            emitter->SetParticleStartDistributionOffset(distributionOffset);
            emitter->SetDragPoint(dragPoint);
            emitter->SetDragForce(dragForce);
            emitter->SetVortexForce(vortexForce);
            if (particleType == ParticleType::RIBBON)
            {
                if (auto ribbonModule = emitter->_particleRenderModule->AsRibbon())
                {
                    ribbonModule->SetStartNormal(startNormal);
                    ribbonModule->SetEndNormal(endNormal);
                    ribbonModule->SetRibbonVector(ribbonvector);
                }
            }
            else
            {
                if (auto spriteModule = emitter->_particleRenderModule->AsSprite())
                {
                    spriteModule->SetFrameInfo(frameInfo);
                    spriteModule->CalculateFrameInfos();
                }
            }
        }
    }

    is.close();
    return newEffect;
}
void ParticleEffectSerializer::PreDeserialize_1_2(std::ifstream& is)
{
    UsedTexturePaths.clear();
    UsedModelPaths.clear();

    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           distributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        Vector4           ribbonvector;
        bool              useWorldSpace;

        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {
            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
            const File::Guid& guid = modelPath;
            File::Path path = guid.ToPath();
            UsedModelPaths.insert(path);
        }

        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&distributionOffset), sizeof(distributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        SIZE_T size = 0;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::string texturepath = std::string(size, '\0');
        is.read(&texturepath[0], size);
        const File::Guid& guid = texturepath;
        File::Path path = guid.ToPath();

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
            is.read(reinterpret_cast<char*>(&ribbonvector), sizeof(ribbonvector));
        }

        UsedTexturePaths.insert(path);
    }

    is.close();
}

void ParticleEffectSerializer::Serialize_1_3(std::ofstream& os, ParticleEffect* effect, File::Path destPath)
{

    const std::string effectname = effect->GetEffectName();
    uint32_t          nameLen    = static_cast<uint32_t>(effectname.size());
    os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    os.write(effectname.c_str(), nameLen);

    // lifetime
    float lifetime = effect->GetLifetime();
    os.write(reinterpret_cast<const char*>(&lifetime), sizeof(lifetime));

    uint32_t count = static_cast<uint32_t>(effect->GetEmitterList().size());
    os.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (auto& emitter : effect->GetEmitterList())
    {
        // name length, name
        const std::string emittername = emitter->GetEmitterName();
        uint32_t          nameLen     = static_cast<uint32_t>(emittername.size());
        os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        os.write(emittername.c_str(), nameLen);

        // worldspaceflag
        {
            auto temp = emitter->GetUseWorldSpace();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter position
        {
            auto temp = emitter->GetEmitterPosition();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation euler
        {
            auto temp = emitter->GetEmitterRotationE();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation quaternion
        {
            auto temp = emitter->GetEmitterRotationQ();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // location type
        {
            auto temp = emitter->_locationType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            if (LocationShape::MESH_SURFACE == emitter->_locationType)
            {
                auto       meshLocator = emitter->_emitLocator->AsMeshSurfaceLocator();
                File::Path path        = meshLocator->GetModelPath();
                path                   = std::filesystem::absolute(path).generic_string();
                const File::Guid&  guid       = path.ToGuid();
                std::string guidstring = guid.string();

                SIZE_T nameLen = guidstring.length();
                os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
                os.write(guidstring.c_str(), nameLen);
            }
        }
        // location factor
        {
            auto temp = emitter->_emitLocator->GetFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity type
        {
            auto temp = emitter->_velocityType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity factor
        {
            auto temp = emitter->GetVelocityFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter lifetime
        {
            float emitterlifetime = emitter->GetEmitterLifetime();
            os.write(reinterpret_cast<const char*>(&emitterlifetime), sizeof(emitterlifetime));
        }

        // particle lifetime
        {
            float particlelifetime = emitter->GetParticleLifetime();
            os.write(reinterpret_cast<const char*>(&particlelifetime), sizeof(particlelifetime));
        }

        // max particles
        {
            float maxParticles = static_cast<float>(emitter->GetMaxParticles());
            os.write(reinterpret_cast<const char*>(&maxParticles), sizeof(maxParticles));
        }

        // emission rate
        {
            float emissionrate = emitter->GetEmissionRate();
            os.write(reinterpret_cast<const char*>(&emissionrate), sizeof(emissionrate));
        }

        // start delay
        {
            float startdelay = emitter->GetStartDelay();
            os.write(reinterpret_cast<const char*>(&startdelay), sizeof(startdelay));
        }

        // spawn burst flag
        {
            float spawnBurst = emitter->GetSpawnBurstFlag();
            os.write(reinterpret_cast<const char*>(&spawnBurst), sizeof(spawnBurst));
        }

        // spawn burst count
        {
            float spawnBurstCount = emitter->GetSpawnBurstCount();
            os.write(reinterpret_cast<const char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        }

        // start color
        {
            Vector3 startcolor = emitter->GetStartColor();
            os.write(reinterpret_cast<const char*>(&startcolor), sizeof(startcolor));
        }

        // start alpha
        {
            float startopacity = emitter->GetStartOpacity();
            os.write(reinterpret_cast<const char*>(&startopacity), sizeof(startopacity));
        }

        // end color
        {
            Vector3 endcolor = emitter->GetEndColor();
            os.write(reinterpret_cast<const char*>(&endcolor), sizeof(endcolor));
        }

        // end alpha
        {
            float endopacity = emitter->GetEndOpacity();
            os.write(reinterpret_cast<const char*>(&endopacity), sizeof(endopacity));
        }

        // start scale
        {
            Vector4 startscale = emitter->GetStartScale();
            os.write(reinterpret_cast<const char*>(&startscale), sizeof(startscale));
        }

        // end scale
        {
            Vector4 endscale = emitter->GetEndScale();
            os.write(reinterpret_cast<const char*>(&endscale), sizeof(endscale));
        }

        // particle axis
        {
            Vector3 axis = emitter->GetParticleAxis();
            os.write(reinterpret_cast<const char*>(&axis), sizeof(axis));
        }

        // scale by velocity flag
        {
            bool scalevelflag = emitter->GetScaleByVelocityFlag();
            os.write(reinterpret_cast<const char*>(&scalevelflag), sizeof(scalevelflag));
        }

        // particle mass
        {
            float mass = emitter->GetParticleMass();
            os.write(reinterpret_cast<const char*>(&mass), sizeof(mass));
        }

        // distribution offset
        {
            Vector3 offset = emitter->GetParticleStartDistributionOffset();
            os.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        }

        // drag point
        {
            Vector4 dragpoint = emitter->GetDragPoint();
            os.write(reinterpret_cast<const char*>(&dragpoint), sizeof(dragpoint));
        }

        // drag force
        {
            Vector4 dragforce = emitter->GetDragForce();
            os.write(reinterpret_cast<const char*>(&dragforce), sizeof(dragforce));
        }
        // drag force
        {
            Vector4 vortex = emitter->GetVortexForce();
            os.write(reinterpret_cast<const char*>(&vortex), sizeof(vortex));
        }

        // render type
        {
            auto rendertype = emitter->_particleType;
            os.write(reinterpret_cast<const char*>(&rendertype), sizeof(rendertype));
        }

        // render module file path
        {
            File::Path modelTexturepath = emitter->_particleRenderModule->GetModelAndTexturePath();
            modelTexturepath            = std::filesystem::absolute(modelTexturepath).generic_string();
            const File::Guid&  guid            = modelTexturepath.ToGuid();
            std::string guidstring      = guid.string();
            SIZE_T      size            = guidstring.length();
            os.write(reinterpret_cast<const char*>(&size), sizeof(size));
            os.write(guidstring.c_str(), size);
        }

        if (ParticleType::SPRITE == emitter->_particleType)
        {
            if (auto spriteModule = emitter->_particleRenderModule->AsSprite())
            {
                Vector4 frameinfo = spriteModule->GetFrameInfo();
                os.write(reinterpret_cast<const char*>(&frameinfo), sizeof(frameinfo));
            }
        }
        else if (ParticleType::RIBBON == emitter->_particleType)
        {
            if (auto ribbonModule = emitter->_particleRenderModule->AsRibbon())
            {
                Vector4 startNormal  = ribbonModule->GetStartNormal();
                Vector4 endNormal    = ribbonModule->GetEndNormal();
                Vector4 ribbonVector = ribbonModule->GetRibbonVector();
                os.write(reinterpret_cast<const char*>(&startNormal), sizeof(startNormal));
                os.write(reinterpret_cast<const char*>(&endNormal), sizeof(endNormal));
                os.write(reinterpret_cast<const char*>(&ribbonVector), sizeof(ribbonVector));
            }
        }
    }
    os.close();
}
ParticleEffect* ParticleEffectSerializer::Deserialize_1_3(EffectID id, const std::string& keyString, std::ifstream& is,
                                                          bool isEditor, std::string_view sceneName)
{
    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    ParticleEffect* newEffect = nullptr;
    if (true == isEditor)
        newEffect = UmParticleManager->RegisterEffectOnEditor();
    else
    {
        auto scenename = std::string(sceneName);
        newEffect      = UmParticleManager->RegisterEffect(id, keyString, scenename);
    }
    newEffect->SetLifetime(lifetime);
    newEffect->SetEffectName(effectname);

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           distributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        Vector4           ribbonvector;
        bool              useWorldSpace;

        Vector3 axis;
        bool    scalevelFlag;

        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {

            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
        }

        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&axis), sizeof(axis));
        is.read(reinterpret_cast<char*>(&scalevelFlag), sizeof(scalevelFlag));

        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&distributionOffset), sizeof(distributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        SIZE_T size = 0;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::string texturepath(size, '\0');
        is.read(&texturepath[0], size);
        const File::Guid& guid             = texturepath;
        File::Path modelTexturePath = guid.ToPath();

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
            is.read(reinterpret_cast<char*>(&ribbonvector), sizeof(ribbonvector));
        }

        {
            auto emitter = UmParticleManager->RegisterEmitter(newEffect, static_cast<SIZE_T>(maxParticles),
                                                              emissionRate, emitterLifetime, locationType,
                                                              locatorFactor, particleType, modelTexturePath.wstring());
            {
                File::Path absolutePath = emitter->_particleRenderModule->GetModelAndTexturePath();
                absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                UmGraphics.LoadTextureResource(std::wstring_view(absolutePath.wstring()), emitter);
                if (LocationShape::MESH_SURFACE == emitter->_locationType)
                {
                    const File::Guid& guid         = modelPath;
                    File::Path absolutePath = guid.ToPath();
                    absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                    UmGraphics.LoadModelResource(std::wstring_view(absolutePath.wstring()), emitter);
                    if (auto meshSurfaceLocator = emitter->_emitLocator->AsMeshSurfaceLocator())
                    {
                        meshSurfaceLocator->SetModelPath(absolutePath.wstring());
                    }
                }
            }
            emitter->SetEmitterName(emitterName);
            emitter->SetUseWorldSpace(useWorldSpace);
            emitter->SetEmitterPosition(emitterPosition);
            emitter->SetEmitterRotationE(emitterRotationE);
            emitter->SetEmitterRotationQ(emitterRotationQ);
            emitter->SetVelocityType(velocityType);
            emitter->SetVelocityFactor(velocityFactor);
            emitter->SetParticleLifetime(particleLifetime);
            emitter->SetStartDelay(startDelay);
            emitter->SetSpawnBurstFlag(spawnBurstFlag);
            emitter->SetSpawnBurstCount(spawnBurstCount);
            emitter->SetStartColor(startColor);
            emitter->SetStartOpacity(startOpacity);
            emitter->SetEndColor(endColor);
            emitter->SetEndOpacity(endOpacity);
            emitter->SetStartScale(startScale);
            emitter->SetEndScale(endScale);
            emitter->SetParticleMass(particleMass);
            emitter->SetParticleStartDistributionOffset(distributionOffset);
            emitter->SetDragPoint(dragPoint);
            emitter->SetDragForce(dragForce);
            emitter->SetVortexForce(vortexForce);
            emitter->SetParticleAxis(axis);
            emitter->SetScaleByVelocityFlag(scalevelFlag);
            if (particleType == ParticleType::RIBBON)
            {
                if (auto ribbonModule = emitter->_particleRenderModule->AsRibbon())
                {
                    ribbonModule->SetStartNormal(startNormal);
                    ribbonModule->SetEndNormal(endNormal);
                    ribbonModule->SetRibbonVector(ribbonvector);
                }
            }
            else
            {
                if (auto spriteModule = emitter->_particleRenderModule->AsSprite())
                {
                    spriteModule->SetFrameInfo(frameInfo);
                    spriteModule->CalculateFrameInfos();
                }
            }
        }
    }

    is.close();
    return newEffect;
}
void ParticleEffectSerializer::PreDeserialize_1_3(std::ifstream& is)
{
    UsedTexturePaths.clear();
    UsedModelPaths.clear();

    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           distributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        Vector4           ribbonvector;
        bool              useWorldSpace;
        Vector3           axis;
        bool              scaleVelflag;

        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {
            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
            const File::Guid& guid = modelPath;
            File::Path path = guid.ToPath();
            UsedModelPaths.insert(path);
        }

        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&axis), sizeof(axis));
        is.read(reinterpret_cast<char*>(&scaleVelflag), sizeof(scaleVelflag));
        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&distributionOffset), sizeof(distributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        SIZE_T size = 0;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::string texturepath = std::string(size, '\0');
        is.read(&texturepath[0], size);
        const File::Guid& guid = texturepath;
        File::Path path = guid.ToPath();

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
            is.read(reinterpret_cast<char*>(&ribbonvector), sizeof(ribbonvector));
        }

        UsedTexturePaths.insert(path);
    }

    is.close();
}

void ParticleEffectSerializer::Serialize_1_4(std::ofstream& os, ParticleEffect* effect, File::Path destPath)
{

    const std::string effectname = effect->GetEffectName();
    uint32_t          nameLen    = static_cast<uint32_t>(effectname.size());
    os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    os.write(effectname.c_str(), nameLen);

    // lifetime
    float lifetime = effect->GetLifetime();
    os.write(reinterpret_cast<const char*>(&lifetime), sizeof(lifetime));

    uint32_t count = static_cast<uint32_t>(effect->GetEmitterList().size());
    os.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (auto& emitter : effect->GetEmitterList())
    {
        // name length, name
        const std::string emittername = emitter->GetEmitterName();
        uint32_t          nameLen     = static_cast<uint32_t>(emittername.size());
        os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        os.write(emittername.c_str(), nameLen);

        // light flag
        {
            auto temp = emitter->GetUseLight();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }
        if (emitter->GetUseLight())
        {
            {
                auto temp = emitter->GetLightIntensity();
                os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            }

            {
                auto temp = emitter->GetLightRange();
                os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            }

            {
                auto temp = emitter->GetLightColor();
                os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            }
        }
        // world space flag
        {
            auto temp = emitter->GetUseWorldSpace();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter position
        {
            auto temp = emitter->GetEmitterPosition();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation euler
        {
            auto temp = emitter->GetEmitterRotationE();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation quaternion
        {
            auto temp = emitter->GetEmitterRotationQ();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // location type
        {
            auto temp = emitter->_locationType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            if (LocationShape::MESH_SURFACE == emitter->_locationType)
            {
                auto       meshLocator = emitter->_emitLocator->AsMeshSurfaceLocator();
                File::Path path        = meshLocator->GetModelPath();
                path                   = std::filesystem::absolute(path).generic_string();
                const File::Guid&  guid       = path.ToGuid();
                std::string guidstring = guid.string();

                SIZE_T nameLen = guidstring.length();
                os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
                os.write(guidstring.c_str(), nameLen);
            }
        }
        // location factor
        {
            auto temp = emitter->_emitLocator->GetFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity type
        {
            auto temp = emitter->_velocityType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity factor
        {
            auto temp = emitter->GetVelocityFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter lifetime
        {
            float emitterlifetime = emitter->GetEmitterLifetime();
            os.write(reinterpret_cast<const char*>(&emitterlifetime), sizeof(emitterlifetime));
        }

        // particle lifetime
        {
            float particlelifetime = emitter->GetParticleLifetime();
            os.write(reinterpret_cast<const char*>(&particlelifetime), sizeof(particlelifetime));
        }

        // max particles
        {
            float maxParticles = static_cast<float>(emitter->GetMaxParticles());
            os.write(reinterpret_cast<const char*>(&maxParticles), sizeof(maxParticles));
        }

        // emission rate
        {
            float emissionrate = emitter->GetEmissionRate();
            os.write(reinterpret_cast<const char*>(&emissionrate), sizeof(emissionrate));
        }

        // start delay
        {
            float startdelay = emitter->GetStartDelay();
            os.write(reinterpret_cast<const char*>(&startdelay), sizeof(startdelay));
        }

        // spawn burst flag
        {
            float spawnBurst = emitter->GetSpawnBurstFlag();
            os.write(reinterpret_cast<const char*>(&spawnBurst), sizeof(spawnBurst));
        }

        // spawn burst count
        {
            float spawnBurstCount = emitter->GetSpawnBurstCount();
            os.write(reinterpret_cast<const char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        }

        // start color
        {
            Vector3 startcolor = emitter->GetStartColor();
            os.write(reinterpret_cast<const char*>(&startcolor), sizeof(startcolor));
        }

        // start alpha
        {
            float startopacity = emitter->GetStartOpacity();
            os.write(reinterpret_cast<const char*>(&startopacity), sizeof(startopacity));
        }

        // end color
        {
            Vector3 endcolor = emitter->GetEndColor();
            os.write(reinterpret_cast<const char*>(&endcolor), sizeof(endcolor));
        }

        // end alpha
        {
            float endopacity = emitter->GetEndOpacity();
            os.write(reinterpret_cast<const char*>(&endopacity), sizeof(endopacity));
        }

        // start scale
        {
            Vector4 startscale = emitter->GetStartScale();
            os.write(reinterpret_cast<const char*>(&startscale), sizeof(startscale));
        }

        // end scale
        {
            Vector4 endscale = emitter->GetEndScale();
            os.write(reinterpret_cast<const char*>(&endscale), sizeof(endscale));
        }

        // particle axis
        {
            Vector3 axis = emitter->GetParticleAxis();
            os.write(reinterpret_cast<const char*>(&axis), sizeof(axis));
        }

        // scale by velocity flag
        {
            bool scalevelflag = emitter->GetScaleByVelocityFlag();
            os.write(reinterpret_cast<const char*>(&scalevelflag), sizeof(scalevelflag));
        }

        // particle mass
        {
            float mass = emitter->GetParticleMass();
            os.write(reinterpret_cast<const char*>(&mass), sizeof(mass));
        }

        // distribution offset
        {
            Vector3 offset = emitter->GetParticleStartDistributionOffset();
            os.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        }
        {
            Vector3 offset = emitter->GetParticleEndDistributionOffset();
            os.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        }
        // drag point
        {
            Vector4 dragpoint = emitter->GetDragPoint();
            os.write(reinterpret_cast<const char*>(&dragpoint), sizeof(dragpoint));
        }

        // drag force
        {
            Vector4 dragforce = emitter->GetDragForce();
            os.write(reinterpret_cast<const char*>(&dragforce), sizeof(dragforce));
        }
        // drag force
        {
            Vector4 vortex = emitter->GetVortexForce();
            os.write(reinterpret_cast<const char*>(&vortex), sizeof(vortex));
        }

        // render type
        {
            auto rendertype = emitter->_particleType;
            os.write(reinterpret_cast<const char*>(&rendertype), sizeof(rendertype));
        }

        // render module file path
        {
            File::Path modelTexturepath = emitter->_particleRenderModule->GetModelAndTexturePath();
            modelTexturepath            = std::filesystem::absolute(modelTexturepath).generic_string();
            const File::Guid&  guid            = modelTexturepath.ToGuid();
            std::string guidstring      = guid.string();
            SIZE_T      size            = guidstring.length();
            os.write(reinterpret_cast<const char*>(&size), sizeof(size));
            os.write(guidstring.c_str(), size);
        }

        if (ParticleType::SPRITE == emitter->_particleType)
        {
            if (auto spriteModule = emitter->_particleRenderModule->AsSprite())
            {
                Vector4 frameinfo = spriteModule->GetFrameInfo();
                os.write(reinterpret_cast<const char*>(&frameinfo), sizeof(frameinfo));
            }
        }
        else if (ParticleType::RIBBON == emitter->_particleType)
        {
            if (auto ribbonModule = emitter->_particleRenderModule->AsRibbon())
            {
                Vector4 startNormal  = ribbonModule->GetStartNormal();
                Vector4 endNormal    = ribbonModule->GetEndNormal();
                Vector4 ribbonVector = ribbonModule->GetRibbonVector();
                os.write(reinterpret_cast<const char*>(&startNormal), sizeof(startNormal));
                os.write(reinterpret_cast<const char*>(&endNormal), sizeof(endNormal));
                os.write(reinterpret_cast<const char*>(&ribbonVector), sizeof(ribbonVector));
            }
        }
    }
    os.close();
}
ParticleEffect* ParticleEffectSerializer::Deserialize_1_4(EffectID id, const std::string& keyString, std::ifstream& is,
                                                          bool isEditor, std::string_view sceneName)
{
    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    ParticleEffect* newEffect = nullptr;
    if (true == isEditor)
        newEffect = UmParticleManager->RegisterEffectOnEditor();
    else
    {
        auto scenename = std::string(sceneName);
        newEffect      = UmParticleManager->RegisterEffect(id, keyString, scenename);
    }
    newEffect->SetLifetime(lifetime);
    newEffect->SetEffectName(effectname);

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           startdistributionOffset;
        Vector3           enddistributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        Vector4           ribbonvector;
        bool              useWorldSpace;
        Vector3           axis;
        bool              scalevelFlag;

        bool    uselight;
        float   lightintensity;
        float   lightrange;
        Vector3 lightcolor;

        is.read(reinterpret_cast<char*>(&uselight), sizeof(uselight));
        if (uselight)
        {
            is.read(reinterpret_cast<char*>(&lightintensity), sizeof(lightintensity));
            is.read(reinterpret_cast<char*>(&lightrange), sizeof(lightrange));
            is.read(reinterpret_cast<char*>(&lightcolor), sizeof(lightcolor));
        }

        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {
            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
        }

        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&axis), sizeof(axis));
        is.read(reinterpret_cast<char*>(&scalevelFlag), sizeof(scalevelFlag));

        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&startdistributionOffset), sizeof(startdistributionOffset));
        is.read(reinterpret_cast<char*>(&enddistributionOffset), sizeof(enddistributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        SIZE_T size = 0;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::string texturepath(size, '\0');
        is.read(&texturepath[0], size);
        const File::Guid& guid             = texturepath;
        File::Path modelTexturePath = guid.ToPath();

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
            is.read(reinterpret_cast<char*>(&ribbonvector), sizeof(ribbonvector));
        }

        {
            auto emitter = UmParticleManager->RegisterEmitter(newEffect, static_cast<SIZE_T>(maxParticles),
                                                              emissionRate, emitterLifetime, locationType,
                                                              locatorFactor, particleType, modelTexturePath.wstring());
            {
                emitter->SetUseLight(uselight);
                if (uselight)
                {
                    emitter->SetLightIntensity(lightintensity);
                    emitter->SetLightRange(lightrange);
                    emitter->SetLightColor(lightcolor);
                    emitter->InitializeLight(sceneName);
                }
            }
            {
                File::Path absolutePath = emitter->_particleRenderModule->GetModelAndTexturePath();
                absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                UmGraphics.LoadTextureResource(std::wstring_view(absolutePath.wstring()), emitter);
                if (LocationShape::MESH_SURFACE == emitter->_locationType)
                {
                    const File::Guid& guid         = modelPath;
                    File::Path absolutePath = guid.ToPath();
                    absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                    UmGraphics.LoadModelResource(std::wstring_view(absolutePath.wstring()), emitter);
                    if (auto meshSurfaceLocator = emitter->_emitLocator->AsMeshSurfaceLocator())
                    {
                        meshSurfaceLocator->SetModelPath(absolutePath.wstring());
                    }
                }
            }
            emitter->SetEmitterName(emitterName);
            emitter->SetUseWorldSpace(useWorldSpace);
            emitter->SetEmitterPosition(emitterPosition);
            emitter->SetEmitterRotationE(emitterRotationE);
            emitter->SetEmitterRotationQ(emitterRotationQ);
            emitter->SetVelocityType(velocityType);
            emitter->SetVelocityFactor(velocityFactor);
            emitter->SetParticleLifetime(particleLifetime);
            emitter->SetStartDelay(startDelay);
            emitter->SetSpawnBurstFlag(spawnBurstFlag);
            emitter->SetSpawnBurstCount(spawnBurstCount);
            emitter->SetStartColor(startColor);
            emitter->SetStartOpacity(startOpacity);
            emitter->SetEndColor(endColor);
            emitter->SetEndOpacity(endOpacity);
            emitter->SetStartScale(startScale);
            emitter->SetEndScale(endScale);
            emitter->SetParticleMass(particleMass);
            emitter->SetParticleStartDistributionOffset(startdistributionOffset);
            emitter->SetParticleEndDistributionOffset(enddistributionOffset);
            emitter->SetDragPoint(dragPoint);
            emitter->SetDragForce(dragForce);
            emitter->SetVortexForce(vortexForce);
            emitter->SetParticleAxis(axis);
            emitter->SetScaleByVelocityFlag(scalevelFlag);
            if (particleType == ParticleType::RIBBON)
            {
                if (auto ribbonModule = emitter->_particleRenderModule->AsRibbon())
                {
                    ribbonModule->SetStartNormal(startNormal);
                    ribbonModule->SetEndNormal(endNormal);
                    ribbonModule->SetRibbonVector(ribbonvector);
                }
            }
            else
            {
                if (auto spriteModule = emitter->_particleRenderModule->AsSprite())
                {
                    spriteModule->SetFrameInfo(frameInfo);
                    spriteModule->CalculateFrameInfos();
                }
            }
        }
    }

    is.close();
    return newEffect;
}
void ParticleEffectSerializer::PreDeserialize_1_4(std::ifstream& is)
{
    UsedTexturePaths.clear();
    UsedModelPaths.clear();

    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           startdistributionOffset;
        Vector3           enddistributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        Vector4           ribbonvector;
        bool              useWorldSpace;
        Vector3           axis;
        bool              scaleVelflag;
        bool              uselight;
        float             lightintensity;
        float             lightrange;
        Vector3           lightcolor;
        is.read(reinterpret_cast<char*>(&uselight), sizeof(uselight));
        if (uselight)
        {
            is.read(reinterpret_cast<char*>(&lightintensity), sizeof(lightintensity));
            is.read(reinterpret_cast<char*>(&lightrange), sizeof(lightrange));
            is.read(reinterpret_cast<char*>(&lightcolor), sizeof(lightcolor));
        }
        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {
            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
            const File::Guid& guid = modelPath;
            File::Path path = guid.ToPath();
            UsedModelPaths.insert(path);
        }

        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&axis), sizeof(axis));
        is.read(reinterpret_cast<char*>(&scaleVelflag), sizeof(scaleVelflag));
        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&startdistributionOffset), sizeof(startdistributionOffset));
        is.read(reinterpret_cast<char*>(&enddistributionOffset), sizeof(enddistributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        SIZE_T size = 0;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::string texturepath = std::string(size, '\0');
        is.read(&texturepath[0], size);
        const File::Guid& guid = texturepath;
        File::Path path = guid.ToPath();

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
            is.read(reinterpret_cast<char*>(&ribbonvector), sizeof(ribbonvector));
        }

        UsedTexturePaths.insert(path);
    }

    is.close();
}


void ParticleEffectSerializer::Serialize_1_5(std::ofstream& os, ParticleEffect* effect, File::Path destPath)
{

    const std::string effectname = effect->GetEffectName();
    uint32_t          nameLen    = static_cast<uint32_t>(effectname.size());
    os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    os.write(effectname.c_str(), nameLen);

    // lifetime
    float lifetime = effect->GetLifetime();
    os.write(reinterpret_cast<const char*>(&lifetime), sizeof(lifetime));

    uint32_t count = static_cast<uint32_t>(effect->GetEmitterList().size());
    os.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (auto& emitter : effect->GetEmitterList())
    {
        // name length, name
        const std::string emittername = emitter->GetEmitterName();
        uint32_t          nameLen     = static_cast<uint32_t>(emittername.size());
        os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        os.write(emittername.c_str(), nameLen);

        // light flag
        {
            auto temp = emitter->GetUseLight();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }
        if (emitter->GetUseLight())
        {
            {
                auto temp = emitter->GetLightIntensity();
                os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            }

            {
                auto temp = emitter->GetLightRange();
                os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            }

            {
                auto temp = emitter->GetLightColor();
                os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            }
        }
        // world space flag
        {
            auto temp = emitter->GetUseWorldSpace();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter position
        {
            auto temp = emitter->GetEmitterPosition();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation euler
        {
            auto temp = emitter->GetEmitterRotationE();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter rotation quaternion
        {
            auto temp = emitter->GetEmitterRotationQ();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // location type
        {
            auto temp = emitter->_locationType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
            if (LocationShape::MESH_SURFACE == emitter->_locationType)
            {
                auto       meshLocator       = emitter->_emitLocator->AsMeshSurfaceLocator();
                File::Path path              = meshLocator->GetModelPath();
                path                         = std::filesystem::absolute(path).generic_string();
                const File::Guid& guid       = path.ToGuid();
                std::string       guidstring = guid.string();

                SIZE_T nameLen = guidstring.length();
                os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
                os.write(guidstring.c_str(), nameLen);
            }
        }
        // location factor
        {
            auto temp = emitter->_emitLocator->GetFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity type
        {
            auto temp = emitter->_velocityType;
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // velocity factor
        {
            auto temp = emitter->GetVelocityFactor();
            os.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
        }

        // emitter lifetime
        {
            float emitterlifetime = emitter->GetEmitterLifetime();
            os.write(reinterpret_cast<const char*>(&emitterlifetime), sizeof(emitterlifetime));
        }

        // particle lifetime
        {
            float particlelifetime = emitter->GetParticleLifetime();
            os.write(reinterpret_cast<const char*>(&particlelifetime), sizeof(particlelifetime));
        }

        // max particles
        {
            float maxParticles = static_cast<float>(emitter->GetMaxParticles());
            os.write(reinterpret_cast<const char*>(&maxParticles), sizeof(maxParticles));
        }

        // emission rate
        {
            float emissionrate = emitter->GetEmissionRate();
            os.write(reinterpret_cast<const char*>(&emissionrate), sizeof(emissionrate));
        }

        // start delay
        {
            float startdelay = emitter->GetStartDelay();
            os.write(reinterpret_cast<const char*>(&startdelay), sizeof(startdelay));
        }

        // spawn burst flag
        {
            float spawnBurst = emitter->GetSpawnBurstFlag();
            os.write(reinterpret_cast<const char*>(&spawnBurst), sizeof(spawnBurst));
        }

        // spawn burst count
        {
            float spawnBurstCount = emitter->GetSpawnBurstCount();
            os.write(reinterpret_cast<const char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        }

        // start color
        {
            Vector3 startcolor = emitter->GetStartColor();
            os.write(reinterpret_cast<const char*>(&startcolor), sizeof(startcolor));
        }

        // start alpha
        {
            float startopacity = emitter->GetStartOpacity();
            os.write(reinterpret_cast<const char*>(&startopacity), sizeof(startopacity));
        }

        // end color
        {
            Vector3 endcolor = emitter->GetEndColor();
            os.write(reinterpret_cast<const char*>(&endcolor), sizeof(endcolor));
        }

        // end alpha
        {
            float endopacity = emitter->GetEndOpacity();
            os.write(reinterpret_cast<const char*>(&endopacity), sizeof(endopacity));
        }

        // start scale
        {
            Vector4 startscale = emitter->GetStartScale();
            os.write(reinterpret_cast<const char*>(&startscale), sizeof(startscale));
        }

        // end scale
        {
            Vector4 endscale = emitter->GetEndScale();
            os.write(reinterpret_cast<const char*>(&endscale), sizeof(endscale));
        }

        // particle axis
        {
            Vector3 axis = emitter->GetParticleAxis();
            os.write(reinterpret_cast<const char*>(&axis), sizeof(axis));
        }

        // particle Rotation
        {
            Vector3 rot = emitter->GetParticleRotation();
            os.write(reinterpret_cast<const char*>(&rot), sizeof(rot));
        }

        // scale by velocity flag
        {
            bool scalevelflag = emitter->GetScaleByVelocityFlag();
            os.write(reinterpret_cast<const char*>(&scalevelflag), sizeof(scalevelflag));
        }

        // particle mass
        {
            float mass = emitter->GetParticleMass();
            os.write(reinterpret_cast<const char*>(&mass), sizeof(mass));
        }

        // distribution offset
        {
            Vector3 offset = emitter->GetParticleStartDistributionOffset();
            os.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        }
        {
            Vector3 offset = emitter->GetParticleEndDistributionOffset();
            os.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        }
        // drag point
        {
            Vector4 dragpoint = emitter->GetDragPoint();
            os.write(reinterpret_cast<const char*>(&dragpoint), sizeof(dragpoint));
        }

        // drag force
        {
            Vector4 dragforce = emitter->GetDragForce();
            os.write(reinterpret_cast<const char*>(&dragforce), sizeof(dragforce));
        }
        // drag force
        {
            Vector4 vortex = emitter->GetVortexForce();
            os.write(reinterpret_cast<const char*>(&vortex), sizeof(vortex));
        }

        // render type
        {
            auto rendertype = emitter->_particleType;
            os.write(reinterpret_cast<const char*>(&rendertype), sizeof(rendertype));
        }

        // render module file path
        {
            File::Path modelTexturepath  = emitter->_particleRenderModule->GetModelAndTexturePath();
            modelTexturepath             = std::filesystem::absolute(modelTexturepath).generic_string();
            const File::Guid& guid       = modelTexturepath.ToGuid();
            std::string       guidstring = guid.string();
            SIZE_T            size       = guidstring.length();
            os.write(reinterpret_cast<const char*>(&size), sizeof(size));
            os.write(guidstring.c_str(), size);
        }

        if (ParticleType::SPRITE == emitter->_particleType)
        {
            if (auto spriteModule = emitter->_particleRenderModule->AsSprite())
            {
                Vector4 frameinfo = spriteModule->GetFrameInfo();
                os.write(reinterpret_cast<const char*>(&frameinfo), sizeof(frameinfo));
            }
        }
        else if (ParticleType::RIBBON == emitter->_particleType)
        {
            if (auto ribbonModule = emitter->_particleRenderModule->AsRibbon())
            {
                Vector4 startNormal  = ribbonModule->GetStartNormal();
                Vector4 endNormal    = ribbonModule->GetEndNormal();
                Vector4 ribbonVector = ribbonModule->GetRibbonVector();
                os.write(reinterpret_cast<const char*>(&startNormal), sizeof(startNormal));
                os.write(reinterpret_cast<const char*>(&endNormal), sizeof(endNormal));
                os.write(reinterpret_cast<const char*>(&ribbonVector), sizeof(ribbonVector));
            }
        }
    }
    os.close();
}
ParticleEffect* ParticleEffectSerializer::Deserialize_1_5(EffectID id, const std::string& keyString, std::ifstream& is,
                                                          bool isEditor, std::string_view sceneName)
{
    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    ParticleEffect* newEffect = nullptr;
    if (true == isEditor)
        newEffect = UmParticleManager->RegisterEffectOnEditor();
    else
    {
        auto scenename = std::string(sceneName);
        newEffect      = UmParticleManager->RegisterEffect(id, keyString, scenename);
    }
    newEffect->SetLifetime(lifetime);
    newEffect->SetEffectName(effectname);

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           startdistributionOffset;
        Vector3           enddistributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        Vector4           ribbonvector;
        bool              useWorldSpace;
        Vector3           axis;
        Vector3           particleRotation;
        bool              scalevelFlag;

        bool    uselight;
        float   lightintensity;
        float   lightrange;
        Vector3 lightcolor;

        is.read(reinterpret_cast<char*>(&uselight), sizeof(uselight));
        if (uselight)
        {
            is.read(reinterpret_cast<char*>(&lightintensity), sizeof(lightintensity));
            is.read(reinterpret_cast<char*>(&lightrange), sizeof(lightrange));
            is.read(reinterpret_cast<char*>(&lightcolor), sizeof(lightcolor));
        }

        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {
            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
        }

        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&axis), sizeof(axis));
        is.read(reinterpret_cast<char*>(&particleRotation), sizeof(particleRotation));
        is.read(reinterpret_cast<char*>(&scalevelFlag), sizeof(scalevelFlag));

        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&startdistributionOffset), sizeof(startdistributionOffset));
        is.read(reinterpret_cast<char*>(&enddistributionOffset), sizeof(enddistributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        SIZE_T size = 0;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::string texturepath(size, '\0');
        is.read(&texturepath[0], size);
        const File::Guid& guid             = texturepath;
        File::Path        modelTexturePath = guid.ToPath();

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
            is.read(reinterpret_cast<char*>(&ribbonvector), sizeof(ribbonvector));
        }

        {
            auto emitter = UmParticleManager->RegisterEmitter(newEffect, static_cast<SIZE_T>(maxParticles),
                                                              emissionRate, emitterLifetime, locationType,
                                                              locatorFactor, particleType, modelTexturePath.wstring());
            {
                emitter->SetUseLight(uselight);
                if (uselight)
                {
                    emitter->SetLightIntensity(lightintensity);
                    emitter->SetLightRange(lightrange);
                    emitter->SetLightColor(lightcolor);
                    emitter->InitializeLight(sceneName);
                }
            }
            {
                File::Path absolutePath = emitter->_particleRenderModule->GetModelAndTexturePath();
                absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                UmGraphics.LoadTextureResource(std::wstring_view(absolutePath.wstring()), emitter);
                if (LocationShape::MESH_SURFACE == emitter->_locationType)
                {
                    const File::Guid& guid         = modelPath;
                    File::Path        absolutePath = guid.ToPath();
                    absolutePath                   = std::filesystem::absolute(absolutePath).generic_string();
                    UmGraphics.LoadModelResource(std::wstring_view(absolutePath.wstring()), emitter);
                    if (auto meshSurfaceLocator = emitter->_emitLocator->AsMeshSurfaceLocator())
                    {
                        meshSurfaceLocator->SetModelPath(absolutePath.wstring());
                    }
                }
            }
            emitter->SetEmitterName(emitterName);
            emitter->SetUseWorldSpace(useWorldSpace);
            emitter->SetEmitterPosition(emitterPosition);
            emitter->SetEmitterRotationE(emitterRotationE);
            emitter->SetEmitterRotationQ(emitterRotationQ);
            emitter->SetVelocityType(velocityType);
            emitter->SetVelocityFactor(velocityFactor);
            emitter->SetParticleLifetime(particleLifetime);
            emitter->SetStartDelay(startDelay);
            emitter->SetSpawnBurstFlag(spawnBurstFlag);
            emitter->SetSpawnBurstCount(spawnBurstCount);
            emitter->SetStartColor(startColor);
            emitter->SetStartOpacity(startOpacity);
            emitter->SetEndColor(endColor);
            emitter->SetEndOpacity(endOpacity);
            emitter->SetStartScale(startScale);
            emitter->SetEndScale(endScale);
            emitter->SetParticleMass(particleMass);
            emitter->SetParticleStartDistributionOffset(startdistributionOffset);
            emitter->SetParticleEndDistributionOffset(enddistributionOffset);
            emitter->SetDragPoint(dragPoint);
            emitter->SetDragForce(dragForce);
            emitter->SetVortexForce(vortexForce);
            emitter->SetParticleAxis(axis);
            emitter->SetParticleRotation(particleRotation);
            emitter->SetScaleByVelocityFlag(scalevelFlag);
            if (particleType == ParticleType::RIBBON)
            {
                if (auto ribbonModule = emitter->_particleRenderModule->AsRibbon())
                {
                    ribbonModule->SetStartNormal(startNormal);
                    ribbonModule->SetEndNormal(endNormal);
                    ribbonModule->SetRibbonVector(ribbonvector);
                }
            }
            else
            {
                if (auto spriteModule = emitter->_particleRenderModule->AsSprite())
                {
                    spriteModule->SetFrameInfo(frameInfo);
                    spriteModule->CalculateFrameInfos();
                }
            }
        }
    }

    is.close();
    return newEffect;
}
void ParticleEffectSerializer::PreDeserialize_1_5(std::ifstream& is)
{
    UsedTexturePaths.clear();
    UsedModelPaths.clear();

    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           startdistributionOffset;
        Vector3           enddistributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        Vector4           vortexForce;
        ParticleType      particleType;
        std::string       modelPath;
        Vector4           startNormal;
        Vector4           endNormal;
        Vector4           ribbonvector;
        bool              useWorldSpace;
        Vector3           axis;
        Vector3           particleRotation;
        bool              scaleVelflag;
        bool              uselight;
        float             lightintensity;
        float             lightrange;
        Vector3           lightcolor;
        is.read(reinterpret_cast<char*>(&uselight), sizeof(uselight));
        if (uselight)
        {
            is.read(reinterpret_cast<char*>(&lightintensity), sizeof(lightintensity));
            is.read(reinterpret_cast<char*>(&lightrange), sizeof(lightrange));
            is.read(reinterpret_cast<char*>(&lightcolor), sizeof(lightcolor));
        }
        is.read(reinterpret_cast<char*>(&useWorldSpace), sizeof(useWorldSpace));
        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        if (LocationShape::MESH_SURFACE == locationType)
        {
            SIZE_T nameLen = 0;
            is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            modelPath = std::string(nameLen, '\0');
            is.read(&modelPath[0], nameLen);
            const File::Guid& guid = modelPath;
            File::Path        path = guid.ToPath();
            UsedModelPaths.insert(path);
        }

        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&axis), sizeof(axis));
        is.read(reinterpret_cast<char*>(&particleRotation), sizeof(particleRotation));
        is.read(reinterpret_cast<char*>(&scaleVelflag), sizeof(scaleVelflag));
        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&startdistributionOffset), sizeof(startdistributionOffset));
        is.read(reinterpret_cast<char*>(&enddistributionOffset), sizeof(enddistributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&vortexForce), sizeof(vortexForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        SIZE_T size = 0;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::string texturepath = std::string(size, '\0');
        is.read(&texturepath[0], size);
        const File::Guid& guid = texturepath;
        File::Path        path = guid.ToPath();

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        if (particleType == ParticleType::RIBBON)
        {
            is.read(reinterpret_cast<char*>(&startNormal), sizeof(startNormal));
            is.read(reinterpret_cast<char*>(&endNormal), sizeof(endNormal));
            is.read(reinterpret_cast<char*>(&ribbonvector), sizeof(ribbonvector));
        }

        UsedTexturePaths.insert(path);
    }

    is.close();
}