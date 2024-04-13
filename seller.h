//
// Created by xinya on 2024/4/11.
//

#ifndef HARBOR_MANAGER__SELLER_H_
#define HARBOR_MANAGER__SELLER_H_

#include "map_object.h"
struct Seller : MapObject {
  Seller() = default;
  Seller(int id, int x, int y) : MapObject{id, -1, Position{x, y}} {}
  virtual void PrintBuy() = 0;
};

struct RobotSeller : Seller {
  RobotSeller() = default;
  RobotSeller(int id, int x, int y) : Seller{id, x, y} {}
  void PrintBuy() override {
    std::cout << "lbot " << position.x << " " << position.y << " "<<  1 <<std::endl;
  }
};

struct ShipSeller : Seller {
  ShipSeller() = default;
  ShipSeller(int id, int x, int y) : Seller{id, x, y} {}
  void PrintBuy() override {
    std::cout << "lboat " << position.x << " " << position.y << std::endl;
  }

};

#endif //HARBOR_MANAGER__SELLER_H_
