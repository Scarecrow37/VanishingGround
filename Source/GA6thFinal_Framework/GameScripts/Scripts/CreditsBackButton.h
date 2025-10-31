#pragma once

class CreditsBackButton : public UISFXNavigationComponent
{
    USING_PROPERTY(CreditsBackButton)

public:
    CreditsBackButton();
    ~CreditsBackButton() override;

public:
    void Submit() override;
    void FocusIn(FocusCallType callType) override;

private:
    void TransitionToMainMenuScene();

public:
    REFLECT_PROPERTY(NextScene)
    GETTER_ONLY(std::string, NextScene) { return File::Guid(ReflectFields->NextSceneGuid).ToPath().string(); }
    PROPERTY(NextScene)
protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    std::string NextSceneGuid;
    REFLECT_FIELDS_END(CreditsBackButton)

};

