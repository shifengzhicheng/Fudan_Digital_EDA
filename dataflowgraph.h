#ifndef DATAFLOWGRAPH_H
#define DATAFLOWGRAPH_H
#include "parser.h"
#include "unordered_map"
// 操作周期宏定义
constexpr auto  T_ASSIGN = 1;  // 赋值操作;
constexpr auto	T_ADD = 1;     // 加法操作
constexpr auto	T_SUB = 1;    // 减法操作
constexpr auto	T_MUL = 1;    // 乘法操作
constexpr auto  T_DIV = 1;// 除法操作
constexpr auto	T_LOAD = 1;// 载入操作
constexpr auto	T_STORE = 1;  // 存储操作
constexpr auto	T_BR = 1;   // 分支操作
constexpr auto	T_LT = 1;   // 小于操作
constexpr auto	T_GT = 1;   // 大于操作
constexpr auto	T_LE = 1;   // 小于等于操作
constexpr auto	T_GE = 1;    // 大于等于操作
constexpr auto	T_EQ = 1;   // 等于操作
constexpr auto	T_PHI = 1;  // Phi 操作
constexpr auto	T_RET = 1;     // 返回操作


class HardwareConstraints {
	public:
		int getLatency(const int type) {
			return latencies[type];
		}
		void setLatency(const int type, int latency) {
			latencies[type] = latency;
		}
		int getcount(const int type) {
			return counts[type];
		}
		void setcount(const int type, int count) {
			counts[type] = count;
		}
	private:
		// 硬件约束，操作延时以及硬件资源约束
		std::unordered_map<int, int> latencies;
		std::unordered_map<int, int> counts;
};

class op {
	private:
		// 操作的变量
		std::vector<std::string> inputvars;
		std::string outputvar;
		// 操作的类型
		int OPtype;

		// 操作的所需周期
		int T;

		int set_T(int type);
	public:
		std::vector<std::string> &getInputvars() {
			return inputvars;
		}
		std::string &getOutputvar() {
			return outputvar;
		}
		int getOPtype() const {
			return OPtype;
		}
		int getLatency() const {
			return T;
		}



		// 操作创建
		// op初始化函数
		op(statement &_statement): T(set_T(_statement.get_type())) {
			OPtype = _statement.get_type();
			for (int k = 0; k < _statement.get_num_oprands(); ++k)
				inputvars.push_back(_statement.get_oprand(k));
			outputvar = _statement.get_var();
		}
		op(): T(0) {};		

};

class node {
		// 链表的节点，这个是链表中的元素
	public:
		// 节点的数据
		op element;
		// 节点指向的节点的下标数组
		std::vector<std::string> InputVar;
		std::vector<int> next;
		// 操作开始周期
		int T_start;
		// 操作结束周期
		int T_end;
		// 构造函数
		node(const op& elemval, std::vector<int> &_next) {
			element = elemval;
			next = _next;
			T_start = T_end = 0;
		}
		node(const op& elemval) {
			element = elemval;
			T_start = T_end = 0;
		}
		node() {
			T_start = T_end = 0;
		}
		int getTstart() const {
			return T_start;
		}
		int getTend() const {
			return T_end;
		}
		void setTstart(int CurrentT) {
			T_start = CurrentT;
		}
		void setTend(int CurrentT) {
			T_end = CurrentT;
		}
		// 操作绑定的寄存器
		void bindregister();
		// 操作绑定的运算资源
		void bindCacRes();
};

enum CONDTYPE {
	IfFalse,
	IfTrue,
	UnConditonal
};

enum IsVISITED {
	UNVISITED,
	VISITED
};

struct InputEdge {
	//数据流图中输入节点结构
	std::string From_Block;//该输入来自哪个模块
	std::string InputBlockVarName;//该输入变量名
	InputEdge(std::string &_From_Block, std::string &_InputBlockVarName) {
		From_Block = _From_Block;
		InputBlockVarName = _InputBlockVarName;
	}
	InputEdge(std::string &_InputBlockVarName) {
		InputBlockVarName = _InputBlockVarName;
	}
};

struct OutputEdge{
	// 有可能有别的需求
	std::string To_Block;// 这个变量应该前往哪一个块
	std::string OutBlockVarName;// 输出变量名
	OutputEdge(std::string _To_Block, std::string _OutBlockVarName) {
		To_Block = _To_Block;
		OutBlockVarName = _OutBlockVarName;
	}
};
struct BranchEdge {
	int cond;// 0false,1true,2无条件
	std::string From_Block;
	std::string To_Block;
	bool Isreturn;
	BranchEdge(std::string _From_Block, std::string _To_Block, int _cond) {
		cond = _cond;
		Isreturn = false;
		From_Block = _From_Block;
		To_Block = _To_Block;
	}
	BranchEdge() {
		cond = 2;
		Isreturn = false;
	}
};

struct Memaccess {
	std::string FromLabel;
	int FromOpIndex;
	std::string arrayName;
	std::string bias;
	Memaccess(std::string _FromLabel, int _FromOpIndex,
	          std::string _arrayName, std::string _bias):
		FromLabel(_FromLabel), FromOpIndex(_FromOpIndex),
		arrayName(_arrayName), bias(_bias) {}
};

class DataFlowGraph {
	private:
		// 数据块名称
		std::string label;

		// 输入的变量的列表
		std::vector<InputEdge> inputList;	

		// 需要输出的变量的列表
		std::vector<OutputEdge> outputList;

		// 主体，在本数据块中的节点列表
		std::vector<node> opList;			

		// 跳转边列表
		std::vector<BranchEdge> Branchs; 

		// 从变量的名字，到这个变量的index的hash表，用于在块内查找邻接数据
		std::unordered_map<std::string, int> vartable;

		// 与内存的交互
		std::vector<Memaccess> memory;

	public:

		// CreateGraph
		// 
		// 用于标记节点是否被访问
		std::vector<bool> Mark;
		// 用于标记节点当前的入度，为0表示可以被直接访问
		std::vector<int> InVertex;
		// 用于标记节点当前的出度
		std::vector<int> OutVertex;
		// DFG所需要的总周期
		int T_total = 0;
		DataFlowGraph(basic_block& bb);
		
		DataFlowGraph() {
			opList.push_back(op());
			BranchEdge out;
			label = "fiction_head";
			out.From_Block = label;
			Branchs.push_back(out);
			opList.push_back(op());
		}

		// operation
		// 创建边
		void CreateEdge(int from, int to);
		bool ispureNumber(std::string var);
		void CreateEdge(std::string Inputvar, int to);
		void CreateEdges(node& CurNode, int to);
		void CreateEdge(node& CurNode, int i, int to);
		// 初始化入度以及访问状况
		void Initialize();
		// get

		// 节点列表
		std::vector<node>& get_opList() {
			return opList;
		}
		// 输出边列表
		std::vector<BranchEdge>& get_Branches() {
			return Branchs;
		}
		// 输入变量列表
		std::vector<InputEdge>& get_inputList() {
			return inputList;
		}
		std::vector<OutputEdge>& get_outputList() {
			return outputList;
		}
		// 模块名
		std::string& get_label() {
			return label;
		}

		// 节点的出度节点
		std::vector<int>& ToVertex(int from) {
			return opList[from].next;
		}

		// 输出变量的哈希表
		std::unordered_map<std::string, int>& myOutvartable() {
			return vartable;
		}

};

#endif
