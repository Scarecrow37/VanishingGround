#include "pchScripts.h"
#include "QTEUIManager.h"
#include <QTE/System/QTESystem.h>
#include <QTE/Track/QTETrack.h>
#include <UI/Panels/Overlay/OverlayPanel.h>
#include <UI/Elements/Image/ImageElement.h>
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
    ResetUI();
    _fieldUI.Active(true);
    _guideUI.Active(true);
    _fieldUI.OnQTEEnter();
    _mainFader.SetFadeMode(Fader::FADE_IN);
    if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
    {
        const auto& noteQueue  = system->GetCurrentQTEAvailQueue();
        bool        validRange = noteQueue.size() <= _notePool.size();
        assert(validRange && "노트 풀의 사이즈가 작습니다.");
        if (validRange)
        {
            for (size_t i = 0; i < noteQueue.size(); ++i)
            {
                UINT id = noteQueue[i].ID;
                if (_notePool[i].TrySetup(noteQueue[i].Time))
                {
                    _activedNote[id] = i;
                }
            }
        }
    }
}

void QTEUIManager::OnQTEButtonPressed()
{
    _fieldUI.OnButtonPressed();
}

void QTEUIManager::OnQTENotePressed(const UINT noteID, const QTE::ResultType result)
{
    if (QTE::NoteUI* noteUI = GetNoteUIFromID(noteID))
    {
        noteUI->OnNotePressed(result);
    }
}

void QTEUIManager::OnQTEPlay()
{
    if (_overlayPanel && _fieldUI.Overlay && _fieldUI.JudgeNote)
    {
        _fieldUI.Update();
        if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
        {
            const float currTime    = system->CurrentTrackTime;
            const float currSpeed   = system->ScaledSpeedFactor;
            const SIZE  panelSize   = _overlayPanel->Size;
            const POINT overlayAbs  = _fieldUI.Overlay->AbsolutePosition;
            const POINT overlayPoint= _fieldUI.Overlay->Point;
            const SIZE  overlaySize = _fieldUI.Overlay->Size;
            const POINT judgeCenter = _fieldUI.JudgeNote->CenterPoint;
            const SIZE  judgeSize   = _fieldUI.JudgeNote->Size;

            const float perfectX    = static_cast<float>(judgeCenter.x);
            const float offsetX     = static_cast<float>(overlayAbs.x);
            const float startX      = static_cast<float>(overlayPoint.x);
            const float endX        = static_cast<float>(panelSize.cx);

            for (auto& [id,_] : _activedNote)
            {
                if (QTE::NoteUI* noteUI = GetNoteUIFromID(id))
                {
                    noteUI->Update(currTime, currSpeed, startX, endX, perfectX, 0.0f);
                }
            }
        }
    }
}

void QTEUIManager::OnQTEExit() 
{
    _mainFader.SetFadeMode(Fader::FADE_OUT);
}

void QTEUIManager::Reset() 
{
    _singletoneComponent.SetSingleTon();
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                const File::Path&    path      = data->GetPath();
                const File::Path     extension = path.extension();
                if (extension == L".UmPrefab")
                {
                    SetNotePrefabGuid(data->GetGuid());
                }
                else
                {
                    UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"프리팹은 .UmPrefab 파일만 지정할 수 있습니다.");
                }
            }
            ImGui::EndDragDropTarget();
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
        }
    });
    FindUIComponents();
    InitializeNotePool();
    ResetUI();
    _backGroundUI.Alpha(0.0f);
    _backGroundUI.Active(true);
    _fieldUI.Active(false);
    _guideUI.Active(false);
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
        _guideUI.Alpha(factor);
        break;
    }
    case Fader::FADE_OUT: {
        SetUIAlpha(factor);
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

    if (_fieldUI.Overlay && _fieldUI.JudgeNote)
    {
        if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
        {
            const auto [validMin, validMax]     = system->GetValidJudgeRange();
            const auto [normalMin, normalMax]   = system->GetNormalJudgeRange();
            const auto [perfectMin, perfectMax] = system->GetPerfectJudgeRange();
            const float tavelTime   = QTE::NoteUI::TRAVEL_PERFECT_TIME;
            const float speedScale  = system->GetQTESpeedScale();
            const POINT panelPoint  = _fieldUI.Overlay->AbsolutePosition;
            const SIZE  panelSize   = _fieldUI.Overlay->Size;
            const POINT judgeCenter = _fieldUI.JudgeNote->CenterPoint;
            const float perfectX    = static_cast<float>(judgeCenter.x);
            const float offsetX     = static_cast<float>(panelPoint.x);
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

void QTEUIManager::ResetUI()
{
    _fieldUI.Reset();
    _activedNote.clear();
    for (auto& noteUI : _notePool)
    {
        noteUI.Reset();
    }
}

void QTEUIManager::InitializeNotePool()
{
    assert(_fieldUI.Overlay && "QTE Overlay가 없으면 노트 인스턴스를 생성하지 않습니다.");

    _notePool.clear();

    File::Guid prefabGuid = ReflectFields->NotePrefabGuid;
    Transform& parent     = _fieldUI.Overlay->transform;
    for (int i = 0; i < ReflectFields->PoolSize; ++i)
    {
        _notePool.emplace_back(prefabGuid, &parent);
    }
}

QTE::NoteUI* QTEUIManager::GetNoteUIFromID(UINT id)
{
    if (_activedNote.contains(id))
    {
        size_t index = _activedNote[id];
        if (index < _notePool.size())
        {
            return &_notePool[index];
        }
    }
    return nullptr;
}

void QTEUIManager::SetNotePrefabGuid(const File::Guid& guid) 
{
    ReflectFields->NotePrefabGuid = guid.string();
}

void QTEUIManager::SetUIAlpha(float factor)
{
    factor = std::clamp(factor, 0.0f, 1.0f);
    _backGroundUI.Alpha(factor);
    _fieldUI.Alpha(factor);
    _guideUI.Alpha(factor);
}

void QTEUIManager::FindUIComponents()
{
    _overlayPanel = GetComponent<OverlayPanel>();
    Transform::ForeachBFS(transform, [this](Transform* curr) {
        if (curr)
        {
            _backGroundUI.MatchUIFromObject(curr->gameObject);
            _fieldUI.MatchUIFromObject(curr->gameObject);
            _guideUI.MatchUIFromObject(curr->gameObject);
        }
    });
}