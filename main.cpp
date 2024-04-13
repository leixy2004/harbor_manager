#include <iostream>
//#include <cstdio>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <array>
#include <cmath>
#include <unordered_map>
//#include <list>
#include <map>
#include <chrono>
//#include <format>
#include "robot.h"
#include "constant.h"
#include "map.h"
#include "ship.h"
#include "berth.h"
#include "goods.h"
#include "bfs.h"
#include "seller.h"
#include "terminal.h"
std::allocator<Grid<int>> Goods::grid_allocator{};
std::vector<Robot> robot;
Map map;
std::vector<RobotSeller> robot_seller;
std::vector<ShipSeller> ship_seller;
std::vector<Terminal> terminal;
std::vector<Ship> ship;
std::vector<Berth> berth;
std::vector<Goods> goods;
std::map<Position, int> goods_map;
int Ship::capacity=0;
Goods::Monitor Goods::monitor{};
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
  for (auto &b : berth) {
    b.Show();
  }
  for (auto &r : robot) {
    r.Show();
  }
  for (auto &g : goods) {
    g.Show();
  }
  fprintf(stderr, "\n");
}

namespace init {
size_t map_hash = 0;
int map_id;
void InitInputMap() {
  std::string str;
  size_t h = 0;
  for (auto &line : map.char_grid) {
    std::cin >> str;
    h ^= std::hash<std::string>{}(str);
    std::copy(str.begin(), str.end(), line.begin());
  }
  map_hash = h;
  map.Init();
}

void InitAllFixedObject() {
  for (int x = 0; x < kN; x++) {
    for (int y = 0; y < kN; y++) {
      if (map.char_grid[x][y] == Map::kRobotSeller) {
        robot_seller.emplace_back(robot_seller.size(), x, y);
      } else if (map.char_grid[x][y] == Map::kShipSeller) {
        ship_seller.emplace_back(ship_seller.size(), x, y);
      } else if (map.char_grid[x][y] == Map::kTerminal) {
        terminal.emplace_back(terminal.size(), x, y);
        GetShipNaviInfo(map, {terminal.back().position}, terminal.back().ship_navi);
      }
    }
  }
}
std::ofstream file("D:\\Downloads\\WindowsRelease\\mapout.txt");

void InitBerth() {
  int id, x, y, speed;
  std::cin >> id >> x >> y >> speed;
  auto &b = berth[id];
  b.id = id;
  b.minx = x;
  b.miny = y;
  b.maxx = x;
  b.maxy = y;
  file<<"Berth"<<b.id<<std::endl;
  while (Map::IsInMap(b.minx - 1, y)
      && map.char_grid[b.minx - 1][y] == Map::kBerth)
    b.minx--;
  while (Map::IsInMap(x, b.miny - 1)
      && map.char_grid[x][b.miny - 1] == Map::kBerth)
    b.miny--;
  while (Map::IsInMap(b.maxx + 1, y)
      && map.char_grid[b.maxx + 1][y] == Map::kBerth)
    b.maxx++;
  while (Map::IsInMap(x, b.maxy + 1)
      && map.char_grid[x][b.maxy + 1] == Map::kBerth)
    b.maxy++;

  b.margin.minx = b.minx - 1;
  b.margin.miny = b.miny - 1;
  b.margin.maxx = b.maxx + 1;
  b.margin.maxy = b.maxy + 1;
  while (Map::IsInMap(b.margin.minx - 1, y)
      && map.char_grid[b.margin.minx - 1][y] == Map::kBerthMargin)
    b.margin.minx--;
  while (Map::IsInMap(x, b.margin.miny - 1)
      && map.char_grid[x][b.margin.miny - 1] == Map::kBerthMargin)
    b.margin.miny--;
  while (Map::IsInMap(b.margin.maxx + 1, y)
      && map.char_grid[b.margin.maxx + 1][y] == Map::kBerthMargin)
    b.margin.maxx++;
  while (Map::IsInMap(x, b.margin.maxy + 1)
      && map.char_grid[x][b.margin.maxy + 1] == Map::kBerthMargin)
    b.margin.maxy++;

  b.loading_speed = speed;
  b.saved_goods = 0;

  LandBfs([&]() {
            std::vector<Position> area;
            for (int i = b.minx; i <= b.maxx; i++) {
              for (int j = b.miny; j <= b.maxy; j++) {
                area.emplace_back(i, j);
              }
            }
            return area;
          }(),
          b.dis, b.pre, map.robot.grid);


  GetShipNaviInfo(map,[&]() {
            std::vector<Position> dest;
            dest.emplace_back(x, y);
            return dest;
          }(),berth[id].ship_navi);
  for (int i=0;i<kN;i++) {
    for (int j=0;j<kN;j++) {
      file<<std::setw(3)<<b.dis[i][j]%1000<<" ";
    }
    file<<std::endl;
  }
  file<<std::endl;
  for (int i=0;i<kN;i++) {
    for (int j=0;j<kN;j++) {
      file<<b.pre[i][j]<<" ";
    }
    file<<std::endl;
  }
  file<<std::endl;
}

bool InitInput() {
  InitInputMap();
  fprintf(stderr,"map init\n");
  InitAllFixedObject();
  fprintf(stderr,"obj init\n");
  int BerthCount;
  std::cin >> BerthCount;
  berth.resize(BerthCount);
  for (int i = 0; i < BerthCount; i++) {
    InitBerth();
  }
  fprintf(stderr,"berth init\n");
  std::cin >> Ship::capacity;
  return ReadOK();
}

}

void Init() {
  using namespace init;
  if (InitInput()) {
//    std::cerr << "Init success" << std::endl;
file.close();
    PrintOK();
  } else {
    std::cerr << "Init failed" << std::endl;
  }
}

namespace input {

void InputGoods() {
  fprintf(stderr, "InputGoods\n");
  int x, y, value;
  std::cin >> x >> y >> value;
//  fprintf(stderr, "Goods: (%d, %d) value: %d\n", x, y, value);
  if (value == 0) { // delete
    fprintf(stderr,"DeleteGoods\n");
    auto &g = goods[goods_map[Position(x, y)]];
    if (g.status == Goods::kOnLand) {
      if (g.robot_id != -1 && robot[g.robot_id].goods_id == g.id) {
        robot[g.robot_id].Refresh();
        g.robot_id = -1;
      }
      g.Update(Goods::kExpired);
    } else if (g.status == Goods::kOnRobot) {

    }
    goods_map.erase(Position(x, y));
    g.DeallocateMemory();
  } else {
//    fprintf(stderr, "0:Goods: (%d, %d) value: %d\n", x, y, value);
    fprintf(stderr,"Tag1 : AddGoods\n");
    goods.emplace_back(goods.size(), x, y, value, current_time);
    fprintf(stderr,"Tag2 : AddGoods\n");

//    fprintf(stderr, "1:Goods %d: (%d, %d) value: %d\n", goods.back().id, x, y, value);
    auto &g = goods.back();
    fprintf(stderr,"Tag3 : AddGoods\n");

//    fprintf(stderr, "Goods %d: (%d, %d) value: %d\n", g.id, x, y, value);
    goods_map[g.position] = g.id;
    fprintf(stderr,"Tag4 : AddGoods\n");

    g.AllocateMemory();
    fprintf(stderr,"Tag5 : AddGoods\n");

    if (g.dis==nullptr) {
      fprintf(stderr, "AllocateMemory failed\n");
    }
    LandBfs({g.position}, *g.dis, *g.pre, map.robot.grid);
    fprintf(stderr,"Tag6 : AddGoods\n");

//    fprintf(stderr, "BFS:Goods %d: (%d, %d) value: %d\n", g.id, x, y, value);
    g.Update(Goods::kOnLand);
    fprintf(stderr,"Tag7 : AddGoods\n");
  }
}

void InputRobot() {
  fprintf(stderr, "InputRobot");
  int carry_goods;
  int id, x, y;
  std::cin >> id >> carry_goods >> x >> y;
  auto &r = robot[id];
  r.position = Position(x, y);
  if (carry_goods) {
    r.status = Robot::kGoingToUnload;
  } else { // not carry goods
    r.status = Robot::kGoingToLoad;
  }
}
void InputShip() {
  fprintf(stderr, "InputShip");

  int ship_id;
  int status;

  std::cin >> ship_id;
  auto &s = ship[ship_id];
  std::cin >> s.nowGoods >> s.position.x>>s.position.y>>s.dir>>status;
  if(status == 1){
    s.status = Ship::kRecover;
  }
  else if(status == 2){
    s.status = Ship::kLoad;
    s.aim = -1;
  }
  else{
    if(s.status==Ship::kLoad)s.status=Ship::kNormal;
  }
}
}
bool Input() {
  using namespace input;
  std::cin >> current_time >> current_value;
  int goods_changed_count, robot_count, ship_count;
  std::cin >> goods_changed_count;
  for (int i = 0; i < goods_changed_count; i++) {
    InputGoods();
  }
  fprintf(stderr,"finished ImportGoods\n");
  std::cin >> robot_count;
  if (robot.size() < robot_count) {
    robot.resize(robot_count);
  }
  for (int _ = 0; _ < robot.size(); _++) {
    InputRobot();
  }
    fprintf(stderr,"finished ImportRobot\n");
  std::cin >> ship_count;
  if (ship.size() < ship_count) {
    ship.resize(ship_count);
  }
  for (int _ = 0; _ < ship.size(); _++) {
    InputShip();
  }
    fprintf(stderr,"finished ImportShip\n");

  return ReadOK();
}


namespace update_robot_goods {

double GetGoodsValue(int id, int x, int y) {
  auto time_cost =
      ((*goods[id].dis)[x][y] * 1.0 + berth[goods[id].berth_id].dis[goods[id].position.x][goods[id].position.y] + 4);
  auto passed_time = (current_time - goods[id].occur_time + 2.0);
  auto res = std::exp(goods[id].value * 1. / time_cost) * std::log(1 + 1.0 * passed_time / kGoodsDuration);
  if (goods[id].value < 80) res *= 0.9;
  return res;
}

int RobotFindGoods(int x, int y) {
    fprintf(stderr, "RobotFindGoods x: %d y: %d\n", x, y);
  int goods_id = -1;
  double max_value = -1;
  for (auto &g : goods) {
    if (g.status != Goods::kOnLand) continue;
    auto &dis = *g.dis;
    if (current_time - g.occur_time + dis[x][y] >= kGoodsDuration) continue;
    double v = GetGoodsValue(g.id, x, y);
    if (g.robot_id != -1) {
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
  fprintf(stderr, "RobotFindGoods goods_id: %d\n", goods_id);
  return goods_id;
}

bool AllocateGoodsToRobot(int id) {
    fprintf(stderr, "AllocateGoodsToRobot id: %d\n", id);
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
void BuyRobot(int x,int y) {
  int id=-1;
  for (auto s:robot_seller) {
    if (id==-1 || s.position.Distance({x,y})<robot_seller[id].position.Distance({x,y})) {
      id=s.id;
    }
  }
  if (current_value<kRobotPrice) {
    return;
  }
  current_value-=kRobotPrice;
  robot_seller[id].PrintBuy();
  robot.emplace_back(robot.size(),robot_seller[id].position.x,robot_seller[id].position.y);
}
void BuyShip(int x,int y) {
  int id=-1;
  for (auto s:ship_seller) {
    if (id==-1 || s.position.Distance({x,y})<ship_seller[id].position.Distance({x,y})) {
      id=s.id;
    }
  }
  if (current_value<kShipPrice) {
    return;
  }
  current_value-=kShipPrice;
  ship_seller[id].PrintBuy();
  ship.emplace_back(ship.size(),ship_seller[id].position.x,ship_seller[id].position.y);
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
  return std::pow(1.0 / (berth[id].dis[x][y] + 5), 1) * (1.5 - 1.0 * berth[id].saved_goods / Ship::capacity);
}

int RobotFindBerth(int x, int y) {
  int berth_id = -1;
  double max_value = -1;
  for (auto &b : berth) {
    if (b.dis[x][y] >= kN * kN) continue;
    double v = GetBerthValue(b.id, x, y);
    if (v > max_value) {
      berth_id = b.id;
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
  fprintf(stderr, "RobotLoadAndUnload\n");
  using namespace update_robot_goods;
  using namespace update_robot_berth;
  int &x = r.position.x;
  int &y = r.position.y;
  if (r.status == Robot::kGoingToLoad) {
    fprintf(stderr, "Robot %d going to load\n", r.id);
    if (r.goods_id == -1) {
      return;
    }
    if (goods[r.goods_id].status != Goods::kOnLand) {
      fprintf(stderr, RED("Robot %d find no goods there.\n"), r.id);
      r.Refresh();
      return;
    }
    if (goods[r.goods_id].pre==nullptr) {
      fprintf(stderr, "goods[%d].pre==nullptr\n", r.goods_id);
      return;
    }
    if ((*goods[r.goods_id].pre)[x][y] == kStay) { // load
      fprintf(stderr,"stay\n");
      if (r.position != goods[r.goods_id].position
          || goods[r.goods_id].status != Goods::kOnLand) {
        fprintf(stderr, RED("Robot find no goods there,\n"));
        r.Refresh();
      }
      r.PrintLoad();
      r.status = Robot::kGoingToUnload;

    }
  } else if (r.status == Robot::kGoingToUnload) {
    fprintf(stderr, "Robot %d going to unload\n", r.id);
    if (r.berth_id == -1) {
      fprintf(stderr, "robot:%d berth_id == -1\n", r.id);
      return;
    }
    if (berth[r.berth_id].pre[x][y] == kStay) {
      fprintf(stderr,"stay\n");
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
  for (auto &ri : robot) {
    for (auto &rj : robot) {
      if (ri.id == rj.id) continue;
      bool i_move = ri.dir != kStay;
      bool j_move = rj.dir != kStay;
      if (map.robot.grid[ri.position.x][ri.position.y] == kWay) {
        i_move = false;
      }
        if (map.robot.grid[rj.position.x][rj.position.y] == kWay) {
            j_move = false;
        }
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
              rj.dir = kStay;
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
  for (auto &b : berth) {
    //if (berth[i].have_ship[current_time+b.transport_time])continue;
    //if (b.have_ship && b.saved_goods < 2 * Ship::capacity) continue;
    if (b.saved_goods - Ship::capacity + ship[id].nowGoods >= 0
        && b.saved_goods - Ship::capacity + ship[id].nowGoods < minUp) {
      minUp = b.saved_goods - Ship::capacity + ship[id].nowGoods;
      dir2 = b.id;
    }
    if (b.saved_goods > maxBerth) {
      maxBerth = b.saved_goods;
      dir1 = b.id;
    }
  }
  if (dir2 != -1)return dir2;
  return dir1;
}

int ShipFindTerminal(int id){
  auto &s=ship[id];
  int minDis = kN * kN, aim = -1;
  for (auto &t : terminal) {
    if (t.ship_navi[s.position.x][s.position.y][s.dir].dis < minDis) {
      minDis = t.ship_navi[s.position.x][s.position.y][s.dir].dis;
      aim = t.id;
    }
  }
  return aim;

}
void UpdateShip(int id) {
  auto &s = ship[id];
  if(s.status==Ship::kRecover){
    return;
  }
  else if(s.status==Ship::kToTerminal){
    auto &t=terminal[s.aim];
    if(s.position==t.position){
      s.status=Ship::kNormal;
      s.aim=-1;
    }
    else{
      int action=t.ship_navi[s.position.x][s.position.y][s.dir].action;
      if(action==ShipNaviInfo::rot0){
        s.PrintRot(0);
      }
      else if(action==ShipNaviInfo::rot1){
        s.PrintRot(1);
      }
      else if(action==ShipNaviInfo::move){
        s.PrintShip();
      }
    }
  }
  else if(s.status==Ship::kToBerth){
    auto &b=berth[s.aim];
    if(b.IsInArea(s.position.x,s.position.y)){
      s.PrintBerth();
    }
    else{
      int action=b.ship_navi[s.position.x][s.position.y][s.dir].action;
      if(action==ShipNaviInfo::rot0){
        s.PrintRot(0);
      }
      else if(action==ShipNaviInfo::rot1){
        s.PrintRot(1);
      }
      else if(action==ShipNaviInfo::move){
        s.PrintShip();
      }
    }
  }
  else if(s.status==Ship::kLoad){
    if(s.nowGoods==Ship::capacity){
      s.status=Ship::kNormal;
    }
    else{
      return;
    }
  }
  if(s.status==Ship::kNormal){
    if(s.nowGoods==Ship::capacity){
      s.aim=ShipFindTerminal(id);
      s.status=Ship::kToTerminal;
    }
    else{
      s.aim=ShipFindBerth(id);
      s.status=Ship::kToBerth;
    }
  }
}


//void RemoveExpiredGoods() {
//  while (goods_removed < goods_added && current_time - goods[goods_removed].occur_time >= kGoodsDuration) {
//    auto &g = goods[goods_removed];
//    if (g.status == Goods::kOnLand) {
//      if (g.robot_id != -1) {
//        robot[g.robot_id].Refresh();
//      }
//      g.Update(Goods::kExpired);
//    } else if (g.status == Goods::kOnRobot) {
////      std::cerr << "Captured Goods should not in list" << std::endl;
//    } else {
//      std::cerr << "BIG ERROR, WRONG GOODS" << std::endl;
//    }
//    g.DeallocateMemory();
//    goods_removed++;
//  }
////  while (!goods_waiting.empty()
////      && goods_waiting.front()->status == Goods::kExpired) {
////    goods_waiting.pop_front();
////  }
//}

void UpdateOutput() {
  fprintf(stderr, "current_time: %d\n", current_time);
//  RemoveExpiredGoods();
  for (auto &r : robot) {
    RobotLoadAndUnload(r);
  }
  fprintf(stderr,"Finished RobotLoadAndUnload\n");
//  fprintf(stderr,"load un\n");
  update_robot_goods::ArrangeAllRobotAndGoods();
  fprintf(stderr,"Finished ArrangeAllRobotAndGoods\n");
//  fprintf(stderr,"ro go un\n");
  update_robot_berth::ArrangeAllRobotAndBerth();
    fprintf(stderr,"Finished ArrangeAllRobotAndBerth\n");
//  fprintf(stderr,"ro ber un\n");

  for (auto &r : robot) {
    UpdateRobotMoveDir(r);
  }
  fprintf(stderr,"Finished UpdateRobotMoveDir\n");
//  fprintf(stderr,"mv ber un\n");

  for (int cnt = 0; cnt < 100 && CheckMoveAndMakeValid(); cnt++) {
//    fprintf(stderr, RED("CheckMoveAndMakeValid\n"));
  }
  fprintf(stderr,"Finished CheckMoveAndMakeValid\n");
//  fprintf(stderr,"checkun\n");

  for (auto &i : robot) {
    if (i.dir != kStay) i.PrintMove();
  }
    fprintf(stderr,"Finished PrintMove\n");
//  fprintf(stderr,"pmv");
  BuyRobot(100,100);
    fprintf(stderr,"Finished BuyRobot\n");
//  fprintf(stderr,"buyr");

  // TODO: update ship
//  for(auto &s:ship){
//    UpdateShip(s.id);
//  }
  BuyShip(100,100);
}

int main() {
  freopen("D:\\Downloads\\WindowsRelease\\new.txt","w",stderr);
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