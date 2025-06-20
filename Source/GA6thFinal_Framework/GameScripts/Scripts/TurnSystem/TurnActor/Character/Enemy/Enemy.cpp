#include "Enemy.h"

Enemy::Enemy()
{
    MaxHP      = 100;
    MaxMP      = 100;
    ChainCount = 0;
    Speed      = 0;
}

Enemy::~Enemy() = default;

void Enemy::Awake() 
{
    Base::Awake();
    gameObject->AddTag(TAG);
}

void Enemy::Update() 
{
    bool isMyTurn = IsMyTurn;
    if (isMyTurn)
    {
        Vector3 delta = Vector3(0, 1080, 0) * Mathf::Deg2Rad * UmTime.DeltaTime();
        gameObject->transform->Rotation *= Quaternion::CreateFromYawPitchRoll(delta);
    }
}

void Enemy::OnTurnStart() 
{
    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 시작.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);

    message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료 3.");
    UmTime.Invoke(this, 1.f, [=]() { UmLogger.Message(LogLevel::LEVEL_TRACE, message); });

    message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료 2.");
    UmTime.Invoke(this, 2.f, [=]() { UmLogger.Message(LogLevel::LEVEL_TRACE, message); });

    message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료 1.");
    UmTime.Invoke(this, 3.f, [=]() 
    {
        UmLogger.Message(LogLevel::LEVEL_TRACE, message);
    });

    UmTime.Invoke(this, 4.f, [=](){ this->MyTurnEnd(); });
}

void Enemy::OnTurnEnd() 
{
    static std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);
}

int Enemy::GetSpeed()
{
    return Speed;
}
