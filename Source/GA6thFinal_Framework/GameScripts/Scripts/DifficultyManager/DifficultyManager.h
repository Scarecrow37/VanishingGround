#pragma once
#include "Utility/SingletonHelper.h"

enum class Difficulty : unsigned char
{
    NORMAL,
    HARD
};

class DifficultyManager : public Component
{
    USING_PROPERTY(DifficultyManager)

public:
    DifficultyManager();

public:
    REFLECT_PROPERTY()

public:
    void SetDifficulty(Difficulty difficulty);

    Difficulty GetDifficulty() const;

protected:
    void Awake() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(DifficultyManager)

private:
    SingletonObject<DifficultyManager> _singletonObject{this};
    SingletonComponent<DifficultyManager> _singletonComponent{this};

    Difficulty _defaultDifficulty = Difficulty::NORMAL;

};