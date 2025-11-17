#pragma once

class DescriptionPanel;
class ProclamationHUD : public Component
{
    USING_PROPERTY(ProclamationHUD)

public:
    ProclamationHUD();
    ~ProclamationHUD() override;

public:
    REFLECT_PROPERTY()

public:
    void SetDescriptionText(int assetID, int damage, int count);
    void FindUI();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ProclamationHUD)

protected:
    void Awake() override;

private:
    DescriptionPanel* _descriptionPanel{nullptr};
    std::string       _descriptionText;
};