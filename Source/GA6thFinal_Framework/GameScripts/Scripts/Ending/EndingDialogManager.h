#pragma once

class EndingDialogManager : public Component
{
    USING_PROPERTY(EndingDialogManager)

public:
    EndingDialogManager();
    ~EndingDialogManager() override;

public:
    void Reset() override;
    void Awake() override;
    void Update() override;

private:
    void TransitionToMainMenuScene();

public:
    REFLECT_PROPERTY(MainMenuScene)
    GETTER_ONLY(std::string, MainMenuScene)
    {
        File::Guid guid = ReflectFields->MainMenuSceneGuid;
        return guid.ToPath().string();
    }
    PROPERTY(MainMenuScene)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string MainMenuSceneGuid;
    REFLECT_FIELDS_END(EndingDialogManager)
private:
    bool _isDialogEnded = true;
    class ChildsAnimationsController* _childsAnimationsController = nullptr;
};

