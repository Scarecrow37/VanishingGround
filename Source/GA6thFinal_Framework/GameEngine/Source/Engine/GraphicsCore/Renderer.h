#pragma once

class Shader;
class FrameResource;
class MeshRenderer;
class RenderScene;
class Renderer
{
    enum class ResterizeMode
    {
        SOLID,
        WIREFRAME,
        END
    };

public:
    Renderer();
    ~Renderer();

public:
    void RegisterRenderQueue(MeshRenderer* component);

public:
    void Initialize();
    void Update();
    void Render();
    void Flip();

private:
    std::vector<MeshRenderer*>     _components;
    std::unordered_map<std::string, std::shared_ptr<RenderScene>> _renderScenes;
    UINT                           _currnetState;
};