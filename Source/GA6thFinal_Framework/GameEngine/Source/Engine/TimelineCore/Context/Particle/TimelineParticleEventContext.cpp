#include "pch.h"
#include "TimelineParticleEventContext.h"
#include "Particle/ParticleComponent.h"

REGISTER_CLASS(Timeline::EventTrack, Timeline::ParticleEventContext)

namespace Timeline
{
    REFLECT_FUNCTION(ParticleEventContext)

    ParticleEventContext::ParticleEventContext() = default;

    ParticleEventContext::~ParticleEventContext() = default;

    void ParticleEventContext::OnNotify()
    {
        if (auto gameObject = GetGameObject().lock())
        {
            if (ParticleComponent* particle = gameObject->GetComponent<ParticleComponent>())
            {
                switch (ReflectFields->Trigger)
                {
                    case TRIGGER_EFFECT_PLAY:
                        UmParticleManager->PlayEffect(particle, ReflectFields->EffectKey);
                        break;
                    case TRIGGER_EFFECT_STOP:
                        UmParticleManager->StopEffect(particle, ReflectFields->EffectKey);
                        break;
                    case TRIGGER_EFFECT_PLAY_AND_STOP: {
                        // 일정 시간 후에 정지.
                        UmTime.Invoke(ReflectFields->TimeToStop, [this, particle]() {
                            UmParticleManager->StopEffect(particle, ReflectFields->EffectKey);
                        });
                        break;
                }
                default:
                    break;
                }
            }
        }
    }
    void ParticleEventContext::ImGuiDrawPropertysEvent()
    {
        ImGuiHelper::AlignedText("Trigger Type", ImGuiHelper::LEFT, 0.8f);
        if (ImGui::RadioButton("Play", ReflectFields->Trigger == TRIGGER_EFFECT_PLAY))
        {
            ReflectFields->Trigger = TRIGGER_EFFECT_PLAY;
        }
        if (ImGui::RadioButton("Stop", ReflectFields->Trigger == TRIGGER_EFFECT_STOP))
        {
            ReflectFields->Trigger = TRIGGER_EFFECT_STOP;
        }
        ImGuiHelper::AlignedText("Effect Key", ImGuiHelper::LEFT, 0.8f);

        float offsetX = ImGui::GetContentRegionAvail().x * 0.4f;
        ImGuiHelper::TextWithVerticalSeparator("Effect Key", offsetX);
        ImGui::InputTextWithHint("##EffectKey", "New Effect Key...", &ReflectFields->EffectKey);

        if (ReflectFields->Trigger == TRIGGER_EFFECT_PLAY_AND_STOP)
        {
            ImGuiHelper::TextWithVerticalSeparator("Stop Timer", offsetX);
            ImGui::DragFloat("##TimeToStop", &ReflectFields->TimeToStop, 0.1f, 0.0f, FLT_MAX, "%.1f sec");
        }
    }
} // namespace Timeline