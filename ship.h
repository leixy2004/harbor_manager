//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__SHIP_H_
#define HARBOR_MANAGER__SHIP_H_
#include "berth.h"
struct Ship {
static int capacity;
enum ShipStatus {
  kNone,
  kIdle,
  kGoIn,
  kGoOut,
  kWaiting,
};
int id{};
int status{};

};

#endif //HARBOR_MANAGER__SHIP_H_
