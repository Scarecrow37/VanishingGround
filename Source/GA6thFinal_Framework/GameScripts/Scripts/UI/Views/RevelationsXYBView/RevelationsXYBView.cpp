#include "pchScripts.h"
#include "RevelationsXYBView.h"
#include "UI/Elements/Image/ImageElement.h"
#include "RevelationSystem/RevelationSystem.h"
#include "Monster/System/MonsterSystem.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/CharacterStats.h"
#include "TurnSystem/TurnAction/Condition/ChainCondition/ChainCondition.h"
#include "TutorialSystem/TutorialSystem.h"

UMREAL_COMPONENT(RevelationsXYBView)

RevelationsXYBView::RevelationsXYBView() = default;
RevelationsXYBView::~RevelationsXYBView() = default;

void RevelationsXYBView::Awake() 
{
    FindElements();
}

void RevelationsXYBView::Start()
{
    _watchHandle = UmWatcher.Watch<WeaponViewModel, WeaponUIData>("Weapon", [this](const WeaponUIData& value) 
    { 
        UpdateUI();
    });
    _disable.SetEnable(true);
    _enable.SetEnable(false);
}

void RevelationsXYBView::FindElements() 
{
    _disable = UiImages{};
    _enable  = UiImages{};
    Transform::ForeachBFS(transform, [this](Transform* curr)
    { 
        GameObject& object = curr->gameObject;
        if (object.CompareTag("Disable X"))
        {
            _disable.X = object.GetComponent<ImageElement>();
        }
        else if (object.CompareTag("Disable Y"))
        {
            _disable.Y = object.GetComponent<ImageElement>();
        }
        else if (object.CompareTag("Disable B"))
        {
            _disable.B = object.GetComponent<ImageElement>();
        }
        else if (object.CompareTag("Enable X"))
        {
            _enable.X = object.GetComponent<ImageElement>();
        }
        else if (object.CompareTag("Enable Y"))
        {
            _enable.Y = object.GetComponent<ImageElement>();
        }
        else if (object.CompareTag("Enable B"))
        {
            _enable.B = object.GetComponent<ImageElement>();
        }
    });
}

void RevelationsXYBView::UiImages::SetEnable(bool enable) 
{
    if (X)
    {
        X->Enable = enable;
    }
    if (Y)
    {
        Y->Enable = enable;
    }
    if (B)
    {
        B->Enable = enable;
    }
}

void RevelationsXYBView::UiImages::SetLeftEnable(bool enable) 
{
    if (X)
    {
        X->Enable = enable;
    }
}

void RevelationsXYBView::UiImages::SetMiddleEnable(bool enable) 
{
    if (Y)
    {
        Y->Enable = enable;
    }
}

void RevelationsXYBView::UiImages::SetRightEnable(bool enable) 
{
    if (B)
    {
        B->Enable = enable;
    }
}

void RevelationsXYBView::UpdateUI()
{
    int revelationIndex = RevelationIndex;
    if (RevelationSystem* revelationSystem = SingletonComponent<RevelationSystem>::GetInstance())
    {
        if (MonsterSystem* monsterSystem = SingletonComponent<MonsterSystem>::GetInstance())
        {
            const auto& roundElements = revelationSystem->GetRoundElementList();
            if (revelationIndex < roundElements.size())
            {
                const auto& revelation = roundElements[revelationIndex];
                if (revelation->IsAction())
                {
                    auto& action         = revelation->GetAction();
                    auto& conditions     = action.GetConditions();
                    auto  CheckCondition = [&action, &conditions](CharacterBase* character) 
                    {
                        if (character)
                        {
                            if (character->IsDead())
                            {
                                return false;
                            }
                        }

                        if (true == conditions.empty())
                        {
                            return true;
                        }

                        TurnAction::ConditionOperator oper = action.LogicOperator;
                        switch (oper)
                        {
                        case TurnAction::ConditionOperator::AND:
                            for (auto& condition : conditions)
                            {
                                if (condition)
                                {
                                    if (false == condition->CheckEvaluate(character))
                                    {
                                        return false;
                                    }
                                }
                            }
                            return true;
                        case TurnAction::ConditionOperator::OR:
                            for (auto& condition : conditions)
                            {
                                if (condition)
                                {
                                    if (true == condition->CheckEvaluate(character))
                                    {
                                        return true;
                                    }
                                }
                            }
                            return false;
                        default:
                            return false;
                        }
                    };
                    
                    //왼쪽 적
                    bool showTutorial = false;
                    bool result = false; // 판단 결과
                    if (auto leftEnemy = monsterSystem->GetSpawnedEnemyFromSpawnPoint(Monster::SpawnPoint::Left).lock())
                    {
                        result = CheckCondition(leftEnemy.get());
                        showTutorial |= result;
                    }
                    _disable.SetLeftEnable(!result);
                    _enable.SetLeftEnable(result);            

                    //가운데 적
                    result = false;
                    if (auto middleEnemy = monsterSystem->GetSpawnedEnemyFromSpawnPoint(Monster::SpawnPoint::Middle).lock())
                    {
                        result = CheckCondition(middleEnemy.get());
                        showTutorial |= result;
                    }
                    _disable.SetMiddleEnable(!result);
                    _enable.SetMiddleEnable(result);    

                    //오른쪽 적
                    result = false;
                    if (auto rightEnemy = monsterSystem->GetSpawnedEnemyFromSpawnPoint(Monster::SpawnPoint::Right).lock())
                    {
                        result = CheckCondition(rightEnemy.get());
                        showTutorial |= result;
                    }
                    _disable.SetRightEnable(!result);
                    _enable.SetRightEnable(result);    

                    //튜토리얼
                    if (showTutorial)
                    {
                        if (TutorialSystem* system = SingletonComponent<TutorialSystem>::GetInstance())
                        {
                            system->Show(805904); //계시 튜토리얼
                        }
                    }
                }
                else
                {
                    _disable.SetEnable(true);
                    _enable.SetEnable(false);
                }
            }
        }
    }
}
