﻿#include "HLS.h"
#include "leftAlgorithm.h"
#include "schedule.h"
#include "Hungarian_algorithm.h"
#include "control_logic.h"
#include <iostream>
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
  void HLS::generate_CFG()
  {
	  // 通过IR生成数据流图以及控制流图
	  CFG = ControlFlowGraph(parsered);
	  // 遍历DFGs向量
	  for (int DFGNode = 0; DFGNode < getCFG().getDFGNodes().size(); DFGNode++)
	  {
		  // 遍历DFGs[DFGNode]的每一个节点，初始节点为0
		  // 将这个节点的图取出来
		  DataFlowGraph &DFG = this->getCFG().getDFGNodes()[DFGNode].DFG;
		  std::cout << std::endl;
		  std::cout << "****************   " << DFG.get_label() << "    ******************" << std::endl;
		  std::cout << std::endl;
		  std::cout << "InputList of DataflowGraph:" << std::endl;
		  for (int i = 0; i < DFG.get_inputList().size(); i++)
		  {
			  std::cout << "From Block: " + DFG.get_inputList()[i].From_Block + ": " + DFG.get_inputList()[i].InputBlockVarName << std::endl;
		  }
		  std::cout << "----------------------------" << std::endl;
		  // 图初始化
		  DFG.Initialize();
		  // 压入所有入度为0的节点
		  int CurrentNode;
		  std::queue<int> tq;
		  for (int i = 0; i < DFG.get_opList().size(); i++)
		  {
			  if (DFG.InVertex[i] == 0)
			  {
				  tq.push(i);
			  }
		  }
		  // 拓扑排序遍历
		  while (!tq.empty())
		  {
			  CurrentNode = tq.front();
			  tq.pop();
			  DFG.Mark[CurrentNode] = VISITED;
			  // 打印信息
			  std::cout << "--"
						<< "CurrentNode index in DFG: " << CurrentNode << "--" << std::endl;
			  std::cout << "---InputVar of CurrentNode: ---" << std::endl;
			  for (int i = 0; i < DFG.get_opList()[CurrentNode].InputVar.size(); i++)
				  std::cout << DFG.get_opList()[CurrentNode].InputVar[i] << std::endl;
			  std::cout << "ToVertex Index of CurrentNode: " << std::endl;
			  for (int i = 0; i < DFG.ToVertex(CurrentNode).size(); i++)
				  std::cout << "Node Index: " << DFG.ToVertex(CurrentNode)[i] << "; " << std::endl;
  
			  for (int i = 0; i < DFG.ToVertex(CurrentNode).size(); i++)
			  {
				  // 得到第i个下一节点，如果这个节点这某个输入变量依赖于当前节点
				  int nextNodeIndex = DFG.ToVertex(CurrentNode)[i];
				  node &nextNode = DFG.get_opList()[nextNodeIndex];
				  // 根据第i个节点的输入变量情况，减少入度
				  DFG.InVertex[nextNodeIndex]--;
				  // 入度小于等于0则进入队列
				  if (DFG.InVertex[DFG.ToVertex(CurrentNode)[i]] == 0)
				  {
					  tq.push(DFG.ToVertex(CurrentNode)[i]);
				  }
			  }
		  }
		  std::cout << std::endl;
		  std::cout << "OutputList of DataflowGraph:" << std::endl;
		  for (int i = 0; i < DFG.get_outputList().size(); i++)
		  {
			  std::cout << "To Block: " + DFG.get_outputList()[i].To_Block + ": " + DFG.get_outputList()[i].OutBlockVarName << std::endl;
		  }
		  std::cout << std::endl;
		  std::cout << "Branch of DataflowGraph:" << std::endl;
		  for (int i = 0; i < DFG.get_Branches().size(); i++)
		  {
			  std::cout << "With Condition: " << DFG.get_Branches()[i].cond << ","
						<< DFG.get_Branches()[i].From_Block + "->" + DFG.get_Branches()[i].To_Block << std::endl;
		  }
		  std::cout << std::endl;
		  std::cout << std::endl;
	  }
  }

void HLS::setTestTime()
{
	int T = 0;
	for (auto i = CFG.getDFGNodes().begin(); i != CFG.getDFGNodes().end(); i++)
	{
		for (auto j = i->DFG.get_opList().begin(); j != i->DFG.get_opList().end(); j++)
		{
			j->setTstart(T++);
			j->setTend(T++);
		}
	}
}

void HLS::travelaround()
{
	// 遍历DFGs向量
	for (int DFGNode = 1; DFGNode < getCFG().getDFGNodes().size(); DFGNode++)
	{
		// 遍历DFGs[DFGNode]的每一个节点，初始节点为0
		// 将这个节点的图取出来
		DataFlowGraph &DFG = this->getCFG().getDFGNodes()[DFGNode].DFG;
		// 图初始化
		DFG.Initialize();
		// 压入所有入度为0的节点
		int CurrentNode;
		std::queue<int> tq;
		for (int i = 0; i < DFG.get_opList().size(); i++)
		{
			if (DFG.InVertex[i] == 0)
			{
				tq.push(i);
			}
		}
		// 拓扑排序遍历
		while (!tq.empty())
		{
			CurrentNode = tq.front();
			tq.pop();
			DFG.Mark[CurrentNode] = VISITED;
			for (int i = 0; i < DFG.ToVertex(CurrentNode).size(); i++)
			{
				// 得到第i个下一节点，如果这个节点这某个输入变量依赖于当前节点
				int nextNodeIndex = DFG.ToVertex(CurrentNode)[i];
				node &nextNode = DFG.get_opList()[nextNodeIndex];
				// 根据第i个节点的输入变量情况，减少入度
				DFG.InVertex[nextNodeIndex]--;
				// 入度小于等于0则进入队列
				if (DFG.InVertex[DFG.ToVertex(CurrentNode)[i]] == 0 && DFG.Mark[DFG.ToVertex(CurrentNode)[i]] == UNVISITED)
				{
					tq.push(DFG.ToVertex(CurrentNode)[i]);
				}
			}
		}
	}
}

void HLS::travelback()
{
	// 遍历DFGs向量
	for (int DFGNode = 1; DFGNode < getCFG().getDFGNodes().size(); DFGNode++)
	{
		// 遍历DFGs[DFGNode]的每一个节点，初始节点为0
		// 将这个节点的图取出来
		DataFlowGraph &DFG = this->getCFG().getDFGNodes()[DFGNode].DFG;
		// 图初始化
		DFG.Initialize();
		// 压入所有入度为0的节点
		int CurrentNode;
		std::queue<int> tq;
		for (int i = 1; i < DFG.get_opList().size(); i++)
		{
			if (DFG.OutVertex[i] == 0)
			{
				tq.push(i);
			}
		}
		// 拓扑排序遍历
		while (!tq.empty())
		{
			CurrentNode = tq.front();
			tq.pop();
			DFG.Mark[CurrentNode] = VISITED;
			node &Node = DFG.get_opList()[CurrentNode];
			for (int i = 0; i < Node.InputVar.size(); i++)
			{
				// 得到当前节点的输入变量，
				int ForeNodeIndex;
				if (DFG.myOutvartable().find(Node.InputVar[i]) != DFG.myOutvartable().end())
					ForeNodeIndex = DFG.myOutvartable()[Node.InputVar[i]];
				else
					break;
				// 根据第i个节点的输入变量情况，减少前面节点的出度
				DFG.OutVertex[ForeNodeIndex]--;
				// 入度小于等于0则进入队列
				if (DFG.OutVertex[ForeNodeIndex] == 0 && DFG.Mark[ForeNodeIndex] == UNVISITED)
				{
					tq.push(ForeNodeIndex);
				}
			}
		}
	}
}

void HLS::perform_scheduling()
{
	improved_schedule_forCFG(CFG);
	std::cout << "Finsh Scheduling" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
}

void HLS::perform_register_allocation_and_binding()
{
	std::vector<graph_node> DFGS = CFG.getDFGNodes();
	REG.push_back(std::vector<std::pair<std::string, int>>());
	for (std::vector<graph_node>::iterator iter = DFGS.begin() + 1; iter != DFGS.end(); iter++)
	{
		REG.push_back(binding((*iter).DFG));
	}
	size_t len = REG.size();
	for (int i = 0; i < len; i++)
	{
		std::string lableName = DFGS[i].DFG.get_label();
		std::cout << lableName << ":" << std::endl;
		for (std::vector<std::pair<std::string, int>>::iterator iter = REG[i].begin(); iter != REG[i].end(); iter++)
		{
			std::cout << iter->first;
			std::cout << "\t" << iter->second << std ::endl;
		}
	}
	std::cout << "Finsh Register Allocation and Binding" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
}
void HLS::perform_calculate_allocation_and_binding()
{
	// std::vector<std::vector<std::pair<std::string, int>>> REG1 = getREG();
	std::vector<graph_node> &DFGS = CFG.getDFGNodes();
	std::vector<computeresource> CORE;

	int i = 0;
	std::cout << "Caculate Allocation and Binding results:" << std::endl;
	for (std::vector<graph_node>::iterator iter = DFGS.begin(); iter != DFGS.end(); iter++)
	{
		std::string lableName = DFGS[i].DFG.get_label();
		std::cout << lableName << ":" << std::endl;
		Hardware hardware;
		CSP.push_back(bindcomputeresource((iter->DFG), REG[i], CORE, hardware));
		i++;
	}
	i = 0;
	// 绑定输出寄存器
	for (; i < CSP.size(); i++)
	{
		bindoutputregister(DFGS[i].DFG, REG[i], CORE, CSP[i]);
	}
	COR = CORE;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "Finsh Calculate Allocation and Binding" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
}

void HLS::synthesize_control_logic()
{
	std::vector<std::vector<std::pair<std::string, int>>> REG1 = getREG();
	std::vector<graph_node> DFGS = CFG.getDFGNodes();
	std::vector<std::vector<std::pair<std::string, int>>>::iterator iter2 = REG.begin();

	for (int i = 0; i < DFGS.size(); i++)
	{
		DataFlowGraph dfg = DFGS[i].DFG;
		std::vector<std::pair<std::string, int>> _REG = *iter2;
		MicrocodeController controller(dfg, *iter2, COR, CSP[i]);

		controller.generateCycles(_REG, CFG);
		std::vector<Cycle> Cycle = controller.getCycle();
		controller.PrintCycle();
		Cycles.push_back(Cycle);
		iter2++;
	}
	std::cout << "Finsh Synthesize Control Logic" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
}

void HLS::genFSM()
{
	outputFSM = FSMachine(CFG, Cycles, REG);
	std::cout << "Finsh Generate Finate Sate Machine." << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
}

void HLS::outputfile()
{
	std::string filename = outputFSM.getFilename();
	std::string filepath("testfile/" + filename + ".v");
	std::vector<std::string> &modulepart = outputFSM.getModule();
	std::vector<std::string> &regDefpart = outputFSM.getRegDef();
	std::vector<std::string> &FSMpart = outputFSM.getFSM();
	std::vector<std::string> &Counterpart = outputFSM.getCounter();
	std::vector<std::string> &perPeriodPart = outputFSM.getPerPeriod();
	std::ofstream output_file(filepath);
	// 检查文件是否成功打开
	if (!output_file.is_open())
	{
		std::cerr << "Failed to open output file!" << std::endl;
		return;
	}

	// 将每一行写入文件
	for (const auto &line : modulepart)
	{
		output_file << line << std::endl;
	}

	for (const auto &line : regDefpart)
	{
		output_file << line << std::endl;
	}

	for (const auto &line : FSMpart)
	{
		output_file << line << std::endl;
	}

	for (const auto &line : Counterpart)
	{
		output_file << line << std::endl;
	}

	for (const auto &line : perPeriodPart)
	{
		output_file << line << std::endl;
	}
	output_file << "endmodule" << std::endl;
	// 关闭文件
	output_file.close();
	std::cout << "Output file: " << filepath << std::endl;
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
