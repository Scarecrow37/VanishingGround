#include "pch.h"
#include "SequencerCommand.h"
#include "Engine/TimelineCore/Context/TimelineEventContext.h"

namespace Command
{
    namespace Sequencer
    {
        ContextData::ContextData(UINT id, float time, std::string_view label, std::string_view typeNameID)
            : ID(id), Time(time), Label(label), TypeNameID(typeNameID)
        {
        }
        ContextData::ContextData(Timeline::EventContext* context)
            : ID(context->ID), Time(context->Time), Label(context->Label), TypeNameID(context->EventType)
        {
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// 
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ChangeMinFrame::ChangeMinFrame(std::weak_ptr<Timeline::EventTrack> track, float frame)
            : UmCommand("ChangeMinFrame"), _eventTrack(track), _prevFrame(frame), _tempFrame(-FLT_MAX)
        {
        }
        bool ChangeMinFrame::Execute()
        {
            auto track = _eventTrack.lock();
            if (nullptr == track || _tempFrame == -FLT_MAX)
            {
                return false;
            }

            track->SetMinFrame(_tempFrame);

            return true;
        }
        void ChangeMinFrame::Undo()
        {
            auto track = _eventTrack.lock();
            if (nullptr == track)
                return;
            _tempFrame = track->GetMinFrame();
            track->SetMinFrame(_prevFrame);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ChangeMaxFrame::ChangeMaxFrame(std::weak_ptr<Timeline::EventTrack> track, float frame)
            : UmCommand("ChangeMaxFrame"), _eventTrack(track), _prevFrame(frame), _tempFrame(-FLT_MAX)
        {
        }
        bool ChangeMaxFrame::Execute()
        {
            auto track = _eventTrack.lock();
            if (nullptr == track || _tempFrame == -FLT_MAX)
            {
                return false;
            }

            track->SetMaxFrame(_tempFrame);

            return true;
        }
        void ChangeMaxFrame::Undo()
        {
            auto track = _eventTrack.lock();
            if (nullptr == track)
                return;
            _tempFrame = track->GetMaxFrame();
            track->SetMaxFrame(_prevFrame);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        AddContext::AddContext(std::weak_ptr<Timeline::EventTrack> track, float time, std::string_view label, std::string_view typeNameID)
            : UmCommand("AddContext"), _eventTrack(track), _context(nullptr),
              _contextData({UINT_MAX, time, label, typeNameID})
        {
        }
        bool AddContext::Execute()
        {
            auto track = _eventTrack.lock();
            if (nullptr == track)
            {
                return false;
            }
               
            _context = track->AddEventFromTypeName(_contextData.Label, _contextData.TypeNameID, _contextData.Time, _contextData.ID);
            if (UINT_MAX == _contextData.ID)
            {
                _contextData.ID = _context->ID;
            }

            return true;
        }
        void AddContext::Undo() 
        {
            auto track = _eventTrack.lock();
            if (nullptr == track)
                return;
            track->RemoveContextFromID(_contextData.ID);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        RemoveContext::RemoveContext(std::weak_ptr<Timeline::EventTrack> track, Timeline::EventContext* context)
            : UmCommand("RemoveContext"), _eventTrack(track), _context(context), _contextData(context)
        {
        }
        bool RemoveContext::Execute() 
        {
            auto track = _eventTrack.lock();
            if (nullptr == track || nullptr == _context)
            {
                return false;
            }

            track->RemoveContextFromID(_contextData.ID);

            return true;
        }
        void RemoveContext::Undo() 
        {
            auto track = _eventTrack.lock();
            if (nullptr == track)
            {
                return;
            }
            _context = track->AddEventFromTypeName(_contextData.Label, _contextData.TypeNameID, _contextData.Time, _contextData.ID);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ChangeContext::ChangeContext(std::weak_ptr<Timeline::EventTrack> track, Timeline::EventContext* context, float changeTime,
                                   std::string_view changelabel, std::string_view changeTypeNameID)
            : UmCommand("ChangeContext"), _eventTrack(track), _context(context),
              _previousData({context->ID, context->Time, context->Label, context->EventType}),
              _changedData({context->ID, changeTime, changelabel, changeTypeNameID})
        {
        }
        bool ChangeContext::Execute() 
        {
            auto track = _eventTrack.lock();
            if (nullptr == track || nullptr == _context)
            {
                return false;
            }

            if (_previousData.Label != _changedData.Label)
            {
                _context->Label = _changedData.Label;
            }
            if (_previousData.Time != _changedData.Time)
            {
                _context->SetTime(_changedData.Time);
            }
            if (_previousData.TypeNameID != _changedData.TypeNameID)
            {
                _context->SetEvent(_changedData.TypeNameID);
            }
            return true;
        }
        void ChangeContext::Undo() 
        {
            auto track = _eventTrack.lock();
            if (nullptr == track || _context == nullptr)
                return;
            if (_previousData.Label != _changedData.Label)
            {
                _context->Label = _previousData.Label;
            }
            if (_previousData.Time != _changedData.Time)
            {
                _context->SetTime(_previousData.Time);
            }
            if (_previousData.TypeNameID != _changedData.TypeNameID)
            {
                _context->SetEvent(_previousData.TypeNameID);
            }
        }
    } // namespace Sequencer
} // namespace Command