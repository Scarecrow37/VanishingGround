#pragma once

class Transform;
class DragDropTransform
{
public:
    static constexpr const char* PREFAB_EXTENSION = ".UmPrefab";
    static void WriteGameObjectFile(Transform* transform, std::string_view outPath);
    static constexpr const char* KEY = "DragDropTransform";
    struct Data
    {
        Transform* pTransform;
    };
};