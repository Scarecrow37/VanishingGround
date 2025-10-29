#pragma once

class ReturnToMapNavi : public UISFXNavigationComponent
{
    USING_PROPERTY(ReturnToMapNavi)

public:
    inline static constexpr const char* TAG = "Item Drop UI ReturnToMapNavi";

    enum class SelectBoxType
    {
        DEFAULT,
        FOCUS
    };

    ReturnToMapNavi();

    /// <summary>
    /// Navigation Route로 자신을 설정하고, 해당 Route를 실행하게 되면 호출되는 함수입니다.
    /// </summary>
    void Submit() override;

    /// <summary>
    /// 포커스가 들어올 때 호출되는 함수입니다.
    /// </summary>
    void FocusIn(FocusCallType callType) override;

    /// <summary>
    /// 포커스가 해당 객체에서 벗어날 때 호출되는 함수입니다.
    /// </summary>
    void FocusOut(FocusCallType callType) override;

public:
    REFLECT_PROPERTY(MapScene)

    GETTER_ONLY(std::string, MapScene) { return _Guid.ToPath().string(); }
    PROPERTY(MapScene)

protected:
    void DeserializedReflectEvent() override;
    void Added() override;
    void Start() override;
    void Awake() override;

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    std::string MapScene;
    REFLECT_FIELDS_END(ReturnToMapNavi)

private:
    File::Guid _Guid;
    class ImageElement* _imageElement;
};