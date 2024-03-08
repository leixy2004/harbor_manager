//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__MAP_H_
#define HARBOR_MANAGER__MAP_H_
#include <array>
#include <iostream>
#include <map>
#include <unordered_map>
#include "position.h"
#include "constant.h"
#include "robot.h"
#include "goods.h"
#include "berth.h"

struct Map {
  char grid[kN][kN]{};
  std::unordered_map<Position,Goods*, HashPosition> goods;
  std::unordered_map<Position,Berth*, HashPosition> berth;
  std::unordered_map<Position,Robot*, HashPosition> robot;

  bool IsReachable(const Position &pos) const {
    return pos.IsInMap()
        && (grid[pos.x][pos.y] != '*' && grid[pos.x][pos.y] != '#');
  }
};

#endif //HARBOR_MANAGER__MAP_H_
