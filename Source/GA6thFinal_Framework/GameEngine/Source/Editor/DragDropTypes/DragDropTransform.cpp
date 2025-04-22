#include "pch.h"

void DragDropTransform::WriteGameObjectFile(Transform* transform, std::string_view outPath)
{
    namespace fs = std::filesystem;
    using fsPath = std::filesystem::path;
    fsPath writePath = outPath;
    writePath /= transform->gameObject->ToString();
    writePath.replace_extension(PREFAB_EXTENSION);
    if (fs::exists(outPath) == false)
    {
        int result = MessageBox(UmApplication.GetHwnd(), L"파일이 이미 존재합니다. 덮어쓰겠습니까?", L"파일이 존재합니다.", MB_YESNO);
        if (result != IDYES)
        {
            return;
        }
    }   
    fs::create_directories(writePath.parent_path());
    YAML::Node node = UmGameObjectFactory.SerializeToYaml(&transform->gameObject);
    if (node.IsNull() == false)
    {
        std::ofstream ofs(writePath, std::ios::trunc);
        if (ofs.is_open())
        {
            ofs << node;
        }
        ofs.close();
    }
}
