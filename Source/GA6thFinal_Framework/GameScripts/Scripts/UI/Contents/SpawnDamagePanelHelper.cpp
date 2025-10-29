#include "pchScripts.h"
#include "SpawnDamagePanelHelper.h"
#include "UI/Contents/SpawnDamagePanel.h"

UMREAL_COMPONENT(SpawnDamagePanelHelper)

SpawnDamagePanelHelper::SpawnDamagePanelHelper() = default;

void SpawnDamagePanelHelper::MakeDamage(const int damage, const std::span<std::string> revelations) const
{
    if (const auto spawnDamagePanel = _spawnDamagePanel.lock())
    {
        [[maybe_unused]] auto _ = spawnDamagePanel->MakeDamage(damage);
    }
}

void SpawnDamagePanelHelper::Awake()
{
    Component::Awake();

    const std::string tag = SpawnDamagePanelTag;
    _spawnDamagePanel     = GameObject::FindComponentWithTag<SpawnDamagePanel>(tag);
}
