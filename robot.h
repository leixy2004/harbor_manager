//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__ROBOT_H_
#define HARBOR_MANAGER__ROBOT_H_
#include <iostream>
#include "position.h"
#include "constant.h"
#include "map_object.h"
struct Robot : MapObject {
  enum RobotStatus {
//    kNone,
//    kIdle,
    kBreakdown,
    kBreakdownWithGoods,
    kGoingToLoad,
    kGoingToUnload,
//    kYielding
  };
  int dir{kStay};
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
    this->status = kGoingToLoad;
    this->dir = kStay;
    this->goods_id = -1;
    this->berth_id = -1;
  }
//  void Update(int status) {
//    if (status == kIdle) {
//      this->Refresh();
//    } else if (status == kBreakdown) {
//        if (this->status!=kIdle && this->status!=kGoingToLoad) {
//            fprintf(stderr, "Robot %d is (not idle) breakdown?\n", this->id);
//        }
//      this->status = kBreakdown;
//    } else if (status == kBreakdownWithGoods) {
//      if (this->status!=kGoingToUnload) {
//        fprintf(stderr, "Robot %d is not unloading?\n", this->id);
//      }
//      this->status = kBreakdownWithGoods;
//    } else if (status == kGoingToLoad) {
//      if (this->status!=kIdle) {
//        fprintf(stderr, "Robot %d is (not idle) recover?\n", this->id);
//      }
//      this->status = kGoingToLoad;
//    } else if (status == kGoingToUnload) {
//        if (this->status!=kGoingToLoad) {
//            fprintf(stderr, "Robot %d is not loaded?\n", this->id);
//        }
//      this->status = kGoingToUnload;
//    }
//  }
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
