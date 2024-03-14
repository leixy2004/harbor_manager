//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__BFS_H_
#define HARBOR_MANAGER__BFS_H_
#include "position.h"
#include "constant.h"
#include "map"
#include <mutex>
#include "map.h"
void Bfs(int n, Position *pos_origin,Grid& dis,Grid& pre,const Map &map);
#endif //HARBOR_MANAGER__BFS_H_
