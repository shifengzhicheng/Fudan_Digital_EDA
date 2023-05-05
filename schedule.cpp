#include "schedule.h"



//硬件资源考虑只有两个加法器、一个乘法器、一个SRAM、一个跳转和一个除法器
//load/store不可同时进行，比较判断需要使用加法器
bool meet_resources_constraint(std::map<int,struct Hardware>& rec,int i,DataFlowGraph& DFG){
    auto iter = rec.find(i);
    switch(DFG.get_opList()[i].element.getOPtype()){
        case OP_ASSIGN:
            return true;
            break;
        case OP_ADD:
            if(iter->second.adder<1){
                return false;
            }
            else{
                iter->second.adder--;
                return true;
            }
            break;
        case OP_SUB:
            if(iter->second.adder<1){
                return false;
            }
            else{
                iter->second.adder--;
                return true;
            }
            break;
        case OP_MUL:
            if(iter->second.mul<1){
                return false;
            }
            else{
                iter->second.mul--;
                return true;
            }
            break;
    case OP_DIV:
            if(iter->second.div<1){
                return false;
            }
            else{
                iter->second.div--;
                return true;
            }
            break;
    case OP_LOAD:
            if(iter->second.sram<1){
                return false;
            }
            else{
                iter->second.sram--;
                return true;
            }
            break;
    case OP_STORE:
            if(iter->second.sram<1){
                return false;
            }
            else{
                iter->second.sram--;
                return true;
            }
            break;
    case OP_BR:
            if(iter->second.jmp<1){
                return false;
            }
            else{
                iter->second.jmp--;
                return true;
            }
            break;
    case OP_LT:
            if(iter->second.adder<1){
                return false;
            }
            else{
                iter->second.adder--;
                return true;
            }
            break;
    case OP_GT:
            if(iter->second.adder<1){
                return false;
            }
            else{
                iter->second.adder--;
                return true;
            }
            break;
    case OP_LE:
            if(iter->second.adder<1){
                return false;
            }
            else{
                iter->second.adder--;
                return true;
            }
            break;
    default:
        return false;
    }

}


//简易的随机列表选择调度法
void table_schedule(ControlFlowGraph& CFG){
    //记录每一周期的资源限制，第一个int指代周期
    struct Hardware hardware_standard;
    std::map<int,struct Hardware> Record;

//标记上一个DFG通过调度后所需的最长周期
    int current_period = 1;

	// 遍历DFGs向量
	for (int DFGNode = 0; DFGNode < CFG.getDFGNodes().size(); DFGNode++) {

		// 遍历DFGs[DFGNode]的每一个节点，初始节点为0
		// 将这个节点的图取出来
		DataFlowGraph& DFG = CFG.getDFGNodes()[DFGNode].DFG;
		// 图初始化
		DFG.Initialize();
		// 压入所有入度为0的节点
		int CurrentNode;
		std::vector<int> tq;
		for (int i = 0; i < DFG.get_opList().size(); i++) {
			if (DFG.InVertex[i] == 0) {
				tq.push_back(i);
			}
		}
		// 拓扑排序遍历
		while (!tq.empty()) {
            //一个while循环是一个周期，更新一次record
            Record.insert(std::pair<int,struct Hardware>(current_period,hardware_standard));
            for(auto iter = tq.begin();iter!=tq.end();iter++){
                if(meet_resources_constraint(Record,*iter,DFG)){
                    DFG.get_opList()[*iter].T_start = current_period;
                    DFG.Mark[*iter] = VISITED;


                    tq.erase(iter);
                }
            }
            for(auto iter = tq.begin();iter!=tq.end();iter++){
                if(DFG.Mark[*iter] == VISITED){
                    for (int i = 0; i < DFG.get_opList()[*iter].next.size(); i++) {
				        // 得到第i个下一节点，如果这个节点这某个输入变量依赖于当前节点
				        int nextNodeIndex = DFG.get_opList()[*iter].next[i];
				        node& nextNode = DFG.get_opList()[nextNodeIndex];
				        // 根据第i个节点的输入变量情况，减少入度
				        for (int k = 0; k < nextNode.InputVar.size(); k++) {
					        if (DFG.myOutvartable()[nextNode.InputVar[k]] == *iter) {
						        DFG.InVertex[nextNodeIndex]--;
					        }
				        }
				        // 入度小于等于0则进入队列
				        if (DFG.InVertex[DFG.get_opList()[*iter].next[i]] == 0 && DFG.Mark[DFG.get_opList()[*iter].next[i]] == UNVISITED) {
					        tq.push_back(DFG.get_opList()[*iter].next[i]);
                        }
			        }
                tq.erase(iter);
                }
            }
            current_period++;
		}
        //一个DFG调度完了
        current_period++;

    }
}








/*修改数据结构如果可以做ALAP，可以实现更优的列表选择
//pair里面第一个int是节点索引值，第二个int是该节点ASAP调度的周期，第三个int是该节点ALAP调度的周期
std::vector<std::map<int,std::pair<int,int>>> ASAP_ALAP(ControlFlowGraph& CFG){
    //标记上一个DFG通过ASAP调度后所需的最长周期
    int lamda = 0;
    std::vector<std::pair<int,int>> schedule_forCFG;

	// 遍历DFGs向量
	for (int DFGNode = 0; DFGNode < CFG.getDFGNodes().size(); DFGNode++) {
        std::map<int,std::pair<int,int>> schedule_forDFG;

		// 遍历DFGs[DFGNode]的每一个节点，初始节点为0
		// 将这个节点的图取出来
		DataFlowGraph& DFG = CFG.getDFGNodes()[DFGNode].DFG;
		// 图初始化
		DFG.Initialize();
		// 压入所有入度为0的节点
		int CurrentNode;
		std::queue<int> tq;
		for (int i = 0; i < DFG.get_opList().size(); i++) {
			if (DFG.InVertex[i] == 0) {
				tq.push(i);
                schedule_forDFG.insert(std::pair<int,std::pair<int,int>>(i,std::make_pair(lamda + 1,0)));
			}
		}
		// 拓扑排序遍历
		while (!tq.empty()) {
			CurrentNode = tq.front();
            int period = schedule_forDFG.find(CurrentNode)->second.first;
			tq.pop();
			DFG.Mark[CurrentNode] = VISITED;
			for (int i = 0; i < DFG.get_opList()[CurrentNode].next.size(); i++) {
				// 得到第i个下一节点，如果这个节点这某个输入变量依赖于当前节点
				int nextNodeIndex = DFG.get_opList()[CurrentNode].next[i];
				node& nextNode = DFG.get_opList()[nextNodeIndex];
				// 根据第i个节点的输入变量情况，减少入度
				for (int k = 0; k < nextNode.InputVar.size(); k++) {
					if (DFG.myOutvartable()[nextNode.InputVar[k]] == CurrentNode) {
						DFG.InVertex[nextNodeIndex]--;
					}
				}
				// 入度小于等于0则进入队列
				if (DFG.InVertex[DFG.get_opList()[CurrentNode].next[i]] == 0 && DFG.Mark[DFG.get_opList()[CurrentNode].next[i]] == UNVISITED) {
					tq.push(DFG.get_opList()[CurrentNode].next[i]);
                    schedule_forDFG.insert(std::pair<int,std::pair<int,int>>(DFG.get_opList()[CurrentNode].next[i],std::make_pair(period + 1,0)));
                    lamda = period + 1;
                }
			}
		}
        //一个DFG图的ASAP做完了，接下去做ALAP



        for (int i = 0; i < DFG.get_opList().size(); i++) {
			if (DFG.ToVertex(i).size() == 0) {
				tq.push(i);
                schedule_forDFG.find(i)->second.second = lamda;
			}
		}
		// 拓扑排序遍历
		while (!tq.empty()) {
			CurrentNode = tq.front();
            int period = schedule_forDFG.find(CurrentNode)->second.second;
			tq.pop();
			DFG.Mark[CurrentNode] = VISITED;
			for (int i = 0; i < DFG.get_opList()[CurrentNode].next.size(); i++) {
				// 得到第i个下一节点，如果这个节点这某个输入变量依赖于当前节点
				int nextNodeIndex = DFG.get_opList()[CurrentNode].next[i];
				node& nextNode = DFG.get_opList()[nextNodeIndex];
				// 根据第i个节点的输入变量情况，减少入度
				for (int k = 0; k < nextNode.InputVar.size(); k++) {
					if (DFG.myOutvartable()[nextNode.InputVar[k]] == CurrentNode) {
						DFG.InVertex[nextNodeIndex]--;
					}
				}
				// 入度小于等于0则进入队列
				if (DFG.InVertex[DFG.get_opList()[CurrentNode].next[i]] <= 0) {
					tq.push(DFG.get_opList()[CurrentNode].next[i]);
				}
			}
		}
        alap.push_back(tq);

}
}

*/
//数据结构少东西，ALAP做不了





