//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__SHIP_H_
#define HARBOR_MANAGER__SHIP_H_
#include <iostream>
#include "map_object.h"
#include "berth.h"
struct Ship : MapObject {
  static int capacity;
  enum ShipStatus {
    kLoad,
    kRecover,
    kNormal,
    kToTerminal,
    kToBerth,
  };
  int aim{};
  int dir{};
  int nowGoods{};
  int nowBerth{};
  Ship() = default;
    Ship(int id, int x, int y) : MapObject{id, kNormal, Position{x, y}} {}
  void PrintShip() const {
    std::cout << "ship " << this->id << std::endl;
  }
  void PrintDept() const {
    std::cout << "dept " << this->id << std::endl;
  }
  void PrintBerth() const {
    std::cout << "berth " << this->id << std::endl;
  }
  void PrintRot(int d) const {
    std::cout << "rot " << this->id << " " << d << std::endl;
  }
};

#endif //HARBOR_MANAGER__SHIP_H_
