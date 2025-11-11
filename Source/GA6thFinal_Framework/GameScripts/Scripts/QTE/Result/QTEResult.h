#pragma once

namespace QTE
{
    struct NoteData;

    enum ResultType
    {
        QTE_RESULT_NONE = 0,

        QTE_RESULT_ALL_CRIT         = 1 << 0,       // 전체 판정: 치명적
        QTE_RESULT_OVER_HIT         = 1 << 1,       // 전체 판정: 무결점
        QTE_RESULT_ALL_CRIT_FAIL    = 1 << 2,       // 전체 판정: 치명적 실패
        QTE_RESULT_OVER_HIT_FAIL    = 1 << 3,       // 전체 판정: 무결점 실패

        QTE_RESULT_PERFECT          = 1 << 10,      // 노트 판정: 치명타
        QTE_RESULT_NORMAL           = 1 << 11,      // 노트 판정: 일격
        QTE_RESULT_MISS             = 1 << 12,      // 노트 판정: 빗나감
    };

    struct NoteResult
    {
        using Button    = Input::Controller::Button;

        NoteResult() = default;
        NoteResult(const NoteData* note) : NoteData(note) {}

        Button          PressedButton   = Button::UNDEFINED;    // 누른 버튼 (Input::Controller::Button)
        const NoteData* NoteData        = nullptr;              // 결과에 해당하는 노트
        ResultType      Result          = QTE_RESULT_NONE;      // 결과 타입
        float           TimeDelta       = 0.0f;                 // 누른 시간과 퍼펙트 시간 차이 (초)

        // 누른 버튼이 존재하는지 여부
        inline bool IsPressedButton() const { return PressedButton != Input::Controller::Button::UNDEFINED; }

        // 유효한 결과인지 여부
        inline bool IsValidResult() const { return Result == QTE_RESULT_PERFECT || Result == QTE_RESULT_NORMAL || Result == QTE_RESULT_MISS; }

        // 히트 여부 (치명타 or 일격)
        inline bool IsHit() const { return Result == QTE_RESULT_PERFECT || Result == QTE_RESULT_NORMAL; }
    };

    struct OverallResult
    {
        OverallResult() = default;
        ~OverallResult() = default;

        int PerfectCount    = 0; // 치명타 카운트
        int NormalCount     = 0; // 일격 카운트
        int MissCount       = 0; // 빗나감 카운트
        int InvalidCount    = 0; // 유효하지 않은 타격 카운트(비어있는 공간에 대한 공격 횟수)
        int ResultFlags     = 0; // 결과 산출 비트 플래그

        std::vector<NoteResult> NoteResults; // 노트 결과 리스트

        /// <summary>ResultType 값을 ResultFlags에 포함되어 있는지 확인합니다.</summary>
        inline bool CompareResult(ResultType result) const { return (ResultFlags & result) != 0; }

        /// <summary>결과가 유효한지 확인합니다.</summary>
        inline bool IsValidResult() const
        {
            return CompareResult(QTE_RESULT_OVER_HIT) || 
                   CompareResult(QTE_RESULT_ALL_CRIT_FAIL) ||
                   CompareResult(QTE_RESULT_ALL_CRIT);
        }
        
        /// <summary>초기 상태로 되돌립니다.</summary>
        inline void Clear() 
        { 
            PerfectCount = 0;
            NormalCount  = 0;
            MissCount    = 0;
            InvalidCount = 0;
            ResultFlags  = QTE_RESULT_NONE;
            NoteResults.clear();
        }

        /// <summary>결과를 갱신합니다. PerfectCount, NormalCount, MissCount 값을 기반으로 ResultFlags을 갱신합니다.</summary>
        inline void UpdateResult()
        {
            // 유효하지 않은 적(Invalid Count)을 친 적이 있을 시에 대한 예외 처리는 하지 않는게 기획의도
            int  total   = static_cast<int>(NoteResults.size());
            bool allCrit = (PerfectCount >= total);
            bool overHit = (PerfectCount + NormalCount >= total);

            ResultFlags  = QTE_RESULT_NONE;
            ResultFlags |= allCrit ? QTE_RESULT_ALL_CRIT : QTE_RESULT_ALL_CRIT_FAIL;
            ResultFlags |= overHit ? QTE_RESULT_OVER_HIT : QTE_RESULT_OVER_HIT_FAIL;
        }
    };

    inline static const char* QTEResultToString(ResultType result)
    {
        switch (result)
        {
        case ResultType::QTE_RESULT_NONE:
            return "None";

        case ResultType::QTE_RESULT_ALL_CRIT:
            return "All Crit";
        case ResultType::QTE_RESULT_OVER_HIT:
            return "Over Hit";
        case ResultType::QTE_RESULT_ALL_CRIT_FAIL:
            return "All Crit Fail";

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