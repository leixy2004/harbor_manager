//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__SHIP_H_
#define HARBOR_MANAGER__SHIP_H_
#include <iostream>
#include "berth.h"
struct Ship {
static int capacity;
enum ShipStatus {
  kNone,
  kIdle,
  kGoTo,
  kAtEnd,
  kGoBack,
  kAtBerth,
};
int id{};
int status{};
int dir{};
int nowGoods{};
int nowBerth{};
void PrintShip() {
    std::cout << "ship " << this->id << " " << dir << std::endl;
}
void PrintGo() {
    std::cout << "go " << this->id << std::endl;
}
};

#endif //HARBOR_MANAGER__SHIP_H_
