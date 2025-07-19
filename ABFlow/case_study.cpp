#include "bits/stdc++.h"
using namespace std;
#define comp_time
long long total_time;
#include "header/stream.h"

int main(int argc, char* argv[]) {
  if (argc != 8) {
    puts("Usage: ./solution.out graph test start end window mode log");
    return 0;
  }
  string graph = argv[1], test = argv[2], log = argv[7];
  tick_t ts = atoll(argv[3]), te = atoll(argv[4]);
  double window = atof(argv[5]);
  int mode = atoi(argv[6]);
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
  cout << window << ' ' << te - ts << ' ' << window * (te - ts) << endl;
  // tick_t width = window * edges.size();
  tick_t width = window * (te - ts);
  cout << width << endl;
  stream sG(tS, tT, width, mode);
  ofstream lout(log);
  lout << setprecision(6);
  cout << "Prepare done" << endl;

  pair<double, tick_t> ans;
  double sum = 0;
  int cntE = 0, cntT = 0;
  for (int i = 0; i < edges.size(); i++) {
    auto [u, v, t, w] = edges[i];
    cntE++;
    if (t < ts || t > te) continue;
    // cout<<t<<' '<<ts<<' '<<te<<endl;
    auto begin = clock();
    bool skip = false;
    for (int j = i; j < edges.size() && get<2>(edges[j]) == get<2>(edges[i]) &&
                    tT.count(get<2>(edges[j]));
         j++)
      skip = true;
    auto flg = sG.edge_update(u, v, t, w, true, skip);
    auto end = clock();
    if (flg) {
      auto [density, width] = sG.result.max();
      ans = max(ans, {density, width});

      // time, density, total edges, modified density, width, runtime
      lout << "process(%): " << (double)(t - ts) / (te - ts)
           << "; density: " << density << "; width(no.): " << width
           << "; width(%): " << (double)width / (te - ts)
           << "; xpos: " << (sG.result.maxt() - ts) / (te - ts)
           << "; comp-time: " << (double)(end - begin) / CLOCKS_PER_SEC;
      if (mode == 5) lout << "; pruned: " << sG.pruned << "; total: " << sG.tot;
      lout << endl;
      // lout << t - (long long)(window * (te - ts)) << ' ' << t << endl;

      // for (auto [xpos, pr] : sG.result.seq) {
      //   auto [density, width] = pr;
      //   static char str[10000];
      //   sprintf(str, "%.2f [%d, %d] ; ", density, xpos, xpos + width);
      //   lout << str << endl;
      // }
      sum += (double)(end - begin) / CLOCKS_PER_SEC;
      cntT++;
    }
  }
  cout << "Total Time: " << sum << ' ' << (double)total_time / CLOCKS_PER_SEC
       << endl;
  cout << "# of maxflow: " << graph::cmaxf << endl;
  return 0;
}