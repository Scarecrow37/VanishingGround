#include "pchScripts.h"
#include "TutorialTestComponent.h"

#include "TutorialSystem/TutorialSystem.h"
#include "Utility/SingletonHelper.h"

UMREAL_COMPONENT(TutorialTestComponent)

TutorialTestComponent::TutorialTestComponent() = default;

void TutorialTestComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    if (ImGui::Button("Show Tutorial"))
    {
        const TutorialSystem* system = SingletonComponent<TutorialSystem>::GetInstance();
        system->Show(805900);
    }

    if (ImGui::Button("Show Tutorials"))
    {
        TutorialSystem* system = SingletonComponent<TutorialSystem>::GetInstance();
        system->Show({805900, 805901});
    }
}