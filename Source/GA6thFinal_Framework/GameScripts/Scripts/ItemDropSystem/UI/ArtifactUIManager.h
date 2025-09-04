#pragma once
#include "UmFramework.h"

class ImageElement;
class GridPanel;

class ArtifactUIManager : public Component
{
    USING_PROPERTY(ArtifactUIManager)
    inline static ArtifactUIManager* static_instance = nullptr;

public:
    inline static constexpr const char* TAG = "Artifact UI Manager";

    static ArtifactUIManager* GetInstance(std::source_location location = std::source_location::current()) 
    {
        if (false == static_instance->gameObject->IsValid())
        {
            static_instance = nullptr;
        }

        if (static_instance)
        {
            return static_instance;
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"ArtifactUIManager가 존재하지 않습니다.", location);
            return nullptr;
        }
    }
    ArtifactUIManager();
    ~ArtifactUIManager() override;

public:
    /// <summary>
    /// Image Element들을 찾아서 등록합니다.
    /// </summary>
    void FindImageElements();

    /// <summary>
    /// Frame Image를 ItemDropUIRootManager의 값으로 설정합니다. 
    /// </summary>
    void UpdateFrameImage();

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ArtifactUIManager)

    void Reset() override;
    void Awake() override;

    void ImGuiDrawPropertysEvent() override;

private:
    GridPanel*                 _frameGridPanel;
    std::vector<ImageElement*> _frameImageElements;

    GridPanel*                 _gridPanel;
    std::vector<ImageElement*> _imageElements;
};
