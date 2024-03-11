//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__BERTH_H_
#define HARBOR_MANAGER__BERTH_H_
#include "position.h"
#include <queue>
#include "bfs.h"
#include "map_area.h"
struct Berth : MapArea {
  int transport_time{};
  int loading_speed{};
  int saved_goods{};
  int distance[kN][kN]{};
  int direction[kN][kN]{};
  void Init() {
    static Position area[kBerthSize * kBerthSize]{};
    for (int i = 0; i < kBerthSize; i++) {
      for (int j = 0; j < kBerthSize; j++) {
        area[i * kBerthSize + j] = Position{top + i, left + j};
      }
    }
    Bfs(kBerthSize * kBerthSize, area, distance, direction);
  }
};

#endif //HARBOR_MANAGER__BERTH_H_
