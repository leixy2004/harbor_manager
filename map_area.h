//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__MAP_AREA_H_
#define HARBOR_MANAGER__MAP_AREA_H_

struct MapArea {
  int id{};
  int left{}, right{}, top{}, bottom{};
  [[nodiscard]] bool IsInArea(int x, int y) const {
    return y >= left && y <= right && x >= top && x <= bottom;
  }

};

#endif //HARBOR_MANAGER__MAP_AREA_H_
