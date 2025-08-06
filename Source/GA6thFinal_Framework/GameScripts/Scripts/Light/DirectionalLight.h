#pragma once
#include "Base/LightComponent.h"
class DirectionalLight : public LightComponent
{
    USING_PROPERTY(DirectionalLight)

public:
    REFLECT_PROPERTY(
        Ambient)

public:
    DirectionalLight();
    virtual ~DirectionalLight();

    GETTER(const Color&, Ambient)
    {
       return _ambient;
    }
    SETTER(const Color&, Ambient)
    { 
        _ambient = value;
        std::memcpy(&ReflectFields->Ambient[0], &_ambient.x, sizeof(ReflectFields->Ambient));
        std::memcpy(&_ambientVector3.x, &_ambient.x, sizeof(_ambientVector3));
    }
    PROPERTY(Ambient)

protected:
    REFLECT_FIELDS_BEGIN(LightComponent)
    std::array<float, 4> Ambient{1.f, 1.f, 1.f, 1.f};
    REFLECT_FIELDS_END(DirectionalLight)

    /*
    역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.
    직접 override 해서 사용합니다.
    */
    virtual void DeserializedReflectEvent() override;

    virtual void Reset() override;

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

private:
    Color _ambient{1.f, 1.f, 1.f, 1.f};
    Vector3 _ambientVector3{1.f, 1.f, 1.f};
};
