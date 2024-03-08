//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__ROBOT_H_
#define HARBOR_MANAGER__ROBOT_H_
#include "position.h"
#include "constant.h"
#include "ship.h"
#include "berth.h"
#include "goods.h"

struct Robot {
  int id;
  int status;
  Position position;
};
enum RobotStatus {
  kIdle,
  kBreakdown,
  kGoingToLoad,
  kLoading,
  kGoingToUnload,
  kUnloading,
  kYielding
};
#endif //HARBOR_MANAGER__ROBOT_H_
