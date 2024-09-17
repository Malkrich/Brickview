#include "Pch.h"
#include "LDrawFileManager.h"

namespace Brickview
{

	struct LdrawFileManagerData
	{
		std::filesystem::path BaseDirectory;
		std::filesystem::path PartsDirectory;
		std::filesystem::path SubPartsDirectory;
		std::filesystem::path PrimitivesDirectory;

		std::unordered_map<std::filesystem::path, LDrawPrimitiveType> PrimitiveTypeOfSubDirs;
		std::unordered_map<std::filesystem::path, LDrawFileType> FileTypeOfDirs;
	};

	static LdrawFileManagerData* s_ldrawFileManagerData = nullptr;

	void LDrawFileManager::init()
	{
		BV_ASSERT(!s_ldrawFileManagerData, "LDrawReader engine already initialized!");

		s_ldrawFileManagerData = new LdrawFileManagerData();
		s_ldrawFileManagerData->BaseDirectory                = "./data/LDraw/";
		s_ldrawFileManagerData->PartsDirectory               = "parts";
		s_ldrawFileManagerData->SubPartsDirectory            = "s";
		s_ldrawFileManagerData->PrimitivesDirectory          = "p";
		s_ldrawFileManagerData->PrimitiveTypeOfSubDirs[""]   = LDrawPrimitiveType::None;
		s_ldrawFileManagerData->PrimitiveTypeOfSubDirs["8"]  = LDrawPrimitiveType::LowRes;
		s_ldrawFileManagerData->PrimitiveTypeOfSubDirs["48"] = LDrawPrimitiveType::HiRes;
		s_ldrawFileManagerData->FileTypeOfDirs["s"]          = LDrawFileType::SubPart;
		s_ldrawFileManagerData->FileTypeOfDirs["48"]         = LDrawFileType::Primitive;
		s_ldrawFileManagerData->FileTypeOfDirs["8"]          = LDrawFileType::Primitive;
	}

	void LDrawFileManager::shutdown()
	{
		delete s_ldrawFileManagerData;
		s_ldrawFileManagerData = nullptr;
	}

	std::filesystem::path LDrawFileManager::getFullPartsDirectory()
	{
		return s_ldrawFileManagerData->BaseDirectory / s_ldrawFileManagerData->PartsDirectory;
	}

	std::filesystem::path LDrawFileManager::getFullSubPartsDirectory()
	{
		return getFullPartsDirectory() / s_ldrawFileManagerData->SubPartsDirectory;
	}

	std::filesystem::path LDrawFileManager::getFullPrimitivesDirectory(LDrawPrimitiveType type)
	{
		return s_ldrawFileManagerData->BaseDirectory / s_ldrawFileManagerData->PrimitivesDirectory / primitiveSubDirFromType(type);
	}

	LDrawFileData LDrawFileManager::getFileFromRawFileName(const std::filesystem::path& fileName)
	{
		// TODO: add a cache system to prevent calling findFile all the time.
		return findFile(fileName);
	}

	LDrawFileData LDrawFileManager::findFile(const std::filesystem::path& fileName)
	{
		LDrawFileType fileType = LDrawFileType::None;
		LDrawPrimitiveType primitiveType = LDrawPrimitiveType::None;

		if (fileName.has_parent_path())
		{
			auto parentDir = fileName.parent_path();
			fileType       = fileTypeFromParentDir(parentDir);
			if (fileType == LDrawFileType::Primitive)
				primitiveType  = primitiveTypeFromParentDir(parentDir);
		}
		else
		{
			fileType = fileTypeFromUnknownFile(fileName);
			if (fileType == LDrawFileType::Primitive)
				primitiveType = primitiveTypeFromUnknownFile(fileName);
		}

		std::filesystem::path fullFilePath = filePathFromTypes(fileType, primitiveType) / fileName.filename();
		return { fullFilePath, fileType, primitiveType };
	}

	LDrawFileType LDrawFileManager::fileTypeFromParentDir(const std::filesystem::path& parentDir)
	{
		BV_ASSERT(s_ldrawFileManagerData->FileTypeOfDirs.contains(parentDir),
			"Parent path {} doesn't exist for file type!", parentDir.string());

		return s_ldrawFileManagerData->FileTypeOfDirs.at(parentDir);
	}

	LDrawFileType LDrawFileManager::fileTypeFromUnknownFile(const std::filesystem::path& fileName)
	{
		// is in parts directory
		if (std::filesystem::exists(getFullPartsDirectory() / fileName))
			return LDrawFileType::Part;
		// is in sub-parts
		else if (std::filesystem::exists(getFullSubPartsDirectory() / fileName))
			return LDrawFileType::SubPart;
		// is in primitive dir
		else if (std::filesystem::exists(getFullPrimitivesDirectory(LDrawPrimitiveType::None) / fileName)
			|| std::filesystem::exists(getFullPrimitivesDirectory(LDrawPrimitiveType::LowRes) / fileName)
			|| std::filesystem::exists(getFullPrimitivesDirectory(LDrawPrimitiveType::HiRes) / fileName))
			return LDrawFileType::Primitive;

		BV_ASSERT(false, "Ldraw file type search failed for file {}!", fileName.filename().string());
		return LDrawFileType::None;
	}

	LDrawPrimitiveType LDrawFileManager::primitiveTypeFromParentDir(const std::filesystem::path& parentDir)
	{
		BV_ASSERT(s_ldrawFileManagerData->PrimitiveTypeOfSubDirs.contains(parentDir),
			"Parent path {} doesn't exist for primitive type!", parentDir.string());

		return s_ldrawFileManagerData->PrimitiveTypeOfSubDirs.at(parentDir);
	}

	LDrawPrimitiveType LDrawFileManager::primitiveTypeFromUnknownFile(const std::filesystem::path& fileName)
	{
		// is in base primitive directory
		if (std::filesystem::exists(getFullPrimitivesDirectory(LDrawPrimitiveType::None) / fileName))
			return LDrawPrimitiveType::None;
		// is in detailed primitive directory
		else if (std::filesystem::exists(getFullPrimitivesDirectory(LDrawPrimitiveType::LowRes) / fileName))
			return LDrawPrimitiveType::LowRes;
		// is in no detailed primitive directory
		else if (std::filesystem::exists(getFullPrimitivesDirectory(LDrawPrimitiveType::HiRes) / fileName))
			return LDrawPrimitiveType::HiRes;

		BV_ASSERT(false, "LDraw primitive search failed for file {}!", fileName.filename().string());
		return LDrawPrimitiveType::None;
	}

	std::filesystem::path LDrawFileManager::filePathFromTypes(LDrawFileType fileType, LDrawPrimitiveType primitiveType)
	{
		switch (fileType)
		{
			case LDrawFileType::None:      return "";
			case LDrawFileType::Part:      return getFullPartsDirectory();
			case LDrawFileType::SubPart:   return getFullSubPartsDirectory();
			case LDrawFileType::Primitive: return getFullPrimitivesDirectory(primitiveType);
		}

		BV_ASSERT(false, "File type unknown!");
		return "";
	}

	std::filesystem::path LDrawFileManager::primitiveSubDirFromType(LDrawPrimitiveType type)
	{
		switch (type)
		{
			case LDrawPrimitiveType::None:   return "";
			case LDrawPrimitiveType::LowRes: return "8";
			case LDrawPrimitiveType::HiRes:  return "48";
		}

		BV_ASSERT(false, "File type unknown!");
		return "";
	}

	// For debug
	std::string LDrawFileManager::fileTypeToString(LDrawFileType type)
	{
		switch (type)
		{
			case LDrawFileType::None:      return "None";
			case LDrawFileType::Part:      return "Parts";
			case LDrawFileType::SubPart:   return "Sub-parts";
			case LDrawFileType::Primitive: return "Primitive";
		}

		return "Unknown";
	}

}