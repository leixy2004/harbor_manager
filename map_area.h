//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__MAP_AREA_H_
#define HARBOR_MANAGER__MAP_AREA_H_

struct MapArea {
  int id{};
  int left{}, right{}, top{}, bottom{};
  bool IsInArea(int x, int y) const {
    return x >= left && x <= right && y >= top && y <= bottom;
  }

};

#endif //HARBOR_MANAGER__MAP_AREA_H_
