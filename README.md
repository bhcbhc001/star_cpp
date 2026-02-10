下列为本次实现的全部更新内容说明：

**总体**:
- 将若干内部方法暴露为公共静态 API，便于直接调用（`Graph`、`Sequence` 等）。
- 保持单头文件可直接包含使用，同时提供了一个示例可执行文件 `example_main.cpp`。

**`IO` 类（增强）**:
- 新增 `read_output_all()`：读取输出文件全部内容为字符串。
- 新增 `write_atomic(filename, data)`：使用临时文件+重命名实现原子写入（如文件系统支持）。
- 新增 `writeline(line)`：便捷追加一行并换行。
- 新增 `bind_input_stream(std::istream&)` / `bind_output_stream(std::ostream&)`：将任意流与文件内容互通的轻量绑定（便于测试与集成）。
- 保留并增强了原有 `createof` 行为（仍可通过 `IO` 操作输入/输出文件）。

**`Random` 类（增强）**:
- 可设置种子：`seed(unsigned)`，并提供 `get_seed()`（用于诊断/重现）。
- 新增分布：`normal(mean,stddev)`、`poisson(mean)`、`bernoulli(p)`。
- 新增加权选择：`weighted_choice(container, weights)`。
- 新增不放回采样：`sample_no_replace(container,k)`。
- 新增水库采样（reservoir sampling）：`reservoir_sample(begin,end,k)`，适用于未知长度流。
- 保留并增强 `randstr`, `randperm`, `choice` 等常用接口。

**`Sequence`（序列生成）**:
- 将生成方法公开为静态方法。
- 新增 `nearly_sorted(n,min,max,fraction,seed)`：生成近乎有序数组（通过少量交换引入乱序）。
- 新增 `block_reversed(n,min,max,blocks,seed)`：按块反转构造部分逆序样例。
- 新增 `geometric_sequence(n,start,ratio)`：生成几何序列。
- 新增 `random_string_array(n,len,charset,seed)`：生成字符串数组样例。

**`Graph`（图论）**:
- 将生成函数公开为静态 API。
- 新增 `random_tree(n,seed)`、`random_graph(n,m,...)`（无向/有向）接口（原有逻辑）。
- 新增 `random_weighted_graph(n,m,minw,maxw,seed)`：为每条边生成权重。
- 新增 `erdos_renyi(n,p,seed)`：Erdos–Rényi 随机图模型（G(n,p)）。
- 新增导出：`to_edge_list_string(edges)` 返回可写文本表示。
- 新增基础算法：`bfs(n,edges,start)` 返回 BFS 访问顺序；`connected_components(n,edges)` 返回连通分量数与分配向量。

**通用工具**:
- 新增 `Timer`（计时器）与 `Stats`（在线均值/方差汇总）类，便于基准与统计。
