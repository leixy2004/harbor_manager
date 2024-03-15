//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__POSITION_H_
#define HARBOR_MANAGER__POSITION_H_
#include "vector.h"
#include "constant.h"

//bool IsInMap(int x, int y) {
//  return x >= 0 && x < kN && y >= 0 && y < kN;
//}

struct Position : Vector {
  Position() = default;
  Position(int x, int y) : Vector(x, y) {};
  Position(Vector v) : Vector(v) {};
  [[nodiscard]] bool IsInMap() const {
    return x >= 0 && x < kN && y >= 0 && y < kN;
  };
  bool operator==(const Position &pos) const {
    return x == pos.x && y == pos.y;
  };
  bool operator!=(const Position &pos) const {
    return !(*this == pos);
  };
  bool operator<(const Position &pos) const {
    return x == pos.x ? y < pos.y : x < pos.x;
  };
  [[nodiscard]] Position Move(int dir) const {
    return *this + kDirVec[dir];
  };
  [[nodiscard]] int Distance(const Position &pos) const {
    return std::abs(x - pos.x) + std::abs(y - pos.y);
  };
};

struct HashPosition {
  size_t operator()(const Position &pos) const {
    return pos.x * kN + pos.y;
  }
};
#endif //HARBOR_MANAGER__POSITION_H_
