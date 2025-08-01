#pragma once

namespace Timeline
{
    class EventTrack;
    class EventContext;
}
class ITimelineEvent;

namespace Command
{
    namespace Sequencer
    {
        struct ContextData
        {
            ContextData(UINT id, float time, std::string_view label, std::string_view typeNameID);
            ContextData(Timeline::EventContext* notify);
            ~ContextData() = default;

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
            ChangeMinFrame(std::weak_ptr<Timeline::EventTrack> system, float frame);
            virtual ~ChangeMinFrame() = default;

        private:
            // UmCommand을(를) 통해 상속됨
            bool Execute() override;
            void Undo() override;

            std::weak_ptr<Timeline::EventTrack> _eventTrack;
            float _prevFrame;
            float _tempFrame;
        };
        /// <summary>
        /// MaxFrame을 바꾸도록 동작하는 명령입니다.
        /// </summary>
        class ChangeMaxFrame : public UmCommand
        {
        public:
            ChangeMaxFrame(std::weak_ptr<Timeline::EventTrack> system, float frame);
            virtual ~ChangeMaxFrame() = default;

        private:
            // UmCommand을(를) 통해 상속됨
            bool Execute() override;
            void Undo() override;

            std::weak_ptr<Timeline::EventTrack> _eventTrack;
            float _prevFrame;
            float _tempFrame;
        };

        /// <summary>
        /// Context를 추가하는 명령입니다.
        /// </summary>
        class AddContext : public UmCommand
        {
        public:
            AddContext(std::weak_ptr<Timeline::EventTrack> system, float time, std::string_view label,
                      std::string_view typeNameID);
            virtual ~AddContext() = default;

        private:
            // UmCommand을(를) 통해 상속됨
            bool Execute() override;
            void Undo() override;

            std::weak_ptr<Timeline::EventTrack> _eventTrack;
            Timeline::EventContext* _context;
            ContextData _contextData;
        };

        /// <summary>
        /// Context를 제거하는 명령입니다.
        /// </summary>
        class RemoveContext : public UmCommand
        {
        public:
            RemoveContext(std::weak_ptr<Timeline::EventTrack> system, Timeline::EventContext* notify);
            virtual ~RemoveContext() = default;

        private:
            // UmCommand을(를) 통해 상속됨
            bool Execute() override;
            void Undo() override;

            std::weak_ptr<Timeline::EventTrack> _eventTrack;
            Timeline::EventContext* _context;    
            ContextData _contextData;
        };

        /// <summary>
        /// Context의 시간을 변경하는 명령입니다.
        /// </summary>
        class ChangeContext : public UmCommand
        {
        public:
            ChangeContext(std::weak_ptr<Timeline::EventTrack> system
                , Timeline::EventContext* notify
                , float changeTime
                , std::string_view changelabel
                , std::string_view changeTypeNameID);
            virtual ~ChangeContext() = default;
        private:
            // UmCommand을(를) 통해 상속됨
            bool Execute() override;
            void Undo() override;

            std::weak_ptr<Timeline::EventTrack> _eventTrack;
            Timeline::EventContext* _context;    
            ContextData _previousData;
            ContextData _changedData;
        };
    } // namespace Sequencer
} // namespace Command