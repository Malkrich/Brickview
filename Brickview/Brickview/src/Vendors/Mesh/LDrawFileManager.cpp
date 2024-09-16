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
		std::unordered_map<std::filesystem::path, LdrawFileType> FileTypeOfDirs;
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
		s_ldrawFileManagerData->PrimitiveTypeOfSubDirs["8"]  = LDrawPrimitiveType::NoDetails;
		s_ldrawFileManagerData->PrimitiveTypeOfSubDirs["48"] = LDrawPrimitiveType::Details;
		s_ldrawFileManagerData->FileTypeOfDirs["s"]          = LdrawFileType::SubPart;
		s_ldrawFileManagerData->FileTypeOfDirs["48"]         = LdrawFileType::Primitive;
		s_ldrawFileManagerData->FileTypeOfDirs["8"]          = LdrawFileType::Primitive;
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

	std::tuple<LdrawFileType, std::filesystem::path> LDrawFileManager::findFile(const std::filesystem::path& fileName)
	{
		LdrawFileType fileType = LdrawFileType::None;
		LDrawPrimitiveType primitiveType = LDrawPrimitiveType::None;

		if (fileName.has_parent_path())
		{
			auto parentDir = fileName.parent_path();
			fileType       = fileTypeFromParentDir(parentDir);
			if (fileType == LdrawFileType::Primitive)
				primitiveType  = primitiveTypeFromParentDir(parentDir);
		}
		else
		{
			fileType = fileTypeFromUnknownFile(fileName);
			if (fileType == LdrawFileType::Primitive)
				primitiveType = primitiveTypeFromUnknownFile(fileName);
		}

		std::filesystem::path fullFilePath = filePathFromTypes(fileType, primitiveType) / fileName.filename();

		return { fileType, fullFilePath };
	}

	LdrawFileType LDrawFileManager::fileTypeFromParentDir(const std::filesystem::path& parentDir)
	{
		BV_ASSERT(s_ldrawFileManagerData->FileTypeOfDirs.contains(parentDir),
			"Parent path {} doesn't exist for file type!", parentDir.string());

		return s_ldrawFileManagerData->FileTypeOfDirs.at(parentDir);
	}

	LdrawFileType LDrawFileManager::fileTypeFromUnknownFile(const std::filesystem::path& fileName)
	{
		// is in parts directory
		if (std::filesystem::exists(getFullPartsDirectory() / fileName))
			return LdrawFileType::Part;
		// is in sub-parts
		else if (std::filesystem::exists(getFullSubPartsDirectory() / fileName))
			return LdrawFileType::SubPart;
		// is in primitive dir
		else if (std::filesystem::exists(getFullPrimitivesDirectory(LDrawPrimitiveType::None) / fileName)
			|| std::filesystem::exists(getFullPrimitivesDirectory(LDrawPrimitiveType::NoDetails) / fileName)
			|| std::filesystem::exists(getFullPrimitivesDirectory(LDrawPrimitiveType::Details) / fileName))
			return LdrawFileType::Primitive;

		BV_ASSERT(false, "Ldraw file type search failed for file {}!", fileName.filename().string());
		return LdrawFileType::None;
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
		else if (std::filesystem::exists(getFullPrimitivesDirectory(LDrawPrimitiveType::NoDetails) / fileName))
			return LDrawPrimitiveType::NoDetails;
		// is in no detailed primitive directory
		else if (std::filesystem::exists(getFullPrimitivesDirectory(LDrawPrimitiveType::Details) / fileName))
			return LDrawPrimitiveType::Details;

		BV_ASSERT(false, "LDraw primitive search failed for file {}!", fileName.filename().string());
		return LDrawPrimitiveType::None;
	}

	std::filesystem::path LDrawFileManager::filePathFromTypes(LdrawFileType fileType, LDrawPrimitiveType primitiveType)
	{
		switch (fileType)
		{
			case LdrawFileType::None:      return "";
			case LdrawFileType::Part:      return getFullPartsDirectory();
			case LdrawFileType::SubPart:   return getFullSubPartsDirectory();
			case LdrawFileType::Primitive: return getFullPrimitivesDirectory(primitiveType);
		}

		BV_ASSERT(false, "File type unknown!");
		return "";
	}

	std::filesystem::path LDrawFileManager::primitiveSubDirFromType(LDrawPrimitiveType type)
	{
		switch (type)
		{
			case LDrawPrimitiveType::None:      return "";
			case LDrawPrimitiveType::NoDetails: return "8";
			case LDrawPrimitiveType::Details:   return "48";
		}

		BV_ASSERT(false, "File type unknown!");
		return "";
	}

	// For debug
	std::string LDrawFileManager::fileTypeToString(LdrawFileType type)
	{
		switch (type)
		{
			case LdrawFileType::None:      return "None";
			case LdrawFileType::Part:      return "Parts";
			case LdrawFileType::SubPart:   return "Sub-parts";
			case LdrawFileType::Primitive: return "Primitive";
		}

		return "Unknown";
	}

}