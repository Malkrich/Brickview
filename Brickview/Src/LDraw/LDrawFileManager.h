#pragma once

namespace Brickview
{

  enum class LDrawFileType
  {
    None = 0,
    Part,
    SubPart,
    Primitive,
  };

  enum class LDrawPrimitiveType
  {
    None = 0,
    LowRes,
    HiRes
  };

  struct LDrawFileData
  {
    std::filesystem::path FilePath;
    LDrawFileType FileType;
    LDrawPrimitiveType PrimitiveType;
  };

  class LDrawFileManager
  {
  public:
    static void Init();
    static void Shutdown();

    static std::filesystem::path GetFullPartsDirectory();
    static std::filesystem::path GetFullSubPartsDirectory();
    static std::filesystem::path GetFullPrimitivesDirectory(LDrawPrimitiveType type);

    static const LDrawFileData& GetFileFromRawFileName(const std::filesystem::path& fileName);

    // For debug
    static std::string FileTypeToString(LDrawFileType type);

  private:
    static LDrawFileData FindFile(const std::filesystem::path& fileName);

    static LDrawFileType FileTypeFromParentDir(const std::filesystem::path& parentDir);
    static LDrawFileType FileTypeFromUnknownFile(const std::filesystem::path& fileName);
    static LDrawPrimitiveType PrimitiveTypeFromParentDir(const std::filesystem::path& parentDir);
    static LDrawPrimitiveType PrimitiveTypeFromUnknownFile(const std::filesystem::path& fileName);

    static std::filesystem::path FilePathFromTypes(LDrawFileType fileType, LDrawPrimitiveType primitiveType);
    static std::filesystem::path PrimitiveSubDirFromType(LDrawPrimitiveType type);
  };

}
