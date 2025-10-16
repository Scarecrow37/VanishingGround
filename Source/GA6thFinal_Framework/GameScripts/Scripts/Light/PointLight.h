#pragma once
#include "Base/LightComponent.h"
class PointLight : public LightComponent
{
    USING_PROPERTY(PointLight)
public:
    REFLECT_PROPERTY(
    //Constant, 
    //Linear, 
    //Quadratic,
    Range
    )

public:
    PointLight();
    virtual ~PointLight();

    /*
    GETTER(float, Constant)
    { 
        return _attenuation.x;
    }
    SETTER(float, Constant)
    {   
        _attenuation.x = value;
        std::memcpy(&_attenuation.x, ReflectFields->Attenuation.data(), sizeof(ReflectFields->Attenuation));
    }
    PROPERTY(Constant)

    GETTER(float, Linear) 
    { 
        return _attenuation.y; 
    }
    SETTER(float, Linear)
    { 
        _attenuation.y = value;
        std::memcpy(ReflectFields->Attenuation.data(), &_attenuation.x, sizeof(ReflectFields->Attenuation));
    }
    PROPERTY(Linear)

    GETTER(float, Quadratic) 
    { 
        return _attenuation.z; 
    }
    SETTER(float, Quadratic)
    { 
        _attenuation.z = value;
        std::memcpy(ReflectFields->Attenuation.data(), &_attenuation.x, sizeof(ReflectFields->Attenuation));
    }
    PROPERTY(Quadratic)
    */

    GETTER(float, Range)
    { 
        return ReflectFields->Range; 
    }
    SETTER(float, Range)
    { 
        ReflectFields->Range = std::max(value, 0.f);
    }
    PROPERTY(Range)

    inline const Vector3& GetAttenuation() 
    { 
        return _attenuation;
    }

    inline const float& GetRange()
    {
        return ReflectFields->Range;
    }

protected:
    REFLECT_FIELDS_BEGIN(LightComponent)
    std::array<float, 3> Attenuation{1.f, 0.1f, 0.1f};
    float Range = 1.f;
    REFLECT_FIELDS_END(PointLight)

    /*
    역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.
    직접 override 해서 사용합니다.
    */
    virtual void DeserializedReflectEvent() override;

    virtual void Reset() override;

public:
    /// <summary>
    /// <para> 에디터 Scene View에 DrawDebug를 그리기 위한 함수입니다. </para>
    /// <para> 에디터 에서만 호출됩니다.                               </para>
    /// </summary>
    virtual void OnDrawDebug() override;

    /// <summary>
    /// <para> 에디터 Scene View에 DrawDebug를 그리기 위한 함수입니다. </para>
    /// <para> 컴포넌트가 Inspector에 선택되었을때만 호출됩니다. </para>
    /// <para> 에디터 에서만 호출됩니다. </para>
    /// </summary>
    virtual void OnDrawDebugSelected() override;

private:
    Vector3 _attenuation{1.f, 0.1f, 0.1f};
};
