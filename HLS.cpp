#include "HLS.h"
#include "leftAlgorithm.h"
#include "schedule.h"
#include <queue>
// 第一部分是基于读到的资源创建块的关系图，块内是不同的有向无环图
/* 这个部分接收的参数是基本块的向量bbs，函数的fn_name，输入的变量vars，以及返回值ret_type
  基本块的结构包含vector<statement> _statements和_label，
  statement是IR语句中的一句代码，有返回值的操作可以使用get_var()得到返回值
  get_type()可以得到语句的操作类型
  get_num_oprands()可以得到操作数的数量，
  然后get_oprand(k)得到第k个操作数(从0开始)
  最后这个操作将整个函数变成一个完整的，因为循环存在，图是有环的
  但因为phi操作代表的依赖是或关系，而且中间存在branch */
void HLS::generate_CFG() {
	// 通过IR生成数据流图以及控制流图
	CFG = ControlFlowGraph(parsered);
}

void HLS::setTestTime() {
	int T = 0;
	for (auto i = CFG.getDFGNodes().begin(); i != CFG.getDFGNodes().end(); i++) {
		for (auto j = i->DFG.get_opList().begin(); j != i->DFG.get_opList().end(); j++) {
			j->setTstart(T++);
			j->setTend(T++);
		}
	}
}

void HLS::travelaround() {
	// 遍历DFGs向量
	for (int DFGNode = 1; DFGNode < getCFG().getDFGNodes().size(); DFGNode++) {
		// 遍历DFGs[DFGNode]的每一个节点，初始节点为0
		// 将这个节点的图取出来
		DataFlowGraph& DFG = this->getCFG().getDFGNodes()[DFGNode].DFG;
		// 图初始化
		DFG.Initialize();
		// 压入所有入度为0的节点
		int CurrentNode;
		std::queue<int> tq;
		for (int i = 0; i < DFG.get_opList().size(); i++) {
			if (DFG.InVertex[i] == 0) {
				tq.push(i);
			}
		}
		// 拓扑排序遍历
		while (!tq.empty()) {
			CurrentNode = tq.front();
			tq.pop();
			DFG.Mark[CurrentNode] = VISITED;
			for (int i = 0; i < DFG.ToVertex(CurrentNode).size(); i++) {
				// 得到第i个下一节点，如果这个节点这某个输入变量依赖于当前节点
				int nextNodeIndex = DFG.ToVertex(CurrentNode)[i];
				node& nextNode = DFG.get_opList()[nextNodeIndex];
				// 根据第i个节点的输入变量情况，减少入度
				for (int k = 0; k < nextNode.InputVar.size(); k++) {
					if (DFG.myOutvartable().find(nextNode.InputVar[k])!= DFG.myOutvartable().end() && DFG.myOutvartable()[nextNode.InputVar[k]] == CurrentNode) {
						DFG.InVertex[nextNodeIndex]--;
					}
				}
				// 入度小于等于0则进入队列
				if (DFG.InVertex[DFG.ToVertex(CurrentNode)[i]] <= 0 && DFG.Mark[CurrentNode] == UNVISITED) {
					tq.push(DFG.ToVertex(CurrentNode)[i]);
				}
			}
		}
	}
}

void HLS::travelback() {
	// 遍历DFGs向量
	for (int DFGNode = 1; DFGNode < getCFG().getDFGNodes().size(); DFGNode++) {
		// 遍历DFGs[DFGNode]的每一个节点，初始节点为0
		// 将这个节点的图取出来
		DataFlowGraph& DFG = this->getCFG().getDFGNodes()[DFGNode].DFG;
		// 图初始化
		DFG.Initialize();
		// 压入所有入度为0的节点
		int CurrentNode;
		std::queue<int> tq;
		for (int i = 1; i < DFG.get_opList().size(); i++) {
			if (DFG.OutVertex[i] == 0) {
				tq.push(i);
			}
		}
		// 拓扑排序遍历
		while (!tq.empty()) {
			CurrentNode = tq.front();
			tq.pop();
			DFG.Mark[CurrentNode] = VISITED;
			node& Node = DFG.get_opList()[CurrentNode];
			for (int i = 0; i < Node.InputVar.size(); i++) {
				// 得到当前节点的输入变量，
				int ForeNodeIndex;
				if (DFG.myOutvartable().find(Node.InputVar[i]) != DFG.myOutvartable().end())
					ForeNodeIndex = DFG.myOutvartable()[Node.InputVar[i]];
				else break;
				// 根据第i个节点的输入变量情况，减少前面节点的出度
				DFG.OutVertex[ForeNodeIndex]--;
				// 入度小于等于0则进入队列
				if (DFG.OutVertex[ForeNodeIndex] <= 0 && DFG.Mark[CurrentNode] == UNVISITED) {
					tq.push(ForeNodeIndex);
				}
			}
		}
	}
}

void HLS::perform_scheduling(){
        improved_schedule_forCFG(CFG);
}

void HLS::perform_register_allocation_and_binding() {
	std::vector<graph_node> DFGS = CFG.getDFGNodes();
	for (std::vector<graph_node>::iterator iter = DFGS.begin(); iter != DFGS.end(); iter++)
	{
		REG.push_back(binding((*iter).DFG));
	}
}
void HLS::perform_calculate_allocation_and_binding() {
	std::vector<std::vector<std::pair<std::string,int>>> REG1 = getREG();
	std::vector<graph_node> DFGS = CFG.getDFGNodes();
	vector<computeresource> CORE;
	std::vector<std::vector<std::pair<std::string, int>>>::iterator iter2 = REG.begin();
	for (std::vector<graph_node>::iterator iter = DFGS.begin(); iter != DFGS.end(); iter++)
	{
		CSP.push_back(bindcomputeresource((iter->DFG), *iter2, CORE));
		iter2++;
	}
	COR = CORE;
}

// 第二部分是基于有向无环图进行拓扑排序，得到每个计算单元的一个基本的时序约束
/* 这个部分接收第一部分生成的图graph进行拓扑排序，包括带周期的ASAP和带周期的ALAP，
  这部分信息将被存储在图的每个节点中 */

  // 第三部分在基本的时序约束的基础上再基于资源进行整数线性规划得到最优的Latency
  /* 这个部分接收上面的带有周期约束条件的节点，进行ILP，实现Latency最小的SDC描述并计算出图中所有节点的运行周期 */

  // 第四部分得到了最优Latency以后，还要这个时候每个单元的busy周期，并将数据与寄存器进行绑定，
  // 假定寄存器数量没有上限，但是需要使用尽可能少的寄存器
  /* 这个部分基于已经有的单元的时序情况分配寄存器来存储不同的数据 */

  // 第五部分对计算的资源同样进行绑定，在有限的计算资源约束下完成与寄存器的配对，需要加入选择器进行配对
  // 使用匈牙利算法或者最小代价匹配实现

  // 第六部分是控制逻辑综合，控制不同块之间的跳转，最终的代码逻辑的实现部分






