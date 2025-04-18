#pragma once

struct DragDropAsset
{
    static constexpr const char* KEY = "DragDropAsset";
    struct Data
    {
        inline static File::Path Path;
    };
};