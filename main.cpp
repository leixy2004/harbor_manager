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
      for (auto &id : berth) {
        if (map.dis[x][y] > id.dis[x][y]) {
          map.dis[x][y] = id.dis[x][y];
          map.pre[x][y] = id.pre[x][y];
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
  fprintf(stderr, "AddGoods x: %d y: %d value: %d\n", x, y, value);
  goods_queue.push(Goods(x, y, current_goods_added, Goods::kNone, value, current_time));
  current_goods_added++;
  goods[goods_queue.back().id] = &goods_queue.back();
  std::thread t(Bfs1,
                goods_queue.back().position,
                goods_queue.back().dis,
                goods_queue.back().pre,
                &(goods_queue.back().status),
                std::ref(map));
//  Bfs1(goods_queue.back().position, goods_queue.back().dis, goods_queue.back().pre, goods_queue.back().status);
  t.detach();
}

void InputRobot(int id) {
  int carry_goods;
  int x, y;
  int status;
  std::cin >> carry_goods >> x >> y >> status;
//  std::cerr << "carry_goods: " << carry_goods << " x: " << x << " y: " << y << " status: " << status << std::endl;
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
  std::cerr << "status: " << status << " berth_id: " << berth_id << std::endl;
  //TODO: update ship
}

}

bool Input() {
  std::cerr << "Input" << std::endl;
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
      / goods[id]->dis[x][y]
      * (current_time - goods[id]->occur_time + 1);
}

int FindGoods(int x, int y) {
  int goods_id = -1;
  double max_value = -1;
  for (int i = 0; i < goods_queue.size(); i++) {
    if (goods_queue[i].status == Goods::kWaiting) {
      fprintf(stderr, "possible goods_id: %d,dis: %d\n", goods_queue[i].id, goods_queue[i].dis[x][y]);
      if (current_time - goods_queue[i].occur_time + goods_queue[i].dis[x][y] >= kGoodsMaxAdded) continue;
      if (goods_queue[i].dis[x][y] >= kN * kN) continue;
      double v = GetGoodsValue(goods_queue[i].id, x, y);
      if (v > max_value) {
        goods_id = goods_queue[i].id;
        max_value = v;
      }
    }
  fprintf(stderr, "FindGoods x: %d y: %d -> %d\n", x, y, goods_id);
  }
  return goods_id;
}

void UpdateRobot(int id) {
  fprintf(stderr, "UpdateRobot id: %d\n", id);
  {
    int x = robot[id].position.x;
    int y = robot[id].position.y;
//    while (true) {
      bool flag=false;
      if (robot[id].status == Robot::kIdle) {
        int goods_id = FindGoods(x, y);
        if (goods_id != -1) {
          robot[id].status = Robot::kGoingToLoad;
          robot[id].goods_id = goods_id;
          goods[goods_id]->status = Goods::kTargeted;
          std::cerr << "goods_id: " << goods_id << std::endl;
        }
      } else if (robot[id].status == Robot::kGoingToLoad) {
        int dir = goods[robot[id].goods_id]->pre[x][y];
        if (dir == -1) {
          robot[id].PrintLoad();
          fprintf(stderr, "robot:%d LOAD\n", id);
          robot[id].status = Robot::kGoingToUnload;
          robot[id].dir = -1;
          goods[robot[id].goods_id]->status = Goods::kOnRobot;
        } else {
          robot[id].dir = (kInverseDir[dir]);
        }
      } else if (robot[id].status == Robot::kGoingToUnload) {
        int dir = map.pre[x][y];
        if (dir == -1) {
          robot[id].PrintUnload();
          fprintf(stderr, "robot:%d UNLOAD\n", id);
          robot[id].status = Robot::kIdle;
          robot[id].dir = -1;
          goods[robot[id].goods_id]->status = Goods::kOnBerth;
          robot[id].goods_id = -1;
          flag=true;
        } else {
          robot[id].dir = (kInverseDir[dir]);
        }
//        if (!flag) break;
//      }
    }

  }
//  std::cerr << "Robot status: " << robot[id].status << std::endl;
}

bool CheckMoveAndMakeValid() {
  //TODO
  return false;
}

void UpdateOutput() {
  while (!goods_queue.empty() && current_time - goods_queue.front().occur_time >= kGoodsMaxAdded) {
    goods_queue.pop();
    current_goods_removed++;
  }

  fprintf(stderr, "goods_queue.size(): %d\n", goods_queue.size());
  for (int i = 0; i < goods_queue.size(); i++) {
    fprintf(stderr, "now_goods[%d].status: %d\n", i, goods_queue[i].status);
  }
  while (CheckMoveAndMakeValid()) {}
  for (int i = 0; i < kRobotCount; i++) {
    UpdateRobot(i);
  }
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
  while (Input()) {
    std::cerr << "Input success" << std::endl;
    UpdateOutput();
  }
  return 0;
}