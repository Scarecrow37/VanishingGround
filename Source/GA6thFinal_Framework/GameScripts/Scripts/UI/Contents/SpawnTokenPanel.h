#pragma once

class TokenElement;

class SpawnTokenPanel : public UIComponent
{
    USING_PROPERTY(SpawnTokenPanel)

    using ArrayColor = std::array<float, 4>;

    static constexpr ArrayColor DEFAULT_COLOR = {0.0f, 0.0f, 0.0f, 1.0f};

    using TokenColorMap = std::unordered_map<int, ArrayColor>;

public:
    SpawnTokenPanel();

public:
    REFLECT_PROPERTY(TokenElementPrefab, BeginOpacity, EndOpacity, LifeTime, CycleTime)

    GETTER_ONLY(std::string, TokenElementPrefab)
    {
        return File::Guid(ReflectFields->TokenElementPrefabGuid).ToPath().string();
    }
    PROPERTY(TokenElementPrefab)

    GETTER(float, BeginOpacity) { return ReflectFields->BeginOpacity; }
    SETTER(float, BeginOpacity) { ReflectFields->BeginOpacity = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(BeginOpacity)

    GETTER(float, EndOpacity) { return ReflectFields->EndOpacity; }
    SETTER(float, EndOpacity) { ReflectFields->EndOpacity = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(EndOpacity)

    GETTER(float, LifeTime) { return ReflectFields->LifeTime; }
    SETTER(float, LifeTime) { ReflectFields->LifeTime = std::max(0.1f, value); }
    PROPERTY(LifeTime)

    GETTER(float, CycleTime) { return ReflectFields->CycleTime; }
    SETTER(float, CycleTime) { ReflectFields->CycleTime = std::max(0.1f, value); }
    PROPERTY(CycleTime)

public:
    void EnqueueToken(int tokenID);

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

    void Update() override;
    void ImGuiDrawPropertysEvent() override;
    void Reset() override;

private:
    std::weak_ptr<TokenElement> GetTokenElement();
    std::weak_ptr<TokenElement> CreateTokenElement();
    std::weak_ptr<TokenElement> MakeToken(int tokenID);
    Color                       GetTokenColor(int tokenID) const;
    void                        SpawnToken();
    void                        EraseChild() const;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    TokenColorMap TokenColors;
    std::string   TokenElementPrefabGuid;
    POINT         BeginPoint;
    POINT         EndPoint;
    float         BeginOpacity = 1.0f;
    float         EndOpacity   = 0.0f;
    float         LifeTime     = 1.0f;
    float         CycleTime    = 0.1f;
    REFLECT_FIELDS_END(SpawnTokenPanel)

private:
    std::queue<int> _tokenQueue;
    float           _elapsedTime;

    // IMGUI DEBUG
    int _newID = 0;
    std::array<float, 4> _newColor;
};