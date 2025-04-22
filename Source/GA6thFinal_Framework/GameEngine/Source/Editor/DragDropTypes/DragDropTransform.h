#pragma once

class Transform;
class DragDropTransform
{
public:
    static constexpr const char* KEY = "DragDropTransform";
    struct Data
    {
        Transform* pTransform;
    };
};

