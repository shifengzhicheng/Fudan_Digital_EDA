#include "schedule.h"

//硬件资源考虑只有两个加法器、一个乘法器、一个SRAM、一个跳转和一个除法器
//load/store不可同时进行，比较判断需要使用加法器
bool meet_resources_constraint(Hardware& rec, int i, DataFlowGraph& DFG) {
    switch (DFG.get_opList()[i].element.getOPtype()) {
    case OP_ASSIGN:
    case OP_BR:
        return true;
        break;
    case OP_ADD:
    case OP_SUB:
    case OP_LT:
    case OP_GT:
    case OP_LE:
        if (rec.get_available_adder() < 1) {
            return false;
        }
        else {
            rec.set_available_adder(rec.get_available_adder() - 1);
            return true;
        }
        break;
    case OP_MUL:
        if (rec.get_available_mul() < 1) {
            return false;
        }
        else {
            rec.set_available_mul(rec.get_available_mul() - 1);
            return true;
        }
        break;
    case OP_DIV:
        if (rec.get_available_div() < 1) {
            return false;
        }
        else {
            rec.set_available_div(rec.get_available_div() - 1);
            return true;
        }
        break;
    case OP_LOAD:
    case OP_STORE:
        if (rec.get_available_sram() < 1) {
            return false;
        }
        else {
            rec.set_available_sram(rec.get_available_sram() - 1);
            return true;
        }
        break;
    default:
        return true;
    }
}

void reset(Hardware& hardware, int i, DataFlowGraph& DFG) {
    switch (DFG.get_opList()[i].element.getOPtype()) {
    case OP_ADD:
    case OP_SUB:
    case OP_LT:
    case OP_GT:
    case OP_LE:
        hardware.set_available_adder(hardware.get_available_adder() + 1);
        break;
    case OP_MUL:
        hardware.set_available_mul(hardware.get_available_mul() + 1);
        break;
    case OP_DIV:
        hardware.set_available_div(hardware.get_available_div() + 1);
        break;
    case OP_LOAD:
    case OP_STORE:
        hardware.set_available_sram(hardware.get_available_sram() + 1);
        break;
    default:;
    }
}


int max(int a, int b) {
    if (a > b) {
        return a;
    }
    else {
        return b;
    }
}

int min(int a, int b) {
    if (a < b) {
        return a;
    }
    else {
        return b;
    }
}

//ASAP结果临时存在node类的T_start中
void ASAP(DataFlowGraph& DFG, Period_Rec& REC) {
    int lamda = 1;          //lamda标记该DFG调度的最长周期
    DFG.Initialize();
    // 压入所有入度为0的节点
    int CurrentNode;
    std::queue<int> tq;

    //在0周期由于数据结构先独立对头虚节点进行处理
    REC.getASAP()[0].first = 0;
    REC.getASAP()[0].second = 0;
    DFG.Mark[0] = VISITED;
    //将头虚节点的后续节点入度减少
    for (int i = 0; i < DFG.ToVertex(0).size(); i++) {
        int nextNodeIndex = DFG.ToVertex(0)[i];
        DFG.InVertex[nextNodeIndex]--;
    }

    //将入度为0的节点入队列
    for (int i = 0; i < DFG.get_opList().size(); i++) {
        if (DFG.InVertex[i] == 0) {
            tq.push(i);
            //DFG开始调度的起始周期为第1周期
            REC.getASAP()[i].first = 1;
            REC.getASAP()[i].second = DFG.get_opList()[i].element.getLatency();
        }
    }

    // 拓扑排序遍历
    while (!tq.empty()) {
        CurrentNode = tq.front();
        int period = REC.getASAP()[CurrentNode].first;               //得到取出这一节点被调度的起始周期
        tq.pop();
        DFG.Mark[CurrentNode] = VISITED;

        for (int i = 0; i < DFG.ToVertex(CurrentNode).size(); i++) {
            int nextNodeIndex = DFG.ToVertex(CurrentNode)[i];
            DFG.InVertex[nextNodeIndex]--;
            //这一后序节点的调度周期至少不早于该周期
            REC.getASAP()[nextNodeIndex].first = max(REC.getASAP()[nextNodeIndex].first, REC.getASAP()[CurrentNode].second + 1);
            REC.getASAP()[nextNodeIndex].second = REC.getASAP()[nextNodeIndex].first + DFG.get_opList()[nextNodeIndex].element.getLatency() - 1;
            // 入度小于等于0则进入队列
            if (DFG.InVertex[nextNodeIndex] <= 0 && DFG.Mark[nextNodeIndex] == UNVISITED) {
                tq.push(nextNodeIndex);
                //将新的入度为0的节点入表,它的调度周期已被确认为前序节点最长运算周期后一个周期
                //记录整个DFG图所需的最长执行周期
                lamda = max(lamda, REC.getASAP()[nextNodeIndex].second);
            }
        }
    }

    DFG.setPeriod(lamda);                                       //将ASAP调度所需最长周期写入DFG中

    //单独对尾虚节点进行处理
    REC.getASAP()[DFG.get_opList().size() - 1].first = lamda + 1;
    REC.getASAP()[DFG.get_opList().size() - 1].second = lamda + 1;
    DFG.Mark[DFG.get_opList().size() - 1] = VISITED;
}

//ALAP结果临时存在node类的T_end中
void ALAP(DataFlowGraph& DFG, Period_Rec& REC) {
    //获得ALAP调度的起始周期
    int lamda = DFG.getPeriod();

    // 图初始化
    DFG.Initialize();
    // 压入所有入度为0的节点
    int CurrentNode;
    std::queue<int> tq;

    //在lamda周期由于数据结构先独立对尾虚节点进行处理
    REC.getALAP()[DFG.get_opList().size() - 1].first = lamda + 1;
    REC.getALAP()[DFG.get_opList().size() - 1].second = lamda + 1;
    DFG.Mark[DFG.get_opList().size() - 1] = VISITED;

    node& Tail = DFG.get_opList()[DFG.get_opList().size() - 1];
    for (int i = 0; i < Tail.InputVar.size(); i++) {
        int ForeNodeIndex;
        if (DFG.myOutvartable().find(Tail.InputVar[i]) != DFG.myOutvartable().end()) {
            ForeNodeIndex = DFG.myOutvartable()[Tail.InputVar[i]];
        }
        else break;
        DFG.OutVertex[ForeNodeIndex]--;
    }

    for (int i = 0; i < DFG.get_opList().size(); i++) {
        if (DFG.OutVertex[i] <= 0) {
            tq.push(i);
            REC.getALAP()[i].second = lamda;
            REC.getALAP()[i].first = lamda - DFG.get_opList()[i].element.getLatency() + 1;
        }
    }

    while (!tq.empty()) {
        CurrentNode = tq.front();
        int period = DFG.get_opList()[CurrentNode].getTend();
        tq.pop();
        DFG.Mark[CurrentNode] = VISITED;
        node& Node = DFG.get_opList()[CurrentNode];
        for (int i = 0; i < Node.InputVar.size(); i++) {
            int ForeNodeIndex;
            if (DFG.myOutvartable().find(Node.InputVar[i]) != DFG.myOutvartable().end()) {
                ForeNodeIndex = DFG.myOutvartable()[Node.InputVar[i]];
            }
            else break;
            // 根据第i个节点的输入变量情况，减少前面节点的出度
            DFG.OutVertex[ForeNodeIndex]--;
            REC.getALAP()[ForeNodeIndex].second = min(REC.getALAP()[ForeNodeIndex].second, REC.getALAP()[CurrentNode].first - 1);
            REC.getALAP()[ForeNodeIndex].first = REC.getALAP()[ForeNodeIndex].second - DFG.get_opList()[ForeNodeIndex].element.getLatency() + 1;

            if (DFG.OutVertex[ForeNodeIndex] <= 0 && DFG.Mark[ForeNodeIndex] == UNVISITED) {
                tq.push(ForeNodeIndex);
            }
        }
    }

    //单独对头虚节点进行处理
    REC.getASAP()[0].first = 0;
    REC.getASAP()[0].second = 0;
    DFG.Mark[0] = VISITED;
}

bool cmp(const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return a.second < b.second;
}

void improved_table_schedule_forDFG(DataFlowGraph& DFG) {
 
    Period_Rec REC(DFG);
    ASAP(DFG, REC);
    ALAP(DFG, REC);

    Hardware hardware_default;

    std::vector<int> Exe_Time_L;
    Exe_Time_L.push_back(0);
    for (int i = 1; i < DFG.get_opList().size() - 1; i++) {
        Exe_Time_L.push_back(DFG.get_opList()[i].element.getLatency());
    }
    Exe_Time_L.push_back(0);


    //调度开始的周期初始化为第1周期
    int current_period = 1;

    DFG.Initialize();
    // 压入所有入度为0的节点
    int CurrentNode;
    std::map<int, int> tq_m;                    //第一个int存的是op的索引，第二个int存的是该操作被ASAP和ALAP调度的周期差

    //在0周期由于数据结构先独立对头虚节点进行处理
    DFG.get_opList()[0].setTstart(0);
    DFG.get_opList()[0].setTend(0);
    DFG.Mark[0] = VISITED;
    //将头虚节点的后续节点入度减少
    for (int i = 0; i < DFG.ToVertex(0).size(); i++) {
        int nextNodeIndex = DFG.ToVertex(0)[i];
        DFG.InVertex[nextNodeIndex]--;
    }

    for (int i = 1; i < DFG.get_opList().size(); i++) {
        if (DFG.InVertex[i] <= 0) {
            tq_m.insert(std::pair<int, int>(i, REC.getALAP()[i].first - REC.getASAP()[i].first));
            DFG.Mark[i] = VISITED;
        }
    }

    int lamda = DFG.getPeriod();

    // 拓扑排序遍历
    while (!tq_m.empty()) {
        //一个while循环是一个周期

        //将tq中元素按照周期差升序排列，tq开始处为最小，结束处为最大，越小越先调度
        std::vector<std::pair<int, int>> tq(tq_m.begin(), tq_m.end());
        std::sort(tq.begin(), tq.end(), cmp);

        //将可调度的进行调度，并设为已访问
        for (auto itera = tq.begin(); itera != tq.end(); itera++) {
            if (DFG.Mark[itera->first] == SCHEDULED) {}
            else{
                if (meet_resources_constraint(hardware_default, itera->first, DFG)) {
                    DFG.get_opList()[itera->first].T_start = current_period;
                    DFG.get_opList()[itera->first].T_end = current_period + DFG.get_opList()[itera->first].element.getLatency() - 1;
                    lamda = max(lamda, DFG.get_opList()[itera->first].T_end);
                    DFG.Mark[itera->first] = SCHEDULED;
                }
            }
        }

        //根据visit标记找到该周期被调度过且已执行完的节点，将他们的后续节点减小入度，并删除已执行完的节点
        for (auto iter = tq.begin(); iter != tq.end(); iter++) {
            //计算执行周期
            int index1 = iter->first;
            if (DFG.Mark[index1] == SCHEDULED) {
                Exe_Time_L[index1]--;
            }

            if (DFG.Mark[index1] == SCHEDULED && Exe_Time_L[index1] <= 0) {
                //减小后序节点的入度
                for (int i = 0; i < DFG.ToVertex(index1).size(); i++) {
                    int nextNodeIndex = DFG.ToVertex(index1)[i];
                    node& nextNode = DFG.get_opList()[nextNodeIndex];
                    DFG.InVertex[nextNodeIndex]--;

                    // 入度小于等于0则进入队列
                    if (DFG.InVertex[nextNodeIndex] <= 0 && DFG.Mark[nextNodeIndex] == UNVISITED) {
                        tq_m.insert(std::pair<int, int>(nextNodeIndex, REC.getALAP()[nextNodeIndex].first - REC.getASAP()[nextNodeIndex].first));
                    }
                }
                //恢复硬件资源
                reset(hardware_default, index1, DFG);

                tq_m.erase(index1);
            }
        }
        current_period++;
    }

    DFG.get_opList()[DFG.get_opList().size() - 1].setTstart(lamda + 1);
    DFG.get_opList()[DFG.get_opList().size() - 1].setTend(lamda + 1);
    DFG.Mark[DFG.get_opList().size() - 1] = VISITED;
    DFG.setPeriod(lamda);
}

//对整体的CFG进行ASAP和ALAP
void improved_schedule_forCFG(ControlFlowGraph& CFG) {
    // 遍历DFGs向量
    for (int DFGNode = 1; DFGNode < CFG.getDFGNodes().size(); DFGNode++) {
        // 遍历DFGs[DFGNode]的每一个节点，初始节点为0
        // 将这个节点的图取出来
        DataFlowGraph& DFG = CFG.getDFGNodes()[DFGNode].DFG;
        improved_table_schedule_forDFG(DFG);
    }
}
