//
// Created by xinya on 2024/3/8.
//

#include "vector.h"
bool Vector::operator==(const Vector &rhs) const {
  return x == rhs.x &&
      y == rhs.y;
}
bool Vector::operator!=(const Vector &rhs) const {
  return !(rhs == *this);
}
std::ostream &operator<<(std::ostream &os, const Vector &vector) {
  os << "x: " << vector.x << " y: " << vector.y;
  return os;
}
Vector Vector::operator+(const Vector &rhs) const {
    return {x + rhs.x, y + rhs.y};
}