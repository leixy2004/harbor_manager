//
// Created by xinya on 2024/3/9.
//

#include "bfs.h"
void Bfs(int n, Position *pos_origin,int dis[kN][kN],int pre[kN][kN]) {
  static Position pos_queue[kN* kN];
  static std::mutex mutex;
  static int head;
  static int tail;
  static bool vis[kN][kN];
  std::lock_guard<std::mutex> lock(mutex);
  head = 0;
  tail = 0;
  for (int i = 0; i < n; i++) {
    pos_queue[tail++] = pos_origin[i];
    int &x = pos_origin[i].x;
    int &y = pos_origin[i].y;
    vis[x][y] = true;
    dis[x][y] = 0;
    pre[x][y]=-1;
  }
  while (head < tail) {
    Position pos = pos_queue[head++];
    for (int dir = 0; dir < 4; dir++) {
      Position new_pos = pos.Move(dir);
      int &x= new_pos.x;
      int &y= new_pos.y;
      if (new_pos.IsInMap() && !vis[x][y]) {
        vis[x][y] = true;
        dis[x][y] = dis[x][y] + 1;
        pre[x][y]=dir;
        pos_queue[tail++] = new_pos;
      }
    }
  }
}