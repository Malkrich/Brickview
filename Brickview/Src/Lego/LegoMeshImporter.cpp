#include "LegoMeshImporter.h"

#include <Geometry/Mesh.h>

#include "LegoMeshReader.h"

namespace Brickview::LegoMeshImporter
{

  Soup::Ref<Soup::Asset> ImportLegoMesh(const std::filesystem::path& filePath)
  {
    LegoMeshReader reader;
    return reader.Read(filePath);
  }

}
