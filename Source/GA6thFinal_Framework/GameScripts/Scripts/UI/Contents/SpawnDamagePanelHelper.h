#pragma once

class SpawnDamagePanel;

class SpawnDamagePanelHelper : public Component
{
    USING_PROPERTY(SpawnDamagePanelHelper)

public:
    SpawnDamagePanelHelper();

public:
    REFLECT_PROPERTY(SpawnDamagePanelTag)

    GETTER(std::string, SpawnDamagePanelTag) { return ReflectFields->SpawnDamagePanelTag; }
    SETTER(std::string, SpawnDamagePanelTag) { ReflectFields->SpawnDamagePanelTag = value; }
    PROPERTY(SpawnDamagePanelTag)

public:
    void MakeDamage(const int damage, const std::span<std::string> revelations) const;

protected:
    void Awake() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string SpawnDamagePanelTag;
    REFLECT_FIELDS_END(SpawnDamagePanelHelper)

private:
    std::weak_ptr<SpawnDamagePanel> _spawnDamagePanel;
};