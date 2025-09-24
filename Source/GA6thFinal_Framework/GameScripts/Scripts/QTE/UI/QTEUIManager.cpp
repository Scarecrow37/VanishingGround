#include "pchScripts.h"
#include "QTEUIManager.h"
#include <QTE/System/QTESystem.h>
#include <QTE/Track/QTETrack.h>
#include <UI/Panels/Overlay/OverlayPanel.h>
#include <UI/Elements/Image/ImageElement.h>
#include <Camera/CameraComponent.h>

#include <BattleSystem/Battle.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

UMREAL_COMPONENT(QTEUIManager)

QTEUIManager::QTEUIManager() = default;
QTEUIManager::~QTEUIManager() = default;

void QTEUIManager::OnQTEEnter() 
{
    if (false == CheckUIValid())
    {
        FindUIComponents();
    }
    // 오브젝트 활성화 QTE UI 페이드 인 시작
    gameObject->ActiveSelf = true;
    _mainFader.SetFadeMode(Fader::FADE_IN);
    
    // QTE UI 위치 및 크기 데이터 저장
    UpdateUITransformData();

    // QTE 시작 시 현재 트랙에 맞게 노트 오브젝트 스폰
    SpawnQTENotesFromCurrentTrack();
}

void QTEUIManager::OnQTENotePressed(QTE::ResultType result)
{
    // TODO : QTE Effect
}

void QTEUIManager::OnQTEStay() 
{
    // UI가 하나라도 존재하지 않으면 실행하지 않음
    if (nullptr == _qteOverlayPanel ||
        nullptr == _qteNoteLineUI   ||
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
                        if (result.IsValidResult() && result.IsPressedButton())
                        {   // 이미 누른 판정이 난 노트는 비활성화
                            object.ActiveSelf   = false;
                            continue;
                        }
                        SIZE  noteSize = noteUI->Size;
                        POINT oldPoint = noteUI->Point;

                        float notePosXFactor    = CalculateNotePosXFactor(time, qteTime);
                        float perfectPos        = _qteJudgePos.x + (_qteJudgeSize.x * 0.5f) - _qtePanelPos.x;
                        float notePosX          = perfectPos * notePosXFactor; // 패널 기준에서의 노트 좌표
                        
                        // 포지션은 판정노트가 1.0 기준이지만 알파는 라인 끝을 기준으로 계산
                        float alphaFactor       = notePosX / _qtePanelSize.x;
                        float alpha             = CalculateNoteAlpha(alphaFactor);

                        if (alpha > 0.0f)
                        {
                            float notePosAbsX   = notePosX - ((float)noteSize.cx * 0.5f) + _qtePanelPos.x; // 절대 좌표로 변환
                            noteUI->Point       = POINT{(LONG)notePosAbsX, oldPoint.y};
                            noteUI->Alpha       = alpha;
                            object.ActiveSelf   = true;
                        }
                        else
                        {
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
    // QTE UI 페이드 아웃 
    _mainFader.SetFadeMode(Fader::FADE_OUT);
    
    // QTE 종료 시 노트 오브젝트 정리
    ClearAllQTENotes();
}

void QTEUIManager::Refresh() 
{
    FindUIComponents();
    UpdateUITransformData();
}

void QTEUIManager::Reset() 
{
    _staticInstance = this;

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
    FindUIComponents();
    UpdateUITransformData();
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
        gameObject->ActiveSelf = false;
        if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
        {
            system->ProcessQTEFadeOutEndEvent();
            _mainFader.SetFadeMode(Fader::FADE_NONE);
        }
    });

    _xybAlphaFader.SetFadeInType(Mathf::EASE_IN, Mathf::SINE);
    _xybAlphaFader.SetFadeOutType(Mathf::EASE_OUT, Mathf::SINE);
    _xybAlphaFader.SetOnFadeInEndCallback([this]() { _xybAlphaFader.SetFadeMode(Fader::FADE_NONE); });
    _xybAlphaFader.SetOnFadeOutEndCallback([this]() { _xybAlphaFader.SetFadeMode(Fader::FADE_NONE); });

    _xybPointFader.SetFadeInType(Mathf::EASE_OUT, Mathf::SINE);
    _xybPointFader.SetFadeOutType(Mathf::EASE_OUT, Mathf::SINE);
    _xybPointFader.SetOnFadeInEndCallback([this]() { _xybPointFader.SetFadeMode(Fader::FADE_NONE); });
    _xybPointFader.SetOnFadeOutEndCallback([this]() { _xybPointFader.SetFadeMode(Fader::FADE_NONE); });
}

void QTEUIManager::Update() 
{
    float factor = _mainFader.Fade();
    auto  mode  = _mainFader.GetFadeMode();
    switch (mode)
    {
        case QTEUIManager::Fader::FADE_NONE:
            break;
        case QTEUIManager::Fader::FADE_IN: {
            SetUIAlpha(factor);
            break;
        }
        case QTEUIManager::Fader::FADE_OUT: {
            SetUIAlpha(factor);
            SetBackgroundUIAlpha(factor);
            break;
        }
        default:
            break;
    }

    UpdateGuideNoteUI();
}

void QTEUIManager::OnEnable() 
{
}

void QTEUIManager::OnDisable() 
{
}

void QTEUIManager::OnDestroy()
{
    if (_staticInstance == this)
    {
        _staticInstance = nullptr;
    }
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

void QTEUIManager::SetBackgroundUIAlpha(float factor) 
{
    factor = std::clamp(factor, 0.0f, 1.0f);
    if (_qteBackgroundUI)
    {
        _qteBackgroundUI->Alpha = factor;
    }
}

void QTEUIManager::SetGuideNoteAlpha(float factor) 
{
    factor = std::clamp(factor, 0.0f, 1.0f);
    if (_qteGuideNoteX)
    {
        _qteGuideNoteX->Alpha = factor;
    }
    if (_qteGuideNoteY)
    {
        _qteGuideNoteY->Alpha = factor;
    }
    if (_qteGuideNoteB)
    {
        _qteGuideNoteB->Alpha = factor;
    }
}

void QTEUIManager::SetUIAlpha(float factor)
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
}

void QTEUIManager::SetActive(bool active) 
{
    gameObject->ActiveSelf = active;
}

void QTEUIManager::StartShowQTEGuideNote() 
{
    _xybOutTimer = 0.0f;
    _xybPointFader.SetTimer(0.0f);
    _xybAlphaFader.SetTimer(0.0f);

    _xybAlphaFader.SetDuration(0.5f);
    _xybPointFader.SetDuration(1.0f);
    _xybAlphaFader.SetFadeMode(Fader::FADE_IN);
    _xybPointFader.SetFadeMode(Fader::FADE_IN);
}

void QTEUIManager::StartHideQTEGuideNote() 
{
    _xybAlphaFader.SetDuration(0.5f);
    _xybPointFader.SetDuration(1.0f);
    _xybAlphaFader.SetFadeMode(Fader::FADE_OUT);
    _xybPointFader.SetFadeMode(Fader::FADE_OUT);
}

void QTEUIManager::UpdateGuideNoteUI() 
{
    _xybAlphaFader.Fade();
    _xybPointFader.Fade();

    POINT point;
    const SIZE& resolution  = UmGraphics.GetResolution();
    const float minY        = static_cast<float>(resolution.cy);
    const float alphaFactor = _xybAlphaFader.GetFadeFactor();
    const float pointFactor = _xybPointFader.GetFadeFactor();
    
    if (_qteGuideNoteX)
    {
        SIZE size             = _qteGuideNoteX->Size;
        point.x               = (LONG)_qteGuideNoteXPos.x - size.cx / 2;
        point.y               = (LONG)std::lerp(minY, 0, pointFactor) - size.cy / 2;
        _qteGuideNoteX->Point = point;
        _qteGuideNoteX->Alpha = alphaFactor;
    }
    if (_qteGuideNoteY)
    {
        SIZE size             = _qteGuideNoteY->Size;
        point.x               = (LONG)_qteGuideNoteYPos.x - size.cx / 2;
        point.y               = (LONG)std::lerp(minY, 0, pointFactor) - size.cy / 2;
        _qteGuideNoteY->Point = point;
        _qteGuideNoteY->Alpha = alphaFactor;
    }
    if (_qteGuideNoteB)
    {
        SIZE size             = _qteGuideNoteB->Size;
        point.x               = (LONG)_qteGuideNoteBPos.x - size.cx / 2;
        point.y               = (LONG)std::lerp(minY, 0, pointFactor) - size.cy / 2;
        _qteGuideNoteB->Point = point;
        _qteGuideNoteB->Alpha = alphaFactor;
    }

    if (_xybPointFader.IsFadeInEnd())
    {
        _xybOutTimer += UmTime.DeltaTime();
        if (_xybOutTimer >= 1.0f)
        {
            StartHideQTEGuideNote();
        }
    }
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

    CameraComponent* camera = CameraComponent::MainCamera();
    if (camera)
    {
        auto enemies = Battle::GetTargetsFromFlags(Battle::ENEMY_TARGET_FLAG_ALL);
        if (3 <= enemies.size())
        {
            auto* left   = enemies[0];
            auto* middle = enemies[1];
            auto* right  = enemies[2];
            if (left && _qteGuideNoteX)
            {
                _qteGuideNoteXPos = camera->WorldToViewport(left->transform->GetWorldPosition());
            }
            if (middle && _qteGuideNoteY)
            {
                _qteGuideNoteYPos = camera->WorldToViewport(middle->transform->GetWorldPosition());
            }
            if (right && _qteGuideNoteB)
            {
                _qteGuideNoteBPos = camera->WorldToViewport(right->transform->GetWorldPosition());
            }
        }
    }
}

bool QTEUIManager::CheckUIValid()
{
    return _qteOverlayPanel && _qteBackgroundUI && _qteNoteLineUI && _qteJudgeNoteUI;
}

void QTEUIManager::FindUIComponents()
{
    _qteOverlayPanel    = nullptr;
    _qteBackgroundUI    = nullptr;
    _qteNoteLineUI      = nullptr;
    _qteJudgeNoteUI     = nullptr;
    _qteGuideNoteX      = nullptr;
    _qteGuideNoteY      = nullptr;
    _qteGuideNoteB      = nullptr;

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
        else if (!_qteGuideNoteX && curr->gameObject->CompareTag("Guide Note X"))
        {
            _qteGuideNoteX = curr->gameObject->GetComponent<ImageElement>();
        }
        else if (!_qteGuideNoteY && curr->gameObject->CompareTag("Guide Note Y"))
        {
            _qteGuideNoteY = curr->gameObject->GetComponent<ImageElement>();
        }
        else if (!_qteGuideNoteB && curr->gameObject->CompareTag("Guide Note B"))
        {
            _qteGuideNoteB = curr->gameObject->GetComponent<ImageElement>();
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
    if (nullptr == _qteGuideNoteX)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"Guide Note X를 찾지 못했습니다.");
    }
    if (nullptr == _qteGuideNoteY)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"Guide Note Y를 찾지 못했습니다.");
    }
    if (nullptr == _qteGuideNoteB)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"Guide Note B를 찾지 못했습니다.");
    }
}

void QTEUIManager::SpawnQTENotesFromCurrentTrack()
{
    if (SingletonComponent<QTESystem>::GetInstance() && _qteOverlayPanel)
    {
        ClearAllQTENotes();
        const auto& notes  = SingletonComponent<QTESystem>::GetInstance()->GetCurrentQTEAvailQueue();
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
                        imageElement->Point       = POINT(-LONG_MAX, 0); // 화면 밖으로 이동
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

float QTEUIManager::CalculateNotePosX(float noteTime, float totalTime)
{
    float factor = CalculateNotePosXFactor(noteTime, totalTime);
    return CalculateNotePosX(factor);
}

float QTEUIManager::CalculateNotePosX(float posFactor)
{
    return (_qteJudgePos.x + (_qteJudgeSize.x * 0.5f) - _qtePanelPos.x) * posFactor;
}

float QTEUIManager::CalculateNoteAlpha(float posFactor)
{
    auto system = SingletonComponent<QTESystem>::GetInstance();
    float alpha = 1.0f;
    if (system)
    {
        auto [fadeInMin, fadeInMax]   = system->GetFadeInPosFactor();
        auto [fadeOutMin, fadeOutMax] = system->GetFadeOutPosFactor();

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
    }
    return alpha;
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

void QTEUIManager::Fader::SetFadeMode(Mode mode) 
{
    _fadeMode = mode;
}

void QTEUIManager::Fader::SetDuration(float duration)
{
    _duration = std::max(duration, 0.0f);
}

void QTEUIManager::Fader::SetTimer(float timer) 
{
    _timer = std::clamp(timer, 0.0f, _duration);
}

void QTEUIManager::Fader::SetFadeInType(Mathf::EaseType type, Mathf::EaseFuncType func) 
{
    _fadeInEaseType = type;
    _fadeInFuncType = func;
}

void QTEUIManager::Fader::SetFadeOutType(Mathf::EaseType type, Mathf::EaseFuncType func) 
{
    _fadeOutEaseType = type;
    _fadeOutFuncType = func;
}

void QTEUIManager::Fader::SetOnFadeInEndCallback(const std::function<void()>& callback)
{
    _onFadeInEndCallback = callback;
}

void QTEUIManager::Fader::SetOnFadeOutEndCallback(const std::function<void()>& callback) 
{
    _onFadeOutEndCallback = callback;
}

float QTEUIManager::Fader::Fade()
{
    switch (_fadeMode)
    {
    case QTEUIManager::Fader::FADE_NONE:
        break;
    case QTEUIManager::Fader::FADE_IN:
    {
        if (IsFadeInEnd())
        {
            return 1.0f;
        }
        else
        {
            FadeIn();
            break;
        }
    }
    case QTEUIManager::Fader::FADE_OUT:
    {
        if (IsFadeOutEnd())
        {
            return 0.0f;
        }
        else
        {
            FadeOut();
            break;
        }
    }
    default:
        break;
    }
    return _fadeFactor;
}

float QTEUIManager::Fader::FadeIn()
{
    _timer += UmTime.DeltaTime();
    _timer = std::min(_timer, _duration);

    float factor = std::min(_timer / _duration, 1.0f);
    _fadeFactor  = Mathf::Ease(_fadeInEaseType, _fadeInFuncType, 0.5f, factor);
    if (IsFadeInEnd() && _onFadeInEndCallback)
    {
        _onFadeInEndCallback();
    }
    return _fadeFactor;
}

float QTEUIManager::Fader::FadeOut()
{
    _timer -= UmTime.DeltaTime();
    _timer = std::max(_timer, 0.0f);

    float factor = std::max(_timer / _duration, 0.0f);
    _fadeFactor  = Mathf::Ease(_fadeOutEaseType, _fadeOutFuncType, 0.5f, factor);
    if (IsFadeOutEnd() && _onFadeOutEndCallback)
    {
        _onFadeOutEndCallback();
    }
    return _fadeFactor;
}

bool QTEUIManager::Fader::IsFadeInEnd() const
{
    return 0.9999f <= _fadeFactor;
}

bool QTEUIManager::Fader::IsFadeOutEnd() const
{
    return 0.0001f >= _fadeFactor;
}
