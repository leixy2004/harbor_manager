//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__MAP_H_
#define HARBOR_MANAGER__MAP_H_
#include "position.h"
#include "constant.h"
//#include "robot.h"
//#include "goods.h"
//#include "berth.h"

struct Map {
  char grid[kN][kN]{};
  bool is_occupied[kN][kN]{};
  static bool IsInMap(int x, int y) {
    return x >= 0 && x < kN && y >= 0 && y < kN;
  }

};

#endif //HARBOR_MANAGER__MAP_H_
