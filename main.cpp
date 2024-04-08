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
std::allocator<Grid> Goods::grid_allocator{};
std::vector<Robot> robot;
Map map;
std::vector<Ship> ship;
std::vector<Berth> berth;
std::vector<Goods> goods;
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

void ShowAll() {
  fprintf(stderr, "Berth: ");
  for (int i = 0; i < kBerthCount; i++) {
    fprintf(stderr, "%d:%d ", i, berth[i].saved_goods);
  }
  fprintf(stderr, "\n");
}

namespace init {
size_t map_hash = 0;
int map_id;
void InitInputMap() {
  std::string str;
  size_t h=0;
  for (auto &line : map.char_grid) {
    std::cin >> str;
    h^=std::hash<std::string> {}(str);
    std::copy(str.begin(), str.end(), line.begin());
  }
  map_hash = h;
  map.Init();
}

void InitAllBerth() {
  for (auto &b : berth) {
    int id, x, y, speed;
    std::cin >> id >> x >> y >> speed;
    b.id = id;
    b.minx = x;
    while (Map::IsInMap(b.minx-1,y) && map.char_grid[b.minx-1][y] == Map::kBerth) {
      b.minx--;
    }
    b.miny = y;
    while (Map::IsInMap(x,b.miny-1) && map.char_grid[x][b.miny-1] == Map::kBerth) {
      b.miny--;
    }
    b.maxx = x;
    while (Map::IsInMap(b.maxx+1,y) && map.char_grid[b.maxx+1][y] == Map::kBerth) {
      b.maxx++;
    }
    b.maxy = y;
    while (Map::IsInMap(x,b.maxy+1) && map.char_grid[x][b.maxy+1] == Map::kBerth) {
      b.maxy++;
    }
    b.loading_speed = speed;
    b.saved_goods = 0;
    std::vector<Position> area;
    for (int i = b.minx; i <= b.maxx; i++) {
      for (int j = b.miny; j <= b.maxy; j++) {
        area.emplace_back(i, j);
      }
    }
    LandBfs(area, b.dis, b.pre, map.robot.grid);
  }

}


bool InitInput() {
  InitInputMap();
  InitAllBerth();
  std::cin >> Ship::capacity;
  return ReadOK();
}

}

void Init() {
  using namespace init;
  if (InitInput()) {
//    std::cerr << "Init success" << std::endl;
    PrintOK();
  } else {
    std::cerr << "Init failed" << std::endl;
  }
}

namespace input {

void UpdateGoods() {
//  fprintf(stderr, "UpdateGoods");
  int x, y, value;
  std::cin >> x >> y >> value;

  goods[goods_added].id = goods_added;
  goods[goods_added].position = Position(x, y);
  goods[goods_added].value = value;
  goods[goods_added].occur_time = current_time;
  goods[goods_added].robot_id = -1;
  goods[goods_added].berth_id = -1;
  [&](Goods &g) {
    g.AllocateMemory();
    LandBfs({g.position}, *g.dis, *g.pre, map.robot.grid);
    g.status = Goods::kOnLand;

  }(goods[goods_added]);
  goods_added++;
}

void InputRobot(Robot &r) {
  int carry_goods;
  int x, y;
  int status;
  std::cin >> carry_goods >> x >> y >> status;
  r.position = Position(x, y);
  if (status) { // robot is running
    if (carry_goods) {
      r.status = Robot::kGoingToUnload;
    } else { // not carry goods
      r.status = Robot::kGoingToLoad;
    }
  } else { // robot is breakdown
    fprintf(stderr, RED("Robot %d breakdown\n"), r.id);
    if (carry_goods) {
      r.status = Robot::kBreakdownWithGoods;
    } else { // not carry goods
      r.status = Robot::kBreakdown;
    }
  }
}
void InputShip(Ship &s) {
  int status;
  int berth_id;
  std::cin >> status >> berth_id;
  if (status == 1) {
    if (berth_id == -1) {
      s.status = Ship::kAtEnd;
    } else {
      s.status = Ship::kAtBerth;
      s.nowBerth = berth_id;
    }
  } else if (status == 2) {
    s.status = Ship::kIdle;
  }

}

}

bool Input() {
  using namespace input;
  std::cin >> current_time >> current_value;
  int temp_goods;
  std::cin >> temp_goods;
  for (int i = 0; i < temp_goods; i++) {
    UpdateGoods();
  }
  for (auto &r: robot) {
    InputRobot(r);
  }
  for (auto &s: ship) {
    InputShip(s);
  }
  return ReadOK();
}

namespace update_robot_goods {

double GetGoodsValue(int id, int x, int y) {
  auto time_cost =
      ((*goods[id].dis)[x][y] * 1.0 + berth[goods[id].berth_id].dis[goods[id].position.x][goods[id].position.y] + 4);
  auto passed_time = (current_time - goods[id].occur_time + 2.0);
  auto res = std::exp(goods[id].value * 1./ time_cost) * std::log(1 + 1.0* passed_time / kGoodsDuration);
  if (goods[id].value<80) res*=0.9;
  return res;
}

int RobotFindGoods(int x, int y) {
  //  fprintf(stderr, "RobotFindGoods x: %d y: %d\n", x, y);
  int goods_id = -1;
  double max_value = -1;
  for (int i = goods_removed; i < goods_added; i++) {
    auto &g = goods[i];
    if (g.status != Goods::kOnLand) continue;
    auto &dis = *g.dis;
    if (current_time - g.occur_time + dis[x][y] >= kGoodsDuration) continue;
    double v = GetGoodsValue(g.id, x, y);
    if (g.robot_id!=-1) {
      double old_v = GetGoodsValue(g.id, robot[g.robot_id].position.x, robot[g.robot_id].position.y);
      if (v > old_v && v > max_value) {
        goods_id = g.id;
        max_value = v;
      }
    } else {
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
    goods[robot[id].goods_id].status = Goods::kOnLand;
    goods[robot[id].goods_id].robot_id = -1;
    robot[id].goods_id = -1;
  }
  int goods_id = RobotFindGoods(robot[id].position.x, robot[id].position.y);
  if (goods_id == -1) {
    //    fprintf(stderr, "Robot %d find no goods available.\n", id);
    return false;
  }
  if (goods[goods_id].robot_id != -1) {
    robot[goods[goods_id].robot_id].Refresh();
    flag = true;
  }
  robot[id].goods_id = goods_id;
  goods[goods_id].robot_id = id;
  return flag;
}

void ArrangeAllRobotAndGoods() {
  bool flag = false;
  do {
    flag = false;
    for (auto r : robot) {
      if (r.status == Robot::kGoingToLoad) {
        flag |= AllocateGoodsToRobot(r.id);
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
  return std::pow(1.0 / (berth[id].dis[x][y] + 5) ,1)* (1.5 - 1.0 * berth[id].saved_goods / Ship::capacity);
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

void AllocateBerthToRobot(Robot &r) {
  int berth_id = RobotFindBerth(r.position.x, r.position.y);
//    int berth_id = id;
  if (berth_id == -1) {
    //    fprintf(stderr, "Robot %d find no berth available.\n", id);
    return;
  }
  r.status = Robot::kGoingToUnload;
  r.berth_id = berth_id;

}

void ArrangeAllRobotAndBerth() {
  for (auto &r : robot) {
    if (r.status == Robot::kGoingToUnload) {
      AllocateBerthToRobot(r);
    }
  }
}

}

void RobotLoadAndUnload(Robot &r) {
  using namespace update_robot_goods;
  using namespace update_robot_berth;
  int &x = r.position.x;
  int &y = r.position.y;
  if (r.status == Robot::kGoingToLoad) {
    if (r.goods_id == -1) {
      return;
    }
    if ((*goods[r.goods_id].pre)[x][y] == kStay) { // load
      if (r.position != goods[r.goods_id].position
          || goods[r.goods_id].status != Goods::kOnLand) {
        fprintf(stderr, RED("Robot find no goods there,\n"));
        r.Refresh();
      }
      r.PrintLoad();
      r.status = Robot::kGoingToUnload;

    }
  } else if (r.status == Robot::kGoingToUnload) {
    if (r.berth_id == -1) {
//      fprintf(stderr, "robot:%d berth_id == -1\n", id);
      return;
    }
    if (berth[r.berth_id].pre[x][y] == kStay) {
      if (!berth[r.berth_id].IsInArea(x, y)) {
        fprintf(stderr, RED("Robot %d (%d, %d) find no berth (id:%d) there.\n"), r.id, x, y, r.berth_id);
        return;
      }
      r.PrintUnload();
      berth[r.berth_id].saved_goods++;
      goods[r.goods_id].Update(Goods::kOnBerth);
      r.Refresh();
    }
  }
}

void UpdateRobotMoveDir(Robot &r) {
  using namespace update_robot_goods;
  using namespace update_robot_berth;
  int &x = r.position.x;
  int &y = r.position.y;
  r.dir = kStay;
  if (r.status == Robot::kGoingToLoad) {
    if (r.goods_id == -1) {
      return;
    }
    int dir = (*goods[r.goods_id].pre)[x][y];
    if (dir != kStay) {
      r.dir = (kTurnBack[dir]);
    }
  } else if (r.status == Robot::kGoingToUnload) {
    if (r.berth_id == -1) {
      //      fprintf(stderr, "robot:%d berth_id == -1\n", id);
      //      AllocateBerthToRobot(id);
      return;
    }
    int dir = berth[r.berth_id].pre[x][y];
    if (dir != kStay) {
      r.dir = (kTurnBack[dir]);
    }
  }
}

bool CheckMoveAndMakeValid() {
  bool flag = false;
  for (auto &ri: robot) {
    for (auto &rj: robot) {
      if (ri.id == rj.id) continue;
      bool i_move = ri.dir != kStay;
      bool j_move = rj.dir != kStay;
      static const int *kTurn[3] = {kTurnRight, kTurnLeft, kTurnBack};
      if (!i_move && !j_move) continue;
      if (i_move && j_move) {
        auto new_pos_i = ri.position.Move(ri.dir);
        auto new_pos_j = rj.position.Move(rj.dir);
        if (new_pos_i == new_pos_j
            || (ri.position == new_pos_j && rj.position == new_pos_i)) { // type 1->' '<-2
          bool change = false;
          for (auto &k : kTurn) {
            auto new_pos = ri.position.Move(k[ri.dir]);
            if (map.robot.IsReachable(new_pos.x, new_pos.y)) {
              ri.dir = k[ri.dir];
              change = true;
              break;
            }
          }
          if (!change) {
            ri.dir = kStay;
          }
          flag = true;
        }
      } else {
        if (i_move) {
          auto new_pos_i = ri.position.Move(ri.dir);
          if (new_pos_i == rj.position) {
            bool change = false;
            for (auto &k : kTurn) {
              auto new_pos = ri.position.Move(k[ri.dir]);
              if (map.robot.IsReachable(new_pos.x, new_pos.y)) {
                ri.dir = k[ri.dir];
                change = true;
                break;
              }
            }
            if (!change) {
              ri.dir = kStay;
            }
            flag = true;
          }
        } else { // j_move
          auto new_pos_j = rj.position.Move(rj.dir);
          if (new_pos_j == ri.position) {
            bool change = false;
//            for (auto &k : kTurn) {
//              if (map.IsEmpty(rj.position.Move(k[rj.dir]))) {
//                rj.dir = k[rj.dir];
//                change = true;
//                break;
//              }
//            }
            if (!change) {
//              rj.dir = kStay;
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
        if (ship[id].nowGoods > ship[id].capacity * 80/ 100
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
    if (g.status == Goods::kOnLand) {
      if (g.robot_id != -1) {
        robot[g.robot_id].Refresh();
      }
      g.Update(Goods::kExpired);
    } else if (g.status == Goods::kOnRobot) {
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
  for (auto &r: robot) {
    RobotLoadAndUnload(r);
  }
  update_robot_goods::ArrangeAllRobotAndGoods();
  update_robot_berth::ArrangeAllRobotAndBerth();
  for (auto &r: robot) {
    UpdateRobotMoveDir(r);
  }
  for (int cnt = 0; cnt < 100 && CheckMoveAndMakeValid(); cnt++) {
//    fprintf(stderr, RED("CheckMoveAndMakeValid\n"));
  }
  for (auto &i : robot) {
    if (i.dir != kStay) i.PrintMove();
  }

  // TODO: update ship
//  if (current_time == 1) {
//    for (int i = 0; i < kShipCount; i++) {
//      berth[i].have_ship++;
//      ship[i].dir = i;
//      ship[i].PrintShip();
//      ship[i].status = Ship::kGoBack;
//      //fprintf(stderr, "goto %d\n", i);
//    }
//  } else {
//    for (int i = 0; i < kShipCount; i++) {
//      UpdateShip(i);
//    }
//  }
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