#pragma once

class MapManagerTracker : public Component
{
    USING_PROPERTY(MapManagerTracker)

public:
    MapManagerTracker();
    ~MapManagerTracker() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MapManagerTracker)

    void Awake() override;
};

