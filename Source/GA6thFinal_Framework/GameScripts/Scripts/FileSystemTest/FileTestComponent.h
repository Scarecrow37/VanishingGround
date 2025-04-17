#pragma once
#include "UmFramework.h"
class FileTestComponent : public Component
{
    USING_PROPERTY(FileTestComponent)
public:
    REFLECT_PROPERTY(ReflectFields->filePath, ReflectFields->fileGuid)

    virtual void Update() override;

public:
    FileTestComponent();
    virtual ~FileTestComponent();

    REFLECT_FIELDS_BEGIN(Component)
    std::string filePath = "Assets/Textures/AO_Noise.png";
    std::string fileGuid = "";
    REFLECT_FIELDS_END(FileTestComponent)
};