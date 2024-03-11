//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__CONSTANT_H_
#define HARBOR_MANAGER__CONSTANT_H_
#include "vector.h"
const int kInf = 0x3f3f3f3f;
const int kN = 200;
const int kGameDuration = 15000;
const int kGoodsDuration = 1000;
const int kGoodsMaxAdded = 500;
const int kRobotCount = 10;
const int kRobotBreakdownTime = 400;
const int kShipCount = 5;
const int kBerthCount = 10;
const int kBerthSize = 4;
const int kBerthConvertTime = 500;
const Vector kDirVec[4] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
enum kDirection { kRight = 0, kLeft = 1, kUp = 2, kDown = 3 };
enum kProcessSignal {
  kRobotIdle,
  kRobotLoad,
  kRobotUnload,
  kRobotMove,
  kRobotRepair,

};
const int kInverseDir[4] = {1, 0, 3, 2};
//int ReverseDir(int dir) {
//  const static int kReverseDir[4] = {kLeft, kRight, kDown, kUp};
//  return kReverseDir[dir];
//}
//int TurnLeft(int dir) {
//  const static int kTurnLeft[4] = {kUp, kDown, kLeft, kRight};
//  return kTurnLeft[dir];
//}
//int TurnRight(int dir) {
//  const static int kTurnRight[4] = {kDown, kUp, kRight, kLeft};
//  return kTurnRight[dir];
//}
#endif //HARBOR_MANAGER__CONSTANT_H_
