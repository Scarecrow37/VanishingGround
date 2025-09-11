#pragma once

namespace QTE
{
    class Track;
    class Note;
    struct Result;
}

class QTEPreviewer
{
    struct NoteDrawData
    {
        const QTE::Result* Result;
    };

public:
    static void Reset();
    static void Draw();

    static void PressedNote(const QTE::Result* result);

private:
    static void  DrawJudgeRange(std::pair<float, float> range, float circleRadius, ImU32 judgeCol, ImU32 bgCol = UINT_MAX - 1);
    static void  DrawNote(const QTE::Result* result, float circleRadius, ImColor noteCol, ImColor bgCol);
    static float CalcNoteAlphaFromPositionX(float posX);

private:
    static inline std::unordered_map<int, QTE::Result> _noteMap;                        // 노트 판정 
    static inline float                     _effectTimer = 0.0f;                        // 퍼펙트 시 나오는 이펙트 타이머
    static inline ImColor                   _effectColor = ImColor(255, 255, 255, 200); // 퍼펙트 시 나오는 이펙트 색상

    static constexpr float      PERFECT_EFFECT_TIME = 0.5f; // 퍼펙트 시 나오는 이펙트 시간
    static constexpr ImColor    PERFECT_EFFECT_COLOR = ImColor(100, 255, 255, 100);
    static constexpr ImColor    NORMAL_EFFECT_COLOR = ImColor(255, 255, 100, 100);
    static constexpr ImColor    MISS_EFFECT_COLOR = ImColor(50, 50, 50, 100);
};
