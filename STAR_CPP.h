#ifndef STAR_CPP_H
#define STAR_CPP_H

#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <set>
#include <cstdlib>
#include <stdexcept>
#include <filesystem>

namespace STAR_CPP {

// 一些通用工具
class Timer {
private:
	std::chrono::time_point<std::chrono::steady_clock> start_time;
public:
	Timer() : start_time(std::chrono::steady_clock::now()) {}
	void reset() { start_time = std::chrono::steady_clock::now(); }
	double elapsed_seconds() const { return std::chrono::duration<double>(std::chrono::steady_clock::now() - start_time).count(); }
};

struct Stats {
	double mean = 0, variance = 0; size_t n = 0;
	void add(double x) {
		++n; double delta = x - mean; mean += delta / n; variance += delta * (x - mean);
	}
	double get_mean() const { return mean; }
	double get_variance() const { return n > 1 ? variance / (n-1) : 0; }
};

	class IO {
	private:
		std::string input_filename;
		std::string output_filename;
		std::ofstream input_file;
		std::ofstream output_file;
		std::mt19937 rng;
		
	public:
		IO(const std::string& input_name, const std::string& output_name = "") 
		: input_filename(input_name), output_filename(output_name),
		rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
			if (!input_name.empty()) {
				input_file.open(input_name);
				if (!input_file.is_open()) {
					throw std::runtime_error("Cannot open input file: " + input_name);
				}
			}
			if (!output_name.empty()) {
				output_file.open(output_name);
				if (!output_file.is_open()) {
					throw std::runtime_error("Cannot open output file: " + output_name);
				}
			}
		}
		
		~IO() {
			if (input_file.is_open()) input_file.close();
			if (output_file.is_open()) output_file.close();
		}
		
		template<typename T>
		void input(const T& data) {
			if (input_file.is_open()) input_file << data;
		}
		
		template<typename T>
		void output(const T& data) {
			if (output_file.is_open()) output_file << data;
		}
		
		// 重载运算符方便使用
		template<typename T>
		IO& operator<<(const T& data) {
			input(data);
			return *this;
		}
		
		// 获取文件名
		std::string get_input_filename() const { return input_filename; }
		std::string get_output_filename() const { return output_filename; }
		
		// 读取整个输出文件为字符串（如果存在）
		std::string read_output_all() {
			if (output_filename.empty() || !std::filesystem::exists(output_filename)) return "";
			std::ifstream in(output_filename, std::ios::binary);
			std::ostringstream ss;
			ss << in.rdbuf();
			return ss.str();
		}

		// 写入（覆盖）文件，使用原子重命名（如果支持）
		void write_atomic(const std::string& filename, const std::string& data) {
			std::filesystem::path p(filename);
			auto tmp = p;
			tmp += ".tmp";
			{
				std::ofstream out(tmp, std::ios::binary);
				if (!out.is_open()) throw std::runtime_error("Cannot open temp file for atomic write: " + tmp.string());
				out << data;
			}
			std::filesystem::rename(tmp, p);
		}

		// 方便地写一行并追加换行
		void writeline(const std::string& line) {
			if (output_file.is_open()) {
				output_file << line << std::endl;
			}
		}

		// 允许从任意 istream/ostream 读写
		void bind_input_stream(std::istream& in) {
			// 这里只是一个轻量适配：将流内容读入 input_filename 方便后续处理
			std::ostringstream ss;
			ss << in.rdbuf();
			if (!input_filename.empty()) {
				std::ofstream f(input_filename, std::ios::binary);
				f << ss.str();
			}
		}

		void bind_output_stream(std::ostream& out) {
			// 将当前 output_file 内容写入提供的 ostream
			if (output_file.is_open()) {
				output_file.flush();
				std::ifstream f(output_filename, std::ios::binary);
				out << f.rdbuf();
			}
		}
		// 运行标程生成输出文件
		void createof(const std::string& executable_path) {
			if (input_filename.empty() || output_filename.empty()) {
				throw std::runtime_error("Input or output filename not set");
			}
			
			// 关闭文件以便标程可以访问
			if (input_file.is_open()) input_file.close();
			if (output_file.is_open()) output_file.close();
			
			// 构造命令行
			std::string command = executable_path + " < " + input_filename + " > " + output_filename;
			
			// 执行命令
			int result = std::system(command.c_str());
			if (result != 0) {
				throw std::runtime_error("Failed to execute program: " + executable_path);
			}
			
			// 重新打开文件
			input_file.open(input_filename, std::ios::app);
			output_file.open(output_filename, std::ios::app);
			if (!input_file.is_open()) {
				throw std::runtime_error("Cannot reopen input file: " + input_filename);
			}
			if (!output_file.is_open()) {
				throw std::runtime_error("Cannot reopen output file: " + output_filename);
			}
		}
	};
	
	class Random {
	private:
		std::mt19937 rng;
		
	public:
		Random() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {}
		Random(unsigned seed) : rng(seed) {}

		// 设置种子以便重现
		void seed(unsigned s) { rng.seed(s); }

		// 获取当前引擎状态的一部分（用于诊断/重现）
		unsigned get_seed() {
			std::uniform_int_distribution<unsigned> dist;
			return dist(rng);
		}
		
		// 生成指定范围内的随机整数
		int randint(int min, int max) {
			std::uniform_int_distribution<int> dist(min, max);
			return dist(rng);
		}
		
		// 生成指定范围内的随机浮点数
		double random(double min = 0.0, double max = 1.0) {
			std::uniform_real_distribution<double> dist(min, max);
			return dist(rng);
		}
		
		// 生成随机字符串
		std::string randstr(int length, const std::string& charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") {
			std::string result;
			std::uniform_int_distribution<int> dist(0, charset.length() - 1);
			for (int i = 0; i < length; ++i) {
				result += charset[dist(rng)];
			}
			return result;
		}

		// 正态分布
		double normal(double mean = 0.0, double stddev = 1.0) {
			std::normal_distribution<double> dist(mean, stddev);
			return dist(rng);
		}

		// 泊松分布
		int poisson(double mean) {
			std::poisson_distribution<int> dist(mean);
			return dist(rng);
		}

		// 伯努利分布
		bool bernoulli(double p = 0.5) {
			std::bernoulli_distribution dist(p);
			return dist(rng);
		}

		// 加权随机选择：weights 长度等于 container 长度
		template<typename T>
		T weighted_choice(const std::vector<T>& container, const std::vector<double>& weights) {
			if (container.empty() || container.size() != weights.size()) throw std::invalid_argument("container and weights size mismatch");
			double sum = 0;
			for (double w : weights) { if (w < 0) throw std::invalid_argument("negative weight"); sum += w; }
			if (sum <= 0) throw std::invalid_argument("non-positive total weight");
			std::uniform_real_distribution<double> dist(0.0, sum);
			double x = dist(rng);
			double cum = 0;
			for (size_t i = 0; i < container.size(); ++i) {
				cum += weights[i];
				if (x <= cum) return container[i];
			}
			return container.back();
		}

		// 从 container 中不放回采样 k 个元素
		template<typename T>
		std::vector<T> sample_no_replace(const std::vector<T>& container, size_t k) {
			if (k > container.size()) throw std::invalid_argument("k larger than container");
			auto idx = randperm(static_cast<int>(container.size()));
			std::vector<T> res;
			for (size_t i = 0; i < k; ++i) res.push_back(container[idx[i]-1]);
			return res;
		}

		// 流式/水库采样：从未知大小的流中采样 k 个元素（元素类型 T）
		template<typename T, typename InputIt>
		std::vector<T> reservoir_sample(InputIt begin, InputIt end, size_t k) {
			std::vector<T> reservoir;
			size_t i = 0;
			for (auto it = begin; it != end; ++it) {
				if (i < k) reservoir.push_back(*it);
				else {
					std::uniform_int_distribution<size_t> dist(0, i);
					size_t j = dist(rng);
					if (j < k) reservoir[j] = *it;
				}
				++i;
			}
			return reservoir;
		}
		
		// 生成随机排列
		std::vector<int> randperm(int n) {
			std::vector<int> result(n);
			for (int i = 0; i < n; ++i) result[i] = i + 1;
			std::shuffle(result.begin(), result.end(), rng);
			return result;
		}
		
		// 从容器中随机选择一个元素
		template<typename T>
		T choice(const std::vector<T>& container) {
			if (container.empty()) throw std::invalid_argument("Cannot choice from empty container");
			std::uniform_int_distribution<size_t> dist(0, container.size() - 1);
			return container[dist(rng)];
		}
	};
	
	// 图论相关
	class Graph {
	public:
		// 生成随机树（父节点编号从1开始）
		static std::vector<std::pair<int, int>> random_tree(int n, int seed = 0) {
			Random rng(seed ? seed : static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
			std::vector<std::pair<int, int>> edges;
			for (int i = 2; i <= n; ++i) {
				int parent = rng.randint(1, i - 1);
				edges.push_back({parent, i});
			}
			return edges;
		}

		// 生成无权随机图（m 条边）
		static std::vector<std::pair<int, int>> random_graph(int n, int m, bool directed = false, int seed = 0) {
			Random rng(seed ? seed : static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
			std::vector<std::pair<int, int>> edges;
			std::set<std::pair<int, int>> edge_set;
			while (static_cast<int>(edges.size()) < m) {
				int u = rng.randint(1, n);
				int v = rng.randint(1, n);
				if (u == v) continue;
				if (!directed && u > v) std::swap(u, v);
				if (edge_set.find({u, v}) == edge_set.end()) {
					edge_set.insert({u, v});
					edges.push_back({u, v});
				}
			}
			return edges;
		}

		// 生成带权随机图（每条边带权重）
		static std::vector<std::tuple<int,int,double>> random_weighted_graph(int n, int m, double minw = 0.0, double maxw = 1.0, int seed = 0) {
			Random rng(seed ? seed : static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
			auto edges = random_graph(n, m, false, seed);
			std::vector<std::tuple<int,int,double>> res;
			for (auto &e: edges) {
				double w = rng.random(minw, maxw);
				res.emplace_back(e.first, e.second, w);
			}
			return res;
		}

		// Erdos-Renyi G(n, p)
		static std::vector<std::pair<int,int>> erdos_renyi(int n, double p, int seed = 0) {
			Random rng(seed ? seed : static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
			std::vector<std::pair<int,int>> edges;
			for (int u = 1; u <= n; ++u) for (int v = u+1; v <= n; ++v) {
				if (rng.random(0.0, 1.0) < p) edges.emplace_back(u, v);
			}
			return edges;
		}

		// 输出为边列表字符串
		static std::string to_edge_list_string(const std::vector<std::pair<int,int>>& edges) {
			std::ostringstream ss;
			for (auto &e: edges) ss << e.first << " " << e.second << "\n";
			return ss.str();
		}

		// 基本 BFS（返回访问顺序）
		static std::vector<int> bfs(int n, const std::vector<std::pair<int,int>>& edges, int start = 1) {
			std::vector<std::vector<int>> adj(n+1);
			for (auto &e: edges) { adj[e.first].push_back(e.second); adj[e.second].push_back(e.first); }
			std::vector<int> vis(n+1, 0), order;
			std::deque<int> dq; dq.push_back(start); vis[start]=1;
			while(!dq.empty()){
				int u = dq.front(); dq.pop_front(); order.push_back(u);
				for(int v: adj[u]) if(!vis[v]){vis[v]=1; dq.push_back(v);} 
			}
			return order;
		}

		// 计算连通分量数和组件分配（1..n）
		static std::pair<int,std::vector<int>> connected_components(int n, const std::vector<std::pair<int,int>>& edges) {
			std::vector<std::vector<int>> adj(n+1);
			for (auto &e: edges) { adj[e.first].push_back(e.second); adj[e.second].push_back(e.first); }
			std::vector<int> comp(n+1, 0);
			int cid = 0;
			for (int i=1;i<=n;++i) if (!comp[i]) {
				cid++; std::deque<int> dq; dq.push_back(i); comp[i]=cid;
				while(!dq.empty()){int u=dq.front();dq.pop_front();for(int v:adj[u]) if(!comp[v]){comp[v]=cid; dq.push_back(v);} }
			}
			return {cid, comp};
		}
	};
	
	// 序列生成相关
	class Sequence {
	public:
		// 生成随机数组
		static std::vector<int> random_array(int n, int min_val, int max_val, int seed = 0) {
			Random rng(seed ? seed : static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
			std::vector<int> result(n);
			for (int i = 0; i < n; ++i) {
				result[i] = rng.randint(min_val, max_val);
			}
			return result;
		}

		// 生成递增数组
		static std::vector<int> increasing_array(int n, int min_val, int max_val, int seed = 0) {
			auto result = random_array(n, min_val, max_val, seed);
			std::sort(result.begin(), result.end());
			return result;
		}

		// 生成递减数组
		static std::vector<int> decreasing_array(int n, int min_val, int max_val, int seed = 0) {
			auto result = random_array(n, min_val, max_val, seed);
			std::sort(result.begin(), result.end(), std::greater<int>());
			return result;
		}

		// 生成接近有序数组（fraction 表示随机打乱的比例）
		static std::vector<int> nearly_sorted(int n, int min_val, int max_val, double fraction = 0.05, int seed = 0) {
			auto v = increasing_array(n, min_val, max_val, seed);
			Random rng(seed);
			int swaps = static_cast<int>(n * fraction);
			for (int i = 0; i < swaps; ++i) {
				int a = rng.randint(0, n-1);
				int b = rng.randint(0, n-1);
				std::swap(v[a], v[b]);
			}
			return v;
		}

		// 生成块逆序数组（将数组分成块并反转部分块）
		static std::vector<int> block_reversed(int n, int min_val, int max_val, int blocks = 4, int seed = 0) {
			auto v = random_array(n, min_val, max_val, seed);
			int block_size = std::max(1, n / blocks);
			for (int b = 0; b < blocks; ++b) {
				int l = b*block_size;
				int r = std::min(n, l + block_size);
				std::reverse(v.begin()+l, v.begin()+r);
			}
			return v;
		}

		// 生成几何序列
		static std::vector<int> geometric_sequence(int n, int start, int ratio) {
			std::vector<int> v(n);
			long long cur = start;
			for (int i=0;i<n;++i){ v[i]=static_cast<int>(cur); cur*=ratio; }
			return v;
		}

		// 生成字符序列（随机字符串数组）
		static std::vector<std::string> random_string_array(int n, int len, const std::string& charset="abcdefghijklmnopqrstuvwxyz", int seed=0) {
			Random rng(seed);
			std::vector<std::string> res; res.reserve(n);
			for (int i=0;i<n;++i) res.push_back(rng.randstr(len, charset));
			return res;
		}
	};
}

#endif // STAR_CPP_H
