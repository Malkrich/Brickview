

#include "LDrawFileManager.h"

namespace Brickview
{

  struct LDrawFileManagerData
  {
    std::filesystem::path BaseDirectory;
    std::filesystem::path PartsDirectory;
    std::filesystem::path SubPartsDirectory;
    std::filesystem::path PrimitivesDirectory;

    std::map<std::filesystem::path, LDrawPrimitiveType> PrimitiveTypeOfSubDirs;
    std::map<std::filesystem::path, LDrawFileType> FileTypeOfDirs;

    // Cache
    std::map<std::filesystem::path, LDrawFileData> ExistingFiles;
  };

  static LDrawFileManagerData* s_LdrawFileManagerData = nullptr;

  void LDrawFileManager::Init()
  {
    SP_ASSERT(!s_LdrawFileManagerData, "LDrawReader engine already initialized!");

    s_LdrawFileManagerData = new LDrawFileManagerData();
    s_LdrawFileManagerData->BaseDirectory = "./data/LDraw/";
    s_LdrawFileManagerData->PartsDirectory = "parts";
    s_LdrawFileManagerData->SubPartsDirectory = "s";
    s_LdrawFileManagerData->PrimitivesDirectory = "p";
    s_LdrawFileManagerData->PrimitiveTypeOfSubDirs[""] = LDrawPrimitiveType::None;
    s_LdrawFileManagerData->PrimitiveTypeOfSubDirs["8"] = LDrawPrimitiveType::LowRes;
    s_LdrawFileManagerData->PrimitiveTypeOfSubDirs["48"] = LDrawPrimitiveType::HiRes;
    s_LdrawFileManagerData->FileTypeOfDirs["s"] = LDrawFileType::SubPart;
    s_LdrawFileManagerData->FileTypeOfDirs["48"] = LDrawFileType::Primitive;
    s_LdrawFileManagerData->FileTypeOfDirs["8"] = LDrawFileType::Primitive;
  }

  void LDrawFileManager::Shutdown()
  {
    delete s_LdrawFileManagerData;
    s_LdrawFileManagerData = nullptr;
  }

  std::filesystem::path LDrawFileManager::GetFullPartsDirectory()
  {
    return s_LdrawFileManagerData->BaseDirectory / s_LdrawFileManagerData->PartsDirectory;
  }

  std::filesystem::path LDrawFileManager::GetFullSubPartsDirectory()
  {
    return GetFullPartsDirectory() / s_LdrawFileManagerData->SubPartsDirectory;
  }

  std::filesystem::path LDrawFileManager::GetFullPrimitivesDirectory(LDrawPrimitiveType type)
  {
    return s_LdrawFileManagerData->BaseDirectory / s_LdrawFileManagerData->PrimitivesDirectory /
      PrimitiveSubDirFromType(type);
  }

  const LDrawFileData& LDrawFileManager::GetFileFromRawFileName(const std::filesystem::path& fileName)
  {
    if (!s_LdrawFileManagerData->ExistingFiles.contains(fileName))
    {
      LDrawFileData fileData = FindFile(fileName);
      s_LdrawFileManagerData->ExistingFiles[fileName] = fileData;
    }

    return s_LdrawFileManagerData->ExistingFiles.at(fileName);
  }

  LDrawFileData LDrawFileManager::FindFile(const std::filesystem::path& fileName)
  {
    LDrawFileType fileType = LDrawFileType::None;
    LDrawPrimitiveType primitiveType = LDrawPrimitiveType::None;

    if (fileName.has_parent_path())
    {
      auto parentDir = fileName.parent_path();
      fileType = FileTypeFromParentDir(parentDir);
      if (fileType == LDrawFileType::Primitive)
        primitiveType = PrimitiveTypeFromParentDir(parentDir);
    }
    else
    {
      fileType = FileTypeFromUnknownFile(fileName);
      if (fileType == LDrawFileType::Primitive)
        primitiveType = PrimitiveTypeFromUnknownFile(fileName);
    }

    std::filesystem::path fullFilePath = FilePathFromTypes(fileType, primitiveType) / fileName.filename();
    return { fullFilePath, fileType, primitiveType };
  }

  LDrawFileType LDrawFileManager::FileTypeFromParentDir(const std::filesystem::path& parentDir)
  {
    SP_ASSERT(s_LdrawFileManagerData->FileTypeOfDirs.contains(parentDir), "Parent path {} doesn't exist for file type!",
      parentDir.string());

    return s_LdrawFileManagerData->FileTypeOfDirs.at(parentDir);
  }

  LDrawFileType LDrawFileManager::FileTypeFromUnknownFile(const std::filesystem::path& fileName)
  {
    // is in parts directory
    if (std::filesystem::exists(GetFullPartsDirectory() / fileName))
      return LDrawFileType::Part;
    // is in sub-parts
    else if (std::filesystem::exists(GetFullSubPartsDirectory() / fileName))
      return LDrawFileType::SubPart;
    // is in primitive dir
    else if (std::filesystem::exists(GetFullPrimitivesDirectory(LDrawPrimitiveType::None) / fileName) ||
      std::filesystem::exists(GetFullPrimitivesDirectory(LDrawPrimitiveType::LowRes) / fileName) ||
      std::filesystem::exists(GetFullPrimitivesDirectory(LDrawPrimitiveType::HiRes) / fileName))
      return LDrawFileType::Primitive;

    SP_ASSERT(false, "Ldraw file type search failed for file {}!", fileName.filename().string());
    return LDrawFileType::None;
  }

  LDrawPrimitiveType LDrawFileManager::PrimitiveTypeFromParentDir(const std::filesystem::path& parentDir)
  {
    SP_ASSERT(s_LdrawFileManagerData->PrimitiveTypeOfSubDirs.contains(parentDir),
      "Parent path {} doesn't exist for primitive type!", parentDir.string());

    return s_LdrawFileManagerData->PrimitiveTypeOfSubDirs.at(parentDir);
  }

  LDrawPrimitiveType LDrawFileManager::PrimitiveTypeFromUnknownFile(const std::filesystem::path& fileName)
  {
    // is in base primitive directory
    if (std::filesystem::exists(GetFullPrimitivesDirectory(LDrawPrimitiveType::None) / fileName))
      return LDrawPrimitiveType::None;
    // is in detailed primitive directory
    else if (std::filesystem::exists(GetFullPrimitivesDirectory(LDrawPrimitiveType::LowRes) / fileName))
      return LDrawPrimitiveType::LowRes;
    // is in no detailed primitive directory
    else if (std::filesystem::exists(GetFullPrimitivesDirectory(LDrawPrimitiveType::HiRes) / fileName))
      return LDrawPrimitiveType::HiRes;

    SP_ASSERT(false, "LDraw primitive search failed for file {}!", fileName.filename().string());
    return LDrawPrimitiveType::None;
  }

  std::filesystem::path LDrawFileManager::FilePathFromTypes(LDrawFileType fileType, LDrawPrimitiveType primitiveType)
  {
    switch (fileType)
    {
      case LDrawFileType::None:
        return "";
      case LDrawFileType::Part:
        return GetFullPartsDirectory();
      case LDrawFileType::SubPart:
        return GetFullSubPartsDirectory();
      case LDrawFileType::Primitive:
        return GetFullPrimitivesDirectory(primitiveType);
    }

    SP_ASSERT(false, "File type unknown!");
    return "";
  }

  std::filesystem::path LDrawFileManager::PrimitiveSubDirFromType(LDrawPrimitiveType type)
  {
    switch (type)
    {
      case LDrawPrimitiveType::None:
        return "";
      case LDrawPrimitiveType::LowRes:
        return "8";
      case LDrawPrimitiveType::HiRes:
        return "48";
    }

    SP_ASSERT(false, "File type unknown!");
    return "";
  }

  // For debug
  std::string LDrawFileManager::FileTypeToString(LDrawFileType type)
  {
    switch (type)
    {
      case LDrawFileType::None:
        return "None";
      case LDrawFileType::Part:
        return "Parts";
      case LDrawFileType::SubPart:
        return "Sub-parts";
      case LDrawFileType::Primitive:
        return "Primitive";
    }

    return "Unknown";
  }

}
