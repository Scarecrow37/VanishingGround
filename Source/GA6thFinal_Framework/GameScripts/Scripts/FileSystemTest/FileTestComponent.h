#pragma once
#include "UmFramework.h"
class FileTestComponent : public Component
{
    USING_PROPERTY(FileTestComponent)
public:
REFLECT_PROPERTY()

 virtual void Update() override;

public:
    FileTestComponent();
    virtual ~FileTestComponent();

    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(FileTestComponent)
};
