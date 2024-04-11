//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__BFS_H_
#define HARBOR_MANAGER__BFS_H_

#include "constant.h"
#include "map.h"
#include "position.h"
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <vector>

struct ShipNaviInfo {
    int dis;
    enum Action {
        stay,
        rot0,
        rot1,
        move,
    } action;
};

void LandBfs(const std::vector<Position> &pos_origin, Grid<int> &dis, Grid<int> &pre, const Grid<int> &map) {
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

void GetShipNaviInfo(const Map& map, const std::vector<Position>& dest, Grid<std::array<ShipNaviInfo, 4>>& out) {
    struct State {
        int x;
        int y;
        int dir;
    };

    auto GetCost = [&](State s) -> int {
        int cost = 1;
        for (int i = 0; i < 6; i++) {
            if (!map.ship.IsReachable(s.x, s.y))
                return kInf;
            if (map.ship.grid[s.x][s.y] == kWay)
                cost = 2;
            int iter_dir;
            switch (i) {
                case 0:
                case 1: iter_dir = s.dir; break;
                case 2: iter_dir = kTurnRight[s.dir]; break;
                case 3:
                case 4: iter_dir = kTurnBack[s.dir]; break;
                case 5: iter_dir = kTurnLeft[s.dir]; break;
            }
            s.x += kDirVec[iter_dir].x;
            s.y += kDirVec[iter_dir].y;
        }
        return cost;
    };

    auto Move = [&](State s, ShipNaviInfo::Action action) -> std::pair<State, int> {
        int cost = GetCost(s);
        if (action == ShipNaviInfo::move) {
            s.x -= kDirVec[s.dir].x;
            s.y -= kDirVec[s.dir].y;
        } else if (action == ShipNaviInfo::rot0) {
            s.dir = kTurnLeft[s.dir];
            s.x -= kDirVec[s.dir].x * 2;
            s.y -= kDirVec[s.dir].y * 2;
        } else if (action == ShipNaviInfo::rot1) {
            s.x += kDirVec[s.dir].x;
            s.y += kDirVec[s.dir].y;
            s.dir = kTurnRight[s.dir];
            s.x -= kDirVec[s.dir].x;
            s.y -= kDirVec[s.dir].y;
        }
        return {s, GetCost(s) == kInf ? kInf : cost};
    };

    std::queue<State> q;
    static bool in_queue[kN][kN][4];

    for (int x = 0; x < kN; x++)
        for (int y = 0; y < kN; y++)
            for (int dir = 0; dir < 4; dir++) {
                out[x][y][dir] = {kInf, ShipNaviInfo::stay};
                in_queue[x][y][dir] = false;
            }
    for (auto& d : dest)
        for (int dir = 0; dir < 4; dir++) {
            out[d.x][d.y][dir] = {0, ShipNaviInfo::stay};
            q.push({d.x, d.y, dir});
            in_queue[d.x][d.y][dir] = true;
        }

    while (!q.empty()) {
        State u = q.front();
        q.pop();
        in_queue[u.x][u.y][u.dir] = false;
        for (auto action : {ShipNaviInfo::rot0, ShipNaviInfo::rot1, ShipNaviInfo::move}) {
            auto [v, cost] = Move(u, action);
            if (cost == kInf)
                continue;
            ShipNaviInfo& dis_v = out[v.x][v.y][v.dir];
            if(dis_v.dis > out[u.x][u.y][u.dir].dis + cost) {
                dis_v = {out[u.x][u.y][u.dir].dis + cost, action};
                if (!in_queue[v.x][v.y][v.dir]) {
                    q.push(v);
                    in_queue[v.x][v.y][v.dir] = true;
                }
            }
        }
    }
}

#endif //HARBOR_MANAGER__BFS_H_
