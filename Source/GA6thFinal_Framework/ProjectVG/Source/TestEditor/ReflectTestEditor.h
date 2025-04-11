#pragma once
#include "UmFramework.h"

class ReflectTestEditor : public EditorTool
{
public:
    ReflectTestEditor();
    virtual ~ReflectTestEditor() override;

    /* ImGui시스템이 초기화된 후 한번 호출 */
    virtual void OnStartGui() override;

protected:
    /* Begin 호출 전에 항상 호출 (Begin 성공 유무 상관 X) */
    virtual void OnPreFrame() override;

    /* Begin 호출 성공 시 호출 */
    virtual void OnFrame() override;

    /* End 호출 후에 항상 호출 (Begin 성공 유무 상관 X) */
    virtual void OnPostFrame() override {};
};

class MyBaseClass : 
    public ReflectSerializer
{
    USING_PROPERTY(MyBaseClass)
public:
    MyBaseClass() = default;
    virtual ~MyBaseClass() override = default;

    REFLECT_PROPERTY()

protected:
    /*
    직렬화 직전 자동으로 호출되는 이벤트 함수입니다.
    직접 override 해서 사용합니다.
    */
    virtual void SerializedReflectEvent() override;
    /*
    역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.
    직접 override 해서 사용합니다.
    */
    virtual void DeserializedReflectEvent() override;

public:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    // DirectX::SimpleMath::Vector3 position; //지원 안함
    std::array<float, 3> position; //대체 컨테이너
    REFLECT_FIELDS_END(MyBaseClass)

public:
    DirectX::SimpleMath::Vector3 _position; //실제 사용할 맴버
};

