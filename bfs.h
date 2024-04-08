//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__BFS_H_
#define HARBOR_MANAGER__BFS_H_
#include "bfs.h"
#include "constant.h"
#include "position.h"
#include <mutex>
#include <iostream>
#include <queue>
#include<functional>
void LandBfs(const std::vector<Position> &pos_origin, Grid &dis, Grid &pre, const Grid &map) {
  static std::queue<Position> pos_queue;
  static std::mutex mutex;
  std::lock_guard<std::mutex> lock(mutex);
  while (!pos_queue.empty()) {
    pos_queue.pop();
  }
  for (int i = 0; i < kN; i++) {
    for (int j = 0; j < kN; j++) {
      dis[i][j] = kInf;
      pre[i][j] = kStay;
    }
  }
  for (auto o: pos_origin) {
    dis[o.x][o.y] = 0;
    pos_queue.push(o);
  }
  while (!pos_queue.empty()) {
    Position pos = pos_queue.front();
    pos_queue.pop();
    for (int dir = 0; dir < 4; dir++) {
      Position new_pos = pos.Move(dir);
      int &nx = new_pos.x;
      int &ny = new_pos.y;
      if (map[nx][ny]!=kBanned && dis[nx][ny] == kInf) {
        dis[nx][ny] = dis[pos.x][pos.y] + 1;
        pre[nx][ny] = dir;
        pos_queue.push(new_pos);
      }
    }
  }
}

//void WaterSpfa(const std::vector<Position> &pos_origin, Grid &dis, Grid &pre,const Grid &map) {
//  static std::queue<Position> pos_queue;
//  static std::mutex mutex;
//  std::lock_guard<std::mutex> lock(mutex);
//  while (!pos_queue.empty()) {
//    pos_queue.pop();
//  }
//  for (int i = 0; i < kN; i++) {
//    for (int j = 0; j < kN; j++) {
//      dis[i][j] = kInf;
//      pre[i][j] = kStay;
//    }
//  }
//  for (auto o: pos_origin) {
//    dis[o.x][o.y] = 0;
//    pos_queue.push(o);
//  }
//  while (!pos_queue.empty()) {
//    Position pos = pos_queue.front();
//    pos_queue.pop();
//    for (int dir = 0; dir < 4; dir++) {
//      Position new_pos = pos.Move(dir);
//      int &nx = new_pos.x;
//      int &ny = new_pos.y;
//      if (map[nx][ny]==kBanned) {
//        continue;
//      }
//      if (map[nx][ny]==kWay && dis[nx][ny] > dis[pos.x][pos.y] + 2) {
//          dis[nx][ny] = dis[pos.x][pos.y] + 2;
//          pre[nx][ny] = dir;
//          pos_queue.push(new_pos);
//        }
//      } else if (dis[nx][ny] > dis[pos.x][pos.y] + 1) {
//        dis[nx][ny] = dis[pos.x][pos.y] + 1;
//        pre[nx][ny] = dir;
//        pos_queue.push(new_pos);
//      }
//    }
//  }
//}

#endif //HARBOR_MANAGER__BFS_H_
