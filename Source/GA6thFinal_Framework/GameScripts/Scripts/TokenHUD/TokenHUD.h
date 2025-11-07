#pragma once

class TokenHUD : public Component
{
    USING_PROPERTY(TokenHUD)

public:
    TokenHUD();
    ~TokenHUD() override;

public:
    void SetupTokenHUD(const File::Guid& guid, MVVM::Model<int>& tokenStack, const std::string& key);
    void RemoveTokenHUD();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TokenHUD)

private:
    std::string _key;
};