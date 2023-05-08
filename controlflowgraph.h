#ifndef CONTROLFLOWGRAPH_H
#define CONTROLFLOWGRAPH_H
#include "dataflowgraph.h"

struct graph_node {
	DataFlowGraph DFG;
	std::vector<BranchEdge> controlnodes;//相关的边
	graph_node(DataFlowGraph &_DFG): DFG(_DFG), controlnodes(DFG.get_Branches()) {}
};

class ControlFlowGraph {
	private:
		// 控制流图需要对应不同的块之间的跳转关系，并生成块之间的数据依赖关系
		std::string func_name;
		int ret_type;
		std::vector<var> vars;
		// 通过块的标签确定块的下标
		std::unordered_map<std::string, int> IndexofDFG;
		// DFG节点向量，装了所有的DFG，其中，DFGs[0]是一个空的DFG，它的作用是传函数的输入变量
		std::vector<graph_node> DFGs;
		// 寻找变量的生成块
		DataFlowGraph* findvarfrom(std::string VarName);
	public:

		// CFG图的创建
		ControlFlowGraph() {};
		ControlFlowGraph(parser& p);

		// 获得CFG图中的DFG节点
		std::vector<graph_node>& getDFGNodes();

		// 获得DFG在节点向量中的下标
		int getIndex(std::string label);

		//获取下一个块所指向的块的下标
		std::vector<int> NextNode(std::string label);

		std::vector<int> NextNode(int index);
		// 返回信息的接口
		std::string getfuncname() {
			return func_name;
		}
		std::vector<var> &getvar() {
			return vars;
		}
		int getRet_type() const {
			return ret_type;
		}

};

#endif
