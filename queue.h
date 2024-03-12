//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__QUEUE_H_
#define HARBOR_MANAGER__QUEUE_H_
#include <stdexcept>
template<typename T, int ReservedSize>
class Queue {
  T data[ReservedSize];
  int head = 0;
  int tail = 0;
 public:
  bool empty() {
    return head == tail;
  }
  bool full() {
    return (tail + 1) % ReservedSize == head;
  }
  void push(const T &value) {
    if (full()) {
      throw std::runtime_error("Queue is full");
    }
    data[tail] = value;
    tail = (tail + 1) % ReservedSize;
  }
  T pop() {
    if (empty()) {
      throw std::runtime_error("Queue is empty");
    }
    T value = data[head];
    head = (head + 1) % ReservedSize;
    return value;
  }
  T& front() {
    if (empty()) {
      throw std::runtime_error("Queue is empty");
    }
    return data[head];
  }
  T& back() {
    if (empty()) {
      throw std::runtime_error("Queue is empty");
    }
    return data[(tail - 1 + ReservedSize) % ReservedSize];
  }
  int size() {
    return (tail - head + ReservedSize) % ReservedSize;
  }
  void clear() {
    head = tail = 0;
  }
  T& operator[](int index) {
    return data[(head + index) % ReservedSize];
  }
  void erase(int index) {
    for (int i = index; i < size() - 1; i++) {
      data[(head + i) % ReservedSize] = data[(head + i + 1) % ReservedSize];
    }
    tail = (tail - 1 + ReservedSize) % ReservedSize;
  }
  void insert(int index, const T &value) {
    if (full()) {
      throw std::runtime_error("Queue is full");
    }
    for (int i = size(); i > index; i--) {
      data[(head + i) % ReservedSize] = data[(head + i - 1) % ReservedSize];
    }
    data[(head + index) % ReservedSize] = value;
    tail = (tail + 1) % ReservedSize;
  }
  void push_front(const T &value) {
    if (full()) {
      throw std::runtime_error("Queue is full");
    }
    head = (head - 1 + ReservedSize) % ReservedSize;
    data[head] = value;
  }
};

#endif //HARBOR_MANAGER__QUEUE_H_
