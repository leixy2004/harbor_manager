//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__BFS_H_
#define HARBOR_MANAGER__BFS_H_
#include "position.h"
#include "map"
#include <mutex>
void Bfs(int n, Position *pos_origin,int dis[kN][kN],int pre[kN][kN]);
#endif //HARBOR_MANAGER__BFS_H_
