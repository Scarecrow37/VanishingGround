#pragma once
#include "UmFramework.h"

class AnimationComponent;

class AnimationEventListener : public Component
{
    USING_PROPERTY(AnimationEventListener)

public:
    AnimationEventListener();
    virtual ~AnimationEventListener();

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _filePath.string(); }
    PROPERTY(FilePath)

public:
    void Start() override;
    void Update() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:
    bool IsLoadedAnimationNotifySet() const;
    bool IsLoadedAnimationComponent() const;
    void SetAnimationNotifyFromGuid(const File::Guid& guid);
    void SetAnimationNotifyFromPath(const File::Path& path);

private:
    void DrawGuiNotifiesTable();

protected:
    File::GuidRef         _guidRef;
    File::Path            _filePath;
    AnimationNotifySet    _animationNotifySet;
    AnimationComponent*   _animationComponent = nullptr;

    REFLECT_FIELDS_BEGIN(Component)
    std::string Guid;
    REFLECT_FIELDS_END(AnimationEventListener)

    // ImGui
    std::string _debugTimelineName;
};