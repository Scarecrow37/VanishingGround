#include "pch.h"
#include "EditorHelper.h"

bool ProcessHelper::IsVisualStudio(std::string& outExeFilePath)
{
    std::string command = "wmic process where \"name='devenv.exe'\" get ExecutablePath";
    FILE*       pipe    = _popen(command.c_str(), "r");

    if (pipe == nullptr)
    {
        outExeFilePath.clear();
        return false;
    }

    char buffer[128]{};
    while (fgets(buffer, sizeof(buffer), pipe))
    {
        if (strstr(buffer, "C:\\"))
        {
            outExeFilePath = std::string(buffer);
            while (outExeFilePath.back() != 'e')
            {
                outExeFilePath.pop_back();
            }
            break;
        }
    }
    fclose(pipe);
    return true;
}
