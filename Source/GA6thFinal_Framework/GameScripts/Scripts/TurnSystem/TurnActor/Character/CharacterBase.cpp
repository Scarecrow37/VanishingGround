#include "pchScripts.h"
#include "CharacterBase.h"
#include "Stats/CharacterStats.h"
#include "TurnSystem/TurnMode/TurnMode.h"

#include <Mesh/SkeletalMeshRenderer.h>
#include <Animation/AnimationComponent.h>


int CharacterBase::GetHP()
{
    CharacterStats* stats = GetCharacterStats();
    if (stats)
    {
        return stats->CurrentHP;
    }
    return 0;
}

int CharacterBase::GetChainCount()
{
    CharacterStats* stats = GetCharacterStats();
    if (stats)
    {
        return stats->CurrentChainCount;
    }
    return 0;
}

int CharacterBase::GetMaxHP()
{
    int             maxHP = 0;
    CharacterStats* stats = GetCharacterStats();
    if (nullptr != stats)
    {
        maxHP = stats->MaxHP;
    }
    return maxHP;
}

int CharacterBase::GetChainRoundCount()
{
    CharacterStats* stats = GetCharacterStats();
    if (stats)
    {
        return stats->CurrentChainRoundCount;
    }
    return 0;
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

int CharacterBase::GetStunResistance()
{
    int stunResistance = 1;
    CharacterStats* stats = GetCharacterStats();
    if (nullptr != stats)
    {
        stunResistance = stats->StunResistance;
    }
    return stunResistance;
}

CharacterBase::CharacterBase() : 
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
        _animationComponent     = modelObject.GetComponent<AnimationComponent>();
        if (nullptr == _skeletalMeshRenderer)
        {
            std::string msg = std::format("{}{}",
                modelObject.ToString(),
                (const char*)u8"의 컴포넌트에 SkeletalMeshRenderer가 없습니다."
            );
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
        if (nullptr == _animationComponent)
        {
            std::string msg = std::format("{}{}",
                modelObject.ToString(),
                (const char*)u8"의 컴포넌트에 AnimationComponent가 없습니다."
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

void CharacterBase::ClearState() 
{
    Base::ClearState();
    _tokenInventory.Clear();
    CharacterStats* stats = GetCharacterStats();
    if (stats)
    {
        stats->CurrentHP                = MaxHP;
        stats->CurrentChainCount        = 0;
        stats->CurrentChainRoundCount   = MaxChainRoundCount;
        _tokenInventory.AddTokenStackFromID(16008, stats->StunResistance);
    }
    if (_animationComponent)
    {
        _animationComponent->ClearOverrideAnimations();
        SetMainAnimation(CharacterBase::IDLE, true, true);
    }
}

void CharacterBase::Revive()
{
    Base::Revive();
    CharacterStats* stats = GetCharacterStats();
    if (stats)
    {
        stats->CurrentHP = stats->MaxHP;
    }
}

void CharacterBase::Dead()
{
    Base::Dead();
    CharacterStats* stats = GetCharacterStats();
    if (stats)
    {
        stats->CurrentHP = 0;
    }
    _tokenInventory.NotifyDead();
}

void CharacterBase::TakeDamage(int damage) 
{
    if (TurnActor::STATE::Dead == GetActorState())
    {
        GameObject& owner = gameObject;
        std::string msg = std::format("{}{}", owner.ToString(), (const char*)u8" 대한 공격 빗나감.");
        UmLogger.Message(LogLevel::LEVEL_DEBUG, msg);
        return;
    }
       
    CharacterStats* stats = GetCharacterStats();
    if (stats)
    {
        stats->CurrentHP -= damage;
        stats->CurrentHP = std::clamp((int)stats->CurrentHP, 0, (int)stats->MaxHP);
        GameObject& owner = gameObject;
        std::string msg = std::format("{}{} {}{}", owner.ToString(), (const char*)u8"이(가)", damage,
            (const char*)u8"의 피해를 입었습니다.");
        UmLogger.Message(LogLevel::LEVEL_DEBUG, msg);
    }
    if (_animationComponent)
    {
        const auto& animData        = _animationComponent->GetLastAnimationData();
        const char* currentAnimName = animData.GetAnimationName().c_str();
        const char* hitAnimName     = GetAnimationName(CharacterBase::HIT);
        if (0 == strcmp(currentAnimName, hitAnimName))
        {
            _animationComponent->PopOverrideAnimation();
        }
        _animationComponent->PushOverrideAnimation(hitAnimName, true,
            [](const AnimationData& data) { return data.IsEnd(); });
    }
}

void CharacterBase::TakeChain(int chainDamage) 
{
    if (TurnActor::STATE::Dead == GetActorState())
        return;

    CharacterStats* stats = GetCharacterStats();
    if (stats)
    {
        int chainCount = stats->CurrentChainCount;
        SetChainCount(chainCount + chainDamage);
    }
}

int CharacterBase::SetChainCount(int value)
{
    CharacterStats* stats = GetCharacterStats();
    if (stats)
    {
        stats->CurrentChainCount = value;
        return stats->CurrentChainCount;
    }
    return 0;
}

int CharacterBase::DecrementChainRoundCount()
{
    CharacterStats* stats = GetCharacterStats();
    if (stats)
    {
        stats->CurrentChainRoundCount = std::clamp((int)stats->CurrentChainRoundCount - 1, 0, (int)stats->MaxChainRoundCount);
        int chainRoundCount = stats->CurrentChainRoundCount;
        if (chainRoundCount == 0)
        {
            stats->CurrentChainCount = 0;
            stats->CurrentChainRoundCount = stats->MaxChainRoundCount;
        }     
        return stats->CurrentChainRoundCount;
    }
    return 0;
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

void CharacterBase::SetMainAnimation(AnimationType type, int flags, bool blend)
{
    if (_animationComponent)
    {
        const char* animKey = GetAnimationName(type);
        _animationComponent->ChangeMainAnimation(animKey, blend);
        _animationComponent->ChangeMainAnimationFlags(flags);
    }
}

void CharacterBase::ClearOverrideAnimations()
{
    if (_animationComponent)
    {
        _animationComponent->ClearOverrideAnimations();
    }
}

bool CharacterBase::IsAnimationEnd()
{
    if (_animationComponent)
    {
        const auto& data = _animationComponent->GetLastAnimationData();
        return data.IsEnd();
    }
    return true;
}