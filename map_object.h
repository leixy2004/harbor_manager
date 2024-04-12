//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__MAP_OBJECT_H_
#define HARBOR_MANAGER__MAP_OBJECT_H_
#include "position.h"
#include <memory>
struct MapObject {
  int id{};
  int status{};
  Position position{};
};

#endif //HARBOR_MANAGER__MAP_OBJECT_H_
