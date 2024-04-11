//
// Created by xinya on 2024/4/11.
//

#ifndef HARBOR_MANAGER__TERMINAL_H_
#define HARBOR_MANAGER__TERMINAL_H_

#include "map_object.h"
#include "constant.h"
struct Terminal : MapObject {
Grid dis{}, pre{};
Terminal() = default;
Terminal(int id, int x, int y) : MapObject{id, -1, Position{x, y}} {}
};

#endif //HARBOR_MANAGER__TERMINAL_H_
