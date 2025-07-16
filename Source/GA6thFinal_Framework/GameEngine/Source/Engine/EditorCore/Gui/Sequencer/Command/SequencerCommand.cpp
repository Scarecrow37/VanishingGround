#include "pch.h"
#include "SequencerCommand.h"

namespace Command
{
    namespace Sequencer
    {
        NotifyData::NotifyData(UINT id, float time, std::string_view label, std::string_view typeNameID)
            : ID(id), Time(time), Label(label), TypeNameID(typeNameID)
        {
        }
        NotifyData::NotifyData(TimelineNotify* notify)
            : ID(notify->ID), Time(notify->Time), Label(notify->Label), TypeNameID(notify->EventName)
        {
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// 
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ChangeMinFrame::ChangeMinFrame(std::weak_ptr<TimelineSystem> system, float frame)
            : UmCommand("ChangeMinFrame"), _timelineSystem(system), _prevFrame(frame), _tempFrame(-FLT_MAX)
        {
        }
        bool ChangeMinFrame::Execute()
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system || _tempFrame == -FLT_MAX)
            {
                return false;
            }

            system->SetMinFrame(_tempFrame);

            return true;
        }
        void ChangeMinFrame::Undo()
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system)
                return;
            _tempFrame = system->GetMinFrame();
            system->SetMinFrame(_prevFrame);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ChangeMaxFrame::ChangeMaxFrame(std::weak_ptr<TimelineSystem> system, float frame)
            : UmCommand("ChangeMaxFrame"), _timelineSystem(system), _prevFrame(frame), _tempFrame(-FLT_MAX)
        {
        }
        bool ChangeMaxFrame::Execute()
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system || _tempFrame == -FLT_MAX)
            {
                return false;
            }

            system->SetMaxFrame(_tempFrame);

            return true;
        }
        void ChangeMaxFrame::Undo()
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system)
                return;
            _tempFrame = system->GetMaxFrame();
            system->SetMaxFrame(_prevFrame);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        AddNotify::AddNotify(std::weak_ptr<TimelineSystem> system, float time, std::string_view label, std::string_view typeNameID)
            : UmCommand("AddNotify"), _timelineSystem(system), _notify(nullptr),
              _notifyData({UINT_MAX, time, label, typeNameID})
        {
        }
        bool AddNotify::Execute()
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system)
            {
                return false;
            }
               
            _notify = system->AddNotify(_notifyData.Label, _notifyData.TypeNameID, _notifyData.Time, _notifyData.ID);
            if (UINT_MAX == _notifyData.ID)
            {
                _notifyData.ID = _notify->ID;
            }

            return true;
        }
        void AddNotify::Undo() 
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system)
                return;
            system->RemoveNotifyFromID(_notifyData.ID);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        RemoveNotify::RemoveNotify(std::weak_ptr<TimelineSystem> system, TimelineNotify* notify)
            : UmCommand("RemoveNotify"), _timelineSystem(system), _notify(notify), _notifyData(notify)
        {
        }
        bool RemoveNotify::Execute() 
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system || nullptr == _notify)
            {
                return false;
            }

            system->RemoveNotifyFromID(_notifyData.ID);

            return true;
        }
        void RemoveNotify::Undo() 
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system)
            {
                return;
            }
            _notify = system->AddNotify(_notifyData.Label, _notifyData.TypeNameID, _notifyData.Time, _notifyData.ID);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ChangeNotify::ChangeNotify(std::weak_ptr<TimelineSystem> system, TimelineNotify* notify, float changeTime,
                                   std::string_view changelabel, std::string_view changeTypeNameID)
            : UmCommand("ChangeNotify"), _timelineSystem(system), _notify(notify),
              _previousData({notify->ID, notify->Time, notify->Label, notify->EventName}),
              _changedData({notify->ID, changeTime, changelabel, changeTypeNameID})
        {
        }
        bool ChangeNotify::Execute() 
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system || nullptr == _notify)
            {
                return false;
            }

            if (_previousData.Label != _changedData.Label)
            {
                _notify->Label = _changedData.Label;
            }
            if (_previousData.Time != _changedData.Time)
            {
                _notify->SetNotifyTime(_changedData.Time);
            }
            if (_previousData.TypeNameID != _changedData.TypeNameID)
            {
                _notify->SetNotifyEvent(_changedData.TypeNameID);
            }
            return true;
        }
        void ChangeNotify::Undo() 
        {
            auto system = _timelineSystem.lock();
            if (nullptr == system || _notify == nullptr)
                return;
            if (_previousData.Label != _changedData.Label)
            {
                _notify->Label = _previousData.Label;
            }
            if (_previousData.Time != _changedData.Time)
            {
                _notify->SetNotifyTime(_previousData.Time);
            }
            if (_previousData.TypeNameID != _changedData.TypeNameID)
            {
                _notify->SetNotifyEvent(_previousData.TypeNameID);
            }
        }
    } // namespace Sequencer
} // namespace Command