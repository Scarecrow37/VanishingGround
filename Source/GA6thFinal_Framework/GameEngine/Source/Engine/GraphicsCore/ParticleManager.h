#pragma once
class ParticleManager
{
public:
    void Initialize();
    void RegisterEffect();
    void DeleteEffect();
    void Update(const float deltaTime);
    
    //임시 render
    void Render();

private:




};
