#include <bits/stdc++.h>
using namespace std;
#include "header/stream.h"

int main(int argc, char* argv[]) {
  if (argc != 7) {
    puts("Usage: ./solution.out graph test window mode output log");
    return 0;
  }
  string graph = argv[1], test = argv[2], output = argv[5], log = argv[6];
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
  for (auto s : tS) cout << s << ' ';
  cout << endl;
  for (auto t : tT) cout << t << ' ';
  cout << endl;
  cout << window << ' ' << edges.size() << ' ' << window * edges.size() << endl;
  int width = window * edges.size();
  cout << width << endl;
  stream sG(tS, tT, window * edges.size(), mode);
  ofstream lout(log);
  lout << setprecision(6);
  cout << "Prepare done" << endl;

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
      // time, density, total edges, modified density, width, runtime
      lout << (double)cntE / edges.size() << ' ' << density << ' '
           << edges.size() << ' ' << (density * (long long)edges.size()) << ' '
           << (double)width / edges.size() << ' '
           << (double)(end - begin) / CLOCKS_PER_SEC;
      if (mode == 5) lout << ' ' << sG.pruned << ' ' << sG.tot;
      lout << endl;
      sum += (double)(end - begin) / CLOCKS_PER_SEC;
      cntT++;
    }
  }
  ofstream fout(output);
  // density, window, total time, avg. time
  fout << ans.first << ' ' << ans.second << ' ' << sum << ' ' << sum / cntT;
  if (mode == 5) fout << ' ' << sG.pruned << ' ' << sG.tot;
  fout << endl;
  fout << (double)sG.dctime / CLOCKS_PER_SEC << endl;
  fout << graph::cmaxf << ' ' << cntT << endl;
  for (auto [k, v] : sG.time_layer) fout << (double)v / CLOCKS_PER_SEC << ' ';
  fout << endl;
  for (auto [k, v] : sG.gsize_layer) fout << (double)v / cntT << ' ';
  fout << endl;
  return 0;
}