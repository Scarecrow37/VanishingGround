#include "pchScripts.h"
#include "QTEUIManager.h"
#include <QTE/System/QTESystem.h>
#include <QTE/Track/QTETrack.h>
#include <UI/Panels/Overlay/OverlayPanel.h>
#include <UI/Elements/Image/ImageElement.h>
#include <UI/Elements/HoldingProgressImage/HoldingProgressImageElement.h>
#include <Camera/CameraComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"
#include <Monster/Common/MonsterCommon.h>

#include "ContentMath/ContentMath.h"

UMREAL_COMPONENT(QTEUIManager)

QTEUIManager::QTEUIManager() = default;
QTEUIManager::~QTEUIManager() = default;

void QTEUIManager::OnQTEEnter() 
{
    _fieldUI.Active(true);
    _guideUI.Active(true);
    _inputViewerUI.Active(true);
    _fieldUI.OnQTEEnter();
    _mainFader.SetFadeMode(Fader::FADE_IN);
    if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
    {
        const auto& noteQueue  = system->GetCurrentQTEAvailQueue();
        bool        validRange = noteQueue.size() <= _fieldUI.NotePool.size();
        assert(validRange && "노트 풀의 사이즈가 작습니다.");
        if (validRange)
        {
            for (int i = 0; i < noteQueue.size(); ++i)
            {
                UINT id = noteQueue[i].ID;
                if (_fieldUI.NotePool[i].TrySetup(noteQueue[i].Time))
                {
                    _activedPoolIndices[id] = i;
                }
            }
        }
    }
}

void QTEUIManager::OnQTEButtonPressed()
{
    _fieldUI.OnButtonPressed();
}

void QTEUIManager::OnQTENotePressed(const UINT noteID, const QTE::NoteResult& result)
{
    if (result.IsValidResult())
    {
        int index = GetIndexFromNoteID(noteID);
        if (index >= 0)
        {
            auto& noteUI = _fieldUI.NotePool[index];
            auto& effectUI = _fieldUI.EffectPool[index];

            noteUI.OnNotePressed(result);
            _inputViewerUI.OnNotePressed(result);
            if (effectUI.Overlay)
            {
                const SIZE size = effectUI.Overlay->Size;
                float      posX = static_cast<float>(-size.cx / 2);
                if (result.Result == QTE::QTE_RESULT_MISS || result.Result == QTE::QTE_RESULT_NORMAL)
                {
                    if (noteUI.Overlay)
                    {
                        const POINT point = noteUI.Overlay->CenterPoint;
                        posX += static_cast<float>(point.x);
                    }
                }
                else if (result.Result == QTE::QTE_RESULT_PERFECT)
                {
                    const POINT point  = _fieldUI.JudgeNote->CenterPoint;
                    posX += static_cast<float>(point.x);
                }
                effectUI.OnNotePressed(result, posX);
            }
        }
    }
}

void QTEUIManager::OnQTEPlay()
{
    if (_overlayPanel && _fieldUI.Overlay && _fieldUI.JudgeNote)
    {
        _fieldUI.Update();
        if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
        {
            const float travelTime  = system->NoteTravelTime;
            const float currTime    = system->CurrentTrackTime;
            const float currSpeed   = system->ScaledSpeedFactor;
            const POINT overlayPoint= _fieldUI.Overlay->Point;
            const SIZE  overlaySize = _fieldUI.Overlay->Size;
            const POINT judgeCenter = _fieldUI.JudgeNote->CenterPoint;

            const float perfectX    = static_cast<float>(judgeCenter.x);
            const float startX      = static_cast<float>(overlayPoint.x);
            const float endX        = static_cast<float>(overlaySize.cx);


            // 페이드 x좌표 구하기... 일단 하드코딩
            float fadeInStartX = system->GetCurrentFadeState().FadeInStartXFactor;
            float fadeInEndX   = system->GetCurrentFadeState().FadeInEndXFactor;
            fadeInStartX *= static_cast<float>(overlaySize.cx);
            fadeInStartX += startX;
            fadeInEndX *= static_cast<float>(overlaySize.cx);
            fadeInEndX += startX;

            float fadeOutStartX = system->GetCurrentFadeState().FadeOutStartXFactor;
            float fadeOutEndX   = system->GetCurrentFadeState().FadeOutEndXFactor;
            fadeOutEndX *= static_cast<float>(overlaySize.cx);
            fadeOutEndX += startX;
            fadeOutStartX *= static_cast<float>(overlaySize.cx);
            fadeOutStartX += startX;
            for (auto& [id,_] : _activedPoolIndices)
            {
                int index = GetIndexFromNoteID(id);
                if (index >= 0)
                {
                    _fieldUI.NotePool[index].Update(currTime, travelTime, currSpeed,
                                                    startX, endX, perfectX, 
                                                    fadeInStartX, fadeInEndX, 
                                                    fadeOutStartX, fadeOutEndX, 
                                                    0.0f);
                }
            }
        }
    }
}

void QTEUIManager::OnQTEExit() 
{
    _mainFader.SetFadeMode(Fader::FADE_OUT);
}

void QTEUIManager::SetQTEProgress(float t) 
{
    SetUIAlpha(t);
    if (_battleGuideUI.Progress)
    {
        _battleGuideUI.Progress->SetElapsedTime(t);
    }
}

bool QTEUIManager::DragDropEvent(File::Guid& out)
{
    bool result = false;
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
        {
            DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
            const File::Path&    path      = data->GetPath();
            const File::Path     extension = path.extension();
            if (extension == L".UmPrefab")
            {
                result = true;
                out    = path.ToGuid();
            }
            else
            {
                UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"프리팹은 .UmPrefab 파일만 지정할 수 있습니다.");
            }
        }
        ImGui::EndDragDropTarget();
    }
    return result;
}

void QTEUIManager::Reset() 
{
    _singletoneComponent.SetSingleTon();
    NotePrefab.SetInputAutoEvent([this]() {
        File::Guid dragDropGuid;
        if (DragDropEvent(dragDropGuid))
        {
            ReflectFields->NotePrefabGuid = dragDropGuid.string();
        }
    });
    EffectPrefab.SetInputAutoEvent([this]() {
        File::Guid dragDropGuid;
        if (DragDropEvent(dragDropGuid))
        {
            ReflectFields->EffectPrefabGuid = dragDropGuid.string();
        }
    });
    ButtonPrefab.SetInputAutoEvent([this]() {
        File::Guid dragDropGuid;
        if (DragDropEvent(dragDropGuid))
        {
            ReflectFields->ButtonPrefabGuid = dragDropGuid.string();
        }
    });
}

void QTEUIManager::Awake() 
{
    _singletoneComponent.TrySingleTon();
}

void QTEUIManager::Start()
{
    _mainFader.SetDuration(1.0f);
    _mainFader.SetFadeInType(Mathf::EASE_IN, Mathf::SINE);
    _mainFader.SetFadeOutType(Mathf::EASE_OUT, Mathf::SINE);
    _mainFader.SetOnFadeInEndCallback([this]() {
        if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
        {
            system->ProcessQTEFadeInEndEvent();
            _mainFader.SetFadeMode(Fader::FADE_NONE);

            _battleGuideUI.Alpha(0.0f);
        }
    });
    _mainFader.SetOnFadeOutEndCallback([this]() {
        if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
        {
            system->ProcessQTEFadeOutEndEvent();
            ResetUI();
            _mainFader.SetFadeMode(Fader::FADE_NONE);
            _backGroundUI.Alpha(0.0f);
            _fieldUI.Active(false);
            _guideUI.Active(false);
            _inputViewerUI.Active(false);

            SetUIAlpha(0.0f);
            _battleGuideUI.Alpha(1.0f);
        }
    });
    FindUIComponents();
    size_t poolSize = static_cast<size_t>(ReflectFields->PoolSize);
    _fieldUI.Initialize(ReflectFields->NotePrefabGuid, ReflectFields->EffectPrefabGuid, poolSize);
    _inputViewerUI.Initialize(ReflectFields->ButtonPrefabGuid, poolSize);
    SetDefaultState();
}

void QTEUIManager::Update() 
{
    float factor = _mainFader.Fade();
    auto  mode   = _mainFader.GetFadeMode();
    switch (mode)
    {
    case Fader::FADE_NONE:
        break;
    case Fader::FADE_IN: {
        _guideUI.Alpha(std::min(factor, 0.8f));
        _battleGuideUI.Alpha(1.0f - factor);
        break;
    }
    case Fader::FADE_OUT: {
        SetUIAlpha(factor);
        _guideUI.Alpha(std::min(factor, 0.8f));
        _battleGuideUI.Alpha(1.0f - factor);
        break;
    }
    default:
        break;
    }
}

void QTEUIManager::SerializedReflectEvent() 
{
}

void QTEUIManager::DeserializedReflectEvent()
{
}

void DebugDrawValidLine(POINT panelPoint, SIZE panelSize, float pointX, FXMVECTOR color)
{
    {
        const XMFLOAT2 start{/*static_cast<float>(panelPoint.x) + */pointX, static_cast<float>(panelPoint.y)};
        const XMFLOAT2 end{/*static_cast<float>(panelPoint.x) + */pointX, static_cast<float>(panelPoint.y + panelSize.cy)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&start), XMLoadFloat2(&end), color);
    }
}

void QTEUIManager::ImGuiDrawPropertysEvent() 
{
    if (ImGui::Button((const char*)u8"새로고침"))
    {
        FindUIComponents();
    }

    DrawDebugJudgeLine();
}

void QTEUIManager::ResetUI()
{
    _fieldUI.Reset();
    _inputViewerUI.Reset();
    _activedPoolIndices.clear();
}

int QTEUIManager::GetIndexFromNoteID(UINT id)
{
    if (_activedPoolIndices.contains(id))
    {
        int index = _activedPoolIndices[id];
        return index;
    }
    return -1;
}

void QTEUIManager::SetUIAlpha(float factor)
{
    factor = std::clamp(factor, 0.0f, 1.0f);
    _backGroundUI.Alpha(factor);
    _fieldUI.Alpha(factor);
    _guideUI.Alpha(factor);
    _inputViewerUI.Alpha(factor);
}

void QTEUIManager::DrawDebugJudgeLine()
{
    if (_fieldUI.Overlay && _fieldUI.JudgeNote)
    {
        if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
        {
            const auto [validMin, validMax]     = system->GetValidJudgeRange();
            const auto [normalMin, normalMax]   = system->GetNormalJudgeRange();
            const auto [perfectMin, perfectMax] = system->GetPerfectJudgeRange();
            const float tavelTime               = system->GetNoteTravelTime();
            const float speedScale              = system->GetQTESpeedScale();
            const POINT panelPoint              = _fieldUI.Overlay->AbsolutePosition;
            const SIZE  panelSize               = _fieldUI.Overlay->Size;
            const POINT judgeCenter             = _fieldUI.JudgeNote->CenterPoint;
            const float perfectX                = static_cast<float>(judgeCenter.x);
            const float offsetX                 = static_cast<float>(panelPoint.x);
            { // Valid
                const float minFactor = QTE::Math::CalculateNotePosXFactor(-validMin, speedScale, tavelTime);
                DebugDrawValidLine(panelPoint, panelSize, offsetX + perfectX * minFactor, Colors::White);
                const float maxFactor = QTE::Math::CalculateNotePosXFactor(-validMax, speedScale, tavelTime);
                DebugDrawValidLine(panelPoint, panelSize, offsetX + perfectX * maxFactor, Colors::White);
            }
            { // Normal
                const float minFactor = QTE::Math::CalculateNotePosXFactor(-normalMin, speedScale, tavelTime);
                DebugDrawValidLine(panelPoint, panelSize, offsetX + perfectX * minFactor, Colors::Blue);
                const float maxFactor = QTE::Math::CalculateNotePosXFactor(-normalMax, speedScale, tavelTime);
                DebugDrawValidLine(panelPoint, panelSize, offsetX + perfectX * maxFactor, Colors::Blue);
            }
            { // Perfect
                const float minFactor = QTE::Math::CalculateNotePosXFactor(-perfectMin, speedScale, tavelTime);
                DebugDrawValidLine(panelPoint, panelSize, offsetX + perfectX * minFactor, Colors::Yellow);
                const float maxFactor = QTE::Math::CalculateNotePosXFactor(-perfectMax, speedScale, tavelTime);
                DebugDrawValidLine(panelPoint, panelSize, offsetX + perfectX * maxFactor, Colors::Yellow);
            }
        }
    }
}

void QTEUIManager::SetDefaultState()
{
    ResetUI();
    _backGroundUI.Alpha(0.0f);
    _backGroundUI.Active(true);
    _fieldUI.Active(false);
    _guideUI.Active(false);
    _inputViewerUI.Active(false);
    _battleGuideUI.Active(true);
}

void QTEUIManager::FindUIComponents()
{
    _overlayPanel = GetComponent<OverlayPanel>();
    Transform::ForeachBFS(transform, [this](Transform* curr) {
        if (curr)
        {
            _backGroundUI.MatchUIFromObject(curr->gameObject);
            _inputViewerUI.MatchUIFromObject(curr->gameObject);
            _fieldUI.MatchUIFromObject(curr->gameObject);
            _guideUI.MatchUIFromObject(curr->gameObject);
            _battleGuideUI.MatchUIFromObject(curr->gameObject);
        }
    });
}