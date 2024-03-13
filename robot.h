//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__ROBOT_H_
#define HARBOR_MANAGER__ROBOT_H_
#include <iostream>
#include "position.h"
#include "constant.h"
#include "queue.h"
#include "map_object.h"
#include "route.h"
struct Robot : MapObject {
  enum RobotStatus {
    kNone,
    kIdle,
    kBreakdown,
    kBreakdownWithGoods,
    kGoingToLoad,
    kGoingToUnload,
    kYielding
  };
  int dir{-1};
  int goods_id{};
  int berth_id{};
  void PrintMove() {
    std::cout << "move " << this->id << " " << dir << std::endl;
  }
  void PrintLoad() {
    std::cout << "get " << this->id << std::endl;
  }
  void PrintUnload() {
    std::cout << "pull " << this->id << std::endl;
  }
  void Refresh() {
    this->status = kIdle;
    this->dir = -1;
    this->goods_id = -1;
    this->berth_id = -1;
  }
  void Show() {
    fprintf(stderr,
            "Robot %d: (%d, %d) status: %d, goods_id: %d, berth_id: %d dir:%d\n",
            this->id,
            this->position.x,
            this->position.y,
            this->status,
            this->goods_id,
            this->berth_id,
            this->dir);
  }
};
#endif //HARBOR_MANAGER__ROBOT_H_
