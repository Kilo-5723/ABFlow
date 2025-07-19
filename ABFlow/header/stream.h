#pragma once

#include <list>
// #include <ranges>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "config.h"
#include "graph.h"
#include "sequence.h"
using namespace std;

random_device rnd;
default_random_engine eng(rnd());
uniform_int_distribution<int> gen(0, 1e9);

struct stream {
  unordered_set<vtx_t> tS, tT;
  tick_t width;
  tick_t curT;
  vtx_t curN;
  graph G;
  list<pair<tick_t, vtx_t>> S, T, V;
  unordered_map<vtx_t, pair<vtx_t, tick_t>> last;
  map<int, long long> time_layer;
  map<int, long long> gsize_layer;
  int tot = 0, pruned = 0;
  long long dctime = 0;
  // 1: brute_force
  // 2: incremental
  // 3: suffix_flow
  // 4: suffix_flow_inher
  // 5: suffix_flow_prune
  int mode;
  stream(unordered_set<vtx_t> tS, unordered_set<vtx_t> tT, tick_t w, int mode)
      : tS(tS), tT(tT), width(w), curT(0), curN(0), mode(mode) {}
  sequence result;
  bool edge_update(vtx_t u, vtx_t v, tick_t t, flow_t w, bool real_time,bool skip=false) {
    curT = real_time ? t : curT + 1;
    bool flg = result.erase(curT - width);

    if (tS.count(v)) return flg;  // ignore edges to S

    while (S.size() && S.front().first <= curT - width) {
      // cout << S.front().first << ' ' << curT - width << endl;
      if (mode >= 4) inher_pre(S.front().first);
      S.pop_front();
    }
    while (T.size() && T.front().first <= curT - width) T.pop_front();
    while (V.size() && V.front().first <= curT - width) {
      G.delv(V.front().second), V.pop_front();  // remove vertex from graph
    }
    if (tT.count(u) || tS.count(v)) return flg;  // not from source nor to sink
    auto uu = ++curN, vv = ++curN;
    V.push_back({curT, uu}), V.push_back({curT, vv});
    if (tS.count(u)) S.push_back({curT, uu});
    if (tT.count(v)) T.push_back({curT, vv});

    if (!tS.count(u) && last.count(u) && last[u].second > curT - width)
      G.adde(last[u].first, uu, inf_flow);
    if (last.count(v) && last[v].second > curT - width)
      G.adde(last[v].first, vv, inf_flow);
    G.adde(uu, vv, w);
    last[u] = {uu, curT}, last[v] = {vv, curT};

    unordered_map<vtx_t, flow_t> flow;
    if (!tT.count(v)) return flg;  // no need to update
    #ifdef comp_time
    total_time -= clock();
    #endif
    if (!skip)
    switch (mode) {
      case 1:
        flow = brute_force(toseq(S), toseq(T));
        break;
      case 2:
        flow = incremental(toseq(S), toseq(T));
        break;
      case 3:
        flow = suffix_flow(toseq(S), toseq(T));
        break;
      case 4:
        flow = suffix_flow_inher(toseq(S), toseq(T));
        break;
      case 5:
        flow = suffix_flow_prune(toseq(S), toseq(T), timeseq(S), timeseq(T));
        break;
      case 10:
        flow = suffix_flow_dcp(toseq(S), toseq(T), timeseq(S), timeseq(T), 0);
        break;
      case 11:
        flow = suffix_flow_dcp(toseq(S), toseq(T), timeseq(S), timeseq(T), 0.1);
        break;
      case 12:
        flow = suffix_flow_dcp(toseq(S), toseq(T), timeseq(S), timeseq(T), 0.2);
        break;
      case 13:
        flow = suffix_flow_dcp(toseq(S), toseq(T), timeseq(S), timeseq(T), 0.3);
        break;
      case 14:
        flow = suffix_flow_dcp(toseq(S), toseq(T), timeseq(S), timeseq(T), 0.4);
        break;
      case 15:
        flow = suffix_flow_dcp(toseq(S), toseq(T), timeseq(S), timeseq(T), 0.5);
        break;
      case 16:
        flow = suffix_flow_dcp(toseq(S), toseq(T), timeseq(S), timeseq(T), 0.6);
        break;
      case 17:
        flow = suffix_flow_dcp(toseq(S), toseq(T), timeseq(S), timeseq(T), 0.7);
        break;
      case 18:
        flow = suffix_flow_dcp(toseq(S), toseq(T), timeseq(S), timeseq(T), 0.8);
        break;
      case 19:
        flow = suffix_flow_dcp(toseq(S), toseq(T), timeseq(S), timeseq(T), 0.9);
        break;
      case 20:
        flow = suffix_flow_dcp(toseq(S), toseq(T), timeseq(S), timeseq(T), 1);
        break;
    }
    #ifdef comp_time
    total_time += clock();
    #endif
    for (auto [ts, s] : S)
      result.insert(ts, {flow[s] / (curT - ts + 1.0), curT - ts + 1});
    return true;
  }
  unordered_set<vtx_t> toset(const list<pair<tick_t, vtx_t>> &V) {
    unordered_set<vtx_t> result;
    for (auto [ts, v] : V) result.insert(v);
    return result;
  }
  vector<vtx_t> toseq(const list<pair<tick_t, vtx_t>> &V) {
    vector<vtx_t> result;
    for (auto [ts, v] : V) result.push_back(v);
    return result;
  }
  vector<tick_t> timeseq(const list<pair<tick_t, vtx_t>> &V) {
    vector<tick_t> result;
    for (auto [ts, v] : V) result.push_back(ts);
    return result;
  }
  void inher_pre(int s) { G.peel(s, toset(T)); }
  unordered_map<vtx_t, flow_t> brute_force(vector<vtx_t> S, vector<vtx_t> T) {
    graph cpyG;
    unordered_map<vtx_t, flow_t> result;
    for (int i = 0; i < S.size(); i++) {
      cpyG = G;
      result[S[i]] = cpyG.maxflow({S.begin() + i, S.end()},
                                  {T.begin(), T.end()}, toset(V));
    }
    return result;
  }
  unordered_map<vtx_t, flow_t> incremental(vector<vtx_t> S, vector<vtx_t> T) {
    graph cpyG = G;
    unordered_map<vtx_t, flow_t> result;
    flow_t cur = 0;
    vector<pair<vtx_t, flow_t>> tmp;
    for (int i = S.size() - 1; i >= 0; i--) {
      cpyG.maxflow({S[i]}, {T.begin(), T.end()}, toset(V));
      result[S[i]] = cur += cpyG.flowout({S[i]});
    }
    return result;
  }
  unordered_map<vtx_t, flow_t> suffix_flow_inher(vector<vtx_t> S,
                                                 vector<vtx_t> T) {
    auto setV = toset(V);
    auto solve = [&](auto &&self, vector<vtx_t> S, unordered_set<vtx_t> V,
                     int layer) {
      if (S.size() <= 1) return;
      vector<vtx_t> Sl = {S.begin(), S.begin() + S.size() / 2},
                    Sr = {S.begin() + S.size() / 2, S.end()};
      gsize_layer[layer] += V.size();
      time_layer[layer] -= clock();
      G.maxflow({Sr.begin(), Sr.end()}, {Sl.begin(), Sl.end()}, V);
      time_layer[layer] += clock();
      auto Vl = G.reachable(Sl, V), Vr = G.reachable(Sr, V);
      {
        unordered_set<vtx_t> empty;
        V.swap(empty);
      }
      self(self, Sl, Vl, layer + 1), self(self, Sr, Vr, layer + 1);
    };
    gsize_layer[0] += G.reachable({S.begin(), S.end()}, {T.begin(), T.end()}, setV).size();
    time_layer[0] -= clock();
    G.maxflow({S.begin(), S.end()}, {T.begin(), T.end()}, setV);
    time_layer[0] += clock();
    dctime -= clock();
    solve(solve, S, G.reachable(S, setV), 1);
    dctime += clock();
    unordered_map<vtx_t, flow_t> result;
    flow_t cur = 0;
    for (int i = S.size() - 1; i >= 0; i--)
      result[S[i]] = cur += G.flowout({S[i]});
    return result;
  }
  unordered_map<vtx_t, flow_t> suffix_flow_prune(vector<vtx_t> S,
                                                 vector<vtx_t> T,
                                                 vector<tick_t> tS,
                                                 vector<tick_t> tT) {
    auto setV = toset(V);
    auto solve = [&](auto &&self, int l2, int l, int r, flow_t rng, flow_t suf,
                     unordered_set<vtx_t> V, int layer) {
      if (!rng) return;
      while (l < r && (rng + suf) / (curT - tS[l] + 1.0) <
                          result.max(tS[l]).first + eps) {
        l++, pruned++;
      }
      if (r - l >= 0 && l - l2 >= r - l) {
        gsize_layer[layer] += V.size();
        time_layer[layer] -= clock();
        G.maxflow({S.begin() + l, S.begin() + r},
                  {S.begin() + l2, S.begin() + l}, V);
        time_layer[layer] += clock();
        l2 = l;
        V = G.reachable({S.begin() + l, S.begin() + r}, V);
      }
      if (r - l <= 1) return;
      int m = l + r >> 1;
      vector<vtx_t> Sr = {S.begin() + m, S.begin() + r},
                    Sl = {S.begin() + l2, S.begin() + m};
      gsize_layer[layer] += V.size();
      time_layer[layer] -= clock();
      G.maxflow({Sr.begin(), Sr.end()}, {Sl.begin(), Sl.end()}, V);
      time_layer[layer] += clock();
      auto Vl = G.reachable(Sl, V), Vr = G.reachable(Sr, V);
      {
        unordered_set<vtx_t> empty;
        V.swap(empty);
      }
      flow_t fr = G.flowout({Sr.begin(), Sr.end()}),
             fl = G.flowout({Sl.begin(), Sl.end()});
      self(self, m, m, r, fr, suf, Vr, layer + 1);
      self(self, l2, l, m, fl, fr + suf, Vl, layer + 1);
    };
    tot += S.size();
    gsize_layer[0] += G.reachable({S.begin(), S.end()}, {T.begin(), T.end()}, setV).size();
    time_layer[0] -= clock();
    G.maxflow({S.begin(), S.end()}, {T.begin(), T.end()}, setV);
    time_layer[0] += clock();
    dctime -= clock();
    solve(solve, 0, 0, S.size(), G.flowout({S.begin(), S.end()}), 0,
          G.reachable(S, setV), 1);
    dctime += clock();
    unordered_map<vtx_t, flow_t> result;
    flow_t cur = 0;
    for (int i = S.size() - 1; i >= 0; i--)
      result[S[i]] = cur += G.flowout({S[i]});
    return result;
  }

  unordered_map<vtx_t, flow_t> suffix_flow_dcp(vector<vtx_t> S, vector<vtx_t> T,
                                               vector<tick_t> tS,
                                               vector<tick_t> tT, double p) {
    auto setV = toset(V);
    auto solve = [&](auto &&self, int l, int r, unordered_set<vtx_t> V) {
      if (r - l <= 1) return;
      int k = max((int)(l + (r - l) * p), 1);
      int m = l + k;
      m = min(m, r - 1);
      vector<vtx_t> Sr = {S.begin() + m, S.begin() + r},
                    Sl = {S.begin() + l, S.begin() + m};
      G.maxflow({Sr.begin(), Sr.end()}, {Sl.begin(), Sl.end()}, V);
      auto Vl = G.reachable(Sl, V), Vr = G.reachable(Sr, V);
      {
        unordered_set<vtx_t> empty;
        V.swap(empty);
      }
      self(self, m, r, Vr);
      self(self, l, m, Vl);
    };
    G.maxflow({S.begin(), S.end()}, {T.begin(), T.end()}, setV);
    dctime -= clock();
    solve(solve, 0, S.size(), G.reachable(S, setV));
    dctime += clock();
    unordered_map<vtx_t, flow_t> result;
    flow_t cur = 0;
    for (int i = S.size() - 1; i >= 0; i--)
      result[S[i]] = cur += G.flowout({S[i]});
    return result;
  }
  unordered_map<vtx_t, flow_t> suffix_flow(vector<vtx_t> S, vector<vtx_t> T) {
    graph cpyG = G;
    auto result = suffix_flow_inher(S, T);
    swap(G, cpyG);
    return result;
  }
};