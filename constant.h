//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__CONSTANT_H_
#define HARBOR_MANAGER__CONSTANT_H_
#include "vector.h"
#include <array>
#define RED(x) ("\033[31m" x "\033[0m")
const int kInf = 0x3f3f3f3f;
const int kN = 200;
const int kGameDuration = 15000;
const int kGoodsDuration = 1000;
const Vector kDirVec[4] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
enum kDirection {
  kStay = -1,
  kRight = 0,
  kLeft = 1,
  kUp = 2,
  kDown = 3
};
enum kCellType {
  kBanned,
  kEmpty,
  kWay,
};
const int kTurnBack[4] = {kLeft, kRight, kDown, kUp};
const int kTurnLeft[4] = {kUp, kDown, kLeft, kRight};
const int kTurnRight[4] = {kDown, kUp, kRight, kLeft};
template <typename T>
using Grid = std::array<std::array<T, kN>, kN>;
#endif //HARBOR_MANAGER__CONSTANT_H_
