#pragma once

class FPSCounter : public Component
{
    USING_PROPERTY(FPSCounter)
public:
    REFLECT_PROPERTY()

public:
    FPSCounter();
    virtual ~FPSCounter();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(FPSCounter)

    virtual void Update() override;

private:
    bool _showFPS;

    int   _frameCount;
    float _elapsedTime;
    int   _fps;
    void  UpdateFPS();
    void  ShowFPS();
    void  ToggleFPS() { _showFPS = !_showFPS; }

};
