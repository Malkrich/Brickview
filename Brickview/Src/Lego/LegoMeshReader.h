#pragma once

#include "Geometry/Mesh.h"
#include "IO/MeshReader.h"

namespace Brickview
{

  struct LoadingQueueFileData;

  class LegoMeshReader : public Soup::MeshReader
  {
  public:
    LegoMeshReader();
    ~LegoMeshReader();

    bool Read(const std::filesystem::path& filePath) override;

  private:
    bool ReadFile(
      const LoadingQueueFileData& file, Soup::Ref<Soup::Mesh> mesh, std::queue<LoadingQueueFileData>& loadingQueue);

  private:
    glm::mat4 LDUToMeterTransform;
    glm::mat4 LDrawToBrickviewSpaceTransform;
  };

}
