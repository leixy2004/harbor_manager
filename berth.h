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
  int dis[kN][kN]{};
  int pre[kN][kN]{};

};

#endif //HARBOR_MANAGER__BERTH_H_
