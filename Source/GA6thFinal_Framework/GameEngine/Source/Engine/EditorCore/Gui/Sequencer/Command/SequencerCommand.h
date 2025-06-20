#pragma once

class TimelineSystem;
class TimelineNotify;
class ITimelineEvent;

namespace Command
{
    namespace Sequencer
    {
        /// <summary>
        /// MinFrame을 바꾸도록 동작하는 명령입니다.
        /// </summary>
        class ChangeMinFrame : public UmCommand
        {
        public:
            ChangeMinFrame(std::weak_ptr<TimelineSystem> system, float frame)
                : UmCommand("ChangeMinFrame"), _timelineSystem(system), _prevFrame(frame), _tempFrame(-FLT_MAX)
            {
            }
            virtual ~ChangeMinFrame() = default;

        private:
            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;

            std::weak_ptr<TimelineSystem> _timelineSystem;
            float _prevFrame;
            float _tempFrame;
        };
        /// <summary>
        /// MaxFrame을 바꾸도록 동작하는 명령입니다.
        /// </summary>
        class ChangeMaxFrame : public UmCommand
        {
        public:
            ChangeMaxFrame(std::weak_ptr<TimelineSystem> system, float frame)
                : UmCommand("ChangeMaxFrame"), _timelineSystem(system), _prevFrame(frame), _tempFrame(-FLT_MAX)
            {
            }
            virtual ~ChangeMaxFrame() = default;

        private:
            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;

            std::weak_ptr<TimelineSystem> _timelineSystem;
            float _prevFrame;
            float _tempFrame;
        };

        class AddNotify : public UmCommand
        {
        public:
            AddNotify(std::weak_ptr<TimelineSystem> system, float time, std::string_view typeNameID)
                : UmCommand("AddNotify")
                , _timelineSystem(system)
                , _notify(nullptr)
                , _time(time)
                , _typeNameID(typeNameID)
                , _id(UINT_MAX)
            {
            }
            virtual ~AddNotify() = default;

        private:
            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;

            std::weak_ptr<TimelineSystem> _timelineSystem;
            TimelineNotify* _notify; 
            float _time;
            std::string _typeNameID;
            UINT _id;
        };

        class RemoveNotify : public UmCommand
        {
        public:
            RemoveNotify(std::weak_ptr<TimelineSystem> system, TimelineNotify* notify);
            virtual ~RemoveNotify() = default;

        private:
            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;

            std::weak_ptr<TimelineSystem> _timelineSystem;
            TimelineNotify* _notify;    
            float _time;     
            std::string _typeNameID;    
            UINT _id;
        };

        class ChangeNotifyTime : public UmCommand
        {
        public:
            ChangeNotifyTime(std::weak_ptr<TimelineSystem> system, TimelineNotify* notify, float changeTime,
                             std::string_view changeTypeNameID);
            virtual ~ChangeNotifyTime() = default;
        private:
            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;

            std::weak_ptr<TimelineSystem> _timelineSystem;
            TimelineNotify* _notify;    
            float _previousTime;
            float _changedTime;
            std::string _previousTypeNameID;
            std::string _changedTypeNameID;
        };
    } // namespace Sequencer
} // namespace Command