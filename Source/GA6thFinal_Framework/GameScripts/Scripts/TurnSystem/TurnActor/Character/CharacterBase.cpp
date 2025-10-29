#include "pchScripts.h"
#include "CharacterBase.h"

#include "Stats/CharacterStats.h"
#include "TurnSystem/TurnMode/TurnMode.h"

#include <Mesh/SkeletalMeshRenderer.h>
#include <Animation/AnimationComponent.h>
#include <Particle/ParticleComponent.h>

#include "Token/Object/Stun/StunToken.h"

#include "ContentMath/ContentMath.h"

REFLECT_FUNCTION(CharacterBase)

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
    _skeletalMeshRenderer = nullptr;
    _animationComponent   = nullptr;
    _particleComponent    = nullptr;

    FindComponent();
    InitAnimationCallback();
}

void CharacterBase::Start() 
{
    Base::Start();
    _tokenInventory.Initialize();
}

bool CharacterBase::FindComponent()
{
    bool valid = false;
    auto* childTransform = transform->Find(MODEL_NAME);
    if (childTransform)
    {
        GameObject& model     = childTransform->gameObject;
        _skeletalMeshRenderer = model.GetComponent<SkeletalMeshRenderer>();
        _animationComponent   = model.GetComponent<AnimationComponent>();
        _particleComponent    = model.GetComponent<ParticleComponent>();

        if (nullptr == _skeletalMeshRenderer)
        {
            std::string msg = std::format("{}{}", model.ToString(), (const char*)u8"의 컴포넌트에 SkeletalMeshRenderer가 없습니다.");
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }
        
        if (nullptr == _animationComponent)
        {
            std::string msg = std::format("{}{}", model.ToString(), (const char*)u8"의 컴포넌트에 AnimationComponent가 없습니다.");
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }

        if (nullptr == _particleComponent)
        {
            std::string msg = std::format("{}{}", model.ToString(), (const char*)u8"의 컴포넌트에 ParticleComponent가 없습니다.");
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        }

        valid = _skeletalMeshRenderer && _animationComponent && _particleComponent;
    }
    return valid;
}

void CharacterBase::InitAnimationCallback() 
{
    if (_animationComponent)
    {
        _animationComponent->SetAnimationPostEventCallback(
            [this](const Timeline::EventContext* context) { OnNotifiedAnimationEvent(context); });
    }
}

void CharacterBase::ClearState() 
{
    Base::ClearState();
    _tokenInventory.Clear();
    if (CharacterStats* stats = GetCharacterStats())
    {
        _tokenInventory.AddTokenStackFromID(TokenObject::StunResistance::ID, stats->StunResistance);    
    }
    if (_animationComponent)
    {
        _animationComponent->ClearOverrideAnimations();
        _animationComponent->SetNextAnimationFlags(ANIMATION_FLAG_USE_LOOP | ANIMATION_FLAG_RESET_FRAME | ANIMATION_FLAG_USE_BLEND);
        _animationComponent->ChangeMainAnimation("Idle");
    }
}

void CharacterBase::Revive()
{
    Base::Revive();
    if (CharacterStats* stats = GetCharacterStats())
    {
        stats->CurrentHP = stats->MaxHP;
        stats->CurrentChainCount = stats->MaxChainRoundCount;
    }
}

void CharacterBase::Dead()
{
    Base::Dead();
    if (CharacterStats* stats = GetCharacterStats())
    {
        stats->CurrentHP = 0;
    }
    _tokenInventory.NotifyDead();
}

void CharacterBase::Heal(int amount) 
{
    if (CharacterStats* stats = GetCharacterStats())
    {
        stats->CurrentHP += amount;

        std::string msg = std::format("{}{}{}{}",
            gameObject->ToString(),
            (const char*)u8"체력이 ",
            amount,
            (const char*)u8" 회복"
        );
        UmLogger.Message(LogLevel::LEVEL_DEBUG, msg);
    }
}

void CharacterBase::HealByPercentage(int percentage) 
{
    if (CharacterStats* stats = GetCharacterStats())
    {
        const int maxHP = stats->MaxHP;
        const int healAmount = ContentMath::CeilPercentage(maxHP, percentage);
        Heal(healAmount);
    }
}

void CharacterBase::TakeDamage(int damage, bool playAnim) 
{
    CharacterStats* stats = GetCharacterStats();
    if (stats)
    {
        stats->CurrentHP -= damage;
        stats->CurrentHP  = std::clamp((int)stats->CurrentHP, 0, (int)stats->MaxHP);
        GameObject& owner = gameObject;
        std::string msg   = std::format("{}{} {}{}", owner.ToString(), (const char*)u8"이(가)", damage,
                                        (const char*)u8"의 피해를 입었습니다.");
        UmLogger.Message(LogLevel::LEVEL_TRACE, msg);
        OnHit();
    }
    if (false == IsDead())
    {
        if (playAnim && _animationComponent)
        {
            _animationComponent->BeginBuildOverrideAnimation();
            _animationComponent->SetNextAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
            // HitAnimation이 이미 있다면 Pop
            const auto& animData = _animationComponent->GetTopAnimationData();
            const char* currentAnimName = animData.GetAnimationName().c_str();
            if (currentAnimName == _animationComponent->GetAnimationNameFromKey("Hit"))
            {
                _animationComponent->PopOverrideAnimation();
            }
            bool pushResult = _animationComponent->PushBackOverrideAnimation("Hit");
            if (pushResult)
            {
                _animationComponent->SetCurrentAnimationPopCondition(
                    [](const AnimationData& data) { return data.IsEnd(); }); // 애니메이션이 끝날 경우 Pop
            }
            _animationComponent->EndBuildOverrideAnimation();
        }
    }
    ShowDamage(damage, {}); // TODO: 다른 방법을 찾아야함.
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

void CharacterBase::OnQTEStart() 
{
    Base::OnQTEStart();
    _tokenInventory.NotifyQTEStart();
}

void CharacterBase::OnQTEEnd() 
{
    Base::OnQTEEnd();
    _tokenInventory.NotifyQTEEnd();
}

void CharacterBase::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
{
}

void CharacterBase::ImGuiDrawPropertysEvent() 
{
    Base::ImGuiDrawPropertysEvent();
    ImGui::Separator();
    if (ImGui::TreeNodeEx("Token##enemy component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID(&_tokenInventory);
        _tokenInventory.DrawImGuiDebugData();
        ImGui::PopID();
        ImGui::TreePop();
    }
}