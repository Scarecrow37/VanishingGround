#pragma once

namespace QTE
{
    class Note;

    enum ResultType
    {
        QTE_RESULT_NONE = 0,
        QTE_RESULT_PERFECT,
        QTE_RESULT_NORMAL,
        QTE_RESULT_MISS
    };

    struct Result
    {
        Result() = default;
        Result(Note* note) : Note(note) {}
        unsigned int    PressedButton = 0;
        Note*           Note        = nullptr;              // 결과에 해당하는 노트
        ResultType      ResultType  = QTE_RESULT_NONE;      // 결과 타입
        float           TimeDelta   = 0.0f;                 // 누른 시간과 퍼펙트 시간 차이 (초)

        inline bool IsPressedButton() const { return PressedButton != 0; }
        inline bool IsValidResult() const { return ResultType != QTE_RESULT_NONE; }
    };

    inline static const char* QTEResultToString(ResultType result)
    {
        switch (result)
        {
        case ResultType::QTE_RESULT_NONE:
            return "None";
        case ResultType::QTE_RESULT_PERFECT:
            return "Perfect";
        case ResultType::QTE_RESULT_NORMAL:
            return "Normal";
        case ResultType::QTE_RESULT_MISS:
            return "Miss";
        default:
            return "Unknown";
        }
    }
}


