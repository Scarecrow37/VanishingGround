#include "pch.h"
#include "TimelineParticleEventContext.h"
#include "Particle/ParticleComponent.h"

namespace Timeline
{
    ParticleEventContext::ParticleEventContext() 
    {
    }

    ParticleEventContext::~ParticleEventContext() 
    {
    }

    void ParticleEventContext::OnNotify() 
    {
        if (GameObject* gameObject = GetGameObject())
        {
            if (ParticleComponent* particle = gameObject->GetComponent<ParticleComponent>())
            {
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
        ImGui::InputTextWithHint("##EffectKey", "New Effect Key...", &ReflectFields->EffectKey);
    }
}