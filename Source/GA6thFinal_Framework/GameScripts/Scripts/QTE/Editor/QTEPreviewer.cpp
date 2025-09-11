#include "pchScripts.h"
#include "QTEPreviewer.h"
#include <QTE/System/QTESystem.h>
#include <QTE/Editor/QTEEditor.h>

void QTEPreviewer::Reset()
{
    _noteMap.clear();
    _effectTimer = 0.0f;
    auto system = QTESystem::GetInstance();
    if (system && system->GetCurrentQTETrack())
    {
        auto resultQueue = system->GetCurrentQTEResultQueue();
        for (const auto& result : resultQueue)
        {
            if (result.Note)
            {
                _noteMap[result.Note->ID] = result;
            }
        }
    }
}

void QTEPreviewer::Draw()
{
    auto* window = ImGui::GetCurrentWindow();
    if (window && window->DrawList)
    {
        auto system = QTESystem::GetInstance();
        if (system && system->GetCurrentQTETrack())
        {
            auto qteTrack = system->GetCurrentQTETrack();

            float                   circleRadius = 30.0f;
            auto*                   drawList     = window->DrawList;
            ImVec2                  offset       = ImGui::GetCursorScreenPos();
            ImVec2                  availSize    = ImGui::GetContentRegionAvail();
            std::pair<float, float> perfectRange = system->GetPerfectJudgeRange();
            std::pair<float, float> normalRange  = system->GetNormalJudgeRange();

            DrawJudgeRange(perfectRange, circleRadius, ImColor(140, 120, 170, 255));
            DrawJudgeRange(normalRange, circleRadius, ImColor(100, 255, 100, 255), ImColor(0.3f, 0.3f, 0.3f, 0.5f));
            if (_effectTimer > 0.0f)
            {
                _effectTimer -= ImGui::GetIO().DeltaTime;
                float   bgAlpha   = (_effectTimer / PERFECT_EFFECT_TIME);
                float   radius    = circleRadius * 2.0f * (1.0f - (_effectTimer / PERFECT_EFFECT_TIME));
                ImColor effectCol = _effectColor;
                effectCol.Value.w *= bgAlpha;
                DrawJudgeRange(normalRange, radius, effectCol);
            }
            else
            {
                _effectTimer = 0.0f;
            }
            auto&  resultQueue = system->GetCurrentQTEResultQueue();
            size_t noteIndex   = system->GetCurrentNoteIndex();
            for (size_t i = 0; i < noteIndex; ++i)
            {
                const auto& result = resultQueue[i];
                DrawNote(&result, circleRadius, ImColor(100, 100, 255, 255), ImColor(100, 100, 255, 100));
            }
        }
    }
}

void QTEPreviewer::PressedNote(const QTE::Result* result)
{
    _effectTimer = PERFECT_EFFECT_TIME;
    if (result->ResultType == QTE::QTE_RESULT_PERFECT)
    {
        _effectColor = PERFECT_EFFECT_COLOR;
    }
    else if (result->ResultType == QTE::QTE_RESULT_NORMAL)
    {
        _effectColor = NORMAL_EFFECT_COLOR;
    }
    else
    {
        _effectColor = MISS_EFFECT_COLOR;
    }
}

void QTEPreviewer::DrawJudgeRange(std::pair<float, float> range, float circleRadius, ImU32 judgeCol, ImU32 bgCol)
{
    auto* window = ImGui::GetCurrentWindow();
    auto* system = QTESystem::GetInstance();
    if (system && window && window->DrawList)
    {
        auto* drawList = window->DrawList;

        ImVec2 offset = ImGui::GetCursorScreenPos();
        ImVec2 availSize = ImGui::GetContentRegionAvail();
        float  centerPosFactor = system->GetJudgePosFactor(); 

        auto& [min, max] = range;
        float centerPosX = availSize.x * centerPosFactor;
        float minPosX    = centerPosX + (availSize.x * min);
        float maxPosX    = centerPosX + (availSize.x * max);

        if (bgCol != UINT_MAX - 1)
        {
            drawList->AddRectFilled(offset + ImVec2(minPosX - circleRadius, 0.0f), offset + ImVec2(maxPosX + circleRadius, availSize.y), bgCol);
        }
        
        drawList->AddCircleFilled(offset + ImVec2(minPosX, availSize.y * 0.5f), circleRadius, judgeCol);
        drawList->AddCircleFilled(offset + ImVec2(maxPosX, availSize.y * 0.5f), circleRadius, judgeCol);
        drawList->AddRectFilled(offset + ImVec2(minPosX, availSize.y * 0.5f - circleRadius), offset + ImVec2(maxPosX, availSize.y * 0.5f + circleRadius), judgeCol);
    }
}

void QTEPreviewer::DrawNote(const QTE::Result* result, float circleRadius, ImColor noteCol, ImColor bgCol)
{
    auto* window = ImGui::GetCurrentWindow();
    auto* system = QTESystem::GetInstance();
    auto  context = result ? result->Note : nullptr;
    if (system && window && window->DrawList && context)
    {
        auto*  drawList        = window->DrawList;
        ImVec2 offset          = ImGui::GetCursorScreenPos();
        ImVec2 availSize       = ImGui::GetContentRegionAvail();
        float  centerPosFactor = system->GetJudgePosFactor();
        float  centerPosX      = availSize.x * centerPosFactor;
        float  noteTime        = context->Time;
        float  timer           = system->GetQTETime();
        float  posX            = (1.0f + timer - noteTime) * centerPosX;
        if (posX > availSize.x)
        {
            return;
        }
        float alphaFactor = CalcNoteAlphaFromPositionX(posX);
        noteCol.Value.w *= alphaFactor;
        bgCol.Value.w *= alphaFactor;

        ImRect noteBgRect = ImRect(offset + ImVec2(posX - circleRadius, 0.0f), offset + ImVec2(posX + circleRadius, availSize.y));
        drawList->AddRectFilled(noteBgRect.Min, noteBgRect.Max, bgCol);
        drawList->AddCircleFilled(offset + ImVec2(posX, availSize.y * 0.5f), circleRadius, noteCol);
    }
}

float QTEPreviewer::CalcNoteAlphaFromPositionX(float posX)
{
    auto system = QTESystem::GetInstance();
    if (system)
    {
        ImVec2 availSize              = ImGui::GetContentRegionAvail();
        auto [fadeInMin, fadeInMax]   = system->GetFadeInPosFactor();
        auto [fadeOutMin, fadeOutMax] = system->GetFadeOutPosFactor();

        float alpha = 1.0f;

        float inStart  = availSize.x * fadeInMin;
        float inEnd    = availSize.x * fadeInMax;
        float outStart = availSize.x * fadeOutMin;
        float outEnd   = availSize.x * fadeOutMax;

        if (posX < inStart)
        {
            alpha = 0.0f; // 아직 페이드인 시작 전
        }
        else if (posX < inEnd)
        {
            float t = (posX - inStart) / (inEnd - inStart);
            alpha   = t; // 0 → 1 보간
        }
        else if (posX > outEnd)
        {
            alpha = 0.0f; // 이미 페이드아웃 끝남
        }
        else if (posX > outStart)
        {
            float t = (posX - outStart) / (outEnd - outStart);
            alpha   = 1.0f - t; // 1 → 0 보간
        }

        return alpha;
    }
    return 1.0f;
}