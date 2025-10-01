#include "pchScripts.h"
#include "CharactorHUDGroup.h"
#include <UI/Panels/Overlay/OverlayPanel.h>

bool CharacterHUDGroup::FindUI() 
{
    auto hudGroup = GameObject::FindWithTag("Character HUD Group").lock();
    if (hudGroup)
    {
        GroupPanel = hudGroup->GetComponent<OverlayPanel>();

        // 그룹 패널 내부를 탐색
        Transform& transform = hudGroup->transform;
        Transform::ForeachBFS(transform, [this](Transform* curr) {
            if (curr->gameObject->CompareTag("Left Monster HUD"))
            {
                EnemyHUDPanel[0] = curr->gameObject->GetComponent<OverlayPanel>();
            }
            else if (curr->gameObject->CompareTag("Middle Monster HUD"))
            {
                EnemyHUDPanel[1] = curr->gameObject->GetComponent<OverlayPanel>();
            }
            else if (curr->gameObject->CompareTag("Right Monster HUD"))
            {
                EnemyHUDPanel[2] = curr->gameObject->GetComponent<OverlayPanel>();
            }
            else if (curr->gameObject->CompareTag("Player HUD"))
            {
                PlayerHUDPanel = curr->gameObject->GetComponent<OverlayPanel>();
            }
        });
    }

    return IsValid();
}

bool CharacterHUDGroup::IsValid()
{
    return GroupPanel && PlayerHUDPanel && EnemyHUDPanel[0] && EnemyHUDPanel[1] && EnemyHUDPanel[2];
}

void CharacterHUDGroup::ActiveUI(bool active) 
{
    if (IsValid())
    {
        // TODO: 그룹 패널 활성화/비활성화
    }
}
