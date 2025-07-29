#pragma once

namespace EnemyAction
{
    class ActionData : public ReflectSerializer
    {
    public:
        USING_PROPERTY(ActionData)
        ActionData(int actionID, std::string_view actionName, std::string_view animationName, int baseDamage)
        {
            ReflectFields->ActionID      = actionID;
            ReflectFields->ActionName    = actionName;
            ReflectFields->AnimationName = animationName;
            ReflectFields->BaseDamage    = baseDamage;
        }
        ActionData()           = default;
        ~ActionData() override = default;
        ActionData(const ActionData& other) { *ReflectFields = *other.ReflectFields; }
        ActionData(ActionData&& other) noexcept { *ReflectFields = std::move(*other.ReflectFields); }
        ActionData& operator=(const ActionData&)     = default;
        ActionData& operator=(ActionData&&) noexcept = default;

    public:
        void SerializedReflectEvent() override;
        void DeserializedReflectEvent() override;
        void ImGuiDrawPropertysEvent() override;

    public:
        REFLECT_PROPERTY(ActionID, ActionName, AnimationName, BaseDamage)

        GETTER_ONLY(int, ActionID) { return ReflectFields->ActionID; }
        PROPERTY(ActionID)
        GETTER_ONLY(std::string, ActionName) { return ReflectFields->ActionName; }
        PROPERTY(ActionName)
        GETTER_ONLY(std::string, AnimationName) { return ReflectFields->AnimationName; }
        PROPERTY(AnimationName)
        GETTER_ONLY(int, BaseDamage) { return ReflectFields->BaseDamage; }
        PROPERTY(BaseDamage)

    private:
        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        int         ActionID   = 0;  // 액션 ID
        std::string ActionName = ""; // 액션 이름
        std::string AnimationName;   // 액션 애니메이션
        int         BaseDamage = 0;  // 기본 데미지
        std::string EventTrackSerializedData;
        REFLECT_FIELDS_END(ActionData)
        Timeline::EventTrack _eventTrack; // 액션에 대한 타임라인 이벤트 트랙
    };
} // namespace EnemyAction