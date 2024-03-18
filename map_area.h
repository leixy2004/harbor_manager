//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__MAP_AREA_H_
#define HARBOR_MANAGER__MAP_AREA_H_
#include "position.h"
struct MapArea {
  int id{};
  int left{}, right{}, top{}, bottom{};
  [[nodiscard]] bool IsInArea(int x, int y) const {
    return y >= left && y <= right && x >= top && x <= bottom;
  }
  [[nodiscard]] Position Center() const {
    return {(top + bottom) / 2, (left + right) / 2};
  }

};

#endif //HARBOR_MANAGER__MAP_AREA_H_
