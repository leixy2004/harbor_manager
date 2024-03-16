//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__MAP_H_
#define HARBOR_MANAGER__MAP_H_
#include "position.h"
#include "constant.h"

struct Map {
  std::array<std::array<char, kN>, kN> grid{};
  Grid dis{};
  Grid pre{};
  Grid berth_id{};
  Grid color{};
//  void Init() {
//    for (int i = 0; i < kN; i++) {
//      for (int j = 0; j < kN; j++) {
//        dis[i][j] = kInf;
//        pre[i][j] = -1;
//      }
//    }
//  }
  static bool IsInMap(int x, int y) {
    return x >= 0 && x < kN && y >= 0 && y < kN;
  }
  [[nodiscard]] bool IsEmpty(int x, int y) const {
    return IsInMap(x, y) && (
        grid[x][y] == '.' || grid[x][y] == 'A' || grid[x][y] == 'B'
    );
  }
  [[nodiscard]] bool IsEmpty(const Position &pos) const {
    return IsEmpty(pos.x, pos.y);
  }
};

#endif //HARBOR_MANAGER__MAP_H_
