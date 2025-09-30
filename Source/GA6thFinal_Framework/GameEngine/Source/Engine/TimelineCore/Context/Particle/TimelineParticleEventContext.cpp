#include "pch.h"
#include "TimelineParticleEventContext.h"
#include "Particle/ParticleComponent.h"

REGISTER_CLASS(Timeline::EventTrack, Timeline::ParticleEventContext)

namespace Timeline
{
    ParticleEventContext::ParticleEventContext() = default;

    ParticleEventContext::~ParticleEventContext() = default;

    void ParticleEventContext::OnNotify()
    {
        if (GameObject* gameObject = GetGameObject())
        {
            if (ParticleComponent* particle = gameObject->GetComponent<ParticleComponent>())
            {
                if (ReflectFields->Trigger == TRIGGER_EFFECT_PLAY_AND_STOP)
                {
                    // UmTime.Invoke();
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