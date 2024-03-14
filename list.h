//
// Created by xinya on 2024/3/14.
//

#ifndef HARBOR_MANAGER__LIST_H_
#define HARBOR_MANAGER__LIST_H_

#include <stdexcept>
#include <array>
template<typename T, int kMaxN>
class List {
  struct Node {
    T data;
    Node *prev;
    Node *next;
  };
  Node *head;
  Node *tail;
  Node *pool;
  Node *free;
  Node nodes[kMaxN];
  int size;
 public:
  typedef Node* iterator;
  List() {
    pool = nodes;
    free = pool;
    head = tail = nullptr;
    size = 0;
    for (int i = 0; i < kMaxN; i++) {
      nodes[i].next = free;
      free = &nodes[i];
    }
  }
  Node* push_back(const T &value) {
    if (free == nullptr) {
      throw std::runtime_error("List is full");
    }
    Node *node = free;
    free = free->next;
    node->data = value;
    node->prev = tail;
    node->next = nullptr;
    if (tail) {
      tail->next = node;
    } else {
      head = node;
    }
    tail = node;
    size++;
    return node;
  }
  void erase(Node *&node) {
    if (node->prev) {
      node->prev->next = node->next;
    } else {
      head = node->next;
    }
    if (node->next) {
      node->next->prev = node->prev;
    } else {
      tail = node->prev;
    }
    node->next = free;
    free = node;
    size--;
    node=nullptr;
  }
  void clear() {
    head = tail = nullptr;
    size = 0;
    free = pool;
    for (int i = 0; i < kMaxN; i++) {
      nodes[i].next = free;
      free = &nodes[i];
    }
  }
  Node *begin() {
    return head;
  }
  Node *end() {
    return nullptr;
  }
  int Size() {
    return size;
  }
  T &front() {
    return head->data;
  }
  T &back() {
    return tail->data;
  }
  bool empty() {
    return size == 0;
  }
  void pop_front() {
    erase(head);
  }
  void pop_back() {
    erase(tail);
  }
  void insert(Node *pos, const T &value) {
    if (free == nullptr) {
      throw std::runtime_error("List is full");
    }
    Node *node = free;
    free = free->next;
    node->data = value;
    node->prev = pos->prev;
    node->next = pos;
    if (pos->prev) {
      pos->prev->next = node;
    } else {
      head = node;
    }
    pos->prev = node;
    size++;
  }
  Node* push_front(const T &value) {
    if (free == nullptr) {
      throw std::runtime_error("List is full");
    }
    Node *node = free;
    free = free->next;
    node->data = value;
    node->prev = nullptr;
    node->next = head;
    if (head) {
      head->prev = node;
    } else {
      tail = node;
    }
    head = node;
    size++;
    return node;
  }
  T &operator[](int index) {
    Node *node = head;
    for (int i = 0; i < index; i++) {
      node = node->next;
    }
    return node->data;
  }
  void erase(int index) {
    Node *node = head;
    for (int i = 0; i < index; i++) {
      node = node->next;
    }
    erase(node);
  }
  void insert(int index, const T &value) {
    Node *node = head;
    for (int i = 0; i < index; i++) {
      node = node->next;
    }
    insert(node, value);
  }


};

#endif //HARBOR_MANAGER__LIST_H_
