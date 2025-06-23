#pragma once

class TimelineSystem;
class TimelineNotify;
class ITimelineEvent;

namespace Command
{
    namespace Sequencer
    {
        struct NotifyData
        {
            NotifyData(UINT id, float time, std::string_view label, std::string_view typeNameID);
            NotifyData(TimelineNotify* notify);
            ~NotifyData() = default;

            UINT            ID;
            float           Time;
            std::string     Label;
            std::string     TypeNameID;
        };

        /// <summary>
        /// MinFrame을 바꾸도록 동작하는 명령입니다.
        /// </summary>
        class ChangeMinFrame : public UmCommand
        {
        public:
            ChangeMinFrame(std::weak_ptr<TimelineSystem> system, float frame);
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
            ChangeMaxFrame(std::weak_ptr<TimelineSystem> system, float frame);
            virtual ~ChangeMaxFrame() = default;

        private:
            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;

            std::weak_ptr<TimelineSystem> _timelineSystem;
            float _prevFrame;
            float _tempFrame;
        };

        /// <summary>
        /// Notify를 추가하는 명령입니다.
        /// </summary>
        class AddNotify : public UmCommand
        {
        public:
            AddNotify(std::weak_ptr<TimelineSystem> system, float time, std::string_view label,
                      std::string_view typeNameID);
            virtual ~AddNotify() = default;

        private:
            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;

            std::weak_ptr<TimelineSystem> _timelineSystem;
            TimelineNotify* _notify;
            NotifyData _notifyData;
        };

        /// <summary>
        /// Notify를 제거하는 명령입니다.
        /// </summary>
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
            NotifyData _notifyData;
        };

        /// <summary>
        /// Notify의 시간을 변경하는 명령입니다.
        /// </summary>
        class ChangeNotify : public UmCommand
        {
        public:
            ChangeNotify(std::weak_ptr<TimelineSystem> system
                , TimelineNotify* notify
                , float changeTime
                , std::string_view changelabel
                , std::string_view changeTypeNameID);
            virtual ~ChangeNotify() = default;
        private:
            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;

            std::weak_ptr<TimelineSystem> _timelineSystem;
            TimelineNotify* _notify;    
            NotifyData _previousData;
            NotifyData _changedData;
        };
    } // namespace Sequencer
} // namespace Command