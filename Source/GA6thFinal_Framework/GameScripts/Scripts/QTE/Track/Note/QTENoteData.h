#pragma once

namespace QTE
{
    struct NoteData
    {
        UINT        ID                   = UINT_MAX;
        float       Time                 = 0.0f; // 노트 시간 (초)
        std::string WeaponAnimationKey   = "";
        float       WeaponAnimationDelay = 0.0f; // 무기 애니메이션 재생 딜레이 (초)
    };
}
