#pragma once

namespace Timeline
{
    class ParticleEventContext : public EventContext
    {
        enum TriggerType
        {
            TRIGGER_EFFECT_PLAY,
            TRIGGER_EFFECT_STOP,
            TRIGGER_EFFECT_PLAY_AND_STOP,
        };
    public:
        USING_PROPERTY(ParticleEventContext)
        ParticleEventContext();
        ~ParticleEventContext() override;

    public:
        REFLECT_PROPERTY() 

        virtual void OnNotify() override;
        virtual void ImGuiDrawPropertysEvent() override;

    private:
        REFLECT_FIELDS_BEGIN(EventContext)
        int         Trigger     = TRIGGER_EFFECT_PLAY;
        std::string EffectKey   = "";
        float       TimeToStop  = 0.0f;
        REFLECT_FIELDS_END(ParticleEventContext)
    };
} // namespace Timeline