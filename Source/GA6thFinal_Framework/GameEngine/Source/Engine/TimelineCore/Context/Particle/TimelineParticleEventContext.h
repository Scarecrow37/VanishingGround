#pragma once

namespace Timeline
{
    class ParticleEventContext : public EventContext
    {
        enum TriggerType
        {
            TRIGGER_EFFECT_PLAY,
            TRIGGER_EFFECT_STOP,
        };
    public:
        USING_PROPERTY(ParticleEventContext)
        ParticleEventContext();
        ~ParticleEventContext();

    public:
        virtual void OnNotify() override;
        virtual void ImGuiDrawPropertysEvent() override;

    private:
        REFLECT_FIELDS_BEGIN(EventContext)
        int Trigger = TRIGGER_EFFECT_PLAY;
        std::string EffectKey = "";
        REFLECT_FIELDS_END(ParticleEventContext)
    };
} // namespace Timeline