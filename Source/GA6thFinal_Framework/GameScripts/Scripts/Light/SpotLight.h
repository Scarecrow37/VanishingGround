#pragma once

#include "PointLight.h"

class SpotLight : public PointLight
{
    USING_PROPERTY(SpotLight)
public:
    REFLECT_PROPERTY(
        Inner, 
        Outer
        )

public:
    SpotLight();
    virtual ~SpotLight();

    GETTER(float, Inner) { return ReflectFields->Inner; }
    SETTER(float, Inner) { ReflectFields->Inner = std::max(value, 0.f); }
    PROPERTY(Inner)

    GETTER(float, Outer) { return ReflectFields->Outer; }
    SETTER(float, Outer) { ReflectFields->Outer = std::max(value, 0.f); }
    PROPERTY(Outer)

protected:
    REFLECT_FIELDS_BEGIN(PointLight)
    float Inner = 0.f;
    float Outer = 100.f;
    REFLECT_FIELDS_END(SpotLight)

    /*
    역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.
    직접 override 해서 사용합니다.
    */
    virtual void Reset() override;

public:
    /// <summary>
    /// <para> 에디터 Scene View에 DrawDebug를 그리기 위한 함수입니다. </para>
    /// <para> 에디터 에서만 호출됩니다.                              </para>
    /// </summary>
    virtual void OnDrawDebug() override;

    /// <summary>
    /// <para> 에디터 Scene View에 DrawDebug를 그리기 위한 함수입니다. </para>
    /// <para> 컴포넌트가 Inspector에 선택되었을때만 호출됩니다.       </para>
    /// <para> 에디터 에서만 호출됩니다. </para>
    /// </summary>
    virtual void OnDrawDebugSelected() override;
};
