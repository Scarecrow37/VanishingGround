#pragma once
class CameraManager : public Component
{
    USING_PROPERTY(CameraManager)

public:
    CameraManager();
    ~CameraManager() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(CameraManager)
};