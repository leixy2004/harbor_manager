#include <iostream>
//#include <cstdio>
#include <algorithm>
//#include <fstream>
#include <array>
#include <list>
#include <chrono>
#include "robot.h"
#include "constant.h"
#include "map.h"
#include "ship.h"
#include "berth.h"
#include "goods.h"
#include "bfs.h"
//#include "thread_pool.h"
std::array<Robot, kRobotCount> robot;
Map map;
std::array<Ship, kShipCount> ship;
std::array<Berth, kBerthCount> berth;
std::array<Goods, kGoodsMaxAdded> goods;
std::list<Goods *> goods_waiting;
//ThreadPool pool(1);
int current_time = 0;
int current_value = 0;
int current_goods_added = 0;
int current_goods_removed = 0;
void PrintOK() {
  std::cout << "OK" << std::endl;
  std::cout.flush();
}
bool ReadOK() {
  static std::string str;
  std::cin >> str;
  return str == "OK";
}

//long long TimeRecord() {
//  static std::chrono::high_resolution_clock::time_point last_time{};
//  auto t = std::chrono::high_resolution_clock::now();
//  auto res = std::chrono::duration_cast<std::chrono::milliseconds>(t - last_time).count();
//  last_time = t;
//  return res;
//}

namespace init {

void InitInputMap() {
  std::string str;
  for (auto &line : map.grid) {
    std::cin >> str;
    std::copy(str.begin(), str.end(), line.begin());
  }
}

void InitAllRobot() {
  for (int i = 0; i < kRobotCount; i++) {
    robot[i].id = i;
    robot[i].Refresh();
  }
}

void InitAllBerth() {
  for (auto &b : berth) {
    int id, x, y, time, speed;
    std::cin >> id >> x >> y >> time >> speed;
    b.id = id;
    b.top = x;
    b.left = y;
    b.bottom = x + kBerthSize - 1;
    b.right = y + kBerthSize - 1;
    b.transport_time = time;
    b.loading_speed = speed;
    b.saved_goods = 0;
    b.Show();
    static Position area[kBerthSize * kBerthSize]{};
    for (int i = 0; i < kBerthSize; i++) {
      for (int j = 0; j < kBerthSize; j++) {
        area[i * kBerthSize + j] = Position{x + i, y + j};
      }
    }
    Bfs(kBerthSize * kBerthSize, area, b.dis, b.pre, map);
  }

//  for (int x = 0; x < kN; x++) {
//    for (int y = 0; y < kN; y++) {
//      map.dis[x][y] = kInf;
//      map.pre[x][y] = kStay;
//      if (!map.IsEmpty(x, y)) continue;
//      for (auto &b : berth) {
//        if (map.dis[x][y] > b.dis[x][y]) {
//          map.dis[x][y] = b.dis[x][y];
//          map.pre[x][y] = b.pre[x][y];
//          if (b.dis[x][y] == 0) {
//            map.berth_id[x][y] = b.id;
//          }
//        }
//      }
//    }
//  }

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
//  map.Init();
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
//  fprintf(stderr, "AddGoods");
  int x, y, value;
  std::cin >> x >> y >> value;
  goods[current_goods_added].id = current_goods_added;
  goods[current_goods_added].position = Position(x, y);
  goods[current_goods_added].value = value;
  goods[current_goods_added].occur_time = current_time;
  goods[current_goods_added].status = Goods::kNone;
  goods[current_goods_added].robot_id = -1;
  [&](Goods *ptr) {
    ptr->AllocateMemory();
    Bfs(1, &ptr->position, *ptr->dis, *ptr->pre, map);
    ptr->status = Goods::kWaiting;
    goods_waiting.push_back(ptr);

  }(&goods[current_goods_added]);
  current_goods_added++;
}

void InputRobot(int id) {
  int carry_goods;
  int x, y;
  int status;
  std::cin >> carry_goods >> x >> y >> status;
  robot[id].position = Position(x, y);
  if (status) { // robot is running
    if (carry_goods) {
      robot[id].status = Robot::kGoingToUnload;
    } else { // not carry goods
      if (robot[id].goods_id == -1) {
        robot[id].status = Robot::kIdle;
      } else {
        robot[id].status = Robot::kGoingToLoad;
      }
    }
  } else { // robot is breakdown
    if (carry_goods) {
      robot[id].status = Robot::kBreakdownWithGoods;
    } else { // not carry goods
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
    } else {
      ship[id].status = Ship::kAtBerth;
      ship[id].nowBerth = berth_id;
    }
  } else if (status == 2) {
    ship[id].status = Ship::kIdle;
  }

}

}

bool Input() {
  using namespace input;
  std::cin >> current_time >> current_value;
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
//  fprintf(stderr, "GetGoodsValue id: %d x: %d y: %d\n", id, x, y);
  return goods[id].value * 1.0
      / ((*goods[id].dis)[x][y] + 5)
      * (current_time - goods[id].occur_time + 5);
}

int RobotFindGoods(int x, int y) {
//  fprintf(stderr, "RobotFindGoods x: %d y: %d\n", x, y);
  int goods_id = -1;
  double max_value = -1;
  for (auto &g : goods_waiting) {
    if (g->status != Goods::kWaiting) continue;
    if (g->dis == nullptr || g->pre == nullptr) {
      fprintf(stderr, "g->dis == nullptr || g->pre== nullptr\n");
      continue;
    }
    auto &dis = *g->dis;
//    auto &pre = *g->pre;
    if (current_time - g->occur_time + dis[x][y] >= kGoodsDuration) continue;
    if (dis[x][y] >= kN * kN) continue;
    double v = GetGoodsValue(g->id, x, y);
//    fprintf(stderr, "goods_id: %d v: %lf\n", g->id, v);
    if (v > max_value) {
      goods_id = g->id;
      max_value = v;
    }
  }
  return goods_id;
}

double GetBerthValue(int id, int x, int y) {
//  fprintf(stderr, "GetBerthValue id: %d x: %d y: %d\n", id, x, y);
  /*return 1.0 * (berth[id].loading_speed)
      / (berth[id].dis[x][y] + 5)
      / (berth[id].saved_goods + 5)
      / (berth[id].transport_time + 5);*/
    return 1.0 / (berth[id].dis[x][y] + 5);
}

int RobotFindBerth(int x, int y) {
  int berth_id = -1;
  double max_value = -1;
  for (int i = 0; i < kBerthCount; i++) {
    if (berth[i].dis[x][y] >= kN * kN) continue;
    double v = GetBerthValue(i, x, y);
    if (v > max_value) {
      berth_id = i;
      max_value = v;
    }
  }
  return berth_id;
}

void AllocateGoodsToRobot(int id) {
//  fprintf(stderr, "AllocateGoodsToRobot id: %d\n", id);
  int goods_id = RobotFindGoods(robot[id].position.x, robot[id].position.y);
  if (goods_id == -1) {
//    fprintf(stderr, "Robot %d find no goods available.\n", id);
    return;
  }
  robot[id].status = Robot::kGoingToLoad;
  robot[id].goods_id = goods_id;
  goods[goods_id].status = Goods::kTargeted;
  goods[goods_id].robot_id = id;
}

void AllocateBerthToRobot(int id) {
  int berth_id = RobotFindBerth(robot[id].position.x, robot[id].position.y);
  if (berth_id == -1) {
    fprintf(stderr, "Robot %d find no berth available.\n", id);
    return;
  }
  robot[id].status = Robot::kGoingToUnload;
  robot[id].berth_id = berth_id;

}

void UpdateRobot(int id) {
  int &x = robot[id].position.x;
  int &y = robot[id].position.y;
  if (robot[id].status == Robot::kIdle) {
    AllocateGoodsToRobot(id);
    robot[id].dir = kStay;
  } else if (robot[id].status == Robot::kGoingToLoad) {
    if (robot[id].goods_id == -1) {
      fprintf(stderr, "robot:%d goods_id == -1\n", id);
      robot[id].Refresh();
      return;
    }
    if (goods[robot[id].goods_id].status != Goods::kTargeted) {
      fprintf(stderr, "robot:%d goods[%d].status != Goods::kTargeted\n", id, robot[id].goods_id);
      robot[id].Refresh();
      return;
    }
    if (goods[robot[id].goods_id].dis == nullptr
        || goods[robot[id].goods_id].pre == nullptr) {
      fprintf(stderr, "goods[%d].dis == nullptr || goods[%d].pre == nullptr\n",
              robot[id].goods_id, robot[id].goods_id);
      robot[id].Refresh();
      return;
    }
    int dir = (*goods[robot[id].goods_id].pre)[x][y];
    if (dir == kStay) { // load
      if (goods[robot[id].goods_id].id != robot[id].goods_id
          || robot[id].position != goods[robot[id].goods_id].position
          || goods[robot[id].goods_id].status != Goods::kTargeted) {
        fprintf(stderr, "Robot find no goods there,\n");
        robot[id].Refresh();
      }
      robot[id].PrintLoad();
      goods[robot[id].goods_id].status = Goods::kCaptured;
      goods[robot[id].goods_id].DeallocateMemory();
      AllocateBerthToRobot(id);
      robot[id].dir = kStay;
    } else {
      robot[id].dir = (kInverseDir[dir]);
    }
  } else if (robot[id].status == Robot::kGoingToUnload) {
    if (robot[id].berth_id == -1) {
      fprintf(stderr, "robot:%d berth_id == -1\n", id);
      AllocateBerthToRobot(id);
      return;
    }
    int dir = berth[robot[id].berth_id].pre[x][y];
    if (dir == kStay) {
      if (!berth[robot[id].berth_id].IsInArea(x, y)) {
        fprintf(stderr, "Robot %d (%d, %d) find no berth (id:%d) there.\n", id, x, y, robot[id].berth_id);
        AllocateBerthToRobot(id);
        return;
      }
      robot[id].PrintUnload();
      berth[robot[id].berth_id].saved_goods++;
      robot[id].Refresh();
    } else {
      robot[id].dir = (kInverseDir[dir]);
    }
  }
}

bool CheckMoveAndMakeValid() {
  bool flag = false;
  for (int i = 0; i < kRobotCount; i++) {
    for (int j = 0; j < kRobotCount; j++) {
      if (i == j) continue;
      bool i_move = robot[i].dir != kStay;
      bool j_move = robot[j].dir != kStay;
      static const int *kTurn[3] = {kTurnRight, kTurnLeft, kInverseDir};
      if (!i_move && !j_move) continue;
      if (i_move && j_move) {
        auto new_pos_i = robot[i].position.Move(robot[i].dir);
        auto new_pos_j = robot[j].position.Move(robot[j].dir);
        if (new_pos_i == new_pos_j
            || (robot[i].position == new_pos_j
                && robot[j].position == new_pos_i)) { // type 1->' '<-2
          bool change = false;
          for (auto &k : kTurn) {
            if (map.IsEmpty(robot[i].position.Move(k[robot[i].dir]))) {
              robot[i].dir = k[robot[i].dir];
              change = true;
              break;
            }
          }
          if (!change) {
            robot[i].dir = kStay;
          }
          flag = true;
        }
      } else {
        if (i_move) {
          auto new_pos_i = robot[i].position.Move(robot[i].dir);
          if (new_pos_i == robot[j].position) {
            bool change = false;
            for (auto &k : kTurn) {
              if (map.IsEmpty(robot[i].position.Move(k[robot[i].dir]))) {
                robot[i].dir = k[robot[i].dir];
                change = true;
                break;
              }
            }
            if (!change) {
              robot[i].dir = kStay;
            }
            flag = true;
          }
        } else { // j_move
          auto new_pos_j = robot[j].position.Move(robot[j].dir);
          if (new_pos_j == robot[i].position) {
            bool change = false;
            for (auto &k : kTurn) {
              if (map.IsEmpty(robot[j].position.Move(k[robot[j].dir]))) {
                robot[j].dir = k[robot[j].dir];
                change = true;
                break;
              }
            }
            if (!change) {
              robot[j].dir = kStay;
            }
            flag = true;
          }
        }
      }
    }
  }
  return flag;
}

int ShipFindBerth(int id) {
  int maxBerth = 0, dir = -1;
  for (int i = 0; i < kBerthCount; i++) {
    //if (berth[i].have_ship[current_time+berth[i].transport_time])continue;
    if (berth[i].have_ship && berth[i].saved_goods < 2 * Ship::capacity) continue;
    if (berth[i].saved_goods >= Ship::capacity) {
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
  fprintf(stderr, "UpdateShip id: %d status %d  dir %d have %d/%d goods\n", id, ship[id].status, ship[id].dir, ship[id].nowGoods, ship[id].capacity);
  if (ship[id].status == Ship::kAtEnd) {
    int dir = ShipFindBerth(id);
    if (dir != -1) {
      ship[id].dir = dir;
      ship[id].PrintShip();
      ship[id].status = Ship::kGoBack;
      ship[id].nowGoods = 0;
      berth[dir].have_ship++;
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
        int goodsNum = std::min(std::min(berth[now].loading_speed, berth[now].saved_goods), ship[id].capacity - ship[id].nowGoods);
        berth[now].saved_goods -= goodsNum;
        ship[id].nowGoods += goodsNum;

      }
      
      
      //to update
      if (berth[now].saved_goods == 0 && ship[id].nowGoods < ship[id].capacity) {
          if (ship[id].nowGoods > ship[id].capacity * 9 / 10 && current_time + 2*berth[now].transport_time + 510 < 15000) {
              ship[id].PrintGo();
              berth[now].have_ship--;
              ship[id].dir = -1;
              ship[id].status = Ship::kGoTo;
              fprintf(stderr, "goto -1\n");
              return;
        }
        int dir = ShipFindBerth(id);
        if (dir != -1) {
            if (current_time + berth[dir].transport_time + 510 > 15000) {
                ship[id].PrintGo();
                berth[now].have_ship--;
                ship[id].dir = -1;
                ship[id].status = Ship::kGoTo;
                fprintf(stderr, "goto -1\n");
                return;
            }
          ship[id].dir = dir;
          ship[id].PrintShip();
          berth[now].have_ship--;
          berth[dir].have_ship++;
          ship[id].status = Ship::kGoBack;
          fprintf(stderr, "goto %d\n", dir);
          return;
        }
      }

    }
    if (current_time + berth[now].transport_time + 10 > 15000) {
        ship[id].PrintGo();
        berth[now].have_ship--;
        ship[id].dir = -1;
        ship[id].status = Ship::kGoTo;
        fprintf(stderr, "goto -1\n");
        return;
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

void RemoveExpiredGoods() {
  while (current_goods_removed < current_goods_added
      && current_time - goods[current_goods_removed].occur_time >= kGoodsDuration) {
    auto &g = goods[current_goods_removed];
    if (g.status == Goods::kWaiting) {
      g.status = Goods::kExpired;
    } else if (g.status == Goods::kTargeted) {
      g.status = Goods::kExpired;
      robot[g.robot_id].Refresh();
    } else if (g.status == Goods::kCaptured) {
//      std::cerr << "Captured Goods should not in list" << std::endl;
    } else {
      std::cerr << "BIG ERROR, WRONG GOODS" << std::endl;
    }
    g.DeallocateMemory();
    current_goods_removed++;
  }
  while (!goods_waiting.empty()
      && goods_waiting.front()->status == Goods::kExpired) {
    goods_waiting.pop_front();
  }
}

void UpdateOutput() {
  RemoveExpiredGoods();
  for (int i = 0; i < kRobotCount; i++) {
    UpdateRobot(i);
  }

  for (int cnt = 0; cnt < 100 && CheckMoveAndMakeValid(); cnt++) {
//    fprintf(stderr, "CheckMoveAndMakeValid No.%d\n", cnt);
  }
  for (auto &i : robot) {
    if (i.dir != kStay) i.PrintMove();
  }
  goods_waiting.remove_if([](Goods *g) {
    if (g->status == Goods::kTargeted) {
      return true;
    } else if (g->status == Goods::kCaptured) {
      g->DeallocateMemory();
      return true;
    } else {
      return false;
    }
  });
  for (int i = 0; i < kBerthCount; i++) {
      fprintf(stderr, "Berth %d has %d\n", i,berth[i].saved_goods);
  }
  if (current_time == 1) {
    for (int i = 0; i < kShipCount; i++) {
      berth[i].have_ship++;
      ship[i].dir = i;
      ship[i].PrintShip();
      ship[i].status = Ship::kGoBack;
      //fprintf(stderr, "goto %d\n", i);
    }
  } else {
    for (int i = 0; i < kShipCount; i++) {
      UpdateShip(i);
    }
  }
}

int main() {
  Init();
  while (Input()) {
//    auto start = std::chrono::high_resolution_clock::now();
    UpdateOutput();
//      auto end = std::chrono::high_resolution_clock::now();
//      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//      std::cerr << "UpdateOutput time: " << duration.count() << "ms" << std::endl;
//      if (duration.count() < 14) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(14 - duration.count()));
//      std::cerr << "UpdateOutput time: " << duration.count() << "ms" << std::endl;
//      }
    PrintOK();
  }
//  }
  return 0;
}