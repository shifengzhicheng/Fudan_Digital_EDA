#ifndef CONTROLFLOWGRAPH_H
#define CONTROLFLOWGRAPH_H
#include "dataflowgraph.h"

struct graph_node {
	DataFlowGraph DFG;
	std::vector<OutputEdge> controlnodes;//相关的边
	graph_node(DataFlowGraph &_DFG): DFG(_DFG), controlnodes(DFG.get_outputEdge()) {}
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
	public:
		std::vector<graph_node>& getDFGNodes() {
			return DFGs;
		}
		int getIndex(std::string label) {
			if (IndexofDFG.find(label) != IndexofDFG.end()) {
				return IndexofDFG[label];
			}
			else return 0;
		}
		ControlFlowGraph() {};
		ControlFlowGraph(parser &p) {
			// 函数的函数名
			func_name = p.get_function_name();
			// 函数的返回类型
			ret_type = p.get_ret_type();
			// 函数变量
			vars = p.get_function_params();
			// 先将所有的块放到控制流图中
			// 首先生成一个空块来装函数的输入变量
			{
				DataFlowGraph dfg;
				dfg.get_opList().push_back(op());
				dfg.get_opList().push_back(op());
				for (int i = 0; i < vars.size(); i++) {
					IndexofDFG[dfg.get_label()] = 0;
					dfg.myOutvartable()[vars[i]._name] = 1;
				}
				dfg.get_outputEdge()[0].To_Block = p.get_basic_blocks()[0].get_label_name();
				DFGs.push_back(graph_node(dfg));
			}
			// 这一步是初步把块生成图
			for (int i = 0; i < p.get_basic_blocks().size(); i++) {
				int currentBlock = i + 1;
				basic_block &bb = p.get_basic_blocks()[i];
				DataFlowGraph DFG(bb);
				IndexofDFG[DFG.get_label()] = currentBlock;
				if (DFG.get_outputEdge().empty()) {
					DFG.get_outputEdge().push_back(OutputEdge(DFG.get_label(),p.get_basic_blocks()[i+1].get_label_name(),UnConditonal));
				}
				DFGs.push_back(graph_node(DFG));
				// 到这里已经完成了分支出度的边OutputEdge
				// 基本节点的生成，节点内数据的依赖
			}
			// 整理DFGs中的数据流关系，并根据DFG中的opList生成DFG的inputList
			for (int i = 0; i < DFGs.size(); i++) {
				//TODO
				for (int j = 0; j < DFGs[i].DFG.get_inputList().size(); j++) {
					// 块中变量与外部块对应
					if (DFGs[i].DFG.get_inputList()[j].From_Block.empty()) {
						std::string label= findvarfrom(DFGs[i].DFG.get_inputList()[j].InputBlockVarName);
						DFGs[i].DFG.get_inputList()[j].From_Block = label;
					}
				}
			}
			// inputList生成完毕，至此，所有的依赖已经完成
		}

		// Operation
		// 这个操作将返回NextNode在graph_node中的索引数组
		std::vector<int> NextNode(std::string label) {
			std::vector<int> nn;
			int index = IndexofDFG[label];
			for (int i = 0; i < DFGs[index].controlnodes.size(); i++) {
				nn.push_back(IndexofDFG[DFGs[index].controlnodes[i].To_Block]);
			}
			return nn;
		}
		std::vector<int> NextNode(int index) {
			std::vector<int> nn;
			for (int i = 0; i < DFGs[index].controlnodes.size(); i++) {
				nn.push_back(IndexofDFG[DFGs[index].controlnodes[i].To_Block]);
			}
			return nn;
		}
		std::string findvarfrom(std::string VarName) {
			for (int i = 0; i < DFGs.size(); i++) {
				// 如果在块的输出变量表中找到了变量，那么就将这个块的标签返回
				std::unordered_map<std::string, int> &map = DFGs[i].DFG.myOutvartable();
				if (map.find(VarName) != map.end() && map[VarName] != 0) {
					return DFGs[i].DFG.get_label();
				}
			}
			return "CANNOTFIND";
		}

};

#endif
