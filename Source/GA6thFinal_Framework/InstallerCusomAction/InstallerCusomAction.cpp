// InstallerCusomAction.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <filesystem>
#include <conio.h>

int main(int argc, char* argv[])
{
    if (3 <= argc)
    {
        namespace fs = std::filesystem;
        size_t removePathLastIndex = std::strlen(argv[2]) - 1;
        if (argv[2][removePathLastIndex] != '\\')
        {
            argv[2][removePathLastIndex] = '\\';
        }
        for (int i = 0; i < argc; i++)
        {
            const char* arg = argv[i];
            std::cout << "arg " << i << ": "; 
            std::cout << arg << std::endl;
        }

        fs::path exeFilePath = argv[0];
        fs::path removePath = argv[2];
        std::vector<fs::path> removeFileList;
        std::vector<fs::path> removeDirList;
        if (fs::exists(removePath))
        {
            fs::recursive_directory_iterator itr(removePath);
            while (itr != fs::end(itr))
            {
                const fs::directory_entry& entry = *itr;
                const fs::path& entryPath = entry.path();
                if (entry.is_directory())
                {
                    removeDirList.emplace_back(entryPath);
                }
                else
                {
                    if (exeFilePath != entryPath)
                    {
                        removeFileList.emplace_back(entryPath);
                    }
                }
                std::cout << entryPath << std::endl;
                itr++;
            }

            for (auto& filePath : removeFileList)
            {
                if (std::filesystem::exists(filePath))
                {
                    std::error_code ec;
                    std::filesystem::remove(filePath, ec);
                    if (ec)
                    {
                        std::cout << filePath << std::endl;
                        std::cout << ec.message() << std::endl;
                        std::cout << ec.value() << std::endl;

                        std::cout << "Press any key to exit." << std::endl;
                        int ch = _getch();
                    }
                }
            }

            for (auto iter = removeDirList.rbegin(); iter != removeDirList.rend(); ++iter)
            {
                const auto& dirPath = *iter;
                if (std::filesystem::exists(dirPath))
                {
                    std::error_code ec;
                    std::filesystem::remove(dirPath, ec);
                    if (ec)
                    {
                        std::cout << dirPath << std::endl;
                        std::cout << ec.message() << std::endl;
                        std::cout << ec.value() << std::endl;

                        std::cout << "Press any key to exit." << std::endl;
                        int ch = _getch();
                    }
                }
            }
        }     
    }
    return 0;
}

