#ifndef STAR_H
#define STAR_H

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

namespace STAR {
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
		
		// 运行标程生成输出文件
		void run(const std::string& executable_path) {
			if (input_filename.empty() || output_filename.empty()) {
				throw std::runtime_error("Input or output filename not set");
			}
			
			// 关闭文件以便标程可以访问
			if (input_file.is_open()) input_file.close();
			if (output_file.is_open()) output_file.close();
			
			// 构造命令行
			std::string command = "powershell" + executable_path + " < " + input_filename + " > " + output_filename;
			
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
	namespace graph {
		// 生成随机树
		std::vector<std::pair<int, int>> random_tree(int n, int seed = 0) {
			Random rng(seed ? seed : static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
			std::vector<std::pair<int, int>> edges;
			for (int i = 2; i <= n; ++i) {
				int parent = rng.randint(1, i - 1);
				edges.push_back({parent, i});
			}
			return edges;
		}
		
		// 生成随机图
		std::vector<std::pair<int, int>> random_graph(int n, int m, bool directed = false, int seed = 0) {
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
	}
	
	// 序列生成相关
	namespace sequence {
		// 生成随机数组
		std::vector<int> random_array(int n, int min_val, int max_val, int seed = 0) {
			Random rng(seed ? seed : static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
			std::vector<int> result(n);
			for (int i = 0; i < n; ++i) {
				result[i] = rng.randint(min_val, max_val);
			}
			return result;
		}
		
		// 生成递增数组
		std::vector<int> increasing_array(int n, int min_val, int max_val, int seed = 0) {
			auto result = random_array(n, min_val, max_val, seed);
			std::sort(result.begin(), result.end());
			return result;
		}
		
		// 生成递减数组
		std::vector<int> decreasing_array(int n, int min_val, int max_val, int seed = 0) {
			auto result = random_array(n, min_val, max_val, seed);
			std::sort(result.begin(), result.end(), std::greater<int>());
			return result;
		}
	}
}

#endif // STAR_H
