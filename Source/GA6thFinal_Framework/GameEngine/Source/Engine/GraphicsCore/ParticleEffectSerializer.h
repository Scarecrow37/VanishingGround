#pragma once
class ParticleEffectSerializer
{
public:
    ParticleEffectSerializer() {};
    virtual ~ParticleEffectSerializer() {}

public:
    void                  Serialize(class ParticleEffect* effect, File::Path destPath);
    class ParticleEffect* Deserialize(File::Path filePath);


};
