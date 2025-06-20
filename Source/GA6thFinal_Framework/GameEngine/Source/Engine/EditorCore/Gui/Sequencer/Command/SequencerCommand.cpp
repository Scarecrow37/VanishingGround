#include "pch.h"
#include "SequencerCommand.h"

namespace Command
{
    namespace Sequencer
    {
        void ChangeMinFrame::Execute()
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system || _tempFrame == -FLT_MAX)
                return;
            system->SetMinFrame(_tempFrame);
        }

        void ChangeMinFrame::Undo()
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system)
                return;
            _tempFrame = system->GetMinFrame();
            system->SetMinFrame(_prevFrame);
        }

        void ChangeMaxFrame::Execute()
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system || _tempFrame == -FLT_MAX)
                return;
            system->SetMaxFrame(_tempFrame);
        }

        void ChangeMaxFrame::Undo()
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system)
                return;
            _tempFrame = system->GetMaxFrame();
            system->SetMaxFrame(_prevFrame);
        }

        void AddNotify::Execute()
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system)
                return;
            _notify = system->AddNotify(_time, _typeNameID);
        }

        void AddNotify::Undo() 
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system)
                return;
            system->RemoveNotifyFromNotify(&_notify);
        }

        RemoveNotify::RemoveNotify(std::weak_ptr<TimelineSystem> system, TimelineNotify* notify)
            : UmCommand("RemoveNotify"), _timelineSystem(system), _notify(notify), _time(notify->Time),
              _typeNameID(notify->EventName)
        {
        }
        void RemoveNotify::Execute() 
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system || nullptr == _notify)
                return;
            _time = _notify->Time;
            _typeNameID = _notify->EventName;
            system->RemoveNotifyFromNotify(&_notify);
        }

        void RemoveNotify::Undo() 
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system)
                return;
            _notify = system->AddNotify(_time, _typeNameID);
        }

        ChangeNotifyTime::ChangeNotifyTime(std::weak_ptr<TimelineSystem> system, TimelineNotify* notify,
                                           float changeTime, std::string_view changeTypeNameID)
            : UmCommand("ChangeNotify"), _timelineSystem(system), _notify(notify), _previousTime(notify->Time),
              _changedTime(changeTime), _previousTypeNameID(notify->EventName), _changedTypeNameID(changeTypeNameID)
        {
        }

        void ChangeNotifyTime::Execute() 
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system || _notify == nullptr)
                return;
            _notify->SetNotifyEvent(_changedTime, _changedTypeNameID);
        }

        void ChangeNotifyTime::Undo() 
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system || _notify == nullptr)
                return;
            _notify->SetNotifyEvent(_previousTime, _previousTypeNameID);
        }
    } // namespace Sequencer
} // namespace Command