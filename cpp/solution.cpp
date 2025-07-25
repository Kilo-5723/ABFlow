#include <bits/stdc++.h>
using namespace std;
#include "header/stream.h"

int main(int argc, char* argv[]) {
  if (argc != 5) {
    puts("Usage: ./solution.out graph test window mod");
    return 0;
  }
  string graph = argv[1], test = argv[2];
  double window = atof(argv[3]);
  int mode = atoi(argv[4]);
  vector<tuple<vtx_t, vtx_t, tick_t, flow_t>> edges;
  ifstream gin(graph);
  while (true) {
    vtx_t u, v;
    tick_t t;
    flow_t w;
    if (!(gin >> u >> v >> t >> w)) break;
    edges.push_back({u, v, t, w});
  }
  cout << "Edge count: " << edges.size() << endl;
  unordered_set<vtx_t> tS, tT;
  ifstream tin(argv[2]);
  int k;
  vtx_t v;
  tin >> k;
  while (k--) tin >> v, tS.insert(v);
  tin >> k;
  while (k--) tin >> v, tT.insert(v);
  cout << "Sources are: ";
  for (auto s : tS) cout << s << ' ';
  cout << endl;
  cout << "Sinks are: ";
  for (auto t : tT) cout << t << ' ';
  cout << endl;
  int width = window * edges.size();
  stream sG(tS, tT, window * edges.size(), mode);
  cout << "Loading completes." << endl;

  pair<double, tick_t> ans;
  double sum = 0;
  int cntE = 0, cntT = 0;
  for (auto [u, v, t, w] : edges) {
    auto begin = clock();
    auto flg = sG.edge_update(u, v, 0, w, false);
    auto end = clock();
    cntE++;
    if (flg) {
      auto [density, width] = sG.result.max();
      ans = max(ans, {density, width});
      sum += (double)(end - begin) / CLOCKS_PER_SEC;
      cntT++;
    }
  }
  cout << "Density: " << ans.first << endl;

  cout << "Window Size: " << ans.second << endl;

  cout << "Total Runtime: " << sum << " second" << endl;
  cout << "Average Runtime per Update: " << sum / cntT << " second" << endl;
  if (mode == 5) {
    cout << "Pruned Computation: " << sG.pruned << endl;
    cout << "Total Computation: " << sG.tot << endl;
  }
  if (mode > 2) {
    cout << "Total Runtime on Each Recursion Level (Starting from 0, outside "
            "the recursion):"
         << endl;
    for (auto [k, v] : sG.time_layer)
      cout << ' ' << (double)v / CLOCKS_PER_SEC << " s ;";
    cout << endl;
    cout << "Total Graph Size on Each Recursion Level (Starting from 0, "
            "outside the recursion):"
         << endl;
    for (auto [k, v] : sG.gsize_layer) cout << ' ' << (double)v / cntT << " ;";
    cout << endl;
  }
  return 0;
}