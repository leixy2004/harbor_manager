//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__ROUTE_H_
#define HARBOR_MANAGER__ROUTE_H_
#include "position.h"
#include "deque.h"
#include "constant.h"
struct Route {
  Deque<int, kN * kN> path{};
  Position start{}, end{};
  void Set(const Position &new_start, const Position &new_end) {
    this->start = new_start;
    this->end = new_end;
  }
  void CalcPath(int direction[kN][kN]) {
    path.clear();
    Position current = start;
    while (current != end) {
      path.push_back(direction[current.x][current.y]);
      current = current.Move(direction[current.x][current.y]);
    }
  }
  int GetNextDirection() {
    if (path.empty()) {
      return -1;
    }
    return path.front();
  }
  void PopDirection() {
    path.pop_front();
  }
};

#endif //HARBOR_MANAGER__ROUTE_H_
