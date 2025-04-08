#pragma once

namespace GE
{
    class Matrix4x4;
    class ISpriteRenderer;
    class ISpriteSystem
    {
    protected:
        explicit ISpriteSystem() = default;
        virtual ~ISpriteSystem() = default;
        ISpriteSystem(const ISpriteSystem&) = delete;
        ISpriteSystem(ISpriteSystem&&) = delete;
        ISpriteSystem& operator=(const ISpriteSystem&) = delete;
        ISpriteSystem& operator=(ISpriteSystem&&) = delete;

    public:      
        virtual void CreateTextRenderer(ISpriteRenderer** ppComponent, const wchar_t* filePath) = 0;
        virtual void RegisterRenderQueue(ISpriteRenderer* pComponent, Matrix4x4* pMatrix) = 0;
        virtual void UnRegisterRenderQueue(ISpriteRenderer* pComponent) = 0;
    };
}