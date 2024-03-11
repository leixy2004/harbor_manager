//
// Created by xinya on 2024/3/9.
//

#ifndef HARBOR_MANAGER__DEQUE_H_
#define HARBOR_MANAGER__DEQUE_H_
template<typename T, int N>
class Deque {
  int data[N];
  int head = 0, tail = 0;
 public:
  void push_back(const T &x) {
    data[tail] = x;
    tail++;
    if (tail >= N) tail -= N;
  }
  void push_front(const T &x) {
    head--;
    if (head < 0) head += N;
    data[head] = x;
  }
  T pop_back() {
    tail--;
    if (tail < 0) tail += N;
    return data[tail];
  }
  T pop_front() {
    T x = data[head];
    head++;
    if (head >= N) head -= N;
    return x;
  }
  T back() {
    return data[(tail + N - 1) % N];
  }
  T front() {
    return data[head];
  }
  bool empty() {
    return head == tail;
  }
  int size() {
    return (tail + N - head) % N;
  }
  T operator[](int i) {
    return data[(head + i) % N];
  }
  void clear() {
    head = tail = 0;
  }
};

#endif //HARBOR_MANAGER__DEQUE_H_
