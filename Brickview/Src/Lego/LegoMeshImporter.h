#pragma once

namespace Soup
{

  class Asset;

}

namespace Brickview::LegoMeshImporter
{

  Soup::Ref<Soup::Asset> ImportLegoMesh(const std::filesystem::path& filePath);

}
