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
  int targeting_robot{};
  Grid dis{};
    Grid pre{};
  //bool have_ship[20005]{ 0 };
  int have_ship{ 0 };
  void Show() {
    fprintf(stderr, "Berth %d: (%d, %d, %d, %d) transport_time: %d, loading_speed: %d, saved_goods: %d\n",
            this->id,
            this->top,
            this->bottom,
            this->left,
            this->right,
            this->transport_time,
            this->loading_speed,
            this->saved_goods);

  }
};

#endif //HARBOR_MANAGER__BERTH_H_
