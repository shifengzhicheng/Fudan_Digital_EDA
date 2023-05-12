#ifndef FSM_H 
#define FSM_H
#include "controlflowgraph.h"
#include "cycleTable.h"
class FSMachine {
private:
	//std::vector<std::vector<Cycle>> m_Cycles;
	std::string filename;
	std::vector<std::string> Modulecode;
	std::unordered_map<std::string, std::string> stateMapping;
	std::vector<std::string> FSMcode;
	std::vector<std::string> CounterCode;
	std::vector<std::string> perPeriodCode;
	std::vector<std::string> regDef;
public:
	FSMachine();
	// 类代理实现生成module块和有限状态机的转换
	FSMachine(ControlFlowGraph &CFG, std::vector<std::vector<Cycle>> &Cycles, std::vector<std::vector<std::pair<std::string, int>>>& REG);
	std::vector<std::string>& getModule();
	std::vector<std::string>& getFSM();
	std::vector<std::string>& getCounter();
	std::vector<std::string>& getPerPeriod();
	std::vector<std::string>& getRegDef();
	std::string getFilename();
	std::unordered_map<std::string, std::string>& getStateMap() {
		return stateMapping;
	}
	// 生成module
	void IOdefinationAppend(int ret_type,std::vector<var>& vars);
	// 生成always逻辑块
	std::string alwayslogic(bool Istiming);
	// 生成if逻辑块
	std::string iflogic(std::string cond);
	// elseif逻辑块
	std::string elseiflogic(std::string cond);
	// else逻辑
	std::string elselogic();
	// 生成begin
	void begin(int i, std::vector<std::string>& code);
	// 生成end
	void end(int i, std::vector<std::string>& code);
	// 生成FSM
	std::string opTrans(Statement m_statement, int& outregIndex, std::vector<std::pair<std::string, int>>& condInState, 
		std::string state, std::vector<std::string>& loadFlag, std::vector<std::string>& storeFlag, std::vector<std::string>& dataSet);
	void FSMgener(ControlFlowGraph &CFG);
	void CounterGener(std::vector<std::vector<Cycle>> &Cycles, ControlFlowGraph &CFG);
	void perPeriodGener(std::vector<std::vector<Cycle>>& Cycles, ControlFlowGraph& CFG);
	void regDefGener(std::vector<std::vector<std::pair<std::string, int>>>& REG);
};
#endif