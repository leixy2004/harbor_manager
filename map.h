//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__MAP_H_
#define HARBOR_MANAGER__MAP_H_
#include <vector>
#include <map>
#include <unordered_map>
#include "position.h"
#include "constant.h"

struct Map {

  enum kObjectType {
    kLand = '.',
    kMainLand = '>',
    kWater = '*',
    kMainWater = '~',
    kBarrier = '#',
    kRobotSeller = 'R',
    kShipSeller = 'S',
    kBerth = 'B',
    kBerthMargin = 'K',
    kCross = 'C',
    kMainCross = 'c',
    kTerminal = 'T',
  };
  std::unordered_map<char, std::pair<int, int>> kConvert = {
      {kLand, {kEmpty, kBanned}},
      {kMainLand, {kWay, kBanned}},
      {kWater, {kBanned, kEmpty}},
      {kMainWater, {kBanned, kWay}},
      {kBarrier, {kBanned, kBanned}},
      {kRobotSeller, {kWay, kBanned}},
      {kShipSeller, {kBanned, kWay}},
      {kBerth, {kWay, kWay}},
      {kBerthMargin, {kBanned, kWay}},
      {kCross, {kEmpty, kEmpty}},
      {kMainCross, {kWay, kWay}},
      {kTerminal, {kBanned, kWay}},
  };
  std::array<std::array<char, kN>, kN> char_grid{};

  struct SubMap {
    Grid grid{};
    bool IsReachable(int x, int y) {
      return grid[x][y] != kBanned;
    }
  } robot{}, ship{};
  static bool IsInMap(int x, int y) {
    return x >= 0 && x < kN && y >= 0 && y < kN;
  }
  void Init() {
    for (int i = 0; i < kN; ++i) {
      for (int j = 0; j < kN; ++j) {
        char c = char_grid[i][j];
        std::tie(robot.grid[i][j], ship.grid[i][j]) = kConvert[c];
      }
    }
  }
};

#endif //HARBOR_MANAGER__MAP_H_
