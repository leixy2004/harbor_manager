#include <iostream>
#include <thread>
#include "robot.h"
#include "constant.h"
#include "map.h"
#include "ship.h"
#include "berth.h"
#include "goods.h"
#include "queue.h"
#include "bfs.h"
Robot robot[kRobotCount];
Map map;
Ship ship[kShipCount];
Berth berth[kBerthCount];
Goods *goods[kN * kN];
Queue<Goods, kGoodsMaxAdded> goods_queue;
int current_time = 0;
int current_value = 0;
int current_goods_added = 0;
int current_goods_removed = 0;
void PrintOK() {
  std::cout << "OK" << std::endl;
  std::cout.flush();
}
bool ReadOK() {
  static std::string s;
  std::cin >> s;
  return s == "OK";
}

namespace init {

void InitInputMap() {
  for (auto &line : map.grid) {
    std::cin >> line;
  }
}

void InitAllRobot() {
  int idx = 0;
  for (int i = 0; i < kN; i++) {
    for (int j = 0; j < kN; j++) {
      if (map.grid[i][j] == 'A') {
        robot[idx].id = idx;
        robot[idx].position = Position(i, j);
        robot[idx].status = Robot::kIdle;
        robot[idx].goods_id = -1;
        robot[idx].dir = -1;
        idx++;
      }
    }
  }
  //TODO: init robot_map, it is wrong!
}

void InitAllBerth() {
  for (auto &b : berth) {
    int id, x, y, time, speed;
    std::cin >> id >> x >> y >> time >> speed;
    std::cerr << "id: " << id << " x: " << x << " y: " << y << " time: " << time << " speed: " << speed << std::endl;
    b.id = id;
    b.top = x;
    b.left = y;
    b.bottom = x + kBerthSize - 1;
    b.right = y + kBerthSize - 1;
    b.transport_time = time;
    b.loading_speed = speed;
    b.saved_goods = 0;
    static Position area[kBerthSize * kBerthSize]{};
    for (int i = 0; i < kBerthSize; i++) {
      for (int j = 0; j < kBerthSize; j++) {
        area[i * kBerthSize + j] = Position{x + i, y + j};
      }
    }
    Bfs(kBerthSize * kBerthSize, area, b.dis, b.pre, map);

  }
  for (int x = 0; x < kN; x++) {
    for (int y = 0; y < kN; y++) {
      map.dis[x][y] = kN * kN;
      map.pre[x][y] = -1;
      if (!map.IsEmpty(x, y)) continue;
      for (auto &b : berth) {
        if (map.dis[x][y] > b.dis[x][y]) {
          map.dis[x][y] = b.dis[x][y];
          map.pre[x][y] = b.pre[x][y];
          if (b.dis[x][y] == 0) {
            map.berth_id[x][y] = b.id;
          }
        }
      }
    }
  }
}

void InitAllShip() {
  std::cin >> Ship::capacity;
  std::cerr << "Ship::capacity: " << Ship::capacity << std::endl;
  for (int i = 0; i < kShipCount; i++) {
    ship[i].id = i;
    ship[i].status = Ship::kIdle;
  }
}

bool InitInput() {
  map.Init();
  InitInputMap();
  InitAllRobot();
  InitAllBerth();
  InitAllShip();
  return ReadOK();
}

}

void Init() {
  using namespace init;
  if (InitInput()) {
    std::cerr << "Init success" << std::endl;
    PrintOK();
  } else {
    std::cerr << "Init failed" << std::endl;
  }
}

namespace input {

void AddGoods() {
  int x, y, value;
  std::cin >> x >> y >> value;
 // fprintf(stderr, "AddGoods x: %d y: %d value: %d\n", x, y, value);
  goods_queue.push(Goods(x, y, current_goods_added, Goods::kNone, value, current_time));
  current_goods_added++;
  goods[goods_queue.back().id] = &goods_queue.back();
  std::thread t(Bfs1,
                goods_queue.back().position,
                goods_queue.back().dis,
                goods_queue.back().pre,
                &(goods_queue.back().status),
                std::ref(map));
//  Bfs1(goods_queue.back().position, goods_queue.back().dis, goods_queue.back().pre, goods_queue.back().robot_id);
  t.detach();
}

void InputRobot(int id) {
  int carry_goods;
  int x, y;
  int status;
  std::cin >> carry_goods >> x >> y >> status;
//  std::cerr << "carry_goods: " << carry_goods << " x: " << x << " y: " << y << " robot_id: " << robot_id << std::endl;
  //fprintf(stderr, "InputRobot id: %d, carry_goods: %d, x: %d, y: %d, status: %d\n", id, carry_goods, x, y, status);
  robot[id].position = Position(x, y);
  if (status) { // robot is running
    if (carry_goods) {
      robot[id].status = Robot::kGoingToUnload;
    } else {
      if (robot[id].goods_id == -1) {
        robot[id].status = Robot::kIdle;
      } else {
        robot[id].status = Robot::kGoingToLoad;
      }
    }
  } else {
    if (carry_goods) {
      robot[id].status = Robot::kBreakdownWithGoods;
    } else {
      robot[id].status = Robot::kBreakdown;
    }
  }
}
void InputShip(int id) {
  int status;
  int berth_id;
  std::cin >> status >> berth_id; 
    if (status == 1) {
      if (berth_id == -1) {
          ship[id].status = Ship::kAtEnd;
      }
      else {
          ship[id].status = Ship::kAtBerth;
          ship[id].nowBerth = berth_id;
      }
  }
    else if (status == 2) {
        ship[id].status = Ship::kIdle;
    }

}

}

bool Input() {
//  std::cerr << "Input" << std::endl;
  using namespace input;
  std::cin >> current_time >> current_value;
  std::cerr << "current_time: " << current_time << std::endl;
  int temp_goods;
  std::cin >> temp_goods;
  for (int i = 0; i < temp_goods; i++) {
    AddGoods();
  }
  for (int i = 0; i < kRobotCount; i++) {
    InputRobot(i);
  }
  for (int i = 0; i < kShipCount; i++) {
    InputShip(i);
  }
  return ReadOK();
}

double GetGoodsValue(int id, int x, int y) {
  return goods[id]->value * 1.0
      / (goods[id]->dis[x][y] + 5)
      * (current_time - goods[id]->occur_time + 1);
}

int FindGoods(int x, int y) {
  int goods_id = -1;
  double max_value = -1;
  for (int i = 0; i < goods_queue.size(); i++) {
    if (goods_queue[i].status == Goods::kWaiting) {
//      fprintf(stderr, "possible goods_id: %d,dis: %d\n", goods_queue[i].id, goods_queue[i].dis[x][y]);
      if (current_time - goods_queue[i].occur_time + goods_queue[i].dis[x][y] >= kGoodsDuration) continue;
      if (goods_queue[i].dis[x][y] >= kN * kN) continue;
      double v = GetGoodsValue(goods_queue[i].id, x, y);
      if (v > max_value) {
        goods_id = goods_queue[i].id;
        max_value = v;
      }
    }
//    fprintf(stderr, "FindGoods x: %d y: %d -> %d\n", x, y, goods_id);
  }
  return goods_id;
}

void EnableRobot(int id) {
  int goods_id = FindGoods(robot[id].position.x, robot[id].position.y);
  if (goods_id != -1) {
    robot[id].status = Robot::kGoingToLoad;
    robot[id].goods_id = goods_id;
    goods[goods_id]->status = Goods::kTargeted;
//        std::cerr << "goods_id: " << goods_id << std::endl;
  }
}

void UpdateRobot(int id) {
//  fprintf(stderr, "UpdateRobot id: %d\n", id);
  {
    int x = robot[id].position.x;
    int y = robot[id].position.y;
//    while (true) {
    bool flag = false;
    if (robot[id].status == Robot::kIdle) {
//      std::thread t(EnableRobot, id);
      EnableRobot(id);
    } else if (robot[id].status == Robot::kGoingToLoad) {
      int dir = goods[robot[id].goods_id]->pre[x][y];
      if (dir == -1) {
        if (goods[robot[id].goods_id]->id != robot[id].goods_id
            || robot[id].position != goods[robot[id].goods_id]->position
            || goods[robot[id].goods_id]->status != Goods::kTargeted) {
          robot[id].status = Robot::kIdle;
          robot[id].dir = -1;
          robot[id].goods_id = -1;
          flag = true;
        }
        robot[id].PrintLoad();
        //fprintf(stderr, "robot:%d LOAD\n", id);
        robot[id].status = Robot::kGoingToUnload;
        robot[id].dir = -1;
        goods[robot[id].goods_id]->status = Goods::kCaptured;
      } else {
        robot[id].dir = (kInverseDir[dir]);
      }
    } else if (robot[id].status == Robot::kGoingToUnload) {
      int dir = map.pre[x][y];
      if (dir == -1) {
        if (map.berth_id[x][y]==0) {
            std::cerr<<"BIG ERROR, WRONG BERTH"<<std::endl;
            robot[id].status = Robot::kIdle;
            robot[id].dir = -1;
            robot[id].goods_id = -1;
            flag = true;
        }
        robot[id].PrintUnload();
        berth[map.berth_id[x][y]].saved_goods ++;
        fprintf(stderr, "robot:%d UNLOAD\n", id);
        robot[id].status = Robot::kIdle;
        robot[id].dir = -1;
//        goods[robot[id].goods_id]->status = Goods::kOnBerth;
        robot[id].goods_id = -1;
        flag = true;
      } else {
        robot[id].dir = (kInverseDir[dir]);
      }
//        if (!flag) break;
//      }
    }
    /*fprintf(stderr,
            "robot_id: %d, robot_status: %d, robot_goods_id: %d, robot_dir: %d\n",
            robot[id].id,
            robot[id].status,
            robot[id].goods_id,
            robot[id].dir);*/
  }
//  std::cerr << "Robot robot_id: " << robot[id].robot_id << std::endl;
}

bool CheckMoveAndMakeValid() {
  // TODO: Need fix
  bool flag = false;
  for (int i = 0; i < kRobotCount; i++) {
    int xi = robot[i].position.x;
    int yi = robot[i].position.y;
    for (int j = 0; j < kRobotCount; j++) {
      if (i == j) continue;
      int xj = robot[j].position.x;
      int yj = robot[j].position.y;
      bool i_move = robot[i].dir != -1;
      bool j_move = robot[j].dir != -1;
      if (!i_move && !j_move) continue;
      if (i_move && j_move) {
        auto new_pos_i = robot[i].position.Move(robot[i].dir);
        auto new_pos_j = robot[j].position.Move(robot[j].dir);
        if (new_pos_i == new_pos_j
            || (xi == new_pos_j.x && yi == new_pos_j.y
                && xj == new_pos_i.x && yj == new_pos_i.y)) { // type 1->' '<-2
          bool change = false;
          static const int *kTurn[3] = {kTurnRight, kTurnLeft, kInverseDir};
          for (int k = 0; k < 3; k++) {
            if (map.IsEmpty(robot[i].position.Move(kTurn[k][robot[i].dir]))) {
              robot[i].dir = kTurn[k][robot[i].dir];
              change = true;
              break;
            }
          }
//          if (!change) {
//            for (int k = 0; k < 4; k++) {
//              if (k == robot[j].dir) continue;
//              if (map.IsEmpty(robot[j].position.Move(k))) {
//                robot[j].dir = k;
//                change = true;
//                break;
//              }
//            }
//          }
          if (!change) {
            robot[i].dir = -1;
//            robot[j].dir = -1;
          }
//          if (map.IsEmpty(robot[i].position.Move(kTurnRight[robot[i].dir]))) {
//            robot[i].dir = kTurnRight[robot[i].dir];
//          } else if (map.IsEmpty(robot[j].position.Move(kTurnRight[robot[j].dir]))) {
//            robot[j].dir = kTurnRight[robot[j].dir];
//          } else if (map.IsEmpty(robot[i].position.Move(kTurnLeft[robot[i].dir]))) {
//            robot[i].dir = kTurnLeft[robot[i].dir];
//          } else if (map.IsEmpty(robot[j].position.Move(kTurnLeft[robot[j].dir]))) {
//            robot[j].dir = kTurnLeft[robot[j].dir];
//          } else if (map.IsEmpty(robot[i].position.Move(kInverseDir[robot[i].dir]))) {
//            robot[i].dir = kInverseDir[robot[i].dir];
//          } else if (map.IsEmpty(robot[j].position.Move(kInverseDir[robot[j].dir]))) {
//            robot[j].dir = kInverseDir[robot[j].dir];
//          } else {
//            robot[i].dir = -1;
////            robot[j].dir = -1;
//          }
          flag = true;
        }
      } else {
        if (i_move) {
          auto new_pos_i = robot[i].position.Move(robot[i].dir);
          if (new_pos_i == robot[j].position) {
            if (map.IsEmpty(robot[i].position.Move(kTurnRight[robot[i].dir]))) {
              robot[i].dir = kTurnRight[robot[i].dir];
            } else if (map.IsEmpty(robot[i].position.Move(kTurnLeft[robot[i].dir]))) {
              robot[i].dir = kTurnLeft[robot[i].dir];
            } else if (map.IsEmpty(robot[i].position.Move(kInverseDir[robot[i].dir]))) {
              robot[i].dir = kInverseDir[robot[i].dir];
            } else {
              robot[i].dir = -1;
            }
            flag = true;
          }
        } else {
          auto new_pos_j = robot[j].position.Move(robot[j].dir);
          if (new_pos_j == robot[i].position) {
            if (map.IsEmpty(robot[j].position.Move(kTurnRight[robot[j].dir]))) {
              robot[j].dir = kTurnRight[robot[j].dir];
            } else if (map.IsEmpty(robot[j].position.Move(kTurnLeft[robot[j].dir]))) {
              robot[j].dir = kTurnLeft[robot[j].dir];
            } else if (map.IsEmpty(robot[j].position.Move(kInverseDir[robot[j].dir]))) {
              robot[j].dir = kInverseDir[robot[j].dir];
            } else {
              robot[j].dir = -1;
            }
            flag = true;
          }
        }
      }
    }
  }
  return flag;
}


int FindBerth(int id) {
    int maxBerth = 0, dir = -1;
    for (int i = 0; i < kBerthCount; i++) {
        //if (berth[i].have_ship[current_time+berth[i].transport_time])continue;
        if (berth[i].have_ship && berth[i].saved_goods<2* ship[id].capacity)continue;
        if (berth[i].saved_goods >= ship[id].capacity) {
            return i;
        }
        if (berth[i].saved_goods > maxBerth) {
            maxBerth = berth[i].saved_goods;
            dir = i;
        }
    }
    return dir;
}

void UpdateShip(int id) {
    fprintf(stderr, "UpdateShip id: %d status %d  dir %d\n", id, ship[id].status,ship[id].dir);
    
    if (ship[id].status == Ship::kAtEnd) {
        int dir = FindBerth(id);
        if (dir != -1) {
            ship[id].dir = dir;
            ship[id].PrintShip();
            ship[id].status = Ship::kGoBack;
            ship[id].nowGoods = 0;
            berth[dir].have_ship ++;
            //berth[dir].have_ship[current_time + berth[dir].transport_time] = 1;
            //fprintf(stderr, "%d\n", current_time + berth[dir].transport_time);
            fprintf(stderr, "goto %d\n", dir);
            return;
        }
    }
    else if (ship[id].status == Ship::kAtBerth) {
        fprintf(stderr, "now in %d\n", ship[id].nowBerth);
        int now = ship[id].nowBerth;
        //berth[now].have_ship ++;
        if (ship[id].nowGoods < ship[id].capacity) {
            if (berth[now].saved_goods > 0) {
                int goodsNum = std::min({ berth[now].loading_speed, berth[now].saved_goods,ship[id].capacity - ship[id].nowGoods });
                berth[now].saved_goods -= goodsNum;
                ship[id].nowGoods += goodsNum;

            }
            //to update
            if (berth[now].saved_goods == 0 && ship[id].nowGoods < ship[id].capacity) {
                int dir = FindBerth(id);
                if (dir != -1) {
                    ship[id].dir = dir;
                    ship[id].PrintShip();
                    berth[now].have_ship --;
                    ship[id].status = Ship::kGoBack;
                    fprintf(stderr, "goto %d\n", dir);
                    return;
                }
            }
        }

        if (ship[id].nowGoods == ship[id].capacity) {
            ship[id].PrintGo();
            berth[now].have_ship--;
            ship[id].dir = -1;
            ship[id].status = Ship::kGoTo;
            fprintf(stderr, "goto -1\n");
            return;
        }
    }
    else if (ship[id].status == Ship::kIdle) {

    }
}

void UpdateOutput() {
  while (!goods_queue.empty() && current_time - goods_queue.front().occur_time >= kGoodsDuration) {
    goods_queue.front().status = Goods::kExpired;
    goods_queue.pop();
    current_goods_removed++;
  }

  //fprintf(stderr, "goods_queue.size(): %d\n", goods_queue.size());
  for (int i = 0; i < goods_queue.size(); i++) {
    //fprintf(stderr, "now_goods_queue[%d].status: %d\n", i, goods_queue[i].status);
  }
  for (int i = 0; i < kRobotCount; i++) {
    UpdateRobot(i);
  }
  for (int i = 0; i < kBerthCount; i++) {
      fprintf(stderr, "Berth id %d  goods %d\n",i,berth[i].saved_goods);
  }
  if (current_time == 1) {
      for (int i = 0; i < kShipCount; i++) {
          berth[i].have_ship++;
          ship[i].dir = i;
          ship[i].PrintShip();
          ship[i].status = Ship::kGoBack;
          fprintf(stderr, "goto %d\n", i);
      }
  }
  else {
      for (int i = 0; i < kShipCount; i++) {
          UpdateShip(i);
      }
  }
//  while (CheckMoveAndMakeValid()) {}
  for (int cnt = 0; cnt < 100 && CheckMoveAndMakeValid(); cnt++) {}
  for (int i = 0; i < kRobotCount; i++) {
    if (robot[i].dir != -1) robot[i].PrintMove();
  }
  PrintOK();
}

int main() {
//  std::cin.tie(nullptr);
//  std::cout.tie(nullptr);
//  std::ios::sync_with_stdio(false);
  Init();
  for (int i = 0; i < kGameDuration; i++) {
    if (Input()) {
      //std::cerr << "Input success" << std::endl;
       
      UpdateOutput();
    }
  }
  return 0;
}