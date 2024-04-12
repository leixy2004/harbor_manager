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
  MapArea margin{};
  int transport_time{};
  int loading_speed{};
  int saved_goods{};
  int have_ship{};
  Grid<int> dis{}, pre{};
  void Show() {
    fprintf(stderr, "Berth %d: (%d, %d, %d, %d) transport_time: %d, loading_speed: %d, saved_goods: %d\n",
            this->id,
            this->minx,
            this->maxx,
            this->miny,
            this->maxy,
            this->transport_time,
            this->loading_speed,
            this->saved_goods);
  }
  Grid<std::array<ShipNaviInfo, 4>> ship_navi;
};

#endif //HARBOR_MANAGER__BERTH_H_
