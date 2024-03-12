//
// Created by xinya on 2024/3/9.
//

#include "bfs.h"
#include "goods.h"
#include "map.h"
#include <iostream>
void Bfs(int n, Position *pos_origin, int dis[kN][kN], int pre[kN][kN],const Map &map) {
  Position pos_queue[kN * kN];
//  static std::mutex mutex;
  int head;
  int tail;
  bool vis[kN][kN]{};
//  std::lock_guard<std::mutex> lock(mutex);
  head = 0;
  tail = 0;
  for (int i = 0; i < kN; i++) {
    for (int j = 0; j < kN; j++) {
      vis[i][j] = false;
      dis[i][j] = kN* kN;
      pre[i][j] = -1;
    }
  }
  for (int i = 0; i < n; i++) {
    pos_queue[tail++] = pos_origin[i];
    int &x = pos_origin[i].x;
    int &y = pos_origin[i].y;
    vis[x][y] = true;
    dis[x][y] = 0;
    pre[x][y] = -1;
  }
  while (head < tail) {
    Position pos = pos_queue[head++];
    for (int dir = 0; dir < 4; dir++) {
      Position new_pos = pos.Move(dir);
      int &nx = new_pos.x;
      int &ny = new_pos.y;
      if (map.IsEmpty(nx,ny) && !vis[nx][ny]) {
        vis[nx][ny] = true;
        dis[nx][ny] = dis[pos.x][pos.y] + 1;
        pre[nx][ny] = dir;
        pos_queue[tail++] = new_pos;
      }
    }
  }
}

void Bfs1(Position pos_origin, int dis[kN][kN], int pre[kN][kN], int *status,const Map &map) {
  Position temp[2];
  temp[0] = pos_origin;
  Bfs(1, temp, dis, pre,map);
  *status = Goods::kWaiting;
  std::cerr<<"BFS robot_id: "<<*status<<std::endl;
}