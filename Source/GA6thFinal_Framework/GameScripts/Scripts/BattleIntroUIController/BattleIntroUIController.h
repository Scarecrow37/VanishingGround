#pragma once
#include "Utility/SingletonHelper.h"
class ChildsAnimationsController;
class FadeUIComponent;

class BattleIntroUIController : public Component
{
    USING_PROPERTY(BattleIntroUIController)

public:
    BattleIntroUIController();
    ~BattleIntroUIController() override;

    /// <summary>
    /// 인트로 UI 연출을 실행합니다.
    /// </summary>
    /// <param name="stage">: 현재 스테이지</param>
    /// <param name="battleCount">: 현재 배틀 횟수</param>
    /// <returns>인트로 총 시간</returns>
    float PlayIntro(int stage, int battleCount);

public:
    REFLECT_PROPERTY(IntroTextShowTime)

    GETTER(float, IntroTextShowTime) { return ReflectFields->IntroTextShowTime; }
    SETTER(float, IntroTextShowTime) { ReflectFields->IntroTextShowTime = value; }
    PROPERTY(IntroTextShowTime)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float IntroTextShowTime = 0.5f;
    REFLECT_FIELDS_END(BattleIntroUIController)

    void Added() override;

private:
    std::weak_ptr<ChildsAnimationsController> _animationController;
    std::weak_ptr<FadeUIComponent>            _fadeUIComponent;
    SingletonComponent<BattleIntroUIController> _singletonComponent;

private:
    void FindAnimations();
};

