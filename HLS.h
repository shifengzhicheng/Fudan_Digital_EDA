#ifndef HLS_H
#define HLS_H
#include "parser.h"
#include "dataflowgraph.h"
#include "controlflowgraph.h"
#include <iostream>

class HLS {
private:
	std::string _generated_rtl_code;
	// parser
	parser parsered;
	// CFG
	ControlFlowGraph CFG;
	// REG
	std::vector<std::vector<std::pair<std::string, int>>> REG;
	//计算资源（包括加法器和乘法器）
	std::vector<computeresource> COR;
	//计算资源匹配结果（匹配的是node结点和计算资源COR的序号）
	std::vector<std::vector<std::pair<node,int>>>  CSP;
public:
	HLS(parser& p) : parsered(p) {}

	ControlFlowGraph& getCFG() {
		return CFG;
	}
	std::vector<std::vector<std::pair<std::string, int>>> getREG() {
		return REG;
	}
	// 执行转换操作
	void convert() {

		// 执行图的生成
		generate_CFG();

		// 执行调度
		perform_scheduling();

		// 执行寄存器分配和绑定
		perform_register_allocation_and_binding();

		// 实现运算资源的分配与绑定
		perform_calculate_allocation_and_binding();

		// 构建控制逻辑
		synthesize_control_logic();

	}

	// 生成RTL操作方法
	void generate_rtl_operations() const;

	// 获取生成的RTL代码
	std::string getRTLCode() const {
		return _generated_rtl_code;
	}
	// 内部方法
	// 实现图的生成
	void generate_CFG();
	//设置测试时间
	void setTestTime();
	// 遍历所有节点的算法
	void travelaround();
	// 实现调度算法
	void perform_scheduling();
	// 实现寄存器分配和绑定
	void perform_register_allocation_and_binding();
	// 实现运算资源的分配与绑定
	void perform_calculate_allocation_and_binding();
	// 控制逻辑综合方法
	void synthesize_control_logic();



};
#endif
