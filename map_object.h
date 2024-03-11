//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__MAP_OBJECT_H_
#define HARBOR_MANAGER__MAP_OBJECT_H_
#include "position.h"
struct MapObject {
  int id{};
  int status{};
  Position position{};
  virtual int Update() = 0;
  MapObject() = default;
  MapObject(int x, int y) : position(x, y) {}
  MapObject(int x, int y, int id, int status) : position(x, y), id(id), status(status) {}
  MapObject(Position position, int id, int status) : position(position), id(id), status(status) {}
};

#endif //HARBOR_MANAGER__MAP_OBJECT_H_
