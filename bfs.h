//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__BFS_H_
#define HARBOR_MANAGER__BFS_H_
#include "position.h"
#include "map"
#include <mutex>
#include "map.h"
void Bfs(int n, Position *pos_origin,int dis[kN][kN],int pre[kN][kN],const Map &map);
void Bfs1(Position pos_origin,int dis[kN][kN],int pre[kN][kN],int *status,const Map &map);
#endif //HARBOR_MANAGER__BFS_H_
