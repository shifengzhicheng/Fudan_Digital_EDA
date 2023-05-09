#include "FSMachine.h"

FSMachine::FSMachine(){}
FSMachine::FSMachine(ControlFlowGraph& CFG)
{
    filename = CFG.getfuncname();
    std::string defination = std::string("module ");
    defination.append(filename);
    Modulecode.push_back(defination);
    IOdefinationAppend(CFG.getRet_type(), CFG.getvar());
    FSMgener(CFG);
}

std::vector<std::string>& FSMachine::getModule()
{
    return Modulecode;
}

std::vector<std::string>& FSMachine::getFSM()
{
    return FSMcode;
}

std::string FSMachine::getFilename()
{
    return filename;
}

void FSMachine::IOdefinationAppend(int ret_type, std::vector<var>& vars)
{
    getModule().push_back("(");
    for (int i = 0; i < vars.size();i++) {
        std::string vardef = vars[i]._name;
        if (vars[i]._array_flag) {
            getModule().push_back("\tinput\t[31:0] " + vardef + "_q0,");
            getModule().push_back("\toutput\treg [31:0] " + vardef + "_address0,");
            getModule().push_back("\toutput\treg [31:0] " + vardef + "_ad0,");
            getModule().push_back("\toutput\treg " + vardef + "_ce0,");
            getModule().push_back("\toutput\treg " + vardef + "_we0,");
        }
        else {
            getModule().push_back("\tinput\t[31:0] " + vardef + ",");
        }
    }
    if (ret_type == RET_VOID) {
    }
    else if (ret_type == RET_INT) {
        getModule().push_back("\toutput\tap_return,");
    }
    getModule().push_back("\tinput\tap_clk,");
    getModule().push_back("\tinput\tap_rst_n,");
    getModule().push_back("\tinput\tap_start,");
    getModule().push_back("\toutput\treg ap_idle,");
    getModule().push_back("\toutput\treg ap_ready,");
    getModule().push_back("\toutput\treg ap_done");
    getModule().push_back(");");
}
enum Istiming{
    iscomb,
    istiming
};
std::string FSMachine::alwayslogic(bool Istiming) {
    if(Istiming) 
        return "\talways @(posedge ap_clk or negedge ap_rst_n)";
    else 
        return "\talways @(*)";
}
std::string FSMachine::iflogic(std::string cond) {
    return std::string("\tif(" + cond + ")");
}
std::string FSMachine::elseiflogic(std::string cond) {
    return std::string("\telse if(" + cond + ")");
}
std::string FSMachine::elselogic() {
    return std::string("\telse");
}
void FSMachine::begin(int i) {
    getFSM().push_back(std::string(i, '\t') + std::string("begin"));
}
void FSMachine::end(int i) {
    getFSM().push_back(std::string(i, '\t') + std::string("end"));
}
void FSMachine::FSMgener(ControlFlowGraph& CFG)
{
    size_t FSMsize = CFG.getDFGNodes().size();
    std::string LastState = "LastState";
    std::string CurrentState = "CurrentState";
    std::string CondReg = "condReg";
    std::string FSM_reg_cur_state(std::string("\n\treg ["
        + std::to_string(FSMsize - 1) + ":0] "
        + CurrentState + ";"));
    std::string FSM_reg_fore_state(std::string("\treg ["
        + std::to_string(FSMsize - 1) + ":0] "
        + LastState + ";"));
    std::string CondReg_def(std::string("\treg [1:0] " + CondReg + ";\n"));
    getFSM().push_back(FSM_reg_cur_state);
    getFSM().push_back(FSM_reg_fore_state);
    getFSM().push_back(CondReg_def);
    for (int i = 0; i < FSMsize; i++) {
        graph_node& CurNode = CFG.getDFGNodes()[i];
        std::string new_state("state_" + CurNode.DFG.get_label());
        std::string state_code(FSMsize, '0');
        state_code[i] = '1';
        state_code = std::string(std::to_string(FSMsize) + "'b" + state_code);
        getStateMap()[CurNode.DFG.get_label()] = state_code;
        getFSM().push_back("\tparameter " + new_state + " = " + state_code + ";");
    }
    getFSM().push_back("\n");
    // 从这里开始生成状态机的跳转逻辑
    getFSM().push_back(alwayslogic(istiming));
    std::string tabs(3, '\t');
    begin(1);
    std::string cond = std::string("!ap_rst_n");
    getFSM().push_back("\t" + iflogic(cond));
    begin(2);
    getFSM().push_back(tabs + LastState + " <= " + "state_fiction_head;");
    getFSM().push_back(tabs + CurrentState + " <= " + "state_fiction_head;");
    end(2);
    end(1);
    getFSM().push_back("\n");
    for (int i = 0; i < FSMsize; i++) {
        graph_node& CurNode = CFG.getDFGNodes()[i];
        getFSM().push_back(alwayslogic(istiming));
        std::string new_state("state_" + CurNode.DFG.get_label());
        begin(1);
        for (int i = 0; i < CurNode.controlnodes.size(); i++) {
            BranchEdge& CurEdge = CurNode.controlnodes[i];
            if (CurEdge.cond == UnConditonal) {
                if (CurEdge.Isreturn) {
                    cond = std::string(
                        CurrentState + " == " + "state_" + CurEdge.From_Block
                        + " & " + CondReg + "[1] == " + "1'b1");
                    getFSM().push_back("\t" + iflogic(cond));
                    begin(2);
                    getFSM().push_back(tabs+"ap_done <= 1'b1;");
                    
                }
                else {
                    cond = std::string(
                        CurrentState + " == " + "state_" + CurEdge.From_Block
                        + " & " + CondReg + "[1] == " + "1'b1");
                    getFSM().push_back("\t" + iflogic(cond));
                    begin(2);
                    getFSM().push_back(tabs + LastState + " <= " + CurrentState + ";");
                    getFSM().push_back(tabs + CurrentState + " <= " + "state_" + CurEdge.To_Block + ";");
                    
                }
            }
            else {
                cond = std::string(
                    CurrentState + " == " + "state_" + CurEdge.From_Block
                    + " & " + CondReg + " == " + "2'b1");
                if (CurEdge.cond == IfTrue) cond.append("1");
                else cond.append("0");
                getFSM().push_back("\t" + iflogic(cond));
                begin(2);
                getFSM().push_back(tabs + LastState + " <= " + CurrentState + ";");
                getFSM().push_back(tabs + CurrentState + " <= " + "state_" + CurEdge.To_Block + ";");
                
            }
            end(2);
        }
        end(1);
        getFSM().push_back("\n");
    }
}
