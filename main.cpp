#include <iostream>
#include "robot.h"
#include "constant.h"
#include "map.h"
#include "ship.h"
#include "berth.h"

Robot robot[kRobotCount];
Map map;
Ship ship[kShipCount];
Berth berth[kBerthCount];
Goods goods[kGoodsMaxAdded];
int current_time = 0;
int current_value = 0;
int current_goods_added = 0;
int current_goods_removed = 0;
void PrintOK() {
  std::cout << "OK" << std::endl;
  std::cout.flush();
}
bool ReadOK() {
    std::string s;
    std::cin >> s;
    return s == "OK";
}

namespace init {

void InitMap() {
  for (auto &line : map.grid) {
    std::cin >> line;
  }
}

void InitRobot() {
  int idx = 0;
  for (int i = 0; i < kN; i++) {
    for (int j = 0; j < kN; j++) {
      if (map.grid[i][j] == 'A') {
        robot[i * kN + j].id = idx++;
        robot[i * kN + j].position = Position(i, j);
        robot[i * kN + j].status = kIdle;
      }

    }
  }
  //TODO: init robot, it is wrong!
}

void InitBerth() {
  int id, x, y, time, speed;
  for (auto &i : berth) {
    std::cin >> id >> x >> y >> time >> speed;
    i.id = id;
    i.left_up_position = Position(x, y);
    i.transport_time = time;
    i.loading_speed = speed;
  }
}

void InitShip() {
  std::cin >> Ship::capacity;
}

bool InitInput() {
  InitMap();
  InitRobot();
  InitBerth();
  InitShip();
  return ReadOK();
}

}

void Init() {
  using namespace init;
  if(InitInput()){
    std::cerr<<"Init success"<<std::endl;
    PrintOK();
  }
  else{
    std::cerr<<"Init failed"<<std::endl;
  }
}

namespace input {

void AddGoods() {
  int x, y, value;
  std::cin >> x >> y >> value;
  std::cerr<<"x: "<<x<<" y: "<<y<<" value: "<<value<<std::endl;
  goods[current_goods_added].position = Position(x, y);
  goods[current_goods_added].value = value;
  goods[current_goods_added].occur_time = current_time;
//    map.goods[Position(x, y)] = &goods[current_goods_added];
    current_goods_added++;


}

void UpdateRobot(int id) {
  int carry_goods;
  int x, y;
  int status;
  std::cin >> carry_goods >> x >> y >> status;
  std::cerr<<"carry_goods: "<<carry_goods<<" x: "<<x<<" y: "<<y<<" status: "<<status<<std::endl;
  //TODO: update robot
}

void UpdateShip(int id) {
  int status;
  int berth_id;
  std::cin >> status >> berth_id;
    std::cerr<<"status: "<<status<<" berth_id: "<<berth_id<<std::endl;
  //TODO: update ship
}

}

bool Input() {
  std::cerr<<"Input"<<std::endl;
  using namespace input;
  std::cin >> current_time >> current_value;
  std::cerr<<"current_time: "<<current_time<<std::endl;
  int temp_goods;
  std::cin >> temp_goods;
  std::cerr<<"temp_goods: "<<temp_goods<<std::endl;
  for (int i = 0; i < temp_goods; i++) {
    AddGoods();
  }
  std::cerr<<"AddGoods success"<<std::endl;
  for (int i = 0; i < kRobotCount; i++) {
    UpdateRobot(i);
  }
    std::cerr<<"UpdateRobot success"<<std::endl;
  for (int i = 0; i < kShipCount; i++) {
    UpdateShip(i);
  }
    std::cerr<<"UpdateShip success"<<std::endl;
  return ReadOK();
}
void UpdateOutput() {
  std::cerr<<"current_time: "<<current_time<<std::endl;
  while (current_goods_removed < current_goods_added) {
    while (current_time >= goods[current_goods_removed].occur_time + kGoodsDuration) {
      map.goods[goods[current_goods_removed].position] = nullptr;
      current_goods_removed++;
    }
  }
  //TODO: update
  PrintOK();
}

int main() {
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
    std::ios::sync_with_stdio(false);
  Init();
  while (Input()) {
    std::cerr<<"Input success"<<std::endl;
//    UpdateOutput();
    PrintOK();
  }
  return 0;
}