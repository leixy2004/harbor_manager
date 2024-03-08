//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__CONSTANT_H_
#define HARBOR_MANAGER__CONSTANT_H_
#include "vector.h"
const int kN = 200;
const int kGameDuration = 15000;
const int kGoodsDuration = 1000;
const int kGoodsMaxAdded = 10*kGameDuration;
const int kRobotCount = 10;
const int kRobotBreakdownTime = 400;
const int kShipCount = 5;
const int kBerthCount = 10;
const int kBerthConvertTime = 500;
const Vector kDir[4] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
#endif //HARBOR_MANAGER__CONSTANT_H_
