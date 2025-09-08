#pragma once
#include "UmFramework.h"

class Stage : public Component
{
    USING_PROPERTY(Stage)

public:
    Stage();
    ~Stage() override;

public:
    int                       GetFirst() const { return _first; }
    int                       GetSecond() const { return _second; }
    bool                      IsEnable() const { return _stageEnable.Get(); }
    const std::array<int, 6>& GetLevelItems() const { return ReflectFields->LevelItems; }

public:
    void SetStageEnable(bool enable) { Enable = enable; }

public:
    void DeserializedReflectEvent() override;

public:
    void RegisterStage(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage);
    void UpdateData(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage);

public:
    REFLECT_PROPERTY(StageID, Level1_1, Level1_2, Level2_1, Level2_2, Level3_1, Level3_2)
    GETTER(std::string, StageID) { return ReflectFields->Stage; }
    SETTER(std::string, StageID)
    { 
        ReflectFields->Stage = value;
        SetupStage();
    }
    PROPERTY(StageID)

    GETTER(int, Level1_1) { return ReflectFields->LevelItems[0]; }
    SETTER(int, Level1_1) { ReflectFields->LevelItems[0] = value; }
    PROPERTY(Level1_1)

    GETTER(int, Level1_2) { return ReflectFields->LevelItems[1]; }
    SETTER(int, Level1_2) { ReflectFields->LevelItems[1] = value; }
    PROPERTY(Level1_2)

    GETTER(int, Level2_1) { return ReflectFields->LevelItems[2]; }
    SETTER(int, Level2_1) { ReflectFields->LevelItems[2] = value; }
    PROPERTY(Level2_1)

    GETTER(int, Level2_2) { return ReflectFields->LevelItems[3]; }
    SETTER(int, Level2_2) { ReflectFields->LevelItems[3] = value; }
    PROPERTY(Level2_2)

    GETTER(int, Level3_1) { return ReflectFields->LevelItems[4]; }
    SETTER(int, Level3_1) { ReflectFields->LevelItems[4] = value; }
    PROPERTY(Level3_1)

    GETTER(int, Level3_2) { return ReflectFields->LevelItems[5]; }
    SETTER(int, Level3_2) { ReflectFields->LevelItems[5] = value; }
    PROPERTY(Level3_2)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string        Stage;
    bool               Enable     = true;
    std::array<int, 6> LevelItems = {0, 0, 0, 0, 0, 0};
    std::string        Key;
    REFLECT_FIELDS_END(Stage)

protected:
    void ImGuiDrawPropertysEvent() override;

private:
    void SetupStage();

private:
    MVVM::Model<bool> _stageEnable = true;
    int               _first  = 0;
    int               _second = 0;
};