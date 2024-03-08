//
// Created by xinya on 2024/3/8.
//

#ifndef HARBOR_MANAGER__BERTH_H_
#define HARBOR_MANAGER__BERTH_H_
#include "position.h"
#include <queue>
#include "goods.h"
struct Berth {
  int id{};
int transport_time{};
int loading_speed{};
Position left_up_position{};
std::queue<Goods> goods_queue;
};

#endif //HARBOR_MANAGER__BERTH_H_
