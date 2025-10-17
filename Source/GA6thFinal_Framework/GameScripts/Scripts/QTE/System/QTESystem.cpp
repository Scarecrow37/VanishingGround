#include "pchScripts.h"
#include "QTESystem.h"
#include <QTE/UI/QTEUIManager.h>
#include <QTE/Editor/QTEEditor.h>
#include <QTE/Track/QTETrack.h>

#include <WeaponSystem/WeaponSystem.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include "Camera/UmCineMotion.h"
#include "CombatUIManager/CombatUIManager.h"

UMREAL_COMPONENT(QTESystem)

QTESystem::QTESystem() 
{
}

QTESystem::~QTESystem() 
{
}

void QTESystem::Reset()
{
    _singletonComponent.SetSingleTon();
}

void QTESystem::Awake()
{
    if (_singletonComponent.TrySingleTon())
    {
        BindInputAction(ControllerButton::X, Action::PRESSED, this, this, &QTESystem::PressedButtonX);
        BindInputAction(ControllerButton::Y, Action::PRESSED, this, this, &QTESystem::PressedButtonY);
        BindInputAction(ControllerButton::B, Action::PRESSED, this, this, &QTESystem::PressedButtonB);
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, (const char*)u8"씬에 QTESystem이 2개 이상 존재하는지 확인해주세요.");
    }
}

void QTESystem::Start() 
{

}

void QTESystem::Update()
{
    if (IsQTEPlaying())
    {
#ifdef _UMEDITOR
        if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow, false))
        {
            PressedQTEButton(Input::ControllerTypes::Button::X);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow, false))
        {
            PressedQTEButton(Input::ControllerTypes::Button::Y);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_RightArrow, false))
        {
            PressedQTEButton(Input::ControllerTypes::Button::B);
        }
#endif // _UMEDITOR

        auto& [controller, button] = _nextControllerEvent;
        if (controller)
        {
            PressedQTEButton(button);
            controller = nullptr;
        }
    }
    if (true == _currQTEPlaying && false == _prevQTEPlaying)
    {
        ProcessQTEEnterEvent();
    }
    _prevQTEPlaying = _currQTEPlaying;
    if (true == _currQTEPlaying)
    {
        ProcessQTEStayEvent();
    }
    if (false == _currQTEPlaying && true == _prevQTEPlaying)
    {
        ProcessQTEExitEvent();
    }
}

void QTESystem::OnDestroy() 
{
    ClearTrack();
}

void QTESystem::SerializedReflectEvent()
{
    ReflectFields->WeaponQTETrackGuids.clear();
    for (auto& [weaponID, trackList] : _weaponIDToTrackTable)
    {
        for (auto& track : trackList)
        {
            const File::Guid& guid = track->GetFilePath().ToGuid();
            ReflectFields->WeaponQTETrackGuids[weaponID].emplace_back(guid.string());
        }
    }
}

void QTESystem::DeserializedReflectEvent() 
{
    ClearTrack();
    for (auto& [weaponID, guids] : ReflectFields->WeaponQTETrackGuids)
    {
        for (auto& guidStr : guids)
        {
            QTE::Track* track = new QTE::Track;
            auto& trackVector = _weaponIDToTrackTable[weaponID];
            File::Guid guid(guidStr);
            if (track->LoadFile(guid))
            {
                trackVector.push_back(track);
            }
            else
            {
                std::string message = std::format("{} {}", (const char*)u8"QTE 트랙 파일 로드에 실패했습니다. 파일 경로: ", guid.ToPath().string());
                UmLogger.Log(LogLevel::LEVEL_ERROR, message);
                delete track;
            }
        }
    }
}

void QTESystem::ImGuiDrawPropertysEvent()
{
    GetEditor().Show();
    
    if (ImGui::TreeNodeEx("Debug##debug"))
    {
        if (_currentQTETrack)
        {
            ImGui::SliderFloat("Frame Timer", &_qteTimer, _currentQTETrack->GetMinFrame(), _currentQTETrack->GetMaxFrame());

            bool allCrit = _overallResult.CompareResult(QTE::QTE_RESULT_ALL_CRIT);
            bool overHit = _overallResult.CompareResult(QTE::QTE_RESULT_OVER_HIT);
            ImGui::Text("Overall Result: %s%s", allCrit ? "All Crit " : "", overHit ? "Over Hit" : "");
        }
        ImGui::TreePop();
    }
    
    if (ImGui::Button("Open QTE Editor"))
    {
        GetEditor().Open();
    }
    if (ImGui::Button("Start QTE"))
    {
        StartQTE();
    }
}

QTE::Track* QTESystem::AddMappingTrackToWeaponID(int weaponID, const File::Path& path)
{
    QTE::Track* track = new QTE::Track;
    auto&       trackVector = _weaponIDToTrackTable[weaponID];
    trackVector.push_back(track);

    // 기본 경로가 아닌 경우 파일 로드 시도
    if (File::NULL_PATH != path && false == track->LoadFile(path))
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, (const char*)u8"QTE 트랙 파일 로드에 실패했습니다.");
        return nullptr;
    }
    return track;
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

void QTESystem::StartQTE(Callback callback)
{
    // 현재 무기에 맞는 QTE 트랙을 선택
    WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance();
    if (weaponSystem)
    {
        auto& weapon = weaponSystem->GetCurrentWeaponStats();
        auto  itr = _weaponIDToTrackTable.find(weapon.WeaponID);
        if (itr != _weaponIDToTrackTable.end() && false == itr->second.empty())
        {
            auto& trackVector = itr->second;
            if (false == trackVector.empty())
            {
                int index        = Random::Range(0, (int)trackVector.size() - 1);
                _currentQTETrack = trackVector[index];
                StartQTE(_currentQTETrack, callback);
            }
        }
        else
        {
            // 현재 무기에 매핑된 QTE 트랙이 없는 경우
            StartQTE(&weapon, callback);
        }
    }
}

void QTESystem::StartQTE(QTE::Track* qteTrack, Callback callback)
{
    if (_currQTEPlaying)
    {
        UmLogger.Log(
            LogLevel::LEVEL_WARNING,
            (const char*)u8"QTE가 진행 중인 상태에서 QTE 시작 요청을 한번 더 했습니다. 다시 한번 확인해주세요.");
        return;
    }
    ResetQTEState();
    _onQTEFinishCallback = callback;

    bool result = false;
    if (qteTrack)
    {
        auto track = qteTrack->GetEventTrack().lock();
        if (track)
        {
            _qteTimer = track->GetMinFrame() - GetDelayFromQTEStart();
            _qteMaxTime = track->GetMaxFrame();
            // 유효한 노트 큐 생성
            auto& noteQueue = track->GetEventContextQueue();
            _noteAvailQueue.reserve(noteQueue.size());
            _overallResult.NoteResults.reserve(noteQueue.size());
            for (auto& note : noteQueue)
            {
                QTE::Note* qteNote = dynamic_cast<QTE::Note*>(note);
                if (qteNote)
                {
                    QTE::NoteData& noteData = _noteAvailQueue.emplace_back(qteNote->ToNoteData());
                    _overallResult.NoteResults.emplace_back(&noteData);
                }
            }
            result = true;
        }
    }
    if (result)
    {
        ProcessQTEEnterEvent();
    }
    else
    {
        StopQTE();
    }
}

#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
void QTESystem::StartQTE(const WeaponStats* weapon, Callback callback) 
{
    if (_currQTEPlaying)
    {
        UmLogger.Log(
            LogLevel::LEVEL_WARNING,
            (const char*)u8"QTE가 진행 중인 상태에서 QTE 시작 요청을 한번 더 했습니다. 다시 한번 확인해주세요.");
        return;
    }
    ResetQTEState();
    _onQTEFinishCallback = callback;

    bool result = false;
    if (weapon)
    {
        int count = weapon->AttackCount;
        _noteAvailQueue.resize(count);
        _overallResult.NoteResults.resize(count);
        float totalTime = 0.0f;
        for (int i = 0; i < count; ++i)
        {
            float time = Random::Range(0.2f, 0.6f);
            totalTime += time;

            _noteAvailQueue[i].ID                   = i + 1;
            _noteAvailQueue[i].Time                 = totalTime;
            _noteAvailQueue[i].WeaponAnimationKey   = "WeaponAttack_01";
            _noteAvailQueue[i].WeaponAnimationDelay = 0.0f;

            _overallResult.NoteResults[i] = &_noteAvailQueue[i];
        }
        _qteTimer   = -GetDelayFromQTEStart();
        _qteMaxTime = totalTime + 1.0f;

        result = true;
    }

    if (result)
    {
        ProcessQTEEnterEvent();
    }
    else
    {
        StopQTE();
    }
}

void QTESystem::StopQTE() 
{
    _currQTEPlaying = false;
}

void QTESystem::PauseQTE(bool pause) 
{
    if (IsQTEPlaying())
    {
        _qtePaused = pause;
    }
}

QTE::ResultType QTESystem::GetQTEResult(float noteTime)
{
    auto& [perfectMin, perfectMax] = ReflectFields->PerfectJudgeRange;
    auto& [normalMin, normalMax]   = ReflectFields->NormalJudgeRange;
    float noteDelta                = _qteTimer - noteTime;
    if (noteDelta >= perfectMin && noteDelta <= perfectMax)
    {
        UmLogger.Log(LogLevel::LEVEL_DEBUG, (const char*)u8"퍼펙트!!");
        return QTE::QTE_RESULT_PERFECT;
    }
    else if (noteDelta >= normalMin && noteDelta <= normalMax)
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

void QTESystem::ResetQTEState() 
{
    _qteTimer            = 0.0f;
    _qteMaxTime          = 0.0f;
    _onQTEFinishCallback = nullptr;
    _currQTEPlaying      = true;
    _qteFadeInEnd        = false;
    _qteFadeOutEnd       = false;
    PauseQTE(false);
    ClearQueue();
}

void QTESystem::ClearTrack()
{
    for (auto& [weaponID, trackList] : _weaponIDToTrackTable)
    {
        for (auto& track : trackList)
        {
            if (track)
            {
                delete track;
            }
        }
    }
    _weaponIDToTrackTable.clear();
}

void QTESystem::ClearQueue()
{
    _currentNoteIndex = 0;
    _noteAvailQueue.clear();
    _overallResult.Clear();
}

void QTESystem::UpdateQTETrack()
{
    if (false == _currQTEPlaying || false == _qteFadeInEnd)
    {
        return;
    }
    if (false == _qtePaused)
    {
        _qteTimer += UmTime.DeltaTime();
    }
    if (_qteTimer < _qteMaxTime && _currentNoteIndex < _noteAvailQueue.size())
    {
        const QTE::NoteData& curNote = _noteAvailQueue[_currentNoteIndex];
        auto& [validMin, validMax]   = ReflectFields->ValidJudgeRange;
        if (_qteTimer > curNote.Time + validMax)
        {
            PressedQTEButton(); // 최대 일격 판정 시간이 지나갔는데 버튼을 누르지 않은 경우, MISS 처리
        }
    }
    else
    {
        // 시간이 모두 경과했거나, 모든 노트를 처리한 경우 QTE 종료
        StopQTE();
    }
}

QTEEditor& QTESystem::GetEditor()
{
    static QTEEditor editor;
    return editor;
}

bool QTESystem::CanPressQTEButton()
{
    if (_currQTEPlaying && _currentNoteIndex < _noteAvailQueue.size())
    {
        const QTE::NoteData& curNote = _noteAvailQueue[_currentNoteIndex];
        return CanPressQTEButton(curNote.Time);
    }
    return false;
}

bool QTESystem::CanPressQTEButton(float noteTime)
{
    auto& [min, max] = ReflectFields->ValidJudgeRange;
    float noteDelta  = _qteTimer - noteTime;
    if (noteDelta >= min && noteDelta <= max)
    {
        return true;
    }
    return false;
}

void QTESystem::PressedQTEButton(Input::Controller::Button buttonType)
{
    if (_currQTEPlaying)
    {
        if (_currentNoteIndex >= _noteAvailQueue.size())
        {
            return;
        }

        const QTE::NoteData& curNote = _noteAvailQueue[_currentNoteIndex];
        QTE::NoteResult& result = _overallResult.NoteResults[_currentNoteIndex];
        ++_currentNoteIndex;

        result.NoteData      = &curNote;
        result.Result        = GetQTEResult(curNote.Time);
        result.TimeDelta     = _qteTimer - curNote.Time;
        result.PressedButton = buttonType;

        auto& inputSystem = ESceneManager::Engine::GetInputSystem();
        switch (result.Result)
        {
            case QTE::QTE_RESULT_PERFECT:
            {
                ++_overallResult.PerfectCount;
                UmAudio.Play("-21000");
                inputSystem.Vibrate(PERFECT_VIBRATION);
                break;
            }
            case QTE::QTE_RESULT_NORMAL:
            {
                ++_overallResult.NormalCount;
                UmAudio.Play("-21010");
                inputSystem.Vibrate(NORMAL_VIBRATION);
                break;
            }
            case QTE::QTE_RESULT_MISS:
            {
                ++_overallResult.MissCount;
                //UmAudio.Play("-21020");
                inputSystem.Vibrate(MISS_VIBRATION);
                break;
            }
            default:
                break;
        }

        ProcessQTENotePressedEvent(result.Result);
    }
}

void QTESystem::PressedButtonX(const Input::Controller& controller)
{
    // Handle button X pressed
    if (CanPressQTEButton())
    {
        _nextControllerEvent = {&controller, Input::ControllerTypes::Button::X};
    }
}

void QTESystem::PressedButtonY(const Input::Controller& controller)
{
    // Handle button Y pressed
    if (CanPressQTEButton())
    {
        _nextControllerEvent = {&controller, Input::ControllerTypes::Button::Y};
    }
}

void QTESystem::PressedButtonB(const Input::Controller& controller)
{
    // Handle button B pressed
    if (CanPressQTEButton())
    {
        _nextControllerEvent = {&controller, Input::ControllerTypes::Button::B};
    }
}

void QTESystem::ProcessQTEEnterEvent() 
{
    QTEUIManager* uiManager = QTEUIManager::GetInstance();
    if (uiManager)
    {
        uiManager->OnQTEEnter();
    }
    TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
    if (turnMode)
    {
        for (auto& character : turnMode->GetCharacters())
        {
            if (character)
            {
                character->OnQTEStart();
            }
        }
        Player* player = turnMode->GetPlayer();
        if (player)
        {
            turnMode->ApplyActions([player](TurnAction& turnAction) { turnAction.OnPlayerQTEStart(*player); });
        }
    }

    if (CombatUIManager* combatUIManager = SingletonComponent<CombatUIManager>::GetInstance())
    {
        combatUIManager->SetActiveUI(false);
    }
}

void QTESystem::ProcessQTENotePressedEvent(QTE::ResultType result)
{
    QTEUIManager* uiManager = QTEUIManager::GetInstance();
    if (uiManager)
    {
        uiManager->OnQTENotePressed(result);
    }
}

void QTESystem::ProcessQTEStayEvent() 
{
    QTEUIManager* uiManager = QTEUIManager::GetInstance();
    if (uiManager)
    {
        uiManager->OnQTEStay();
    }
    UpdateQTETrack();
}

void QTESystem::ProcessQTEExitEvent() 
{
    // 결과 갱신
    _overallResult.UpdateResult();

    QTEUIManager* uiManager = QTEUIManager::GetInstance();
    if (uiManager)
    {
        uiManager->OnQTEExit();
    }
    TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
    if (turnMode)
    {
        for (auto& character : turnMode->GetCharacters())
        {
            if (character)
            {
                character->OnQTEEnd();
            }
        }
        Player* player = turnMode->GetPlayer();
        if (player)
        {
            turnMode->ApplyActions([player, this](TurnAction& turnAction) {
                turnAction.OnPlayerQTEResult(*player, _overallResult); 
                });
        }
    }

    if (CombatUIManager* combatUIManager = SingletonComponent<CombatUIManager>::GetInstance())
    {
        combatUIManager->SetActiveUI(true);
    }
}

void QTESystem::ProcessQTEFadeInEndEvent() 
{
    _qteFadeInEnd = true;
}

void QTESystem::ProcessQTEFadeOutEndEvent() 
{
    _qteFadeOutEnd = true;
    if (_onQTEFinishCallback)
    {
        _onQTEFinishCallback(_overallResult);
        _onQTEFinishCallback = nullptr;
    }
    auto camera = dynamic_cast<UmCineMotion*>(CameraComponent::MainCamera());
    if (camera)
    {
        camera->StartRail(false);
    }
}