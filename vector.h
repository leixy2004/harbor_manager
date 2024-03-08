//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__VECTOR_H_
#define HARBOR_MANAGER__VECTOR_H_

#include <ostream>
struct Vector {
  int x,y;
    Vector() = default;
    Vector(int x, int y) : x(x), y(y) {};

  friend std::ostream &operator<<(std::ostream &os, const Vector &vector);
  bool operator==(const Vector &rhs) const;
  bool operator!=(const Vector &rhs) const;
  Vector operator+(const Vector &rhs) const;
};

#endif //HARBOR_MANAGER__VECTOR_H_
