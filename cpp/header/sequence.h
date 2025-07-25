#pragma once

#include <map>

#include "config.h"
using namespace std;
const double eps = 1e-9;
struct sequence {
  map<tick_t, pair<double, tick_t>> seq;
  pair<double, tick_t> max(tick_t x = -inf_tick) {
    auto it = seq.lower_bound(x);
    return it == seq.end() ? pair<double, tick_t>{0, 0} : it->second;
  }
  double maxt(tick_t x = -inf_tick) {
    auto it = seq.lower_bound(x);
    return it == seq.end() ? 0 : it->first;
  }
  void insert(tick_t x, pair<double, tick_t> v) {
    auto it = seq.lower_bound(x);
    if (it != seq.end() && it->second.first > v.first - eps) return;
    seq[x] = v, it = seq.find(x);
    while (it != seq.begin() && prev(it)->second.first < v.first + eps)
      seq.erase(prev(it));
  }
  bool erase(tick_t x) {
    bool res = false;
    while (seq.size() && seq.begin()->first <= x)
      seq.erase(seq.begin()), res = true;
    return res;
  }
};