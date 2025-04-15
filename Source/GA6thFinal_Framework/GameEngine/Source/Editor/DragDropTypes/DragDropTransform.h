#pragma once

struct DragDropTransform
{
    static constexpr const char* key = "Transform";
    struct Data
    {
        Transform*                            pTransform;
        std::function<void(std::string_view)> serializedFunc;
    };
};