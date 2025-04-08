#pragma once

namespace GE
{
    class ITextRenderer;
    class ITextSystem
    {
    protected:
        explicit ITextSystem() = default;
        virtual ~ITextSystem() = default;
        ITextSystem(const ITextSystem&) = delete;
        ITextSystem(ITextSystem&&) = delete;
        ITextSystem& operator=(const ITextSystem&) = delete;
        ITextSystem& operator=(ITextSystem&&) = delete;

    public:      
        virtual void CreateTextRenderer(ITextRenderer** ppComponent) = 0;
        virtual void RegisterRenderQueue(ITextRenderer* pComponent) = 0;
        virtual void UnRegisterRenderQueue(ITextRenderer* pComponent) = 0;
    };
}