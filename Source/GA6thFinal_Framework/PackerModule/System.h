#pragma once

namespace Packer
{
    class System
    {
    public:
        static uint32_t Pack(const std::filesystem::path& rootSourcePath, const std::filesystem::path& destinationPath);

        static uint32_t Unpack(const std::filesystem::path& packagePath, const std::filesystem::path& destinationPath);

    private:
        static uint32_t CreateFile(std::istream& sourceStream, const std::filesystem::path& destinationPath);

        static uint32_t ReadFileDataFromChunk(std::istream& sourceStream, std::string& buffer);

        static std::filesystem::path ReadFilePathFromChunk(std::istream& sourceStream);

        static uint32_t WriteFileChunk(std::ostringstream& chunkStream, const std::filesystem::directory_entry& entry,
                                       const std::filesystem::path& rootPath);

        static uint32_t WritePathChunk(std::ostringstream& chunkStream, const std::filesystem::directory_entry& entry,
                                       const std::filesystem::path& rootPath);

        static uint32_t WriteDataChunk(std::ostringstream& chunkStream, const std::filesystem::directory_entry& entry);
    };
} // namespace Packer