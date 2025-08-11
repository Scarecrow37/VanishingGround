#pragma once

/// <summary>
/// 액션을 수행할 대상입니다.
/// </summary>
enum class TurnTarget
{
    SELF,           // 마지막으로 공격한 캐릭터
    PLAYER,         // 플레이어
    ENEMY,          // 마지막으로 공격당한 적
    ALL_ENEMIES,    // 모든 적
    ALL             // 모든 캐릭터
};