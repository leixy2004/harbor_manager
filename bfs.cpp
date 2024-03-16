//
// Created by xinya on 2024/3/9.
//

#include "bfs.h"
#include "map.h"
#include <iostream>
void Bfs(int n, Position *pos_origin, Grid &dis, Grid &pre, const Map &map) {
  static Position pos_queue[kN * kN];
  static std::mutex mutex;
  int head;
  int tail;
  std::lock_guard<std::mutex> lock(mutex);
  head = 0;
  tail = 0;
  for (int i = 0; i < kN; i++) {
    for (int j = 0; j < kN; j++) {
      dis[i][j] = kInf;
      pre[i][j] = -1;
    }
  }
  for (int i = 0; i < n; i++) {
    pos_queue[tail++] = pos_origin[i];
    int &x = pos_origin[i].x;
    int &y = pos_origin[i].y;
    dis[x][y] = 0;
    pre[x][y] = -1;
  }
  while (head < tail) {
    Position pos = pos_queue[head++];
    for (int dir = 0; dir < 4; dir++) {
      Position new_pos = pos.Move(dir);
      int &nx = new_pos.x;
      int &ny = new_pos.y;
      if (map.IsEmpty(nx, ny) && dis[nx][ny] == kInf) {
        dis[nx][ny] = dis[pos.x][pos.y] + 1;
        pre[nx][ny] = dir;
        pos_queue[tail++] = new_pos;
      }
    }
  }
}
