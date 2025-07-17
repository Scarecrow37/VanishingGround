#pragma once
class ParticleEffectSerializer
{
public:
    ParticleEffectSerializer() {};
    virtual ~ParticleEffectSerializer() {}

public:
    void                  Serialize(class ParticleEffect* effect, const std::filesystem::path& destPath);
    class ParticleEffect* Deserialize(const std::filesystem::path& filePath);


};
