#include "pchScripts.h"
#include "CharacterBase.h"
#include "Stats/CharacterStats.h"
#include "TurnSystem/TurnMode/TurnMode.h"

#include <Mesh/SkeletalMeshRenderer.h>

int CharacterBase::GetMaxHP()
{
    int maxHP = 0;
    CharacterStats* stats = GetCharacterStats();
    if (nullptr != stats)
    {
        maxHP = stats->MaxHP;
    }
    return maxHP;
}

int CharacterBase::GetMaxMP()
{
    int maxMP = 0;
    CharacterStats* stats = GetCharacterStats();
    if (nullptr != stats)
    {
        maxMP = stats->MaxMP;
    }
    return maxMP;
}

int CharacterBase::GetMaxChainRoundCount()
{
    int maxChainCount = 1;
    CharacterStats* stats = GetCharacterStats();
    if (nullptr != stats)
    {
        maxChainCount = stats->MaxChainRoundCount;
    }
    return maxChainCount;
}

CharacterBase::CharacterBase() : 
    _hp(0), 
    _chainCount(0) , 
    _chainRoundCount(1) ,
    _tokenInventory(this)
{
}

CharacterBase::~CharacterBase() = default;

void CharacterBase::Awake() 
{
    Base::Awake();
    gameObject->AddTag(TAG);

    InitMeshModel();
}

void CharacterBase::InitMeshModel()
{
    auto* modelTransform = transform->Find(MODEL_NAME);
    if (modelTransform)
    {
        GameObject& modelObject = modelTransform->gameObject;
        _skeletalMeshRenderer   = modelObject.GetComponent<SkeletalMeshRenderer>();
        if (nullptr == _skeletalMeshRenderer)
        {
            std::string msg = std::format("{}{}",
                modelObject.ToString(),
                (const char*)u8"의 컴포넌트에 SkeletalMeshRenderer가 없습니다."
            );
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
    }
    else
    {
        std::string msg = std::format("{}{} {}{}",
            gameObject->ToString(), 
            (const char*)u8"의 자식 오브젝트에",
            MODEL_NAME, 
            (const char*)u8"이(가) 없습니다."
        );
        UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
    }
}

void CharacterBase::Revive() 
{
    Base::Revive();
    _hp = MaxHP;
}

void CharacterBase::Dead()
{
    Base::Dead();
    _hp = 0;
}

void CharacterBase::TakeDamage(int damage) 
{
    _hp -= damage;
    _hp = std::clamp(_hp, 0, (int)MaxHP);
    GameObject& owner = gameObject;
    std::string msg =
        std::format("{}{} {}{}", 
            owner.ToString(),
            (const char*)u8"이(가)",
            damage,
            (const char*)u8"의 피해를 입었습니다.");
    UmLogger.Log(LogLevel::LEVEL_DEBUG, msg);
}

int CharacterBase::DecrementChainRoundCount()
{
    _chainRoundCount = std::clamp(_chainRoundCount - 1, 0, GetMaxChainRoundCount());
    if (_chainRoundCount == 0)
    {
        _chainCount      = 0;
        _chainRoundCount = GetMaxChainRoundCount();
    }
    return _chainRoundCount;
}


void CharacterBase::OnCombatStart() 
{
    _tokenInventory.NotifyCombatStart();
}

void CharacterBase::OnRoundStart()
{
    Base::OnRoundStart();
    DecrementChainRoundCount();
    _tokenInventory.NotifyRoundStart();
}

void CharacterBase::OnRoundEnd()
{
    // End먼저? 아니면 이벤트 먼저?
    Base::OnRoundEnd();
    _tokenInventory.NotifyRoundEnd();
}

void CharacterBase::OnEachTurnStart(CharacterBase* destination)
{
    Base::OnEachTurnStart(destination);
    _tokenInventory.NotifyEachTurnStart(destination);
}

void CharacterBase::OnTurnStart()
{
    Base::OnTurnStart();
    _tokenInventory.NotifyTurnStart();
}

void CharacterBase::OnTurnEnd() 
{
    Base::OnTurnEnd();
    _tokenInventory.NotifyTurnEnd();
}

void CharacterBase::OnHit() 
{
    Base::OnHit();
    _tokenInventory.NotifyHit();
}

void CharacterBase::OnDead() 
{
    Base::OnDead();
    _tokenInventory.NotifyDead();
}

void CharacterBase::OnKill(CharacterBase* destination) 
{
    Base::OnKill(destination);
    _tokenInventory.NotifyKill(destination);
}

void CharacterBase::OnTokenAdded(int tokenID) 
{
    Base::OnTokenAdded(tokenID);
    _tokenInventory.NotifyTokenAdded(tokenID);
}

void CharacterBase::OnTokenRemoved(int tokenID) 
{
    Base::OnTokenRemoved(tokenID);
    _tokenInventory.NotifyTokenRemoved(tokenID);
}

void CharacterBase::ImGuiDrawPropertysEvent() 
{
    ImGui::Separator();
    _tokenInventory.DrawImGuiDebugData();
}
