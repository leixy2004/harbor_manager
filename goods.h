//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__GOODS_H_
#define HARBOR_MANAGER__GOODS_H_
#include "position.h"
#include "map_object.h"
#include "constant.h"
#include <memory>
#include <iostream>
struct Goods : MapObject {
  enum GoodsStatus {
    kOnLand,
    kOnRobot,
    kOnBerth,
    kOnShip,
    kExpired,
  };
  static struct Monitor {
    std::array<int,3> status_count{};
    std::array<int,3> status_value{};
  } monitor;
  int value{};
  int occur_time{};
  int robot_id{};
  int berth_id{};
  static std::allocator<Grid> grid_allocator;
  Grid *dis{},*pre{};
  ~Goods() {
    DeallocateMemory();
  }
  void AllocateMemory() {
//    if (this->dis != nullptr || this->pre != nullptr) {
//      DeallocateMemory();
//    }
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

  void Update(int new_status) {
    monitor.status_count[status] -= 1;
    monitor.status_value[status] -= value;
    status = new_status;
    monitor.status_count[status] += 1;
    monitor.status_value[status] += value;
  }
};

//std::allocator<Grid> Goods::grid_allocator{};

#endif //HARBOR_MANAGER__GOODS_H_
