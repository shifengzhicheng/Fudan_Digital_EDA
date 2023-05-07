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
            return true;
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


int max(int a,int b){
    if(a > b){
        return a;
    }
    else {
        return b;
    }
}

//ASAP结果临时存在node类的T_start中
void ASAP(DataFlowGraph& DFG){
    int lamda = 0;          //lamda标记该DFG调度的最长周期
    DFG.Initialize();
		// 压入所有入度为0的节点
	int CurrentNode;
	std::queue<int> tq;
	for (int i = 0; i < DFG.get_opList().size(); i++) {
		if (DFG.InVertex[i] == 0) {
			tq.push(i);
            DFG.get_opList()[i].setTstart(0);          //设置该DFG开始调度的起始周期为第0周期
		}
	}
	// 拓扑排序遍历
	while (!tq.empty()) {
		CurrentNode = tq.front();
        int period = DFG.get_opList()[CurrentNode].getTstart();               //得到取出这一节点被调度的周期
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
                DFG.get_opList()[DFG.ToVertex(CurrentNode)[i]].setTstart(period + 1);           //将新的入度为0的节点入表并调度在前序节点后一个周期
                lamda = max(lamda,period + 1);
            }
	    }
	}
    DFG.setPeriod(lamda);                                       //将ASAP调度所需最长周期写入DFG中
}

//ALAP结果临时存在node类的T_end中
void ALAP(DataFlowGraph& DFG){
    //获得ALAP调度的起始周期
    int lamda = DFG.getPeriod();

		// 图初始化
		DFG.Initialize();
		// 压入所有入度为0的节点
		int CurrentNode;
		std::queue<int> tq;
		for (int i = 0; i < DFG.get_opList().size(); i++) {
			if (DFG.OutVertex[i] == 0) {
				tq.push(i);
                DFG.get_opList()[i].setTstart(lamda);          //设置该DFG开始调度的起始周期为第lamda周期
			}
		}
		// 拓扑排序遍历
		while (!tq.empty()) {
			CurrentNode = tq.front();
            int period = DFG.get_opList()[CurrentNode].getTstart();
			tq.pop();
			DFG.Mark[CurrentNode] = VISITED;
            node& Node = DFG.get_opList()[CurrentNode];
			for (int i = 0; i < Node.InputVar.size(); i++) {
				int ForeNodeIndex;
				if (DFG.myOutvartable().find(Node.InputVar[i]) != DFG.myOutvartable().end()){
					ForeNodeIndex = DFG.myOutvartable()[Node.InputVar[i]];
                }
                else break;
				// 根据第i个节点的输入变量情况，减少前面节点的出度
				DFG.OutVertex[ForeNodeIndex]--;
				if (DFG.OutVertex[ForeNodeIndex] <= 0 && DFG.Mark[CurrentNode] == UNVISITED) {
					tq.push(ForeNodeIndex);
                    DFG.get_opList()[ForeNodeIndex].setTstart(period - 1);           //将新的入度为0的节点入表并调度在前序节点后一个周期
				}
			}
    }
}

bool cmp(const std::pair<int,int>& a,const std::pair<int,int>& b){
    return a.second < b.second;
}

void improved_table_schedule_forDFG(DataFlowGraph& DFG){

    //记录每一周期的资源限制，第一个int指代周期
    struct Hardware hardware_standard;
    std::map<int,struct Hardware> Record;

    //调度开始的周期初始化为第0周期
    int current_period = 0;

	DFG.Initialize();
	// 压入所有入度为0的节点
	int CurrentNode;
	std::vector<std::pair<int,int>> tq;                                 //第一个int存的是op的索引，第二个int存的是该操作被ASAP和ALAP调度的周期差
	for (int i = 0; i < DFG.get_opList().size(); i++) {
		if (DFG.InVertex[i] == 0) {
			tq.push_back(std::make_pair(i,DFG.get_opList()[i].getTend() - DFG.get_opList()[i].getTstart()));
		}
	}
		// 拓扑排序遍历
		while (!tq.empty()) {
            //一个while循环是一个周期，更新一次record
            Record.insert(std::pair<int,struct Hardware>(current_period,hardware_standard));
            std::sort(tq.begin(),tq.end(),cmp);                          //将tq中元素按照周期差升序排列，tq开始处为最小，结束处为最大，越小越先调度
            //将可调度的进行调度，并设为已访问
            for(auto iter = tq.begin();iter!=tq.end();iter++){
                if(meet_resources_constraint(Record,iter->first,DFG)){
                    DFG.get_opList()[iter->first].T_start = current_period;
                    DFG.get_opList()[iter->first].T_end = current_period;
                    DFG.Mark[iter->first] = VISITED;
                }
            }
            //根据visit标记找到已调度完的节点，将他们的后续节点减小入度，并删除已调度完的节点
            for(auto iter = tq.begin();iter!=tq.end();iter++){
                if(DFG.Mark[iter->first] == VISITED){
                    for (int i = 0; i < DFG.get_opList()[iter->first].next.size(); i++) {
				        // 得到第i个下一节点，如果这个节点这某个输入变量依赖于当前节点
				        int nextNodeIndex = DFG.get_opList()[iter->first].next[i];
				        node& nextNode = DFG.get_opList()[nextNodeIndex];
				        // 根据第i个节点的输入变量情况，减少入度
				        for (int k = 0; k < nextNode.InputVar.size(); k++) {
					        if (DFG.myOutvartable()[nextNode.InputVar[k]] == iter->first) {
						        DFG.InVertex[nextNodeIndex]--;
					        }
				        }
				        // 入度小于等于0则进入队列
				        if (DFG.InVertex[DFG.get_opList()[iter->first].next[i]] == 0 && DFG.Mark[DFG.get_opList()[iter->first].next[i]] == UNVISITED) {
					        tq.push_back(std::make_pair(DFG.get_opList()[iter->first].next[i],DFG.get_opList()[DFG.get_opList()[iter->first].next[i]].getTend() - DFG.get_opList()[DFG.get_opList()[iter->first].next[i]].getTstart()));
                        }
			        }
                tq.erase(iter);
                }
            }
            current_period++;
		}
}

//对整体的CFG进行ASAP和ALAP
void improved_schedule_forCFG(ControlFlowGraph& CFG){
	// 遍历DFGs向量
	for (int DFGNode = 0; DFGNode < CFG.getDFGNodes().size(); DFGNode++) {
		// 遍历DFGs[DFGNode]的每一个节点，初始节点为0
		// 将这个节点的图取出来
		DataFlowGraph& DFG = CFG.getDFGNodes()[DFGNode].DFG;
        improved_table_schedule_forDFG(DFG);
    }
}