# ABFlow: Alert Bursting Flow Query in Streaming Temporal Flow Networks

ABFlow-tr.pdf is the full technical report.

Folder cpp/ contains all codes in this work.

# Compile

Execute the following command to compile the source code:

```
g++ -O3 -std=c++20 cpp/solution.cpp -o ABFlow
```

# Data Format

sample/sample.gra is a sample graph (STFN)

- Each line contains four integers:

  u v t c

  which indicate a TFN update <e = (u, v), t, c>

sample/sample.qry is a sample query

- Line 1 contains an array of integers:

  n s1 s2 ... sn

  where n is the number of sources, and {s1, ..., sn} is the source set S

- Line 2 contains an array of integers:

  m t1 t2 ... tm

  where m is the number of sinks, and {t1, ..., tm} is the sink set T

# Execution

To run the ABFlow query, execute the following command:

```
./ABFlow graph_file query_file w_ratio mode_id
```

- graph_file is the file path of graph data
- query_file is the file path of query data
- w_ratio is the ratio of sliding window size w over the total size of the timespan of graph data
- mode_id is the id for algorithms in this paper

To run the sample with sliding window size w = 3 updates (6 updates in total):

- For Algorithm 1 (Baseline) in this paper, execute:
  ```
  ./ABFlow sample/sample.gra sample/sample.qry 0.5 1
  ```


- For Algorithm 3 (SuffixFlow_inc) in this paper, execute:
  ```
  ./ABFlow sample/sample.gra sample/sample.qry 0.5 2
  ```


- For Algorithm 4 (SuffixFlow_rec) in this paper, execute:
  ```
  ./ABFlow sample/sample.gra sample/sample.qry 0.5 3
  ```


- For Algorithm 5 (SuffixFlow_str) in this paper, execute:
  ```
  ./ABFlow sample/sample.gra sample/sample.qry 0.5 4
  ```


- For Algorithm 7 (SuffixFlow_prune) in the appendix of the full technical report, execute:
  ```
  ./ABFlow sample/sample.gra sample/sample.qry 0.5 5
  ```


# Example

To run SuffixFlow_rec in this paper on the sample graph and query, execute:

```
g++ -O3 -std=c++20 cpp/solution.cpp -o ABFlow
./ABFlow sample/sample.gra sample/sample.qry 0.5 4
```