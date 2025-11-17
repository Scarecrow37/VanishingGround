#pragma once

class BGMComponent : public Component
{
    USING_PROPERTY(BGMComponent)

public:
    BGMComponent();
    ~BGMComponent() override;

    REFLECT_PROPERTY(AudioID)

    SETTER(int, AudioID) { ReflectFields->AudioID = value; }
    GETTER(int, AudioID) { return ReflectFields->AudioID; }
    PROPERTY(AudioID)

    static void PlayBGM(int id, bool useFade = true);
    static void PlayBGM(const std::string& id, bool useFade = true);

private:
    void Start() override;
    void OnDestroy() override;

private:

    REFLECT_FIELDS_BEGIN(Component)
    int AudioID = 0;
    REFLECT_FIELDS_END(BGMComponent)
};