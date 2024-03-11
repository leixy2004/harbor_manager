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
    kGoingToLoad,
    kLoading,
    kGoingToUnload,
    kUnloading,
    kYielding
  };
  Route route{};
  void PrintMove(int dir) {
    std::cout << "move " << this->id << " " << dir << std::endl;
  }
  void PrintLoad() {
    std::cout << "get " << this->id << std::endl;
  }
  void PrintUnload() {
    std::cout << "pull " << this->id << std::endl;
  }
  int Update() override {
    if (status == kIdle) {
      return kRobotIdle;
    }
    if (status == kGoingToLoad) {
      int direction = route.GetNextDirection();
      if (direction == -1) {
        status = kLoading;
      } else {
        auto next_position = position.Move(direction);
        // TODO: check if the next position is valid
        position = next_position;
        return kRobotMove;
      }
    }
    if (status == kGoingToUnload) {
      int direction = route.GetNextDirection();
      if (direction == -1) {
        status = kUnloading;
      } else {
        auto next_position = position.Move(direction);
        // TODO: check if the next position is valid
        position = next_position;
        return kRobotMove;
      }
    }
    {

      if (status == kLoading) {
        PrintLoad();
        status = kGoingToUnload;
      }
      if (status == kUnloading) {
        PrintUnload();
        status = kIdle;
      }
    }
  }
};
#endif //HARBOR_MANAGER__ROBOT_H_
