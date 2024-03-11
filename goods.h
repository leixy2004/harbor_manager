//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__GOODS_H_
#define HARBOR_MANAGER__GOODS_H_
#include "position.h"
#include "map_object.h"

struct Goods : MapObject {
  enum GoodsStatus {
    kNone,
    kWaiting,
    kTargeted,
    kOnRobot,
    kOnBerth,
    kOnShip,
    kSold,
    kExpired,
  };
  int value{};
  int occur_time{};
  int dis[kN][kN]{};
  int pre[kN][kN]{};

  Goods() = default;
  Goods(int x, int y, int value, int occur_time) : MapObject(x, y), value(value), occur_time(occur_time) {}
  Goods(int x, int y, int id, int status, int value, int occur_time) : MapObject(x, y, id, status),
                                                                       value(value),
                                                                       occur_time(occur_time) {}

};

#endif //HARBOR_MANAGER__GOODS_H_
