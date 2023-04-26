#ifndef DATAFLOWGRAPH_H
#define DATAFLOWGRAPH_H
#include "parser.h"
#include "unordered_map"
// 操作周期宏定义
#define	T_ASSIGN 1  // 赋值操作
#define	T_ADD 1     // 加法操作
#define	T_SUB 1    // 减法操作
#define	T_MUL 1    // 乘法操作
#define	T_DIV 1    // 除法操作
#define	T_LOAD 1   // 载入操作
#define	T_STORE 1  // 存储操作
#define	T_BR 1     // 分支操作
#define	T_LT 1     // 小于操作
#define	T_GT 1     // 大于操作
#define	T_LE 1     // 小于等于操作
#define	T_GE 1     // 大于等于操作
#define	T_EQ 1     // 等于操作
#define	T_PHI 1    // Phi 操作
#define	T_RET 1     // 返回操作


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


		// 操作绑定的寄存器
		void bindregister();
		// 操作绑定的运算资源
		void bindCacRes();
		// 操作创建
		// op初始化函数
		op(statement &_statement): T(set_T(_statement.get_type())) {
			OPtype = _statement.get_type();
			for (int k = 0; k < _statement.get_num_oprands(); ++k)
				inputvars.push_back(_statement.get_oprand(k));
			outputvar = _statement.get_var();
		}
		op(): T(0) {};
		int set_T(int type) {
			int result = 0;
			switch (type) {
				case OP_ASSIGN:
					result = T_ASSIGN;
					break;
				case OP_ADD:
					result = T_ADD;
					break;
				case OP_SUB:
					result = T_SUB;
					break;
				case OP_MUL:
					result = T_MUL;
					break;
				case OP_DIV:
					result = T_DIV;
					break;
				case OP_LOAD:
					result = T_LOAD;
					break;
				case OP_STORE:
					result = T_STORE;
					break;
				case OP_BR:
					result = T_BR;
					break;
				case OP_LT:
					result = T_LT;
					break;
				case OP_GT:
					result = T_GT;
					break;
				case OP_LE:
					result = T_LE;
					break;
				case OP_GE:
					result = T_GE;
					break;
				case OP_EQ:
					result = T_EQ;
					break;
				case OP_PHI:
					result = T_PHI;
					break;
				case OP_RET:
					result = T_RET;
					break;
				default:
					// handle error case
					result = 0;
					break;
			}
			return result;
		}

};

class node {
		// 链表的节点，这个是链表中的元素
	public:
		// 节点的入度
		int InVertex;
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
			InVertex = 0;
			T_start = T_end = 0;
		}
		node(const op& elemval) {
			element = elemval;
			InVertex = 0;
			T_start = T_end = 0;
		}
		node() {
			InVertex = 0;
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

struct OutputEdge {
	int cond;// 0false,1true,2无条件
	std::string From_Block;
	std::string To_Block;
	bool Isreturn;
	OutputEdge(std::string _From_Block, std::string _To_Block, int _cond) {
		cond = _cond;
		Isreturn = false;
		From_Block = _From_Block;
		To_Block = _To_Block;
	}
	OutputEdge() {
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

		// 主体，在本数据块中的节点列表
		std::vector<node> ops;			

		// 跳转边列表
		std::vector<OutputEdge> outputlist; 

		// 从变量的名字，到这个变量的index的hash表，用于在块内查找邻接数据
		std::unordered_map<std::string, int> vartable;

		// 与内存的交互
		std::vector<Memaccess> memory;
//		std::vector<int> Mark;
// 
		// 私有方法，生成节点之间的边
		void CreateEdge(int from, int to) {
			if (from < ops.size()) {
				//TODO
				// 在from和to之间加边，同时to的入度++
				// from的next数组中push进了to
				ops[from].next.push_back(to);
			}
		}
		// 找到节点的输入变量的输出节点的index，然后在其next中加上to
		void CreateEdge(std::string Inputvar, int to) {
			// Check if Inputvar is pure numbers
			bool isPureNumber = true;
			for (char c : Inputvar) {
				if (!std::isdigit(c)) {
					isPureNumber = false;
					break;
				}
			}

			if (isPureNumber) {
				return;
			}
			bool AlreadyIn = vartable.find(Inputvar) != vartable.end();
			if (AlreadyIn && vartable[Inputvar] != 0) {
				CreateEdge(vartable[Inputvar], to);
			}
			else {
				if(!AlreadyIn)
					inputList.push_back(InputEdge(Inputvar));
				CreateEdge(0, to);
			}
		}
		void CreateEdges(node &CurNode, int to) {
			for (int i = 0; i < CurNode.element.getInputvars().size(); i++)
			{
				CreateEdge(CurNode, i, to);
			}
		}
		void CreateEdge(node& CurNode, int i, int to) {
			bool isPureNumber = true;
			for (char c : CurNode.element.getInputvars()[i]) {
				if (!std::isdigit(c)) {
					isPureNumber = false;
					break;
				}
			}
			if (isPureNumber) {
				return;
			}
			if (vartable.find(CurNode.element.getInputvars()[i]) != vartable.end() && vartable[CurNode.element.getInputvars()[i]] != 0) {
				CreateEdge(vartable[CurNode.element.getInputvars()[i]], to);
			}
			else {
				inputList.push_back(InputEdge(CurNode.element.getInputvars()[i]));
				CreateEdge(0, to);
			}
		}
	public:

		// CreateGraph
		// 
		// 用于标记节点是否被访问
		std::vector<bool> Mark;
		// 用于标记节点当前的入度，为0表示可以被直接访问
		std::vector<int> InVertex;
		void Initialize() {
			InVertex = std::vector<int>(get_opList().size());
			Mark = std::vector<bool>(get_opList().size(),UNVISITED);
			for (int i = 0; i < get_opList().size(); i++) {
				InVertex[i] = get_opList()[i].InputVar.size();
			}
		}
		DataFlowGraph(basic_block& bb) {
			label = bb.get_label_name();
			int node_num = bb.get_statements().size();
			// 压入一个没有入度的空节点，这个节点关乎inputList
			ops.push_back(op());
			// 这里处理输入节点表并将这个节点提供的变量名hash到0
			// 这个操作需要在控制流完成之后完成，实际上只要检测到没有定义的变量就直接
			// 假设是从块外来的即可
			//for (int i = 0; i < inputList.size(); i++) {
			//	vartable[inputList[i].InputBlockVarName] = 0;
			//}
			for (int i = 0; i < node_num; i++) {
				//TODO
				int currentNode = i + 1;
				node CurNode(op(bb.get_statements()[i]));
				// 当前节点的输出变量放入当前块的变量表中
				if (!CurNode.element.getOutputvar().empty()) {
					vartable[CurNode.element.getOutputvar()] = currentNode;
				}
				// 处理分支指令
				// BR
				if (CurNode.element.getOPtype() == OP_BR) {
					if (CurNode.element.getInputvars().size() == 1) {
						outputlist.push_back(OutputEdge(label, CurNode.element.getInputvars()[0], UnConditonal));
					} else if (CurNode.element.getInputvars().size() == 3) {
						CurNode.InputVar.push_back(CurNode.element.getInputvars()[0]);
						// 连线失败则会在0节点和currentNode之间创建一个边，说明currentNode依赖于其他块的输出
						CreateEdge(CurNode.element.getInputvars()[0], currentNode);
						outputlist.push_back(OutputEdge(label, CurNode.element.getInputvars()[1], IfFalse));
						outputlist.push_back(OutputEdge(label, CurNode.element.getInputvars()[2], IfTrue));
					}
				}
				// 处理RET
				else if (CurNode.element.getOPtype() == OP_RET) {
					if (!CurNode.element.getInputvars().empty()) {
						CurNode.InputVar.push_back(CurNode.element.getInputvars()[0]);
						CreateEdge(CurNode, 0, currentNode);
					}
					OutputEdge returnEdge(label, CurNode.element.getInputvars()[0], UnConditonal);
					returnEdge.Isreturn = true;
					outputlist.push_back(returnEdge);
				}

				// 处理phi
				else if (CurNode.element.getOPtype() == OP_PHI) {
					for (int k = 0; k < CurNode.element.getInputvars().size(); k++) {
						if (k % 2 == 0) {
							// 将输入节点压入
							CurNode.InputVar.push_back(CurNode.element.getInputvars()[k]);
							// 连线失败则会在0节点和currentNode之间创建一个边，说明currentNode依赖于其他块的输出
							CreateEdge(CurNode, k, currentNode);
						}
					}
				}
				// 处理load
				else if (CurNode.element.getOPtype() == OP_LOAD) {
					// load操作比较特殊，需要有与memory交互的数据流
					// 取地址，然后根据地址索引
					// 访存请求建立
					memory.push_back(Memaccess(label, currentNode,
					                           CurNode.element.getInputvars()[0], CurNode.element.getInputvars()[1]));
					for(int i = 0;i<CurNode.element.getInputvars().size();i++)
						CurNode.InputVar.push_back(CurNode.element.getInputvars()[i]);
					CreateEdges(CurNode, currentNode);
				}
				// 处理正常节点
				else {
					// 节点的入度初始化
					for (int i = 0; i < CurNode.element.getInputvars().size(); i++)
						CurNode.InputVar.push_back(CurNode.element.getInputvars()[i]);
					CreateEdges(CurNode, currentNode);
				}

				ops.push_back(CurNode);
			}
		}
		DataFlowGraph() {
			OutputEdge out;
			label = "fiction_head";
			out.From_Block = label;
			outputlist.push_back(out);
		}

		// operation
		// 


		// get

		// 节点列表
		std::vector<node> &get_opList() {
			return ops;
		}
		// 输出边列表
		std::vector<OutputEdge> &get_outputEdge() {
			return outputlist;
		}
		// 输入变量列表
		std::vector<InputEdge> &get_inputList() {
			return inputList;
		}
		// 模块名
		std::string &get_label() {
			return label;
		}
		// 节点的出度节点
		std::vector<int>& ToVertex(int from) {
			return ops[from].next;
		}
		// 输出变量的哈希表
		std::unordered_map<std::string, int>& myOutvartable() {
			return vartable;
		}
};

#endif
