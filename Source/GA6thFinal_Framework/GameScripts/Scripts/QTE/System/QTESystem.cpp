#include "pchScripts.h"
#include "QTESystem.h"
#include <QTE/UI/QTEUIManager.h>
#include <QTE/Editor/QTEEditor.h>
#include <QTE/Track/QTETrack.h>

#include <WeaponSystem/WeaponSystem.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include "Camera/UmCineMotion.h"
#include "CombatUIManager/CombatUIManager.h"

#include "Token/Object/Focus/FocusToken.h"

UMREAL_COMPONENT(QTESystem)

QTESystem::QTESystem() = default;

QTESystem::~QTESystem() = default;

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
    if (IsPlaying)
    {
#ifdef _UMEDITOR
        if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow, false))
        {
            ProcessQTEButtonPressedEvent();
            if (CanPressQTEButton())
            {
                PressedQTEButton(_keyBinder.GetKeyX());
            }
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_UpArrow, false))
        {
            ProcessQTEButtonPressedEvent();
            if (CanPressQTEButton())
            {
                PressedQTEButton(_keyBinder.GetKeyY());
            }
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow, false))
        {
            ProcessQTEButtonPressedEvent();
            if (CanPressQTEButton())
            {
                PressedQTEButton(_keyBinder.GetKeyB());
            }
        }
#endif // _UMEDITOR

        auto& [controller, button] = _nextKeyEvent;
        if (controller)
        {
            PressedQTEButton(button);
            controller = nullptr;
        }
    }
    if (QTE::STATE_PLAYING == _currState)
    {
        ProcessQTEPlayingEvent();
    }
    _prevState = _currState;
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
        bool allCrit = _overallResult.CompareResult(QTE::QTE_RESULT_ALL_CRIT);
        bool overHit = _overallResult.CompareResult(QTE::QTE_RESULT_OVER_HIT);
        ImGui::Text("Overall Result: %s%s", allCrit ? "All Crit " : "", overHit ? "Over Hit" : "");

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

QTE::Track* QTESystem::AddMappingTrackToWeaponID(const int weaponID, const File::Path& path)
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

bool QTESystem::RemoveMappingTrackToWeaponID(const int weaponID, int index)
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

QTE::Track* QTESystem::GetMappingTrackToWeaponID(const int weaponID, const int index)
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
    WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance();
    if (weaponSystem)
    {
        auto& weapon = weaponSystem->GetCurrentWeaponElement().Stats;
        if (_weaponIDToTrackTable.contains(weapon.WeaponID))
        {
            auto& trackVector = _weaponIDToTrackTable[weapon.WeaponID];
            if (false == trackVector.empty())
            {
                int index = Random::Range(0, (int)trackVector.size() - 1);
                _currentQTETrack = trackVector[index];
                StartQTE(trackVector[index]);
            }
        }
        else
        {
            // 현재 무기에 매핑된 QTE 트랙이 없는 경우
            StartQTE(&weapon);
        }
    }
}

void QTESystem::StartQTE(QTE::Track* qteTrack)
{
    //assert(_currState == QTE::STATE_WAITING && "QTE가 완료되기 전까진 Start를 호출하면 안됩니다.");
    if (_currState == QTE::STATE_WAITING)
    {
        ResetQTEState();
        if (qteTrack)
        {
            Timeline::EventTrack* track = qteTrack->GetEventTrack().lock().get();
            assert(track && "QTE를 진행하기 위한 트랙이 없습니다.");
            if (track)
            {
                _currTime   = track->GetMinFrame() - GetDelayFromQTEStart();
                _totalTime = track->GetMaxFrame();
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
                ProcessQTEEnterEvent();
            }
        }
    }
}

#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
void QTESystem::StartQTE(const WeaponStats* weapon) 
{
    //assert(_currState == QTE::STATE_WAITING && "QTE가 완료되기 전까진 Start를 호출하면 안됩니다.");
    if (_currState == QTE::STATE_WAITING)
    {
        ResetQTEState();
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
            _currTime  = -GetDelayFromQTEStart();
            _totalTime = totalTime + 1.0f;
            ProcessQTEEnterEvent();
        }
    }
}

void QTESystem::StopQTE() 
{
    if (_currState != QTE::STATE_WAITING)
    {
        _currState = QTE::STATE_FADE_OUT;
    }
}

void QTESystem::PauseQTE(const bool pause) 
{
    _isPaused = pause;
}

void QTESystem::ClearKeyBindState() 
{
    _keyBinder.ClearBindState();
}

void QTESystem::PushKeyBindState(const QTE::KeyBindState& bindState)
{
    _keyBinder.PushKeyBindState(bindState);
}

void QTESystem::PopKeyBindState()
{
    _keyBinder.PopKeyBindState();
}

QTE::Callback::Handle QTESystem::RegisterCallback(const QTE::Callback& callback)
{
    return _callbackHandler.RegisterCallback(callback);
}

bool QTESystem::UnRegisterCallback(QTE::Callback::Handle handle)
{
    return _callbackHandler.UnRegisterCallback(handle);
}

QTE::ResultType QTESystem::GetQTEResult(const float noteTime)
{
    auto [perfectMin, perfectMax] = ReflectFields->PerfectJudgeRange;
    auto [normalMin, normalMax]   = ReflectFields->NormalJudgeRange;
    float noteDelta               = _currTime - noteTime;

    // 집중 토큰이 있다면 치명타 범위가 일격 범위랑 같아짐.
    if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
    {
        Player*         player         = turnMode->GetPlayer();
        TokenInventory& tokenInventory = player->GetTokenInventory();
        if (tokenInventory.HasTokenFromID(TokenObject::Focus::ID))
        {
            perfectMin = normalMin;
            perfectMax = normalMax;
        }
    }

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
    _currState  = QTE::STATE_WAITING;
    _prevState  = QTE::STATE_WAITING;
    _currTime   = 0.0f;
    _totalTime  = 0.0f;
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
    if (false == _isPaused)
    {
        _currTime += UmTime.DeltaTime();
    }
    if (_currTime < _totalTime && _currentNoteIndex < _noteAvailQueue.size())
    {
        const QTE::NoteData& curNote = _noteAvailQueue[_currentNoteIndex];
        auto& [validMin, validMax]   = ReflectFields->ValidJudgeRange;

        // 디버깅 용도. (노트랑 완벽히 같은 시간으로 설정 후 클릭 이벤트 보내기.)
        //if (curNote.Time < _currTime)
        //{
        //    _currTime = curNote.Time;
        //    PressedQTEButton(Input::Controller::Button::B);
        //    // 잘나오는데요??? 걍 렉때메 판정이 이상해보임.
        //}

        if (_currTime > curNote.Time + validMax)
        {
            PressedQTEButton(); // 최대 일격 판정 시간이 지나갔는데 버튼을 누르지 않은 경우, MISS 처리
        }
    }
    else
    {
        // 시간이 모두 경과했거나, 모든 노트를 처리한 경우 QTE 종료
        StopQTE();
        ProcessQTEExitEvent();
    }
}

QTEEditor& QTESystem::GetEditor()
{
    static QTEEditor editor;
    return editor;
}

bool QTESystem::CanPressQTEButton()
{
    if (QTE::STATE_PLAYING == _currState)
    {
        if (_currentNoteIndex < _noteAvailQueue.size())
        {
            const QTE::NoteData& curNote = _noteAvailQueue[_currentNoteIndex];
            return CanPressQTEButton(curNote.Time);
        }
    }
    return false;
}

bool QTESystem::CanPressQTEButton(const float noteTime)
{
    auto& [min, max] = ReflectFields->ValidJudgeRange;
    float noteDelta  = _currTime - noteTime;
    if (noteDelta >= min && noteDelta <= max)
    {
        return true;
    }
    return false;
}

void QTESystem::PressedQTEButton(const Input::Controller::Button buttonType)
{
    if (QTE::STATE_PLAYING == _currState)
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
        result.TimeDelta     = _currTime - curNote.Time;
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
        ProcessQTENotePressedEvent(result.NoteData->ID, result.Result);
    }
}

void QTESystem::PressedButtonX(const Input::Controller& controller)
{
    ProcessQTEButtonPressedEvent();
    // Handle button X pressed
    if (CanPressQTEButton())
    {
        _nextKeyEvent = {&controller, _keyBinder.GetKeyX()};
    }
}

void QTESystem::PressedButtonY(const Input::Controller& controller)
{
    ProcessQTEButtonPressedEvent();
    // Handle button Y pressed
    if (CanPressQTEButton())
    {
        _nextKeyEvent = {&controller, _keyBinder.GetKeyY()};
    }
}

void QTESystem::PressedButtonB(const Input::Controller& controller)
{
    ProcessQTEButtonPressedEvent();
    // Handle button B pressed
    if (CanPressQTEButton())
    {
        _nextKeyEvent = {&controller, _keyBinder.GetKeyB()};
    }
}

void QTESystem::ProcessQTEEnterEvent() 
{
    UmAudio.FadeOut();

    _currState = QTE::STATE_FADE_IN;
    _callbackHandler.ProcessQTEFadeInStartEvent();
    QTEUIManager* uiManager = SingletonComponent<QTEUIManager>::GetInstance();
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

void QTESystem::ProcessQTEExitEvent() 
{
    UmAudio.FadeIn();

    _overallResult.UpdateResult(); // 결과 갱신
    _currState = QTE::STATE_FADE_OUT;
    _callbackHandler.ProcessQTEFadeOutStartEvent(_overallResult);
    
    if (QTEUIManager* uiManager = SingletonComponent<QTEUIManager>::GetInstance())
    {
        uiManager->OnQTEExit(); // 여기서 페이드 아웃 시작
    }
    if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
    {
        for (auto& character : turnMode->GetCharacters())
        {
            if (character)
            {
                character->OnQTEEnd();
            }
        }
        if (Player* player = turnMode->GetPlayer())
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
    _currState = QTE::STATE_PLAYING;
}

void QTESystem::ProcessQTEFadeOutEndEvent() 
{
    _currState = QTE::STATE_WAITING;
    _callbackHandler.ProcessQTEFadeOutFinishEvent(_overallResult);

    auto camera = dynamic_cast<UmCineMotion*>(CameraComponent::MainCamera());
    if (camera)
    {
        camera->StartRail(false);
    }
}

void QTESystem::ProcessQTEPlayingEvent() 
{
    UpdateQTETrack();
    QTEUIManager* uiManager = SingletonComponent<QTEUIManager>::GetInstance();
    if (uiManager)
    {
        uiManager->OnQTEPlay();
    }
}

void QTESystem::ProcessQTEButtonPressedEvent()
{
    _callbackHandler.ProcessQTEButtonPressedEvent();
    QTEUIManager* uiManager = SingletonComponent<QTEUIManager>::GetInstance();
    if (uiManager)
    {
        uiManager->OnQTEButtonPressed();
    }
}

void QTESystem::ProcessQTENotePressedEvent(const UINT noteID, const QTE::ResultType result)
{
    _callbackHandler.ProcessQTENotePressedEvent(noteID, result);
    if (QTEUIManager* uiManager = SingletonComponent<QTEUIManager>::GetInstance())
    {
        uiManager->OnQTENotePressed(noteID, result);
    }
}