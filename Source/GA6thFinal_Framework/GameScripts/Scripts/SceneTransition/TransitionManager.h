#pragma once
#include "Utility/SingletonHelper.h"
class TransitionManager : public Component
{
    USING_PROPERTY(TransitionManager)

public:
    TransitionManager();
    ~TransitionManager() override;
    void SceneTransitionFade(std::string_view inPreset, std::string_view outPreset, std::function<void(void)> callback);

    bool IsTransitioning() const { return _isTransitioning; }

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TransitionManager)

    void Awake() override;

private:
    SingletonObject<TransitionManager> _singletonObject{this};
    class SceneTransitionComponent* _transitionController = nullptr;

    bool _isTransitioning = false;

};
