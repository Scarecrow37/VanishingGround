#include "pch.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"
#include "ParticleEffectSerializer.h"

void ParticleEffectSerializer::OnFileRegistered(const File::Path& path)
{
    
}

void ParticleEffectSerializer::OnFileUnregistered(const File::Path& path)
{
    
}

void ParticleEffectSerializer::OnFileModified(const File::Path& path)
{
    
}


void ParticleEffectSerializer::OnFileRemoved(const File::Path& path)
{
    
}


void ParticleEffectSerializer::OnFileRenamed(const File::Path& oldPath, const File::Path& newPath)
{
    
}


void ParticleEffectSerializer::OnFileMoved(const File::Path& oldPath, const File::Path& newPath)
{
    
}

void ParticleEffectSerializer::OnRequestedSave()
{
    
}

void ParticleEffectSerializer::OnPostRequestedSave()
{
    
}

void ParticleEffectSerializer::OnRequestedLoad()
{
    
}

void ParticleEffectSerializer::OnPostRequestedLoad()
{
    
}

void ParticleEffectSerializer::OnRequestedInspect(const File::Path& path)
{
    
}

void ParticleEffectSerializer::OnRequestedOpen(const File::Path& path)
{
    
}

void ParticleEffectSerializer::OnRequestedCopy(const File::Path& path)
{
    
}

void ParticleEffectSerializer::OnRequestedPaste(const File::Path& path)
{
    
}

void ParticleEffectSerializer::Serialize(ParticleEffect* effect, File::Path destPath)
{
    std::ofstream os(destPath.string(), std::ios::binary);

    const std::string effectname = effect->GetEffectName();
    uint32_t          nameLen    = static_cast<uint32_t>(effectname.size());
    os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    os.write(effectname.c_str(), nameLen);

    // lifetime
    float lifetime = effect->GetLifetime();
    os.write(reinterpret_cast<const char*>(&lifetime), sizeof(lifetime));

    uint32_t count = static_cast<uint32_t>(effect->GetEmitterList().size());
    os.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto* emitter : effect->GetEmitterList())
    {
        // name length, name
        const std::string emittername = emitter->GetEmitterName();
        uint32_t          nameLen     = static_cast<uint32_t>(emittername.size());
        os.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        os.write(emittername.c_str(), nameLen);

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
            float maxParticles = emitter->GetMaxParticles();
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
            Vector3 offset = emitter->GetParticleDistributionOffset();
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
            const std::wstring modeltexturepath = emitter->_particleRenderModule->GetModelAndTexturePath();
            int                sizeNeeded =
                WideCharToMultiByte(CP_UTF8, 0, modeltexturepath.data(), static_cast<int>(modeltexturepath.size()),
                                    nullptr, 0, nullptr, nullptr);
            std::string result(sizeNeeded, 0);
            WideCharToMultiByte(CP_UTF8, 0, modeltexturepath.data(), static_cast<int>(modeltexturepath.size()),
                                result.data(), sizeNeeded, nullptr, nullptr);

            uint32_t pathnameLen = static_cast<uint32_t>(result.size());
            os.write(reinterpret_cast<const char*>(&pathnameLen), sizeof(pathnameLen));
            os.write(result.c_str(), pathnameLen);
        }

        if (ParticleType::SPRITE == emitter->_particleType)
        {
            // frame info
            Vector4 frameinfo = static_cast<SpriteModule*>(emitter->_particleRenderModule)->GetInitialFrameInfo();
            os.write(reinterpret_cast<const char*>(&frameinfo), sizeof(frameinfo));
        }
    }
    os.close();
}

ParticleEffect* ParticleEffectSerializer::Deserialize(File::Path filepath)
{
    std::ifstream is(filepath.string(), std::ios::binary);
    if (!is.is_open())
        return nullptr;

    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    auto newEffect = UmParticleManager.RegisterEffect();
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

        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
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
        uint32_t pathnameLen = 0;
        is.read(reinterpret_cast<char*>(&pathnameLen), sizeof(pathnameLen));
        std::string utf8Path(pathnameLen, '\0');
        is.read(&utf8Path[0], pathnameLen);
        int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), nullptr, 0);
        std::wstring modelTexturePath(wideSize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), modelTexturePath.data(),
                            wideSize);

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        {
            auto emitter =
                UmParticleManager.RegisterEmitter(newEffect, maxParticles, emissionRate, emitterLifetime, locationType,
                                                  locatorFactor, particleType, modelTexturePath);
            emitter->SetEmitterName(emitterName);
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
            emitter->SetParticleDistributionOffset(distributionOffset);
            emitter->SetDragPoint(dragPoint);
            emitter->SetDragForce(dragForce);
            emitter->SetVortexForce(vortexForce);
        }
    }

    is.close();
    return newEffect;

}
