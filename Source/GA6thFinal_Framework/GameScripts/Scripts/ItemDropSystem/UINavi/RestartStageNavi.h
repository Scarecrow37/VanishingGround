#pragma once

class RestartStageNavi : public UISFXNavigationComponent
{
    USING_PROPERTY(RestartStageNavi)

public:
    inline static constexpr const char* TAG = "Item Drop UI Restart Button";

    enum class SelectBoxType
    {
        DEFAULT,
        FOCUS
    };

    RestartStageNavi();

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

protected:
    void Awake() override;
    void Start() override;

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    REFLECT_FIELDS_END(RestartStageNavi)

private:
    class ImageElement* _imageElement;

};