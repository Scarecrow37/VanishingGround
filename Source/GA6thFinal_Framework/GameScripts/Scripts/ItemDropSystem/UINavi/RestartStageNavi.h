#pragma once

class RestartStageNavi : public UINavigationComponent
{
    USING_PROPERTY(RestartStageNavi)

public:
    /// <summary>
    /// Navigation Route로 자신을 설정하고, 해당 Route를 실행하게 되면 호출되는 함수입니다.
    /// </summary>
    void Submit() override;

protected:
    void OnEnable() override;

protected:
    REFLECT_FIELDS_BEGIN(UINavigationComponent)
    REFLECT_FIELDS_END(RestartStageNavi)

};