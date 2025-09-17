#include "pchScripts.h"
#include "QTEUIManager.h"
#include <QTE/System/QTESystem.h>
#include <QTE/Track/QTETrack.h>
#include <UI/Panels/Overlay/OverlayPanel.h>
#include <UI/Elements/Image/ImageElement.h>

QTEUIManager::QTEUIManager() = default;
QTEUIManager::~QTEUIManager() = default;

void QTEUIManager::OnQTEEnter() 
{
    gameObject->ActiveSelf = true;

    UpdateUITransformData();

    // QTE 시작 시 현재 트랙에 맞게 노트 오브젝트 스폰
    SpawnQTENotesFromCurrentTrack();

    // QTE UI 페이드 인 시작
    _qteUIAlphaFactor = 0.0f;
}

void QTEUIManager::OnQTENotePressed(QTE::ResultType result)
{
    // TODO : QTE Effect
}

void QTEUIManager::OnQTEStay() 
{
    if (nullptr == _qteOverlayPanel ||
        nullptr == _qteNoteLineUI ||
        nullptr == _qteBackgroundUI ||
        nullptr == _qteJudgeNoteUI  )
    {
        return;
    }

    auto* system = SingletonComponent<QTESystem>::GetInstance();
    if (system)
    {
        auto* track = system->GetCurrentQTETrack();
        if (track)
        {
            const float qteTime  = system->GetQTETime();
            const POINT judgePos = _qteJudgeNoteUI->Point;
            const auto& results  = system->GetCurrentQTEResultQueue();
            for (const auto& result : results)
            {
                QTE::Note* note = result.Note;
                if (note)
                {
                    const int     id     = note->ID;
                    const float   time   = note->Time;

                    ImageElement* noteUI = FindNoteUIFromNoteID(id);
                    if (noteUI)
                    {
                        GameObject& object      = noteUI->gameObject;
                        if (QTE::QTE_RESULT_NONE != result.ResultType)
                        {
                            // 이미 판정이 난 노트는 비활성화
                            noteUI->Alpha       = 0.0f;
                            object.ActiveSelf   = false;
                            continue;
                        }

                        POINT       oldPoint    = noteUI->Point;
                        float       notePosX    = CalculateNotePosXAbsolute(time, qteTime);

                        float       notePosXToPanel = notePosX - _qtePanelPos.x;
                        float       notePosXFactorToPanel = notePosXToPanel / _qtePanelSize.x;
                        float       alphaFactor = CalculateNoteAlpha(notePosXFactorToPanel);
                        if (alphaFactor > 0.0f)
                        {
                            noteUI->Point       = POINT{(LONG)notePosX, oldPoint.y};
                            noteUI->Alpha       = alphaFactor;
                            object.ActiveSelf   = true;
                        }
                        else
                        {
                            noteUI->Alpha       = 0.0f;
                            object.ActiveSelf   = false;
                        }
                    }
                }
            }
        }
    }
}

void QTEUIManager::OnQTEExit() 
{
    // QTE 종료 시 노트 오브젝트 정리
    ClearAllQTENotes();
    // QTE UI 페이드 아웃
    _qteUIAlphaFactor = 1.0f;
}

void QTEUIManager::Reset() 
{
    Base::Reset();
    _singletonComponent.SetSingleTon();

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
    Base::Awake();
    if (_singletonComponent.TrySingleTon())
    {
        // Do Something
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"씬에 QTEUIManager가 2개 이상 존재하는지 확인해주세요.");
    }
}

void QTEUIManager::Start() 
{
    Base::Start();
    _qteSystem = SingletonComponent<QTESystem>::GetInstance();
    FindUIComponents();
    UpdateUITransformData();
}

void QTEUIManager::Update() 
{
    Base::Update();
    float alpha = 1.0f;
    if (_qteSystem)
    {
        if (_qteSystem->IsQTEPlaying())
        {
            if (_qteUIAlphaFactor < 1.0f)
            {
                _qteUIAlphaFactor += UmTime.DeltaTime();
                _qteUIAlphaFactor = std::clamp(_qteUIAlphaFactor, 0.0f, 1.0f);
                alpha = Mathf::Ease(Mathf::EASE_IN, Mathf::EaseFuncType::SINE, 0.5f, _qteUIAlphaFactor);
            }
            else
            {
                alpha = 1.0f;
            }
        }
        else
        {
            if (_qteUIAlphaFactor > 0.0f)
            {
                _qteUIAlphaFactor -= UmTime.DeltaTime();
                _qteUIAlphaFactor = std::max(_qteUIAlphaFactor, 0.0f);
                alpha = Mathf::Ease(Mathf::EASE_OUT, Mathf::EaseFuncType::SINE, 0.5f, _qteUIAlphaFactor);
            }
            else
            {
                alpha = 0.0f;
                gameObject->ActiveSelf = false;
            }
        }
    }
    SetUIAlpha(alpha);
}

void QTEUIManager::SerializedReflectEvent() 
{
}

void QTEUIManager::DeserializedReflectEvent()
{
    _notePrefabGuid = File::Guid(ReflectFields->NotePrefabGuid);
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
}

void QTEUIManager::SetNotePrefabGuid(const File::Guid& guid) 
{
    _notePrefabGuid               = guid;
    ReflectFields->NotePrefabGuid = guid.string();
}

void QTEUIManager::SetUIAlpha(float factor) 
{
    factor = std::clamp(factor, 0.0f, 1.0f);
    if (_qteBackgroundUI)
    {
        _qteBackgroundUI->Alpha = factor;
    }
    if (_qteNoteLineUI)
    {
        _qteNoteLineUI->Alpha = factor;
    }
    if (_qteJudgeNoteUI)
    {
        _qteJudgeNoteUI->Alpha = factor;
    }
}

float QTEUIManager::GetJudgeNotePosXFactor() const
{
    float judgePosXToPanel = _qteJudgePos.x - _qtePanelPos.x;
    return _qtePanelSize.x / judgePosXToPanel;
}

void QTEUIManager::UpdateUITransformData()
{
    if (_qteOverlayPanel)
    {
        POINT pos    = _qteOverlayPanel->AbsolutePosition;
        _qtePanelPos = Vector2((float)pos.x, (float)pos.y);

        SIZE size     = _qteOverlayPanel->Size;
        _qtePanelSize = Vector2((float)size.cx, (float)size.cy);
    }
    if (_qteJudgeNoteUI)
    {
        POINT pos    = _qteJudgeNoteUI->AbsolutePosition;
        _qteJudgePos = Vector2((float)pos.x, (float)pos.y);

        SIZE size     = _qteJudgeNoteUI->Size;
        _qteJudgeSize = Vector2((float)size.cx, (float)size.cy);
    }
}

void QTEUIManager::FindUIComponents()
{
    _qteOverlayPanel = nullptr;
    _qteNoteLineUI = nullptr;
    _qteJudgeNoteUI = nullptr;

    Transform::ForeachBFS(transform, [this](Transform* curr) {
        if (!_qteOverlayPanel && curr->gameObject->CompareTag("QTE Panel"))
        {
            _qteOverlayPanel = curr->gameObject->GetComponent<OverlayPanel>();
        }
        else if (!_qteBackgroundUI && curr->gameObject->CompareTag("QTE Background"))
        {
            _qteBackgroundUI = curr->gameObject->GetComponent<ImageElement>();
        }
        else if (!_qteNoteLineUI && curr->gameObject->CompareTag("QTE Note Line"))
        {
            _qteNoteLineUI = curr->gameObject->GetComponent<ImageElement>();
        }
        else if (!_qteJudgeNoteUI && curr->gameObject->CompareTag("QTE Judge Note"))
        {
            _qteJudgeNoteUI = curr->gameObject->GetComponent<ImageElement>();
        }
    });

    if (nullptr == _qteOverlayPanel)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"QTE UI Panel을 찾지 못했습니다.");
    }
    if (nullptr == _qteNoteLineUI)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"QTE Note Line UI를 찾지 못했습니다.");
    }
    if (nullptr == _qteJudgeNoteUI)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"QTE Judge Note UI를 찾지 못했습니다.");
    }
}

void QTEUIManager::SpawnQTENotesFromCurrentTrack()
{
    if (_qteSystem && _qteOverlayPanel)
    {
        ClearAllQTENotes();
        const auto& notes  = _qteSystem->GetCurrentQTEAvailQueue();
        const auto* prefab = UmGameObjectFactory.GetOriginPrefab(_notePrefabGuid);
        for (auto& note : notes)
        {   // 노트 프리팹을 복제해서 비활성화 상태로 대기열에 추가
            if (note && prefab && prefab->front())
            {
                GameObject* clone = GameObject::Instantiate(prefab->front().get());
                if (clone)
                {
                    clone->SetActive(false);
                    ImageElement* imageElement = clone->GetComponent<ImageElement>();
                    if (imageElement)
                    {
                        imageElement->transform->SetParent(transform, false);
                        _noteSpawnTable[note->ID] = imageElement;
                    }
                }
            }
        }
    }
}

void QTEUIManager::ClearAllQTENotes() 
{
    for (auto& [id, clone] : _noteSpawnTable)
    {
        if (clone)
        {
            GameObject::Destroy(clone->gameObject);
            clone = nullptr;
        }
    }
    _noteSpawnTable.clear();
}

float QTEUIManager::CalculateNotePosXFactor(float noteTime, float totalTime)
{
    float      factor = 0.0f;
    QTESystem* system = SingletonComponent<QTESystem>::GetInstance();
    if (system)
    {
        auto* track = system->GetCurrentQTETrack();
        if (track)
        {
            const float systemSpeed = system->GetQTESpeedScale();
            const float trackSpeed  = track->GetQTESpeedScale();

            float scaledTotalTime = totalTime * systemSpeed * trackSpeed;
            float scaledNoteTime  = noteTime * systemSpeed * trackSpeed;

            factor = 1.0f + scaledTotalTime - scaledNoteTime;
        }
    }
    return factor;
}

float QTEUIManager::CalculateNotePosXAbsolute(float noteTime, float totalTime)
{
    float factor = CalculateNotePosXFactor(noteTime, totalTime);
    return CalculateNotePosXAbsolute(factor);
}

float QTEUIManager::CalculateNotePosXAbsolute(float posFactor)
{
    return _qteJudgePos.x * posFactor;
}

float QTEUIManager::CalculateNoteAlpha(float posFactor)
{
    auto system = SingletonComponent<QTESystem>::GetInstance();
    if (system)
    {
        auto [fadeInMin, fadeInMax]   = system->GetFadeInPosFactor();
        auto [fadeOutMin, fadeOutMax] = system->GetFadeOutPosFactor();

        float alpha = 1.0f;

        if (posFactor < fadeInMin)
        {
            alpha = 0.0f; // 아직 페이드인 시작 전
        }
        else if (posFactor < fadeInMax)
        {
            float t = (posFactor - fadeInMin) / (fadeInMax - fadeInMin);
            alpha   = t; // 0 → 1 보간
        }
        else if (posFactor > fadeOutMax)
        {
            alpha = 0.0f; // 이미 페이드아웃 끝남
        }
        else if (posFactor > fadeOutMin)
        {
            float t = (posFactor - fadeOutMin) / (fadeOutMax - fadeOutMin);
            alpha   = 1.0f - t; // 1 → 0 보간
        }

        return alpha;
    }
    return 1.0f;
}

ImageElement* QTEUIManager::FindNoteUIFromNoteID(int noteID) const
{
    auto itr = _noteSpawnTable.find(noteID);
    if (itr != _noteSpawnTable.end())
    {
        return itr->second;
    }
    return nullptr;
}
