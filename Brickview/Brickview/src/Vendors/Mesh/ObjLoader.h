#pragma once

#include <filesystem>

#include "Mesh/Mesh.h"

namespace Brickview
{

	enum ObjComponents : uint32_t
	{
		Position = 0,
		Texture = 1,
		Normal = 2
	};

	struct ObjFace
	{
		struct ObjVertexIndex
		{
			uint32_t PositionIndex;
			// TODO : add unsigned int TextureIndex;
			uint32_t NormalIndex;
		};

		ObjVertexIndex Indices[3];
	};

	bool operator==(const ObjFace::ObjVertexIndex& o1, const ObjFace::ObjVertexIndex& o2);

	class ObjLoader
	{
	public:
		static bool load(const std::filesystem::path& filePath, Ref<Mesh> mesh);
	};

}

namespace std
{
	template<>
	struct hash<Brickview::ObjFace::ObjVertexIndex>
	{
		size_t operator()(const Brickview::ObjFace::ObjVertexIndex& indices) const
		{
			size_t seed = 0;
			Brickview::hashCombine<uint32_t>(seed, indices.NormalIndex);
			Brickview::hashCombine<uint32_t>(seed, indices.PositionIndex);
			return seed;
		}
	};
}