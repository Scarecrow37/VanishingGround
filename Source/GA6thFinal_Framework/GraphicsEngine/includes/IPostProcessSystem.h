#pragma once

namespace GE
{
    enum class FilterType { Bloom, Blur, End };
    enum FILTER_FLAG
    { 
        Bloom   = 1 << 0,
        Blur    = 1 << 1
    };
    class IFilter;
    class IPostProcessSystem
    {
    protected:
        explicit IPostProcessSystem() = default;
        virtual ~IPostProcessSystem() = default;
        IPostProcessSystem(const IPostProcessSystem&) = delete;
        IPostProcessSystem(IPostProcessSystem&&) = delete;
        IPostProcessSystem& operator=(const IPostProcessSystem&) = delete;
        IPostProcessSystem& operator=(IPostProcessSystem&&) = delete;

    public:
        virtual void GetFilter(IFilter** ppFilter, FilterType type) = 0;
    };
}