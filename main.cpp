#include <iostream>
//#include <cstdio>
#include <algorithm>
#include <fstream>
#include <array>
#include <cmath>
//#include <list>
#include <chrono>
//#include <format>
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
//std::list<Goods *> goods_waiting;
//ThreadPool pool(1);
int current_time = 0;
int current_value = 0;
int goods_added = 0;
int goods_removed = 0;
void PrintOK() {
  std::cout << "OK" << std::endl;
  std::cout.flush();
}
bool ReadOK() {
  static std::string str;
  std::cin >> str;
  return str == "OK";
}

int goods_expired = 0;
int goods_expired_value = 0;
int goods_waiting = 0;
int goods_waiting_value = 0;
int goods_on_robot = 0;
int goods_on_robot_value = 0;
int goods_on_ship = 0;
int goods_on_ship_value = 0;
int goods_on_berth = 0;
int goods_on_berth_value = 0;
int time_cost = 0;
void ShowAll() {
  //  fprintf(f, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
  //          goods_waiting,
  //          goods_waiting_value,
  //          goods_on_robot,
  //          goods_on_robot_value,
  //          goods_on_ship,
  //          goods_on_ship_value,
  //          goods_on_berth,
  //          goods_on_berth_value,
  //          goods_expired,
  //          goods_expired_value
  //  );
  fprintf(stderr,
          "Goods: W %d:%d (%.1lf) -> R %d:%d (%.1lf) -> B %d:%d (%.1lf)\n",
          goods_waiting,
          goods_waiting_value,
          goods_waiting_value * 1.0 / goods_waiting,
          goods_on_robot,
          goods_on_robot_value,
          goods_on_robot_value * 1.0 / goods_on_robot,
          goods_on_berth,
          goods_on_berth_value,
          goods_on_berth_value * 1.0 / goods_on_berth);
  fprintf(stderr,
          "Expired: %d:%d(%.1lf)   ",
          goods_expired,
          goods_expired_value,
          goods_expired_value * 1.0 / goods_expired);
  fprintf(stderr,
          "All %d:%d(%.1lf)\n",
          goods_waiting + goods_on_robot + goods_on_berth + goods_expired,
          goods_waiting_value + goods_on_robot_value + goods_on_berth_value + goods_expired_value,
          (goods_waiting_value + goods_on_robot_value + goods_on_berth_value + goods_expired_value) * 1.0
              / (goods_waiting + goods_on_robot + goods_on_berth + goods_expired));

  fprintf(stderr, "Berth: ");
  for (int i = 0; i < kBerthCount; i++) {
    fprintf(stderr, "%d:%d ", i, berth[i].saved_goods);
  }
  fprintf(stderr, "\n");
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

  for (int x = 0; x < kN; x++) {
    for (int y = 0; y < kN; y++) {
      map.dis[x][y] = kInf;
      map.pre[x][y] = kStay;
      map.berth_id[x][y] = -1;
      if (!map.IsEmpty(x, y)) continue;
      for (auto &b : berth) {
        if (map.dis[x][y] > b.dis[x][y]) {
          map.dis[x][y] = b.dis[x][y];
          map.pre[x][y] = b.pre[x][y];
          map.berth_id[x][y] = b.id;
        }
      }
    }
  }
  auto f = fopen("statistics.txt", "w");

  for (int x = 0; x < kN; x++) {
    for (int y = 0; y < kN; y++) {
      if (map.berth_id[x][y] == -1) {
        fprintf(f, " ");
      } else {
        fprintf(f, "%d", map.berth_id[x][y]);
      }
    }
    fprintf(f, "\n");
  }
  fprintf(f, "\n");
  for (int x = 0; x < kN; x++) {
    for (int y = 0; y < kN; y++) {
      if (map.dis[x][y] == kInf) {
        fprintf(f, "    ");
      } else {
        fprintf(f, "%3d ", map.dis[x][y]);
      }
    }
    fprintf(f, "\n");
  }
  fprintf(f, "\n");
  for (int x = 0; x < kN; x++) {
    for (int y = 0; y < kN; y++) {
      if (map.pre[x][y] == kStay) {
        fprintf(f, " ");
      } else {
        static const char *kDirChar = "><AV";
        fprintf(f, "%c", kDirChar[kInverseDir[map.pre[x][y]]]);
      }
    }
    fprintf(f, "\n");
  }
  fprintf(f, "\n");
  fclose(f);

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
int near_dis = 20;
//void AddColor(int goods_id) {
//  for (int i = 0; i < kN; i++) {
//    for (int j = 0; j < kN; j++) {
//      auto d = (*goods[goods_id].dis)[i][j];
//      if (d < near_dis) {
//        map.color[i][j] += (near_dis - d) * goods[goods_id].value;
//      }
//    }
//  }
//}

void RemoveColor(int goods_id) {
  for (int i = 0; i < kN; i++) {
    for (int j = 0; j < kN; j++) {
      auto d = (*goods[goods_id].dis)[i][j];
      if (d < near_dis) {
        map.color[i][j] -= (near_dis - d) * goods[goods_id].value;
      }
    }
  }
}
namespace input {

void AddGoods() {
//  fprintf(stderr, "AddGoods");
  int x, y, value;
  std::cin >> x >> y >> value;
  goods[goods_added].id = goods_added;
  goods[goods_added].position = Position(x, y);
  goods[goods_added].value = value;
  goods[goods_added].occur_time = current_time;
  goods[goods_added].status = Goods::kNone;
  goods[goods_added].robot_id = -1;
  goods[goods_added].berth_id = map.berth_id[x][y];
  [&](Goods &g) {
    g.AllocateMemory();
    Bfs(1, &g.position, *g.dis, *g.pre, map);
    g.status = Goods::kWaiting;
//    AddColor(g.id);
    goods_waiting++;
    goods_waiting_value += g.value;
//    goods_waiting.push_back(ptr);

  }(goods[goods_added]);
  goods_added++;
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
      robot[id].status = Robot::kGoingToLoad;
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

namespace update_robot_goods {

double GetGoodsValue(int id, int x, int y) {
//  fprintf(stderr, "GetGoodsValue id: %d x: %d y: %d\n", id, x, y);
//  static double limit = 0.1;
//    if (current_time - goods[id].occur_time +(*goods[id].dis)[x][y]>= kGoodsDuration) {
//        return -1;
//    }
  auto time_cost =
      ((*goods[id].dis)[x][y] * 1.0 + berth[goods[id].berth_id].dis[goods[id].position.x][goods[id].position.y] + 5);
  auto passed_time = (current_time - goods[id].occur_time + 2.0);
  auto res = std::exp(goods[id].value * 1.0 / time_cost) * std::log(1 + passed_time / kGoodsDuration);
//  if (goods[id].value * 1.0 / time_cost < limit) {
//    res*=0.8;
//  }
//  if ((kGoodsDuration-passed_time)/(*goods[id].dis)[x][y]<1.1){
//    res*=1.1;
//  }
//  fprintf(stderr, "time_cost: %lf value:%d\n", time_cost, goods[id].value);
  return res;
}

int RobotFindGoods(int x, int y) {
  //  fprintf(stderr, "RobotFindGoods x: %d y: %d\n", x, y);
  int goods_id = -1;
  double max_value = -1;
  for (int i = goods_removed; i < goods_added; i++) {
    auto &g = goods[i];
    if (g.status != Goods::kWaiting && g.status != Goods::kTargeted) continue;
    auto &dis = *g.dis;
    if (current_time - g.occur_time + dis[x][y] >= kGoodsDuration) continue;
    double v = GetGoodsValue(g.id, x, y);
    if (g.status == Goods::kTargeted) {
      double old_v = GetGoodsValue(g.id, robot[g.robot_id].position.x, robot[g.robot_id].position.y);
      if (v > old_v && v > max_value) {
        goods_id = g.id;
        max_value = v;
      }
    } else if (g.status == Goods::kWaiting) {
      if (v > max_value) {
        goods_id = g.id;
        max_value = v;
      }
    }
  }
  return goods_id;
}

bool AllocateGoodsToRobot(int id) {
  //  fprintf(stderr, "AllocateGoodsToRobot id: %d\n", id);
  if (robot[id].status != Robot::kGoingToLoad) {
    fprintf(stderr, "Robot %d is not going to load?\n", id);
    return false;
  }
  bool flag = false;
  if (robot[id].goods_id != -1) {
    goods[robot[id].goods_id].status = Goods::kWaiting;
    goods[robot[id].goods_id].robot_id = -1;
    robot[id].goods_id = -1;
  }
  int goods_id = RobotFindGoods(robot[id].position.x, robot[id].position.y);
  if (goods_id == -1) {
    //    fprintf(stderr, "Robot %d find no goods available.\n", id);
    return false;
  }
  if (goods[goods_id].status == Goods::kTargeted) {
    robot[goods[goods_id].robot_id].Refresh();
    flag = true;
  }
  robot[id].goods_id = goods_id;
  goods[goods_id].status = Goods::kTargeted;
  goods[goods_id].robot_id = id;
  return flag;
}

void ArrangeAllRobotAndGoods() {
  bool flag = false;
  do {
    flag = false;
    for (int i = 0; i < kRobotCount; i++) {
      if (robot[i].status == Robot::kGoingToLoad) {
        flag |= AllocateGoodsToRobot(i);
      }
    }
  } while (flag);
}

}

namespace update_robot_berth {

double GetBerthValue(int id, int x, int y) {
  //  fprintf(stderr, "GetBerthValue id: %d x: %d y: %d\n", id, x, y);
  /*return 1.0 * (berth[id].loading_speed)
      / (berth[id].dis[x][y] + 5)
      / (berth[id].saved_goods + 5)
      / (berth[id].transport_time + 5);*/
  if (current_time + berth[id].transport_time + 5 > 15000) {
    return -1;
  }
  if (current_time > 12000) {
    return 1.0 / (berth[id].dis[x][y] + 5) * (1.0 * berth[id].saved_goods / Ship::capacity)
        / (berth[id].transport_time + 5);
  }
  return 1.0 / (berth[id].dis[x][y] + 5) * (1.5 - 1.0 * berth[id].saved_goods / Ship::capacity);
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

void AllocateBerthToRobot(int id) {
  int berth_id = RobotFindBerth(robot[id].position.x, robot[id].position.y);
  //  int berth_id = id;
  if (berth_id == -1) {
    //    fprintf(stderr, "Robot %d find no berth available.\n", id);
    return;
  }
  robot[id].status = Robot::kGoingToUnload;
  robot[id].berth_id = berth_id;

}

void ArrangeAllRobotAndBerth() {
  for (int i = 0; i < kRobotCount; i++) {
    if (robot[i].status == Robot::kGoingToUnload) {
      AllocateBerthToRobot(i);
    }
  }
}

}

void RobotLoadAndUnload(int id) {
  using namespace update_robot_goods;
  using namespace update_robot_berth;
  int &x = robot[id].position.x;
  int &y = robot[id].position.y;
  if (robot[id].status == Robot::kGoingToLoad) {
    if (robot[id].goods_id == -1) {
      return;
    }
    if ((*goods[robot[id].goods_id].pre)[x][y] == kStay) { // load
      if (robot[id].position != goods[robot[id].goods_id].position
          || goods[robot[id].goods_id].status != Goods::kTargeted) {
        fprintf(stderr, RED("Robot find no goods there,\n"));
        robot[id].Refresh();
      }
      robot[id].PrintLoad();
      robot[id].status = Robot::kGoingToUnload;
      goods[robot[id].goods_id].status = Goods::kCaptured;
//      RemoveColor(robot[id].goods_id);
      goods_waiting--;
      goods_waiting_value -= goods[robot[id].goods_id].value;
      goods_on_robot++;
      goods_on_robot_value += goods[robot[id].goods_id].value;
    }
  } else if (robot[id].status == Robot::kGoingToUnload) {
    if (robot[id].berth_id == -1) {
//      fprintf(stderr, "robot:%d berth_id == -1\n", id);
      return;
    }
    if (berth[robot[id].berth_id].pre[x][y] == kStay) {
      if (!berth[robot[id].berth_id].IsInArea(x, y)) {
        fprintf(stderr, RED("Robot %d (%d, %d) find no berth (id:%d) there.\n"), id, x, y, robot[id].berth_id);
        return;
      }
      robot[id].PrintUnload();
      berth[robot[id].berth_id].saved_goods++;
      goods_on_robot--;
      goods_on_robot_value -= goods[robot[id].goods_id].value;
      goods_on_berth++;
      goods_on_berth_value += goods[robot[id].goods_id].value;
      robot[id].Refresh();
    }
  }
}

void UpdateRobotMoveDir(int id) {
  using namespace update_robot_goods;
  using namespace update_robot_berth;
  int &x = robot[id].position.x;
  int &y = robot[id].position.y;
  robot[id].dir = kStay;
  if (robot[id].status == Robot::kGoingToLoad) {
    if (robot[id].goods_id == -1) {
      return;
    }
    int dir = (*goods[robot[id].goods_id].pre)[x][y];
    if (dir != kStay) {
      robot[id].dir = (kInverseDir[dir]);
    }
  } else if (robot[id].status == Robot::kGoingToUnload) {
    if (robot[id].berth_id == -1) {
      //      fprintf(stderr, "robot:%d berth_id == -1\n", id);
      //      AllocateBerthToRobot(id);
      return;
    }
    int dir = berth[robot[id].berth_id].pre[x][y];
    if (dir != kStay) {
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
            || (robot[i].position == new_pos_j && robot[j].position == new_pos_i)) { // type 1->' '<-2
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
//            for (auto &k : kTurn) {
//              if (map.IsEmpty(robot[j].position.Move(k[robot[j].dir]))) {
//                robot[j].dir = k[robot[j].dir];
//                change = true;
//                break;
//              }
//            }
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
  int maxBerth = 0, dir1 = -1;
  int minUp = Ship::capacity + 1, dir2 = -1;
  for (int i = 0; i < kBerthCount; i++) {
    //if (berth[i].have_ship[current_time+berth[i].transport_time])continue;
    if (berth[i].have_ship && berth[i].saved_goods < 2 * Ship::capacity) continue;
    if (berth[i].saved_goods - Ship::capacity + ship[id].nowGoods >= 0
        && berth[i].saved_goods - Ship::capacity + ship[id].nowGoods < minUp) {
      minUp = berth[i].saved_goods - Ship::capacity + ship[id].nowGoods;
      dir2 = i;
    }
    if (berth[i].saved_goods > maxBerth) {
      maxBerth = berth[i].saved_goods;
      dir1 = i;
    }
  }
  if (dir2 != -1)return dir2;
  return dir1;
}

void UpdateShip(int id) {
  /*fprintf(stderr,
          "UpdateShip id: %d status %d  dir %d have %d/%d goods\n",
          id,
          ship[id].status,
          ship[id].dir,
          ship[id].nowGoods,
          ship[id].capacity);*/
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
      //      fprintf(stderr, "goto %d\n", dir);
      return;
    }
  } else if (ship[id].status == Ship::kAtBerth) {
    //    fprintf(stderr, "now in %d\n", ship[id].nowBerth);
    int now = ship[id].nowBerth;
    //berth[now].have_ship ++;
    if (ship[id].nowGoods == ship[id].capacity) {
      ship[id].PrintGo();
      berth[now].have_ship--;
      ship[id].dir = -1;
      ship[id].status = Ship::kGoTo;
      //      fprintf(stderr, "goto -1\n");
      return;
    }
    if (current_time + berth[now].transport_time + 2 > 15000) {
      ship[id].PrintGo();
      berth[now].have_ship--;
      ship[id].dir = -1;
      ship[id].status = Ship::kGoTo;
      //fprintf(stderr, "goto -1\n");
      return;
    } else if (ship[id].nowGoods < ship[id].capacity) {
      //to update
      if (berth[now].saved_goods == 0) {
        if (ship[id].nowGoods > ship[id].capacity * 9 / 10
            && current_time + 2 * berth[now].transport_time + 510 < 15000) {
          ship[id].PrintGo();
          berth[now].have_ship--;
          ship[id].dir = -1;
          ship[id].status = Ship::kGoTo;
          //fprintf(stderr, "goto -1\n");
          return;
        }
        int dir = ShipFindBerth(id);
        if (dir != -1 && current_time + berth[dir].transport_time + 510 <= 15000) {
          ship[id].dir = dir;
          ship[id].PrintShip();
          berth[now].have_ship--;
          berth[dir].have_ship++;
          ship[id].status = Ship::kGoBack;
          //          fprintf(stderr, "goto %d\n", dir);
          return;
        }
      } else if (berth[now].saved_goods > 0) {
        int goodsNum =
            std::min(std::min(berth[now].loading_speed, berth[now].saved_goods), ship[id].capacity - ship[id].nowGoods);
        berth[now].saved_goods -= goodsNum;
        ship[id].nowGoods += goodsNum;
      }

    }
  } else if (ship[id].status == Ship::kIdle) {

  }
}

void RemoveExpiredGoods() {
  while (goods_removed < goods_added && current_time - goods[goods_removed].occur_time >= kGoodsDuration) {
    auto &g = goods[goods_removed];
    if (g.status == Goods::kWaiting) {
      g.status = Goods::kExpired;
//      RemoveColor(g.id);
      goods_waiting--;
      goods_waiting_value -= g.value;
      goods_expired++;
      goods_expired_value += g.value;
    } else if (g.status == Goods::kTargeted) {
      g.status = Goods::kExpired;
      robot[g.robot_id].Refresh();
//      RemoveColor(g.id);
      goods_waiting--;
      goods_waiting_value -= g.value;
      goods_expired++;
      goods_expired_value += g.value;

    } else if (g.status == Goods::kCaptured) {
//      std::cerr << "Captured Goods should not in list" << std::endl;
    } else {
      std::cerr << "BIG ERROR, WRONG GOODS" << std::endl;
    }
    g.DeallocateMemory();
    goods_removed++;
  }
//  while (!goods_waiting.empty()
//      && goods_waiting.front()->status == Goods::kExpired) {
//    goods_waiting.pop_front();
//  }
}

void UpdateOutput() {
  RemoveExpiredGoods();
  for (int i = 0; i < kRobotCount; i++) {
    RobotLoadAndUnload(i);
  }
  update_robot_goods::ArrangeAllRobotAndGoods();
  update_robot_berth::ArrangeAllRobotAndBerth();
  for (int i = 0; i < kRobotCount; i++) {
    UpdateRobotMoveDir(i);
  }
  for (int cnt = 0; cnt < 100 && CheckMoveAndMakeValid(); cnt++) {
//    fprintf(stderr, RED("CheckMoveAndMakeValid\n"));
  }
  for (auto &i : robot) {
    if (i.dir != kStay) i.PrintMove();
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
//    ShowAll();
    PrintOK();
  }
//  }
  return 0;
}