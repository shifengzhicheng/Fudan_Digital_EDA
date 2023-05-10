#include "FSMachine.h"

FSMachine::FSMachine() {}
FSMachine::FSMachine(ControlFlowGraph& CFG, std::vector<std::vector<Cycle>> &Cycles)
{
    filename = CFG.getfuncname();
    std::string defination = std::string("module ");
    defination.append(filename);
    Modulecode.push_back(defination);
    IOdefinationAppend(CFG.getRet_type(), CFG.getvar());
    FSMgener(CFG);
    CounterGener(Cycles, CFG);
    perPeriodGener(Cycles, CFG);
}

std::vector<std::string>& FSMachine::getModule()
{
    return Modulecode;
}

std::vector<std::string>& FSMachine::getFSM()
{
    return FSMcode;
}

std::vector<std::string>& FSMachine::getCounter()
{
    return CounterCode;
}

std::vector<std::string>& FSMachine::getPerPeriod()
{
    return perPeriodCode;
}

std::string FSMachine::getFilename()
{
    return filename;
}

void FSMachine::IOdefinationAppend(int ret_type, std::vector<var>& vars)
{
    getModule().push_back("(");
    for (int i = 0; i < vars.size(); i++) {
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
    getModule().push_back("\toutput\treg ap_done");
    getModule().push_back(");");
}
enum Istiming {
    iscomb,
    istiming
};
std::string FSMachine::alwayslogic(bool Istiming) {
    if (Istiming)
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
void FSMachine::begin(int i, std::vector<std::string>& code) {
    code.push_back(std::string(i, '\t') + std::string("begin"));
}
void FSMachine::end(int i, std::vector<std::string>& code) {
    code.push_back(std::string(i, '\t') + std::string("end"));
}
void FSMachine::FSMgener(ControlFlowGraph& CFG)
{
    size_t FSMsize = CFG.getDFGNodes().size();
    getFSM().push_back("\n");
    // 定义Mem寄存器
    for (auto it = CFG.Memtable().begin(); it != CFG.Memtable().end(); it++) {
        std::string CurVar = it->first;
        std::string CurVar_def(std::string("\treg [31:0] Mem_" + CurVar + ";"));
        getFSM().push_back(CurVar_def);
    }
    getFSM().push_back("\n");
    // 定义状态机跳转所需要的寄存器以及连线
    std::string LastState = "LastState";
    std::string CurrentState = "CurrentState";
    std::string Cond = "cond";
    std::string Branch_ready = "branch_ready";
    std::string FSM_reg_cur_state(std::string("\treg ["
        + std::to_string(FSMsize - 1) + ":0] "
        + CurrentState + ";"));
    std::string FSM_reg_fore_state(std::string("\treg ["
        + std::to_string(FSMsize - 1) + ":0] "
        + LastState + ";"));
    std::string Cond_def(std::string("\twire " + Cond + ";"));
    std::string Branch_def(std::string("\treg " + Branch_ready + ";"));
    getFSM().push_back(FSM_reg_cur_state);
    getFSM().push_back(FSM_reg_fore_state);
    getFSM().push_back(Cond_def);
    getFSM().push_back(Branch_def);
    getFSM().push_back("\n");

    // 给不同的状态编码
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

    // rst_n的生成逻辑
    getFSM().push_back(alwayslogic(istiming));
    std::string tabs(3, '\t');
    begin(1, FSMcode);
    std::string cond = std::string("!ap_rst_n");
    getFSM().push_back("\t" + iflogic(cond));
    begin(2, FSMcode);
    getFSM().push_back(tabs + LastState + " <= " + "state_fiction_head;");
    getFSM().push_back(tabs + CurrentState + " <= " + "state_fiction_head;");
    end(2, FSMcode);

    // 其他状态跳转的生成逻辑
    for (int i = 0; i < FSMsize; i++) {
        graph_node& CurNode = CFG.getDFGNodes()[i];
        std::string new_state("state_" + CurNode.DFG.get_label());
        for (int i = 0; i < CurNode.controlnodes.size(); i++) {
            BranchEdge& CurEdge = CurNode.controlnodes[i];
            if (CurEdge.cond == UnConditonal) {
                if (CurEdge.Isreturn) {
                    cond = std::string(
                        CurrentState + " == " + "state_" + CurEdge.From_Block
                        + " & " + Branch_ready + " == " + "1'b1");
                    getFSM().push_back("\t" + elseiflogic(cond));
                    begin(2, FSMcode);
                    getFSM().push_back(tabs + "ap_done <= 1'b1;");

                }
                else {
                    cond = std::string(
                        CurrentState + " == " + "state_" + CurEdge.From_Block
                        + " & " + Branch_ready + " == " + "1'b1");
                    getFSM().push_back("\t" + elseiflogic(cond));
                    begin(2, FSMcode);
                    getFSM().push_back(tabs + LastState + " <= " + CurrentState + ";");
                    getFSM().push_back(tabs + CurrentState + " <= " + "state_" + CurEdge.To_Block + ";");

                }
            }
            else {
                cond = std::string(
                    CurrentState + " == " + "state_" + CurEdge.From_Block
                    + " & " + Branch_ready + " == " 
                    + "1'b1 & cond == 1'b");
                if (CurEdge.cond == IfTrue) cond.append("1");
                else cond.append("0");
                getFSM().push_back("\t" + elseiflogic(cond));
                begin(2, FSMcode);
                getFSM().push_back(tabs + LastState + " <= " + CurrentState + ";");
                getFSM().push_back(tabs + CurrentState + " <= " + "state_" + CurEdge.To_Block + ";");

            }
            getFSM().push_back(tabs + Branch_ready + " <= 1'b0;");
            end(2, FSMcode);
        }
    }
    end(1, FSMcode);
    getFSM().push_back("\n");
}
void FSMachine::CounterGener(std::vector<std::vector<Cycle>>& Cycles, ControlFlowGraph& CFG)
{
    CounterCode.push_back(std::string("reg[31:0] counter"));
    CounterCode.push_back(alwayslogic(istiming));
    begin(1, CounterCode);
    CounterCode.push_back(iflogic(std::string("!ap_rst_n")));
    CounterCode.push_back("\t\t" + std::string("counter <= 0"));
    for (std::vector<graph_node>::iterator iter = CFG.getDFGNodes().begin(); iter != CFG.getDFGNodes().end(); iter++)
    {
        std::string cond;
        int totalPeriod = iter->DFG.getPeriod();
        cond += ("CurrentState == state_" + iter->DFG.get_label());
        cond += " && ";
        cond += ("counter == " + std::to_string(totalPeriod));
        CounterCode.push_back(iflogic(cond));
        CounterCode.push_back("\t\t" + std::string("counter <= 0"));
    }
    CounterCode.push_back(elselogic());
    CounterCode.push_back("\t\t" + std::string("counter <= counter + 1"));
    end(1, CounterCode);
}
void FSMachine::perPeriodGener(std::vector<std::vector<Cycle>>& Cycles, ControlFlowGraph& CFG)
{
    std::vector<std::string> toReg;
    std::vector<std::string> toWire;
    toReg.push_back("\t" + std::string("always@(counter)"));
    toReg.push_back("\t" + std::string("case(CurrentState)"));
    for (std::vector<graph_node>::iterator iter = CFG.getDFGNodes().begin() + 1; iter != CFG.getDFGNodes().end(); iter++)
    {
        std::string branch_state = "state_" + iter->DFG.get_label();
        toReg.push_back("\t" + branch_state + ": begin");
        int totalPeriod = iter->DFG.getPeriod();
        toReg.push_back("\t\t" + std::string("case(counter)"));
        for (int i = 1; i <= totalPeriod; i++)
        {
            toReg.push_back("\t\t32'b" + std::to_string(i) + ": begin");
            toReg.push_back(std::string("test  test  test"));
            end(2, toReg);
        }
        toReg.push_back("\t\t" + std::string("endcase"));
        end(1, toReg);
    }
    toReg.push_back("\t" + std::string("endcase"));
    perPeriodCode.insert(perPeriodCode.end(), toReg.begin(), toReg.end());
    perPeriodCode.insert(perPeriodCode.end(), toWire.begin(), toWire.end());
}