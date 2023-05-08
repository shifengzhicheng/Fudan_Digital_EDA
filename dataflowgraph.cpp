#include "dataflowgraph.h"

int op::set_T(int type) {
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

void DataFlowGraph::CreateEdge(int from, int to) {
    if (from < opList.size()) {
        //TODO
        // 在from和to之间加边，同时to的入度++
        // from的next数组中push进了to
        opList[from].next.push_back(to);
    }
}
// 判断是否是纯数字
bool DataFlowGraph::ispureNumber(std::string InputVar) {
    bool isPureNumber = true;
    for (char c : InputVar) {
        if (!std::isdigit(c)) {
            isPureNumber = false;
            break;
        }
    }
    return isPureNumber;
}
// 找到节点的输入变量的输出节点的index，然后在其next中加上to
void DataFlowGraph::CreateEdge(std::string Inputvar, int to) {
    CreateEdge(vartable[Inputvar], to);
}
void DataFlowGraph::CreateEdges(node& CurNode, int to) {
    for (int i = 0; i < CurNode.element.getInputvars().size(); i++)
    {
        CreateEdge(CurNode, i, to);
    }
}
void DataFlowGraph::CreateEdge(node& CurNode, int i, int to) {
    std::string InputVar = CurNode.element.getInputvars()[i];
    bool isPureNumber = ispureNumber(InputVar);
    if (isPureNumber) {
        return;
    }
    bool AlreadyIn = vartable.find(InputVar) != vartable.end();
    if (AlreadyIn && vartable[InputVar] != 0) {
        CreateEdge(vartable[InputVar], to);
    }
    else {
        if (!AlreadyIn) {
            inputList.push_back(InputEdge(InputVar));
            vartable[InputVar] = 0;
        }
        CreateEdge(0, to);
    }
}

DataFlowGraph::DataFlowGraph(basic_block& bb) {
    label = bb.get_label_name();
    int node_num = bb.get_statements().size();
    // 压入一个没有入度的空节点，这个节点关乎inputList
    opList.push_back(op());
    // 这里处理输入节点表并将这个节点提供的变量名hash到0
    // 这个操作需要在控制流完成之后完成，实际上只要检测到没有定义的变量就直接
    // 假设是从块外来的即可
    //for (int i = 0; i < inputList.size(); i++) {
    //  vartable[inputList[i].InputBlockVarName] = 0;
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
                Branchs.push_back(BranchEdge(label, CurNode.element.getInputvars()[0], UnConditonal));
            }
            else if (CurNode.element.getInputvars().size() == 3) {
                CurNode.InputVar.push_back(CurNode.element.getInputvars()[0]);
                // 连线失败则会在0节点和currentNode之间创建一个边，说明currentNode依赖于其他块的输出
                CreateEdge(CurNode, 0, currentNode);
                Branchs.push_back(BranchEdge(label, CurNode.element.getInputvars()[1], IfFalse));
                Branchs.push_back(BranchEdge(label, CurNode.element.getInputvars()[2], IfTrue));
            }
        }
        // 处理RET
        else if (CurNode.element.getOPtype() == OP_RET) {
            if (!CurNode.element.getInputvars().empty()) {
                CurNode.InputVar.push_back(CurNode.element.getInputvars()[0]);
                CreateEdge(CurNode, 0, currentNode);
            }
            BranchEdge returnEdge(label, CurNode.element.getInputvars()[0], UnConditonal);
            returnEdge.Isreturn = true;
            Branchs.push_back(returnEdge);
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
            for (int i = 0; i < CurNode.element.getInputvars().size(); i++)
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

        opList.push_back(CurNode);
    }
    opList.push_back(op());
}

void DataFlowGraph::Initialize() {
    InVertex = std::vector<int>(get_opList().size());
    OutVertex = std::vector<int>(get_opList().size());
    Mark = std::vector<int>(get_opList().size(), UNVISITED);
    for (int i = 0; i < get_opList().size(); i++) {
        if (get_opList()[i].element.getOPtype() != OP_PHI) {
            for (auto Var : get_opList()[i].InputVar) {
                if (ispureNumber(Var)) continue;
                InVertex[i]++;
            }
        }
        else {
            InVertex[i] = 1;
        }
        OutVertex[i] = ToVertex(i).size();
    }
}