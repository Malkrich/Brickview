#pragma once

#include "Lego/LegoPart.h"

namespace Brickview
{

  struct LegoPartComponent
  {
    LegoPartID ID;

    LegoPartComponent(LegoPartID id)
      : ID(id)
    {
    }
    LegoPartComponent(const LegoPartComponent&) = default;
  };

}
