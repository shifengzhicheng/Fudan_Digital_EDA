#include "controlflowgraph.h"

std::vector<graph_node>& ControlFlowGraph::getDFGNodes() {
    return DFGs;
}

int ControlFlowGraph::getIndex(std::string label) {
    if (IndexofDFG.find(label) != IndexofDFG.end()) {
        return IndexofDFG[label];
    }
    else return 0;
}

ControlFlowGraph::ControlFlowGraph(parser& p) {
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
        for (int i = 0; i < vars.size(); i++) {
            IndexofDFG[dfg.get_label()] = 0;
            dfg.myOutvartable()[vars[i]._name] = 1;
        }
        dfg.get_Branches()[0].To_Block = p.get_basic_blocks()[0].get_label_name();
        DFGs.push_back(graph_node(dfg));
    }
    // 这一步是初步把块生成图
    for (int i = 0; i < p.get_basic_blocks().size(); i++) {
        int currentBlock = i + 1;
        basic_block& bb = p.get_basic_blocks()[i];
        DataFlowGraph DFG(bb);
        IndexofDFG[DFG.get_label()] = currentBlock;
        if (DFG.get_Branches().empty()) {
            DFG.get_Branches().push_back(BranchEdge(DFG.get_label(), p.get_basic_blocks()[i + 1].get_label_name(), UnConditonal));
        }
        DFGs.push_back(graph_node(DFG));
        // 到这里已经完成了分支出度的边OutputEdge
        // 基本节点的生成，节点内数据的依赖
    }
    // 整理DFGs中的数据流关系，并根据DFG中的opList生成DFG的inputList
    int MemIndex = 0;
    for (int i = 0; i < DFGs.size(); i++) {
        //TODO
        DataFlowGraph& CurDFG = DFGs[i].DFG;
        for (int j = 0; j < CurDFG.get_inputList().size(); j++) {
            // 块中变量与外部块对应
            if (CurDFG.get_inputList()[j].From_Block.empty()) {
                std::string InputBlockVarName = CurDFG.get_inputList()[j].InputBlockVarName;
                DataFlowGraph* ToBlock = findvarfrom(InputBlockVarName);
                // 整理DFGs中的数据流关系，并根据DFG中的opList生成DFG的outputList
                ToBlock->get_outputList().push_back(OutputEdge(CurDFG.get_label(), InputBlockVarName));
                CurDFG.get_inputList()[j].From_Block = ToBlock->get_label();
                if (ToBlock->get_label() != "fiction_head") {
                    MemMap[InputBlockVarName] = MemIndex++;
                    Data.push_back(InputBlockVarName);
                }
            }
        }
    }
    // inputList生成完毕，至此，所有的依赖已经完成
    // 循环结束之后把输出的变量都输出到虚节点
    for (int i = 0; i < DFGs.size(); i++) {
        DataFlowGraph& CurDFG = DFGs[i].DFG;
        int ToIndex = CurDFG.get_opList().size() - 1;
        for (int j = 0; j < CurDFG.get_outputList().size(); j++) {
            std::string OutBlockVarName = CurDFG.get_outputList()[j].OutBlockVarName;
            int FromIndex = CurDFG.myOutvartable()[OutBlockVarName];
            // 在from和to之间创建一个边
            CurDFG.CreateEdge(FromIndex, ToIndex);
            CurDFG.get_opList()[ToIndex].InputVar.push_back(OutBlockVarName);
        }
    }
}

// Operation
// 这个操作将返回NextNode在graph_node中的索引数组
std::vector<int> ControlFlowGraph::NextNode(std::string label) {
    int index = IndexofDFG[label];
    return NextNode(index);
}
std::vector<int> ControlFlowGraph::NextNode(int index) {
    std::vector<int> nn;
    for (int i = 0; i < DFGs[index].controlnodes.size(); i++) {
        nn.push_back(IndexofDFG[DFGs[index].controlnodes[i].To_Block]);
    }
    return nn;
}
DataFlowGraph* ControlFlowGraph::findvarfrom(std::string VarName) {
    for (int i = 0; i < DFGs.size(); i++) {
        // 如果在块的输出变量表中找到了变量，那么就将这个块的标签返回
        std::unordered_map<std::string, int>& map = DFGs[i].DFG.myOutvartable();
        if (map.find(VarName) != map.end() && map[VarName] != 0) {
            return &DFGs[i].DFG;
        }
    }
    return NULL;
}