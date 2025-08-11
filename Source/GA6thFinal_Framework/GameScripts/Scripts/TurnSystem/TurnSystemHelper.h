#pragma once
#include <vector>
#include <TurnSystem/TurnAction/TurnActionEnums.h>

class CharacterBase;

namespace TurnSystemHelper
{
    /// <summary>
    /// 전달받은 대상에 대한 설명 문자열을 반환합니다.
    /// </summary>
    /// <param name="target :">대상</param>
    /// <returns></returns>
    inline constexpr std::u8string_view GetTurnTargetToolTip(TurnTarget target)
    {
        switch (target)
        {
        case TurnTarget::SELF:
            return u8"마지막으로 공격한 캐릭터";
        case TurnTarget::PLAYER:
            return u8"플레이어";
        case TurnTarget::ENEMY:
            return u8"마지막으로 공격 당한 적";
        case TurnTarget::ALL_ENEMIES:
            return u8"모든 적";
        case TurnTarget::ALL:
            return u8"모든 캐릭터";
        default:
            return u8"알수 없는 타입";
        }
    };

    /// <summary>
    /// enum 값에 따른 대상 항목을 반환합니다. empty인 백터를 반환할 수 있습니다.
    /// </summary>
    /// <param name="target :">대상</param>
    /// <returns></returns>
    std::vector<CharacterBase*> GetTargetCharacters(TurnTarget target);

};

namespace TurnSystemHelper
{
    /// <summary>
    /// ImGui로 TurnTarget을 편집 가능한 Combobox를 Draw합니다.
    /// </summary>
    /// <param name="target :">수정 가능한 변수</param>
    /// <param name="showComboTitle :">콤보 박스 제목 표시 여부</param>
    /// <returns>이번 프레임 편집 여부</returns>
    bool DrawTargetComboboxWithToolTip(TurnTarget& target, bool showComboTitle = false);
};