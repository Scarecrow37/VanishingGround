#include "pchScripts.h"
#include "QTESystem.h"
#include <QTE/Editor/QTEEditor.h>
#include <QTE/Track/QTETrack.h>
#include <WeaponSystem/WeaponSystem.h>

QTESystem::QTESystem() 
{
    // 전역 인스턴스는 하나만 존재해야 합니다.
    assert(_staticInstance == nullptr && "QTESystem is already exist in scene");
    _staticInstance = this;
}

QTESystem::~QTESystem() 
{
    // 전역 인스턴스는 하나만 존재해야 합니다.
    assert(_staticInstance == this && "QTESystem instance is not this");
    _staticInstance = nullptr;
}

void QTESystem::Awake() 
{
    BindInputAction(ControllerButton::X, Action::PRESSED, this, this, &QTESystem::PressedButtonX);
    BindInputAction(ControllerButton::Y, Action::PRESSED, this, this, &QTESystem::PressedButtonY);
    BindInputAction(ControllerButton::B, Action::PRESSED, this, this, &QTESystem::PressedButtonB);
}

void QTESystem::Update()
{
    if (_isQTEPlaying && _currentQTETrack)
    {
        UpdateQTEDelay();
        if (IsQTEDelayEnd())
        {
            UpdateQTETrack();
        }
        ImGui::Begin("QTE Preview", nullptr);
        {
            QTEPreviewer::SetPreviewTimer(_qteTimer);
            QTEPreviewer::SetPreviewTrack(_currentQTETrack);
            QTEPreviewer::Draw();
        }
        ImGui::End();
    }
}

void QTESystem::SerializedReflectEvent() {}

void QTESystem::DeserializedReflectEvent() {}

void QTESystem::ImGuiDrawPropertysEvent()
{
    if (ImGui::Button("Open QTE Editor"))
    {
        GetEditor().Open();
    }
    GetEditor().Show();
}

void QTESystem::StartQTE() 
{
    if (_isQTEPlaying)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"QTE가 진행 중인 상태에서 QTE 시작 요청을 한번 더 했습니다. 다시 한번 확인해주세요.");
        return;
    }

    _isQTEPlaying = true;
    _noteAvailQueue.clear();
    _noteResultQueue.clear();
    _currentNoteIndex = 0;

    // 현재 무기에 맞는 QTE 트랙을 선택
    WeaponSystem* weaponSystem = WeaponSystem::GetInstance();
    if (weaponSystem)
    {
        auto& weapon = weaponSystem->GetCurrentWeaponStats();
        auto  itr    = _weaponIDToTrackTable.find(weapon.WeaponID);
        if (itr != _weaponIDToTrackTable.end() && false == itr->second.empty())
        {
            // 여러 트랙이 있는 경우 랜덤 선택
            int index = Random::Range(0, (int)itr->second.size() - 1);
            _currentQTETrack = &itr->second[index];
            StartQTE(_currentQTETrack);
        }
    }
}

void QTESystem::StartQTE(QTE::Track* qteTrack)
{
    if (qteTrack)
    {
        auto track = qteTrack->GetEventTrack().lock();
        if (track)
        {
            _qteTimer = track->GetMinFrame();

            // 유효한 노트 큐 생성
            auto& noteQueue = track->GetEventContextQueue();
            _noteAvailQueue.reserve(noteQueue.size());
            _noteResultQueue.reserve(noteQueue.size());
            for (auto& note : noteQueue)
            {
                QTE::Note* qteNote = dynamic_cast<QTE::Note*>(note);
                if (qteNote)
                {
                    _noteAvailQueue.push_back(qteNote);
                    _noteResultQueue.emplace_back(qteNote);
                }
            }
        }
    }
}

bool QTESystem::IsQTEDelayEnd()
{
    if (_delayTimer > 0.0f)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool QTESystem::IsQTETimeEnd()
{
    auto track = _currentQTETrack->GetEventTrack().lock();
    if (track && _currentQTETrack)
    {
        float minFrame = track->GetMinFrame();
        float maxFrame = track->GetMaxFrame();
        if (_qteTimer >= maxFrame)
        {
            return true;
        }
    }
    return false;
}

QTE::ResultType QTESystem::GetQTEResult(QTE::Note* note)
{
    auto& [perfectMin, perfectMax] = ReflectFields->PerfectJudgeRange;
    auto& [normalMin, normalMax]   = ReflectFields->NormalJudgeRange;
    if (note)
    {
        float noteTime = note->Time;
        if (noteTime >= perfectMin && noteTime <= perfectMax)
        {
            return QTE::QTE_RESULT_PERFECT;
        }
        else if (noteTime >= normalMin && noteTime <= normalMax)
        {
            return QTE::QTE_RESULT_NORMAL;
        }
        else
        {
            return QTE::QTE_RESULT_MISS;
        }
    }
    return QTE::QTE_RESULT_NONE;
}

void QTESystem::ResetQTETimer() 
{
    _delayTimer = 0.0f;
    _qteTimer   = 0.0f;
}

void QTESystem::UpdateQTEDelay()
{
    if (_delayTimer > 0.0f)
    {
        float deltaTime = UmTime.DeltaTime();
        float scale     = ReflectFields->QTESpeedScale;
        if (_currentQTETrack)
        {
            scale *= _currentQTETrack->GetQTESpeedScale();
        }
        deltaTime *= scale;

        _delayTimer -= deltaTime;
    }
    else
    {
        _delayTimer = 0.0f;
    }
}

void QTESystem::UpdateQTETrack()
{
    auto track = _currentQTETrack->GetEventTrack().lock();
    if (track && _currentQTETrack)
    {
        float minFrame = track->GetMinFrame();
        float maxFrame = track->GetMaxFrame();
        if (_qteTimer < maxFrame)
        {
            float deltaTime = UmTime.DeltaTime();
            float scale     = ReflectFields->QTESpeedScale;
            scale *= _currentQTETrack->GetQTESpeedScale();
            deltaTime *= scale;
            _qteTimer += deltaTime;
            QTE::Note* curNote = _noteAvailQueue[_currentNoteIndex];
            if (curNote && _qteTimer >= curNote->Time)
            {
                // 노트 시간이 지나갔는데 버튼을 누르지 않은 경우, MISS 처리
                PressedQTEButton();
            }
        }
        else
        {
            _isQTEPlaying = false;
        }
    }
}

QTEEditor& QTESystem::GetEditor()
{
    static QTEEditor editor;
    return editor;
}

void QTESystem::PressedQTEButton(Input::ControllerTypes::Button type) 
{
    if (_isQTEPlaying)
    {
        QTE::Note*   curNote        = _noteAvailQueue[_currentNoteIndex];
        QTE::Result& result         = _noteResultQueue[_currentNoteIndex];
        ++_currentNoteIndex;

        result.Note                 = curNote;
        result.PressedButton        = type;
        result.ResultType           = GetQTEResult(curNote);
        result.TimeDelta            = curNote ? _qteTimer - curNote->Time : 0.0f;
    }
}

void QTESystem::PressedButtonX(const Input::Controller& controller)
{
    // Handle button X pressed
    PressedQTEButton(Input::ControllerTypes::Button::X);
}

void QTESystem::PressedButtonY(const Input::Controller& controller)
{
    // Handle button Y pressed
    PressedQTEButton(Input::ControllerTypes::Button::Y);
}

void QTESystem::PressedButtonB(const Input::Controller& controller)
{
    // Handle button B pressed
    PressedQTEButton(Input::ControllerTypes::Button::B);
}