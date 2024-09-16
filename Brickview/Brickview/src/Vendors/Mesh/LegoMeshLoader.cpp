#include "Pch.h"
#include "LegoMeshLoader.h"

#include "Utils/StringUtils.h"
#include "LDrawReader.h"

#include <glm/glm.hpp>

namespace Brickview
{

	LegoMeshFileData::LegoMeshFileData(const std::filesystem::path& filePath, LdrawFileType type, const glm::mat4& transform)
		: FilePath(filePath)
		, Transform(transform)
	{}

	bool LegoMeshLoader::load(const std::filesystem::path& filePath, Ref<Mesh> mesh)
	{
		if (!std::filesystem::exists(filePath))
		{
			BV_LOG_ERROR("Couldn't load: {}", filePath.generic_string());
			return false;
		}

		std::queue<LegoMeshFileData> loadingQueue;
		LegoMeshFileData initialFile(filePath, LdrawFileType::Part, glm::mat4(1.0f));
		loadingQueue.push(initialFile);

		while (!loadingQueue.empty())
		{
			const LegoMeshFileData& file = loadingQueue.front();
			readFile(file, mesh, loadingQueue);
			loadingQueue.pop();
		}

		convertToDm(mesh);

		return true;
	}

	bool LegoMeshLoader::readFile(const LegoMeshFileData& file, Ref<Mesh> mesh, std::queue<LegoMeshFileData>& loadingQueue)
	{
		std::filesystem::path currentFilePath = file.FilePath;
		glm::mat4 currentTransform            = file.Transform;

		LDrawReader reader(currentFilePath);

		if (!reader.isValid())
		{
			BV_LOG_ERROR("Couldn't read file in {}", currentFilePath.generic_string());
			return false;
		}

		while (reader.readLine())
		{
			LDrawLineType lineType = reader.getLineType();

			switch (lineType)
			{
				case LDrawLineType::Triangle:
				{
					LDrawTriangleData t = reader.getLineData<LDrawTriangleData>();
					mesh->addTriangle(t.p0, t.p1, t.p2, currentTransform);
					break;
				}
				case LDrawLineType::Quadrilateral:
				{
					LDrawQuadData q = reader.getLineData<LDrawQuadData>();
					mesh->addQuad(q.p0, q.p1, q.p2, q.p3, currentTransform);
					break;
				}
				case LDrawLineType::SubFileRef:
				{
					LDrawSubFileRefData sf = reader.getLineData<LDrawSubFileRefData>();
					auto [newFileType, newFilePath] = LDrawFileManager::findFile(sf.FilePath);

					if (newFileType != LdrawFileType::None)
					{
						BV_LOG_INFO("Adding file {} to the loading queue", newFilePath.string());

						LegoMeshFileData newFile;
						newFile.FilePath  = newFilePath;
						newFile.Transform = currentTransform * sf.Transform;
						loadingQueue.push(newFile);
						break;
					}
				}

#if 0
				case LineType::Comment:
					BV_LOG_INFO("LDraw file comment: {}", reader.getComment());
					break;
#endif
				default:
					break;
			}
		}

		return true;
	}

	void LegoMeshLoader::convertToDm(Ref<Mesh> mesh)
	{
		// Convert LDU to mm
		// 1 LDU = 0.4 mm
		mesh->scale(0.4);
		// Convert mm to dm
		// 1 mm = 0.01 dm;
		mesh->scale(0.01);
	}

}