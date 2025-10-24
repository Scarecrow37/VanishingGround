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

UMREAL_COMPONENT(QTEUIManager)

QTEUIManager::QTEUIManager() = default;
QTEUIManager::~QTEUIManager() = default;

namespace Math
{
}

void QTEUIManager::OnQTEEnter() 
{
    StartBeginQTEAnimation();

    // 오브젝트 활성화 QTE UI 페이드 인 시작
    _mainFader.SetFadeMode(Fader::FADE_IN);

    ResetNotePool();
    if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
    {
        const auto& noteQueue = system->GetCurrentQTEAvailQueue();
        bool InvalidRange = noteQueue.size() > _notePool.size();
        assert(InvalidRange && "노트 풀의 사이즈가 작습니다.");
        if (false == InvalidRange)
        {
            for (size_t i = 0; i < noteQueue.size(); ++i)
            {
                UINT id = noteQueue[i].ID;
                if (_notePool[i].TrySetup())
                {
                    _activedNote[id] = i;
                }
            }
        }
    }
}

void QTEUIManager::OnQTEButtonPressed() const
{
    if (_qteJudgeNoteUI)
    {
        _qteJudgeNoteUI->Setup();
        _qteJudgeNoteUI->StartAnimation();
    }
}

void QTEUIManager::OnQTENotePressed(const UINT noteID, const QTE::ResultType result)
{
    if (QTE::NoteUI* noteUI = GetNoteUIFromID(noteID))
    {
        noteUI->OnNotePressed(result);
    }
}

void QTEUIManager::OnQTEStay() 
{
    auto* system = SingletonComponent<QTESystem>::GetInstance();
    if (system)
    {
        float currTime  = system->GetQTETime();
        float currSpeed = system->GetQTESpeedScale();
        if (QTE::Track* track = system->GetCurrentQTETrack())
        {
            currSpeed *= track->GetQTESpeedScale();
        }

        const RECT judgeRect   = _fieldUI.JudgeNote->AbsoluteRect;
        const RECT overlayRect = _fieldUI.Overlay->AbsoluteRect;

        const float startX   = static_cast<float>(((POINT)_fieldUI.Overlay->AbsoluteCenterPoint).x);
        const float endX     = static_cast<float>(((POINT)_fieldUI.JudgeNote->AbsoluteCenterPoint).x);
        const float perfectX = static_cast<float>();
        const float offsetX  = 0.0f;

        for (auto& noteUI : _notePool)
        {
            if (noteUI.IsAvailable())
            {
                noteUI.Update(currTime, currSpeed, startX, endX, perfectX, offsetX);
            }
        }
    }
}

void QTEUIManager::OnQTEExit() 
{
    // QTE UI 페이드 아웃 
    _mainFader.SetFadeMode(Fader::FADE_OUT);

}

void QTEUIManager::Refresh() 
{
    FindUIComponents();
    for (auto& note : _notePool)
    {
        note.Reset();
    }
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
                const File::Path&    extension = path.extension();
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
    InitializeNotePool();
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
        SetQTEBarUIAlpha(0.0f);
        SetQTEAnimBarUIAlpha(0.0f);
        SetBackgroundUIAlpha(0.0f);

        if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
        {
            system->ProcessQTEFadeOutEndEvent();
            _mainFader.SetFadeMode(Fader::FADE_NONE);
        }
    });
    FindUIComponents();
    SetBackgroundUIAlpha(0.0f);
}

void QTEUIManager::Update() 
{
    UpdateQTEUI();
}

void QTEUIManager::SerializedReflectEvent() 
{
}

void QTEUIManager::DeserializedReflectEvent()
{
}

void QTEUIManager::ImGuiDrawPropertysEvent() 
{
    if (nullptr == _qteOverlayPanel)
    {
        ImGui::TextUnformatted((const char*)u8"QTE OverlayPanel UI가 없습니다.");
    }
    if (nullptr == _qteBackgroundUI)
    {
        ImGui::TextUnformatted((const char*)u8"QTE Background UI가 없습니다.");
    }
    if (nullptr == _qteNoteLineUI)
    {
        ImGui::TextUnformatted((const char*)u8"QTE Note Line UI가 없습니다.");
    }
    if (nullptr == _qteJudgeNoteUI)
    {
        ImGui::TextUnformatted((const char*)u8"QTE JudgeNote UI가 없습니다.");
    }
    if (ImGui::Button((const char*)u8"새로고침"))
    {
        FindUIComponents();
    }

    if (ImGui::TreeNodeEx("Debug##qte_manager", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text((const char*)u8"QTE Panel Absolute Pos : (%.1f, %.1f)", _qtePanelPos.x, _qtePanelPos.y);
        ImGui::Text((const char*)u8"QTE Judge Absolute Pos : (%.1f, %.1f)", _qteJudgePos.x, _qteJudgePos.y);
        // Guide Note Pos
        ImGui::Text((const char*)u8"Guide Note X Absolute Pos : (%.1f, %.1f)", _enemyXPos.x, _enemyXPos.y);
        ImGui::Text((const char*)u8"Guide Note Y Absolute Pos : (%.1f, %.1f)", _enemyYPos.x, _enemyYPos.y);
        ImGui::Text((const char*)u8"Guide Note B Absolute Pos : (%.1f, %.1f)", _enemyBPos.x, _enemyBPos.y);

        ImGui::TreePop();
    }
}

void QTEUIManager::InitializeNotePool() 
{
    _notePool.clear();

    File::Guid prefabGuid = ReflectFields->NotePrefabGuid;
    Transform& parent     = transform;
    for (int i = 0; i < ReflectFields->PoolSize; ++i)
    {
        _notePool.emplace_back(prefabGuid, &parent);
    }
}

void QTEUIManager::ResetNotePool() 
{
    _activedNote.clear();
    for (size_t i = 0; i < _notePool.size(); ++i)
    {
        _notePool[i].Reset();
    }
}

QTE::NoteUI* QTEUIManager::GetNoteUIFromID(UINT id)
{
    if (_activedNote.contains(id))
    {
        size_t index = _activedNote[id];
        if (index >= _notePool.size())
        {
            return &_notePool[index];
        }
    }
    return nullptr;
}

void QTEUIManager::StartBeginQTEAnimation() const
{
    if (_qteStartAnimationUI)
    {
        _qteStartAnimationUI->Setup();
        _qteStartAnimationUI->StartAnimation();
    }
}

void QTEUIManager::SetNotePrefabGuid(const File::Guid& guid) 
{
    ReflectFields->NotePrefabGuid = guid.string();
}

void QTEUIManager::SetBackgroundUIAlpha(float factor) 
{
    factor = std::clamp(factor, 0.0f, 1.0f);
    if (_qteBackgroundUI)
    {
        _qteBackgroundUI->Alpha = factor;
    }
}

void QTEUIManager::SetQTEBarUIAlpha(float factor)
{
    factor = std::clamp(factor, 0.0f, 1.0f);

    if (_qteNoteLineUI)
    {
        _qteNoteLineUI->Alpha = factor;
    }
    if (_qteJudgeNoteUI)
    {
        _qteJudgeNoteUI->Alpha = factor;
    }
    if (_qteFlow)
    {
        _qteFlow->Alpha = factor;
    }
}

void QTEUIManager::SetQTEAnimBarUIAlpha(float factor)
{
    factor = std::clamp(factor, 0.0f, 1.0f);

    if (_qteStartAnimationUI)
    {
        _qteStartAnimationUI->Alpha = factor;
    }
}

void QTEUIManager::SetActive(bool active) 
{
    gameObject->ActiveSelf = active;
}

void QTEUIManager::UpdateQTEUI() 
{
    float factor = _mainFader.Fade();
    auto  mode   = _mainFader.GetFadeMode();
    switch (mode)
    {
    case Fader::FADE_NONE:
        SetQTEBarUIAlpha(factor);
        SetQTEAnimBarUIAlpha(0.0f);
        break;
    case Fader::FADE_IN: {
        SetQTEBarUIAlpha(0.0f);
        SetQTEAnimBarUIAlpha(factor);
        break;
    }
    case Fader::FADE_OUT: {
        SetQTEBarUIAlpha(factor);
        SetQTEAnimBarUIAlpha(0.0f);
        SetBackgroundUIAlpha(factor);
        break;
    }
    default:
        break;
    }
}

void QTEUIManager::FindUIComponents()
{
    Transform::ForeachBFS(transform, [this](Transform* curr) {
        if (curr)
        {
            _backGroundUI.MatchUIFromObject(curr->gameObject);
            _fieldUI.MatchUIFromObject(curr->gameObject);
        }
    });
}