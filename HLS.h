#ifndef HLS_H
#define HLS_H
#include "parser.h"
#include "dataflowgraph.h"
#include "controlflowgraph.h"
#include <iostream>
#include"computeresource.h"
#include "cycleTable.h"
#include "FSMachine.h"
class HLS {
private:
	std::string _generated_rtl_code;
	// parser
	parser parsered;
	// CFG
	ControlFlowGraph CFG;
	// REG
	std::vector<std::vector<std::pair<std::string, int>>> REG;
	//计算资源（包括加法器、乘法器和除法器）
	std::vector<computeresource> COR;
	//计算资源匹配结果（匹配的是node结点的编号和计算资源COR的序号）
	std::vector<std::vector<std::pair<int, int>>>  CSP;
	//每个周期的所有需要执行的语句，statement包括输入变量与其对应的寄存器、op的type、绑定的计算资源、输出到的寄存器
	std::vector<std::vector<Cycle>> Cycles;
	// 代理实现输出文件的类
	FSMachine outputFSM;
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
	void travelback();
	// 实现调度算法
	void perform_scheduling();
	// 实现寄存器分配和绑定
	void perform_register_allocation_and_binding();
	// 实现运算资源的分配与绑定
	void perform_calculate_allocation_and_binding();
	// 控制逻辑综合方法
	void synthesize_control_logic();
	void genFSM();
	// 输出verilog文件
	void outputfile();



};
#endif
