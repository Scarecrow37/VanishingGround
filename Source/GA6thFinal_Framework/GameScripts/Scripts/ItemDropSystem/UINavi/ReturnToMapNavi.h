#pragma once

class ReturnToMapNavi : public UINavigationComponent
{
    USING_PROPERTY(ReturnToMapNavi)

public:
    ReturnToMapNavi();

    /// <summary>
    /// Navigation Route로 자신을 설정하고, 해당 Route를 실행하게 되면 호출되는 함수입니다.
    /// </summary>
    void Submit() override;

public:
    REFLECT_PROPERTY(MapScene)

    GETTER_ONLY(std::string, MapScene) { return _guidRef.ToPath().string(); }
    PROPERTY(MapScene)

protected:
    void DeserializedReflectEvent() override;

protected:
    REFLECT_FIELDS_BEGIN(UINavigationComponent)
    std::string MapScene;
    REFLECT_FIELDS_END(ReturnToMapNavi)

private:
    File::GuidRef _guidRef;
};