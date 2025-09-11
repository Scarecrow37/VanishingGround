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
    const std::array<int, 6>& GetDropItems() const { return ReflectFields->DropItems; }
    const std::string&        GetStagePath() const { return ReflectFields->StagePath; }

public:
    void SetStageEnable(bool enable) { ReflectFields->Enable = _stageEnable = enable; }

public:
    void DeserializedReflectEvent() override;

public:
    void RegisterStage(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage);
    void UpdateData(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage);

public:
    REFLECT_PROPERTY(StagePath, StageID, Level1_1, Level1_2, Level2_1, Level2_2, Level3_1, Level3_2)    

    GETTER_ONLY(std::string, StagePath) { return ReflectFields->StagePath; }
    PROPERTY(StagePath)

    GETTER(std::string, StageID) { return ReflectFields->Stage; }
    SETTER(std::string, StageID)
    {
        ReflectFields->Stage = value;
        SetupStage();
    }
    PROPERTY(StageID)

    GETTER(int, Level1_1) { return ReflectFields->DropItems[0]; }
    SETTER(int, Level1_1) { ReflectFields->DropItems[0] = value; }
    PROPERTY(Level1_1)

    GETTER(int, Level1_2) { return ReflectFields->DropItems[1]; }
    SETTER(int, Level1_2) { ReflectFields->DropItems[1] = value; }
    PROPERTY(Level1_2)

    GETTER(int, Level2_1) { return ReflectFields->DropItems[2]; }
    SETTER(int, Level2_1) { ReflectFields->DropItems[2] = value; }
    PROPERTY(Level2_1)

    GETTER(int, Level2_2) { return ReflectFields->DropItems[3]; }
    SETTER(int, Level2_2) { ReflectFields->DropItems[3] = value; }
    PROPERTY(Level2_2)

    GETTER(int, Level3_1) { return ReflectFields->DropItems[4]; }
    SETTER(int, Level3_1) { ReflectFields->DropItems[4] = value; }
    PROPERTY(Level3_1)

    GETTER(int, Level3_2) { return ReflectFields->DropItems[5]; }
    SETTER(int, Level3_2) { ReflectFields->DropItems[5] = value; }
    PROPERTY(Level3_2)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string        Stage;
    std::string        StagePath;
    bool               Enable    = true;
    std::array<int, 6> DropItems = {0, 0, 0, 0, 0, 0};
    REFLECT_FIELDS_END(Stage)

protected:
    void ImGuiDrawPropertysEvent() override;

private:
    void SetupStage();

private:
    MVVM::Model<bool> _stageEnable = true;
    std::string       _key;
    int               _first  = 0;
    int               _second = 0;
};