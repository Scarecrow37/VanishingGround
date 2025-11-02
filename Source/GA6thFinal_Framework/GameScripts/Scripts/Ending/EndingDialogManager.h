#pragma once

class EndingDialogManager : public Component, public InputReceiver
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
    void StartDialogSequence();
    bool IsDialogSequenceFinished()const
    {
        return _isDialogEnded;
    }
    void PlayNextDialog();
    // 바로 skip 대사 재생
    void SkipCurrentDialog(const Input::Controller& constorller);

public:
    REFLECT_PROPERTY(MainMenuScene, DialogInterval)
    GETTER_ONLY(std::string, MainMenuScene)
    {
        File::Guid guid = ReflectFields->MainMenuSceneGuid;
        return guid.ToPath().string();
    }
    PROPERTY(MainMenuScene)
    GETTER(float, DialogInterval)
    {
        return ReflectFields->DialogInterval;
    }
    SETTER(float, DialogInterval)
    { 
        ReflectFields->DialogInterval = std::max(value, 0.5f);
    }
    PROPERTY(DialogInterval)
        
protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string MainMenuSceneGuid;
    float       DialogInterval = 2.0f;
    REFLECT_FIELDS_END(EndingDialogManager)

private: 
    bool                              _isDialogEnded = true;
    bool                              _isSequencePlaying = false;
    float                             _currentTimer               = 0.0f;
    size_t                            _currentDialogIndex         = 0;
    size_t                            _totalDialogCount           = 0;
    bool                              _isDialogFading             = false;
    bool                              _skipRequested              = false;
    bool                              _goToMainMenuSceneFlag = false;
    class ChildsAnimationsController* _childsAnimationsController = nullptr;
};

