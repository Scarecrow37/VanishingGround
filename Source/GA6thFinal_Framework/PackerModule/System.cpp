#include "pch.h"
#include "System.h"
#include "Types.h"

uint32_t Packer::System::Pack(const std::filesystem::path& rootSourcePath, const std::filesystem::path& destinationPath)
{
    uint32_t chunkSize = 0;

    // 경로 생성
    if (std::filesystem::exists(destinationPath.parent_path()) == false)
    {
        std::filesystem::create_directories(destinationPath.parent_path());
    }

    // 파일 스트림 열기
    std::ofstream destinationFileStream(destinationPath, std::ios::out | std::ios::binary | std::ios::trunc);
    destinationFileStream.exceptions(std::ios::failbit | std::ios::badbit);

    uint32_t packSize = 0;

    // 임시 스트림
    std::filesystem::path absoluteRoot = std::filesystem::absolute(rootSourcePath);
    std::ostringstream tempStream(std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    for (const auto& entry : std::filesystem::recursive_directory_iterator(rootSourcePath))
    {
        if (entry.is_regular_file())
        {
            packSize += WriteFileChunk(tempStream, entry, absoluteRoot);
        }
    }

    destinationFileStream.write(reinterpret_cast<const char*>(&FOURCC_RIFF), FOURCC_COUNT);
    destinationFileStream.write(reinterpret_cast<const char*>(&packSize), SIZE_COUNT);
    destinationFileStream.write(reinterpret_cast<const char*>(&FOURCC_PACK), FOURCC_COUNT);
    destinationFileStream << tempStream.rdbuf();

    chunkSize = FOURCC_COUNT + SIZE_COUNT + FOURCC_COUNT + packSize;

    if (IsEven()(packSize) == false)
    {
        destinationFileStream << 0x00;
        chunkSize += 1;
    }

    destinationFileStream.close();

    return chunkSize;
}

uint32_t Packer::System::Unpack(const std::filesystem::path& packagePath, const std::filesystem::path& destinationPath)
{
    uint32_t totalFileSize = 0;

    // file stream open
    std::ifstream packageFileStream(packagePath, std::ios::in | std::ios::binary);

    Fourcc   fourccBuffer = 0;
    uint32_t sizeBuffer   = 0;

    // RIFF 헤더 읽기
    packageFileStream.read(reinterpret_cast<char*>(&fourccBuffer), FOURCC_COUNT);
    if (fourccBuffer != FOURCC_RIFF)
    {
        throw std::exception("RIFF header not found");
    }

    // 팩 크기 읽기
    packageFileStream.seekg(SIZE_COUNT, std::ios::cur);

    // PACK 청크 읽기
    packageFileStream.read(reinterpret_cast<char*>(&fourccBuffer), FOURCC_COUNT);
    if (fourccBuffer != FOURCC_PACK)
    {
        throw std::exception("PACK chunk not found");
    }

    // File 청크 읽기
    packageFileStream.read(reinterpret_cast<char*>(&fourccBuffer), FOURCC_COUNT);
    while (fourccBuffer == FOURCC_FILE)
    {
        totalFileSize += CreateFile(packageFileStream, destinationPath);
        packageFileStream.read(reinterpret_cast<char*>(&fourccBuffer), FOURCC_COUNT);
        if (packageFileStream.gcount() != FOURCC_COUNT)
            break;
    }
    return totalFileSize;
}

uint32_t Packer::System::CreateFile(std::istream& sourceStream, const std::filesystem::path& destinationPath)
{
    // 파일 크기 읽기
    sourceStream.seekg(SIZE_COUNT, std::ios::cur);

    const std::filesystem::path filePath = ReadFilePathFromChunk(sourceStream);
    std::string                 data;
    const uint32_t              dataSize = ReadFileDataFromChunk(sourceStream, data);

    const std::filesystem::path fullPath = destinationPath / filePath;

    if (std::filesystem::exists(fullPath.parent_path()) == false)
    {
        std::filesystem::create_directories(fullPath.parent_path());
    }

    std::ofstream(fullPath, std::ios::out | std::ios::binary | std::ios::trunc) << data;

    return dataSize;
}

uint32_t Packer::System::ReadFileDataFromChunk(std::istream& sourceStream, std::string& buffer)
{
    uint32_t fourccBuffer = 0;
    uint32_t sizeBuffer   = 0;

    sourceStream.read(reinterpret_cast<char*>(&fourccBuffer), FOURCC_COUNT);
    if (fourccBuffer != FOURCC_DATA)
    {
        throw std::exception("Data chunk not found");
    }

    sourceStream.read(reinterpret_cast<char*>(&sizeBuffer), SIZE_COUNT);
    uint32_t dataSize = 0;
    if constexpr (IsBigEndian()())
    {
        dataSize = ByteSwap()(sizeBuffer);
    }
    else
    {
        dataSize = sizeBuffer;
    }

    buffer.reserve(dataSize);
    sourceStream.read(buffer.data(), dataSize);

    if (IsEven()(dataSize) == false)
    {
        sourceStream.seekg(1, std::ios::cur);
    }

    return dataSize;
}

std::filesystem::path Packer::System::ReadFilePathFromChunk(std::istream& sourceStream)
{
    uint32_t fourccBuffer = 0;
    uint32_t sizeBuffer   = 0;

    sourceStream.read(reinterpret_cast<char*>(&fourccBuffer), FOURCC_COUNT);
    if (fourccBuffer != FOURCC_PATH)
    {
        throw std::exception("Path chunk not found");
    }

    sourceStream.read(reinterpret_cast<char*>(&sizeBuffer), SIZE_COUNT);
    uint32_t pathSize = 0;
    if constexpr (IsBigEndian()())
    {
        pathSize = ByteSwap()(sizeBuffer);
    }
    else
    {
        pathSize = sizeBuffer;
    }

    std::string filePathString(pathSize, 0);
    sourceStream.read(filePathString.data(), pathSize);

    if (IsEven()(pathSize) == false)
    {
        sourceStream.seekg(1, std::ios::cur);
    }

    return std::filesystem::path{filePathString};
}

uint32_t Packer::System::WriteFileChunk(std::ostringstream& chunkStream, const std::filesystem::directory_entry& entry,
                                        const std::filesystem::path& rootPath)
{
    uint32_t chunkSize = 0;
    if (entry.is_regular_file())
    {
        uint32_t fileSize = 0;
        // 임시 스트림
        std::ostringstream tempStream(std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
        fileSize += WritePathChunk(tempStream, entry, rootPath);
        fileSize += WriteDataChunk(tempStream, entry);

        chunkStream.write(reinterpret_cast<const char*>(&FOURCC_FILE), FOURCC_COUNT);
        chunkStream.write(reinterpret_cast<const char*>(&fileSize), SIZE_COUNT);
        chunkStream << tempStream.rdbuf();

        chunkSize = FOURCC_COUNT + SIZE_COUNT + fileSize;

        if (IsEven()(fileSize) == false)
        {
            chunkStream << 0x00;
            chunkSize += 1;
        }
    }

    return chunkSize;
}

uint32_t Packer::System::WritePathChunk(std::ostringstream& chunkStream, const std::filesystem::directory_entry& entry, const std::filesystem::path& rootPath)
{
    uint32_t chunkSize = 0;
    if (entry.is_regular_file())
    {
        const std::filesystem::path& path = std ::filesystem::relative(std::filesystem::absolute(entry.path()), rootPath);
        const uint32_t               pathSize = static_cast<uint32_t>(path.string().size());

        chunkStream.write(reinterpret_cast<const char*>(&FOURCC_PATH), FOURCC_COUNT);
        chunkStream.write(reinterpret_cast<const char*>(&pathSize), SIZE_COUNT);
        chunkStream << path.string();

        chunkSize = FOURCC_COUNT + SIZE_COUNT + pathSize;

        if (IsEven()(pathSize) == false)
        {
            chunkStream << 0x00;
            chunkSize += 1;
        }
    }
    return chunkSize;
}

uint32_t Packer::System::WriteDataChunk(std::ostringstream& chunkStream, const std::filesystem::directory_entry& entry)
{
    uint32_t chunkSize = 0;
    if (entry.is_regular_file())
    {
        const std::filesystem::path& path     = entry.path();
        const uint32_t               dataSize = static_cast<uint32_t>(entry.file_size());

        chunkStream.write(reinterpret_cast<const char*>(&FOURCC_DATA), FOURCC_COUNT);
        chunkStream.write(reinterpret_cast<const char*>(&dataSize), SIZE_COUNT);
        std::ifstream sourceFileStream(path, std::ios::in | std::ios::binary);
        sourceFileStream.exceptions(std::ios::failbit | std::ios::badbit);
        chunkStream << sourceFileStream.rdbuf();
        sourceFileStream.close();

        chunkSize += FOURCC_COUNT + SIZE_COUNT + dataSize;

        if (IsEven()(dataSize) == false)
        {
            chunkStream << 0x00;
            chunkSize += 1;
        }
    }
    return chunkSize;
}