//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__GOODS_H_
#define HARBOR_MANAGER__GOODS_H_
#include "position.h"
#include "map_object.h"
#include "list.h"
#include "constant.h"

struct Goods : MapObject {
  enum GoodsStatus {
    kNone,
    kWaiting,
    kTargeted,
    kCaptured,
//    kOnBerth,
//    kOnShip,
//    kSold,
    kExpired,
  };
  int value{};
  int occur_time{};
  Grid *dis{};
  Grid *pre{};
//  List<Goods *, kGoodsMaxAdded>::iterator iter{};
  int robot_id{};
  Goods() = default;
  Goods(int x, int y, int value, int occur_time) : MapObject(x, y), value(value), occur_time(occur_time) {}
  Goods(int x, int y, int id, int status, int value, int occur_time) : MapObject(x, y, id, status),
                                                                       value(value),
                                                                       occur_time(occur_time) {}
  void AllocateMemory() {
    if (dis || pre) {
      DeallocateMemory();
    }
    dis = new Grid;
    pre = new Grid;
  }
  void DeallocateMemory() {
    delete[] dis;
    delete[] pre;
    this->dis = nullptr;
    this->pre = nullptr;
  }
};

#endif //HARBOR_MANAGER__GOODS_H_
