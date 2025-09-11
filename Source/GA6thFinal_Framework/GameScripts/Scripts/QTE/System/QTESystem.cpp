#include "pchScripts.h"
#include "QTESystem.h"
#include <QTE/Editor/QTEEditor.h>
#include <QTE/Track/QTETrack.h>
#include <WeaponSystem/WeaponSystem.h>

QTESystem::QTESystem() 
{
}

QTESystem::~QTESystem() 
{
    if (this == _staticInstance)
    {
        _staticInstance = nullptr;
    }
}

void QTESystem::Reset()
{
    _staticInstance = this;
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
        ImGuiWindow* sceneWindow = ImGui::FindWindowByName("Scene");
        if (sceneWindow)
        {
            ImGui::SetNextWindowSize(sceneWindow->Size);
            ImGui::SetNextWindowPos(sceneWindow->Pos);
        }
        else
        {
            const SIZE& size = UmApplication.GetClientSize();
            ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2((float)size.cx / 2.0f, 100.0f), ImGuiCond_FirstUseEver);
        }
        ImGuiHelper::StyleBuilder style;
        style.PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
        ImGui::Begin("QTE Preview", nullptr);
        {
            style.PopStyle();
            QTEPreviewer::Draw();
        }
        ImGui::End();
    }
}

void QTESystem::SerializedReflectEvent()
{
    ReflectFields->WeaponQTETrackData.clear();
    for (auto& [weaponID, trackList] : _weaponIDToTrackTable)
    {
        for (auto& track : trackList)
        {
            ReflectFields->WeaponQTETrackData[weaponID].emplace_back(track->SerializedReflectFields());
        }
    }
}

void QTESystem::DeserializedReflectEvent() 
{
    for (auto& trackList : _weaponIDToTrackTable)
    {
        for (auto& track : trackList.second)
        {
            if (track)
            {
                delete track;
            }
        }
    }
    _weaponIDToTrackTable.clear();
    for (auto& [weaponID, trackDataList] : ReflectFields->WeaponQTETrackData)
    {
        for (auto& trackData : trackDataList)
        {
            QTE::Track* track = new QTE::Track;
            auto& trackVector = _weaponIDToTrackTable[weaponID];
            track->DeserializedReflectFields(trackData);
            trackVector.push_back(track);
        }
    }
}

void QTESystem::ImGuiDrawPropertysEvent()
{
    GetEditor().Show();
    if (ImGui::Button("Open QTE Editor"))
    {
        GetEditor().Open();
    }
    if (ImGui::Button("Start QTE"))
    {
        StartQTE();
    }
}

bool QTESystem::AddMappingTrackToWeaponID(int weaponID, const File::Path& path)
{
    QTE::Track* track = new QTE::Track;
    auto&       trackVector = _weaponIDToTrackTable[weaponID];
    trackVector.push_back(track);
    track->SetWeaponID(weaponID);

    if (File::NULL_PATH != path)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, (const char*)u8"QTE 트랙 파일 로드에 실패했습니다.");
        track->LoadFile(path);
    }
    return true;
}

bool QTESystem::RemoveMappingTrackToWeaponID(int weaponID, int index)
{
    auto itr = _weaponIDToTrackTable.find(weaponID);
    if (itr != _weaponIDToTrackTable.end())
    {
        auto& trackVector = itr->second;
        if (trackVector.empty())
        {
            return false;
        }
        // -1이면 마지막 트랙의 인덱스로 지정
        if (-1 == index)
        {
            index = (int)trackVector.size() - 1;
        }
        if (index >= 0 && index < (int)trackVector.size())
        {
            auto track = trackVector[index];
            if (track)
            {
                delete track;
            }
            trackVector.erase(trackVector.begin() + index);
            return true;
        }
    }
    return false;
}

QTE::Track* QTESystem::GetMappingTrackToWeaponID(int weaponID, int index)
{
    auto itr = _weaponIDToTrackTable.find(weaponID);
    if (itr != _weaponIDToTrackTable.end())
    {
        auto& trackVector = itr->second;
        if (index >= 0 && index < (int)trackVector.size())
        {
            return trackVector[index];
        }
    }
    return nullptr;
}

void QTESystem::StartQTE()
{
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
            _currentQTETrack = itr->second[index];
            StartQTE(_currentQTETrack);
        }
    }
}

void QTESystem::StartQTE(QTE::Track* qteTrack)
{
    if (_isQTEPlaying)
    {
        UmLogger.Log(
            LogLevel::LEVEL_WARNING,
            (const char*)u8"QTE가 진행 중인 상태에서 QTE 시작 요청을 한번 더 했습니다. 다시 한번 확인해주세요.");
        return;
    }

    _isQTEPlaying = true;
    _noteAvailQueue.clear();
    _noteResultQueue.clear();
    _currentNoteIndex = 0;

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
        float noteTime = _qteTimer - note->Time;
        if (noteTime >= perfectMin && noteTime <= perfectMax)
        {
            UmLogger.Log(LogLevel::LEVEL_DEBUG, (const char*)u8"퍼펙트!!");
            return QTE::QTE_RESULT_PERFECT;
        }
        else if (noteTime >= normalMin && noteTime <= normalMax)
        {
            UmLogger.Log(LogLevel::LEVEL_DEBUG, (const char*)u8"일격!!");
            return QTE::QTE_RESULT_NORMAL;
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_DEBUG, (const char*)u8"미스!!");
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

        float scale     = ReflectFields->QTESpeedScale * _currentQTETrack->GetQTESpeedScale();
        float deltaTime = UmTime.DeltaTime() * scale;
        _qteTimer += deltaTime;

        if (_qteTimer < maxFrame && _currentNoteIndex < _noteAvailQueue.size())
        {
            QTE::Note* curNote = _noteAvailQueue[_currentNoteIndex];
            auto& [normalMin, normalMax] = ReflectFields->NormalJudgeRange;
            if (curNote && _qteTimer + normalMax >= curNote->Time)
            {   // 최대 일격 판정 시간이 지나갔는데 버튼을 누르지 않은 경우, MISS 처리
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
        if (_currentNoteIndex >= _noteAvailQueue.size())
        {
            return;
        }
        QTE::Note*   curNote        = _noteAvailQueue[_currentNoteIndex];
        QTE::Result& result         = _noteResultQueue[_currentNoteIndex];
        ++_currentNoteIndex;

        result.Note                 = curNote;
        result.PressedButton        = type;
        result.ResultType           = GetQTEResult(curNote);
        result.TimeDelta            = curNote ? _qteTimer - curNote->Time : 0.0f;

        QTEPreviewer::PressedNote(&result);
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