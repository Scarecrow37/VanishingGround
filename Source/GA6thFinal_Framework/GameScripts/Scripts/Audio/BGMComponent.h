#pragma once

class BGMComponent : public Component
{
    USING_PROPERTY(BGMComponent)

public:
    BGMComponent();
    ~BGMComponent() override;

    REFLECT_PROPERTY(AudioID, UseFade)

    SETTER(int, AudioID) { ReflectFields->AudioID = value; }
    GETTER(int, AudioID) { return ReflectFields->AudioID; }
    PROPERTY(AudioID)

    SETTER(bool, UseFade) { ReflectFields->UseFade = value; }
    GETTER(bool, UseFade) { return ReflectFields->UseFade; }
    PROPERTY(UseFade)

    static void PlayBGM(int id, bool useFade = true);
    static void PlayBGM(const std::string& id, bool useFade = true);

private:
    void Start() override;

private:

    REFLECT_FIELDS_BEGIN(Component)
    int AudioID = 0;
    bool UseFade = true;
    REFLECT_FIELDS_END(BGMComponent)
};