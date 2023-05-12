#include "FSMachine.h"
#include <algorithm>
FSMachine::FSMachine() {}
FSMachine::FSMachine(ControlFlowGraph& CFG, std::vector<std::vector<Cycle>>& Cycles, std::vector<std::vector<std::pair<std::string, int>>>& REG)
{
    filename = CFG.getfuncname();
    std::string defination = std::string("module ");
    defination.append(filename);
    Modulecode.push_back(defination);
    IOdefinationAppend(CFG.getRet_type(), CFG.getvar());
    FSMgener(CFG);
    CounterGener(Cycles, CFG);
    perPeriodGener(Cycles, CFG);
    regDefGener(REG);
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

std::vector<std::string>& FSMachine::getRegDef()
{
    return regDef;
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
        getModule().push_back("\toutput\t[31:0] ap_return,");
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

std::string FSMachine::opTrans(Statement m_statement, int& outregIndex, std::vector<std::pair<std::string, int>>& condInState,
    std::string state, std::vector<std::string>& loadFlag, std::vector<std::string>& storeFlag, std::vector<std::string>& dataSet)
{
    switch (m_statement.optype)
    {
    case OP_ASSIGN:
    {
        if (m_statement.regs[0] == -1)
            return std::string("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + m_statement.vars[0] + ";");
        else
            return std::string("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + "reg_" + std::to_string(m_statement.regs[0]) + ";");
    }
    case OP_ADD:
    {
        std::string oprand1, oprand2;
        if (m_statement.regs[0] == -1)
            oprand1 = m_statement.vars[0];
        else
            oprand1 = "reg_" + std::to_string(m_statement.regs[0]);
        if (m_statement.regs[1] == -1)
            oprand2 = m_statement.vars[1];
        else
            oprand2 = "reg_" + std::to_string(m_statement.regs[1]);
        return std::string("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + oprand1 + " + " + oprand2 + ";");
    }
    case OP_SUB:
    {
        std::string oprand1, oprand2;
        if (m_statement.regs[0] == -1)
            oprand1 = m_statement.vars[0];
        else
            oprand1 = "reg_" + std::to_string(m_statement.regs[0]);
        if (m_statement.regs[1] == -1)
            oprand2 = m_statement.vars[1];
        else
            oprand2 = "reg_" + std::to_string(m_statement.regs[1]);
        return std::string("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + oprand1 + " - " + oprand2 + ";");
    }
    case OP_MUL:
    {
        std::string oprand1, oprand2;
        if (m_statement.regs[0] == -1)
            oprand1 = m_statement.vars[0];
        else
            oprand1 = "reg_" + std::to_string(m_statement.regs[0]);
        if (m_statement.regs[1] == -1)
            oprand2 = m_statement.vars[1];
        else
            oprand2 = "reg_" + std::to_string(m_statement.regs[1]);
        return std::string("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + oprand1 + " * " + oprand2 + ";");
    }
    case OP_DIV:
    {
        std::string oprand1, oprand2;
        if (m_statement.regs[0] == -1)
            oprand1 = m_statement.vars[0];
        else
            oprand1 = "reg_" + std::to_string(m_statement.regs[0]);
        if (m_statement.regs[1] == -1)
            oprand2 = m_statement.vars[1];
        else
            oprand2 = "reg_" + std::to_string(m_statement.regs[1]);
        return std::string("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + oprand1 + " / " + oprand2 + ";");
    }
    case OP_LOAD:
    {
        std::string str_var = m_statement.vars[0];
        //std::string str_add = m_statement.vars[1];
        std::string severalLine;
        if (find(loadFlag.begin(), loadFlag.end(), str_var) == loadFlag.end())
        {
            loadFlag.push_back(str_var);
            severalLine += ("\t\t" + str_var + "_ce0 <= 1;\n");
            severalLine += ("\t\t" + str_var + "_address0 <= reg_" + std::to_string(m_statement.regs[1]) + ";");
        }
        else
        {
            for (std::vector<std::string>::iterator iter = loadFlag.begin(); iter != loadFlag.end(); iter++)
            {
                if (*iter == str_var)
                {
                    loadFlag.erase(iter);
                    break;
                }
            }
            severalLine += ("\t\t" + str_var + "_ce0 <= 0;\n");
            severalLine += ("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + str_var + "_q0;");
        }
        return severalLine;
        //return std::string("\t\ttest");
    }
    case OP_STORE:
    {
        std::string str_var = m_statement.vars[0];
        std::string severalLine;
        if (std::find(storeFlag.begin(), storeFlag.end(), str_var) == storeFlag.end())
        {
            storeFlag.push_back(str_var);
            severalLine += ("\t\t" + str_var + "_we0 <= 1;\n");
            severalLine += ("\t\t" + str_var + "_address0 <= reg_" + std::to_string(m_statement.regs[1]) + ";\n");
            severalLine += ("\t\t" + str_var + "_ad0 <= reg_" + std::to_string(m_statement.regs[2]) + ";");
        }
        else
        {
            for (std::vector<std::string>::iterator iter = storeFlag.begin(); iter != storeFlag.end(); iter++)
            {
                if (*iter == str_var)
                {
                    storeFlag.erase(iter);
                    break;
                }
            }
            severalLine += ("\t\t" + str_var + "_we0 <= 0;");
        }
        return severalLine;
    }
    case OP_BR:
    {
        std::string str_reg, str_mem;
        str_mem = "Mem_" + dataSet[m_statement.outreg];
        str_reg = "reg_" + std::to_string(m_statement.regs[0]);
        return std::string("\t\t" + str_mem + " <= " + str_reg + ";");
        break;
    }
    case OP_LT:
    {
        std::string oprand1, oprand2;
        if (m_statement.regs[0] == -1)
            oprand1 = m_statement.vars[0];
        else
            oprand1 = "reg_" + std::to_string(m_statement.regs[0]);
        if (m_statement.regs[1] == -1)
            oprand2 = m_statement.vars[1];
        else
            oprand2 = "reg_" + std::to_string(m_statement.regs[1]);
        condInState.push_back(std::pair<std::string, int>(state, m_statement.outreg));
        return std::string("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + "(" + oprand1 + " < " + oprand2 + ");");
    }
    case OP_GT:
    {
        std::string oprand1, oprand2;
        if (m_statement.regs[0] == -1)
            oprand1 = m_statement.vars[0];
        else
            oprand1 = "reg_" + std::to_string(m_statement.regs[0]);
        if (m_statement.regs[1] == -1)
            oprand2 = m_statement.vars[1];
        else
            oprand2 = "reg_" + std::to_string(m_statement.regs[1]);
        condInState.push_back(std::pair<std::string, int>(state, m_statement.outreg));
        return std::string("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + "(" + oprand1 + " > " + oprand2 + ");");
    }
    case OP_LE:
    {
        std::string oprand1, oprand2;
        if (m_statement.regs[0] == -1)
            oprand1 = m_statement.vars[0];
        else
            oprand1 = "reg_" + std::to_string(m_statement.regs[0]);
        if (m_statement.regs[1] == -1)
            oprand2 = m_statement.vars[1];
        else
            oprand2 = "reg_" + std::to_string(m_statement.regs[1]);
        condInState.push_back(std::pair<std::string, int>(state, m_statement.outreg));
        return std::string("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + "(" + oprand1 + " <= " + oprand2 + ");");
    }
    case OP_GE:
    {
        std::string oprand1, oprand2;
        if (m_statement.regs[0] == -1)
            oprand1 = m_statement.vars[0];
        else
            oprand1 = "reg_" + std::to_string(m_statement.regs[0]);
        if (m_statement.regs[1] == -1)
            oprand2 = m_statement.vars[1];
        else
            oprand2 = "reg_" + std::to_string(m_statement.regs[1]);
        condInState.push_back(std::pair<std::string, int>(state, m_statement.outreg));
        return std::string("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + "(" + oprand1 + " >= " + oprand2 + ");");
    }
    case OP_EQ:
    {
        std::string oprand1, oprand2;
        if (m_statement.regs[0] == -1)
            oprand1 = m_statement.vars[0];
        else
            oprand1 = "reg_" + std::to_string(m_statement.regs[0]);
        if (m_statement.regs[1] == -1)
            oprand2 = m_statement.vars[1];
        else
            oprand2 = "reg_" + std::to_string(m_statement.regs[1]);
        condInState.push_back(std::pair<std::string, int>(state, m_statement.outreg));
        return std::string("\t\treg_" + std::to_string(m_statement.outreg) + " <= " + "(" + oprand1 + " == " + oprand2 + ");");
    }
    case OP_PHI:
    {
        std::string oprand1, oprand2;
        if (m_statement.regs[0] == -1)
            oprand1 = m_statement.vars[0];
        else
            oprand1 = "reg_" + std::to_string(m_statement.regs[0]);
        if (m_statement.regs[1] == -1)
            oprand2 = m_statement.vars[1];
        else
            oprand2 = "reg_" + std::to_string(m_statement.regs[1]);
        std::string severalLine;
        severalLine += ("\t\tif(" + std::string("LastState") + " == state_" + m_statement.label[0] + ")\n");
        severalLine += ("\t\t\treg_" + std::to_string(m_statement.outreg) + " <= " + oprand1 + ";\n");
        severalLine += ("\t\telse if(" + std::string("LastState") + " == state_" + m_statement.label[1] + ")\n");
        severalLine += ("\t\t\treg_" + std::to_string(m_statement.outreg) + " <= " + oprand2 + ";");
        return severalLine;
    }
    case OP_RET:
    {
        if (m_statement.outreg != -1)
            outregIndex = m_statement.outreg;
        break;
    }
    default:
        break;
    }
    return std::string();
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
    getFSM().push_back(tabs + "ap_done <= 1'b0;");
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
    CounterCode.push_back(std::string("\treg[31:0] counter;"));
    CounterCode.push_back(alwayslogic(istiming));
    begin(1, CounterCode);
    CounterCode.push_back(iflogic(std::string("!ap_rst_n")));
    begin(1, CounterCode);
    CounterCode.push_back("\t\t" + std::string("counter <= 0;"));
    //CounterCode.push_back("\t\t" + std::string("branch_ready <= 1;"));
    end(1, CounterCode);
    for (std::vector<graph_node>::iterator iter = CFG.getDFGNodes().begin(); iter != CFG.getDFGNodes().end(); iter++)
    {
        std::string cond;
        int totalPeriod = iter->DFG.getPeriod() + 1;
        cond += ("CurrentState == state_" + iter->DFG.get_label());
        cond += " && ";
        cond += ("counter == " + std::to_string(totalPeriod));
        if (iter->DFG.get_label() == std::string("fiction_head"))
            cond += (" && ap_start == 1'b1");
        CounterCode.push_back(elseiflogic(cond));
        begin(1, CounterCode);
        CounterCode.push_back("\t\t" + std::string("counter <= 0;"));
        if (iter->DFG.get_label() == std::string("fiction_head"))
            CounterCode.push_back("\t\t" + std::string("branch_ready <= 1;"));
        end(1, CounterCode);
    }
    CounterCode.push_back(elselogic());
    CounterCode.push_back("\t\t" + std::string("counter <= counter + 1;"));
    end(1, CounterCode);
    CounterCode.push_back("\n");
}
void FSMachine::perPeriodGener(std::vector<std::vector<Cycle>>& Cycles, ControlFlowGraph& CFG)
{
    std::vector<std::string> toReg;
    std::vector<std::string> toWire;
    int outregIndex = 0;
    std::vector<std::pair<std::string, int>> condInState;
    std::vector<std::string> loadFlag, storeFlag;
    toReg.push_back("\t" + std::string("always@(counter)"));
    toReg.push_back("\t" + std::string("case(CurrentState)"));
    //toReg.push_back("\t" + CFG.getDFGNodes()[0].DFG.get_label() + ": begin");
    for (std::vector<graph_node>::iterator iter = CFG.getDFGNodes().begin() + 1; iter != CFG.getDFGNodes().end(); iter++)
    {
        std::string branch_state = "state_" + iter->DFG.get_label();
        toReg.push_back("\t" + branch_state + ": begin");
        int totalPeriod = iter->DFG.getPeriod();
        toReg.push_back("\t\t" + std::string("case(counter)"));
        int block = iter - CFG.getDFGNodes().begin();
        for (int i = 0; i <= totalPeriod + 1; i++)
        {
            toReg.push_back("\t\t32'd" + std::to_string(i) + ": begin");
            if (i == 0)
            {
                for (std::vector<Statement>::iterator iter_statement = Cycles[block][i].Statements.begin();
                    iter_statement != Cycles[block][i].Statements.end(); iter_statement++)
                {
                    std::string str_reg, str_mem;
                    str_mem = "Mem_" + CFG.getDataSet()[iter_statement->regs[0]];
                    str_reg = "reg_" + std::to_string(iter_statement->outreg);
                    toReg.push_back("\t\t" + str_reg + " <= " + str_mem + ";");
                }
            }
            /*else if (i == totalPeriod + 1)
            {
                for (std::vector<Statement>::iterator iter_statement = Cycles[block][i].Statements.begin();
                    iter_statement != Cycles[block][i].Statements.end(); iter_statement++)
                {
                    std::string str_reg, str_mem;
                    str_mem = "Mem_" + CFG.getDataSet()[iter_statement->outreg];
                    str_reg = "reg_" + std::to_string(iter_statement->regs[0]);
                    toReg.push_back("\t\t" + str_mem + " <= " + str_reg + ";");
                }
            }*/
            else
            {
                for (std::vector<Statement>::iterator iter_statement = Cycles[block][i].Statements.begin();
                    iter_statement != Cycles[block][i].Statements.end(); iter_statement++)
                {
                    std::string str = opTrans(*iter_statement, outregIndex, condInState, branch_state, loadFlag, storeFlag, CFG.getDataSet());
                    if (!str.empty())
                        toReg.push_back(str);
                }
            }
            if (i == totalPeriod + 1)
                toReg.push_back("\t\tbranch_ready <= 1;");
            end(2, toReg);
        }
        toReg.push_back("\t\t" + std::string("endcase"));
        end(1, toReg);
    }
    toReg.push_back("\t" + std::string("endcase"));
    if (outregIndex)
        toWire.push_back("\tassign ap_return = reg_" + std::to_string(outregIndex) + ";");
    if (!condInState.empty())
    {
        std::string tmp;
        tmp += "\tassign cond = ";
        tmp += "((CurrentState == " + condInState[0].first + ") & reg_" + std::to_string(condInState[0].second) + ")";
        for (std::vector<std::pair<std::string, int>>::iterator iter = condInState.begin() + 1; iter != condInState.end(); iter++)
        {
            tmp += " || ((CurrentState == " + iter->first + ") & reg_" + std::to_string(iter->second) + ")";
        }
        tmp += ";";
        toWire.push_back(tmp);
    }
    perPeriodCode.insert(perPeriodCode.end(), toReg.begin(), toReg.end());
    perPeriodCode.insert(perPeriodCode.end(), toWire.begin(), toWire.end());
}
void FSMachine::regDefGener(std::vector<std::vector<std::pair<std::string, int>>>& REG)
{
    int regMax = 0;
    for (std::vector<std::vector<std::pair<std::string, int>>>::iterator iter = REG.begin(); iter != REG.end(); iter++)
    {
        if (!iter->empty() && iter->back().second > regMax)
            regMax = iter->back().second;
    }
    for (int i = 1; i <= regMax; i++)
    {
        regDef.push_back("\treg [31:0] reg_" + std::to_string(i) + ";");
    }
}