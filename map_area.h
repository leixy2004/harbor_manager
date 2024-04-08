//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__MAP_AREA_H_
#define HARBOR_MANAGER__MAP_AREA_H_
#include "map_object.h"
struct MapArea : MapObject {
  int miny{}, maxy{}, minx{}, maxx{};
  [[nodiscard]] bool IsInArea(int x, int y) const {
    return y >= miny && y <= maxy && x >= minx && x <= maxx;
  }

};

#endif //HARBOR_MANAGER__MAP_AREA_H_
