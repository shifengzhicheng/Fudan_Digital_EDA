#ifndef FSM_H
#define FSM_H
#include "ControlFlowGraph.h"
#include <iostream>
#include <fstream>
class FSMachine {
private:
	std::string filename;
	std::vector<std::string> Modulecode;
	std::unordered_map<std::string, std::string> stateMapping;
	std::vector<std::string> FSMcode;
public:
	FSMachine();
	// 类代理实现生成module块和有限状态机的转换
	FSMachine(ControlFlowGraph &CFG);
	std::vector<std::string>& getModule();
	std::vector<std::string>& getFSM();
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
	void begin(int i);
	// 生成end
	void end(int i);
	// 生成FSM
	void FSMgener(ControlFlowGraph &CFG);
};
#endif