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
    /* Begin 호출 전에 항상 호출 (Begin성공 유무 상관 X) */
    virtual void OnPreFrame() override {};

    /* Begin 호출 성공 시 호출 */
    virtual void OnFrame() override;

    /* End 호출 후에 항상 호출 (Begin성공 유무 상관 X) */
    virtual void OnPostFrame() override {};
};

class ReflectTestClass : 
    public ReflectSerializer
{
public:
    ReflectTestClass() = default;
    virtual ~ReflectTestClass() override = default;

    void Func()
    { 
        std::string data = this->SerializedReflectFields(); //호출 가능.
        this->DeserializedReflectFields(data);              //호출 가능.
    }

    virtual void SerializedReflectEvent() override
    {

    }
    virtual void DeserializedReflectEvent() override
    {

    }
};

