//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__GOODS_H_
#define HARBOR_MANAGER__GOODS_H_
#include "position.h"
#include "map_object.h"
#include "list.h"
#include "constant.h"
#include <memory>
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
  int robot_id{};
  Grid *dis{};
  Grid *pre{};
  static std::allocator<Grid> grid_allocator;
  Goods() = default;
  Goods(int x, int y, int value, int occur_time) : MapObject(x, y), value(value), occur_time(occur_time) {}
  Goods(int x, int y, int id, int status, int value, int occur_time) : MapObject(x, y, id, status),
                                                                       value(value),
                                                                       occur_time(occur_time) {}

  void AllocateMemory() {
    this->dis = grid_allocator.allocate(1);
    this->pre = grid_allocator.allocate(1);
    grid_allocator.construct(this->dis);
    grid_allocator.construct(this->pre);
  }
  void DeallocateMemory() {
    if (this->dis != nullptr) {
      grid_allocator.destroy(this->dis);
      grid_allocator.deallocate(this->dis, 1);
      this->dis = nullptr;
    }
    if (this->pre != nullptr) {
      grid_allocator.destroy(this->pre);
      grid_allocator.deallocate(this->pre, 1);
      this->pre = nullptr;
    }
  }
};

//std::allocator<Grid> Goods::grid_allocator{};

#endif //HARBOR_MANAGER__GOODS_H_
