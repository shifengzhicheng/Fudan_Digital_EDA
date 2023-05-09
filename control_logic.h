#pragma once
#ifndef CONTROL_H
#define CONTROL_H
#include <algorithm>
#include "Hungarian_algorithm.h"
#include "dataflowgraph.h"

bool varPeridCmp_stop(varPeriod a, varPeriod b)
{
	return a.stopp < b.stopp;
}

class Register {
private:
	varPeriod now_reg_data;						//寄存器中当前存储的变量
	std::vector<varPeriod> all_reg_datas;		//寄存器中会存储的所有的变量

public:
	int reg_index;								//寄存器的下标，与ryh的pair的索引值相对应
	Register() {};

	// 更新并获取当前周期中寄存器存储的值
	// data中会存储当前周期下寄存器存储的值
	bool getData(int cycle, varPeriod& data) {
		for (std::vector<varPeriod>::iterator iter = all_reg_datas.begin(); iter != all_reg_datas.end(); iter++) {
			if ((*iter).startp <= cycle && (*iter).stopp >= cycle) {
				now_reg_data = (*iter);
				data = now_reg_data;
				return true;
			}
		}
		return false;
	}

	// 设置当前寄存器中存储的值
	bool setData(varPeriod data) {
		for (std::vector<varPeriod>::iterator iter = all_reg_datas.begin(); iter != all_reg_datas.end(); iter++) {
			if ((*iter).var == data.var) {
				now_reg_data = data;
				return true;
			}
		}
		std::cout << "寄存器中没有存储过该变量！" << std::endl;
		return false;
	}

	// 将变量存入寄存器中（添加存储的数据）
	void addData(varPeriod var) {
		all_reg_datas.push_back(var);
	}

	// 返回该寄存器在不同周期会存储的所有变量
	std::vector<varPeriod> getAllDatas() {
		return all_reg_datas;
	}
};

// 将ryh的pair结构转换为寄存器类
void Pair2Register(DataFlowGraph &DFG, std::vector<std::pair<std::string, int>> REG, std::vector<Register>& Regs) {
	std::vector<varPeriod> varPeriods = graph2VarPeriods(DFG);
	for (std::vector<std::pair<std::string, int>>::iterator iter = REG.begin(); iter != REG.end(); iter++) {
		int flag = 0;			//用于表示是否已经添加过该寄存器
		varPeriod v;
		for (std::vector<Register>::iterator it = Regs.begin(); it != Regs.end(); it++) {
			if ((*it).reg_index == iter->second) {		//表示已经有该寄存器了
				for (std::vector<varPeriod>::iterator i = varPeriods.begin(); i != varPeriods.end(); i++) {
					if ((*i).var == iter->first) {		//成功找到了该变量，记录下其开始与结束周期
						v.startp = (*i).startp;
						v.var = (*i).var;
						v.stopp = (*i).stopp;
						flag = 1;
						(*it).addData(v);				//添加到寄存器中
						break;
					}
				}
				break;
			}
		}
		if (flag == 0) {											//未找到该寄存器，则新建一个寄存器
			Register r;
			r.reg_index = iter->second;
			for (std::vector<varPeriod>::iterator i = varPeriods.begin(); i != varPeriods.end(); i++) {
				if ((*i).var == iter->first) {			//成功找到了该变量，记录下其开始与结束周期
					v.startp = (*i).startp;
					v.var = (*i).var;
					v.stopp = (*i).stopp;
					r.addData(v);							//添加到寄存器中
					break;
				}
			}
			Regs.push_back(r);							//将新建的寄存器加入向量中
		}
	}
}

bool find(std::vector<int> v, int data) {
	for (int i = 0; i < v.size(); i++) {
		if (v[i] == data)
			return true;
	}
	return false;
}

class Mux {
private:
	std::vector<int> mux_inputs;				//选择器连接的所有输入寄存器的下标
	int mux_selectInput;						//选择的输入数据对应的寄存器
	int output_compute;							//连接的输出的计算资源
	int flag;									//0为左边，1为右边

public:
	int mux_index;								//选择器下标
	Mux() { }
	Mux(int L_or_R, int _compute, std::vector<computeresource> com) {
		std::vector<int> inputs;
		output_compute = _compute;
		flag = L_or_R;
		if (flag == 0)
			inputs = com[output_compute].Ainputregisters;
		else
			inputs = com[output_compute].Binputregisters;	
		mux_inputs = inputs;
	}

	//添加输入数据,input为添加的寄存器的下标
	void addInput(int input) {
		mux_inputs.push_back(input);
	}

	//设置当前选择的输入寄存器下标
	bool setInput(int select) {
		for (int i = 0; i < mux_inputs.size(); i++) {
			if (mux_inputs[i] == select) {
				mux_selectInput = select;
				return true;
			}
		}
		std::cout << "选择器输入端不包括该寄存器！" << std::endl;
		return false;
	}

	//获取选择的输入寄存器下标
	int getInput() {
		return mux_selectInput;
	}

	//获取所有的输入寄存器下标
	std::vector<int> getAllInput() {
		return mux_inputs;
	}

	//获取选择器连接到的计算资源
	int getCompute() {
		return output_compute;
	}

	bool chooseReg(int cycle, DataFlowGraph dfg, std::vector<Register> REGs,
				std::vector<std::pair<std::string, int>> REGi,
				std::vector<std::pair<int, int>> CSP,
				std::vector<computeresource> com,
				Register& reg, std::string& _var) {
		reg.reg_index = -1;		//先设置默认值，如果最后该周期未找到则表示选择器当前周期是空闲的
		_var = "NULL";
		std::vector<node> List = dfg.get_opList();
		std::vector<int> curDFGinput;
	
		//首先遍历选择器输入端,选出当前块下会使用到的所有寄存器
		for (int i = 0; i < REGs.size(); i++) {
			int index = REGs[i].reg_index;
			for (int j = 0; j < mux_inputs.size(); j++) {
				if (find(mux_inputs, index))
					curDFGinput.push_back(index);
			}
		}

		for (int i = 0; i < curDFGinput.size(); i++) {
			varPeriod v;
			if (REGs[curDFGinput[i] - 1].getData(cycle, v)) {	//该寄存器在当前周期存储着变量
				std::cout << v.var << std::endl;
				//接下来需要判断这个变量是否是在当前选择器所连接的计算资源中被使用
				for (std::vector<std::pair<int, int>>::iterator iter = CSP.begin(); iter != CSP.end(); iter++) {
					if (iter->second == output_compute) {
						//计算资源匹配上了，看对应node的输入变量是否包括此时寄存器中存储的变量v
						std::vector<std::string> input = List[iter->first+1].InputVar;
						//实际上输入列表最多只有两个变量
						for (int j = 0; j < input.size(); j++) {
							std::cout << "变量名为：" << v.var << "    input为：" << input[j] << std::endl;
							if (input[j] == v.var) {
								//成功找到
								reg = REGs[curDFGinput[i] - 1];
								_var = v.var;
								return true;
							}
						}
					}
				}
			}
		}
	}
};

struct cycletable {
	std::string var;	//变量
	int com;			//变量var活跃期间使用的计算资源
	int reg;			//使用的寄存器
	int mux;			//使用的选择器
};

// 控制器
class MicrocodeController {
private:
	std::vector<Register> Regs;				//所有的寄存器
	std::vector<Mux> Muxs;					//所有的选择器
	DataFlowGraph DFG;						//表示当前控制器是处在哪一块中的
	std::vector<std::pair<int, int>> CSP;	//计算资源匹配结果
	std::vector<computeresource> Compute;				
	std::vector<std::pair<cycletable, int>> CycleTables;	//存储的是每个周期活跃的变量、与其相关的寄存器和选择器
	std::vector<Cycle> C;

public:
	MicrocodeController(DataFlowGraph& dfg, std::vector<std::pair<std::string, int>> _REG,
		std::vector<computeresource> _compute, std::vector<std::pair<int, int>> _CSP) {
		CSP = _CSP;
		DFG = dfg;
		Compute = _compute;
		Pair2Register(DFG, _REG, Regs);

		std::cout << DFG.get_label() << std::endl;

		for (int i = 0; i < _compute.size(); i++) {
			Mux _mL(0, i, _compute);
			Mux _mR(1, i, _compute);
			_mL.mux_index = 2 * i;
			_mR.mux_index = 2 * i + 1;
			Muxs.push_back(_mL);
			Muxs.push_back(_mR);
		}
	}

	//添加寄存器
	void addReg(Register reg) {
		Regs.push_back(reg);
	}

	//添加选择器
	void addMux(Mux mux) {
		Muxs.push_back(mux);
	}

	void generateTables(std::vector<std::pair<std::string, int>> _REG) {
		int cycle;		//所处周期，遍历使用
		std::vector<varPeriod> V = graph2VarPeriods(DFG);
		sort(V.begin(), V.end(), varPeridCmp_stop);
		int total_cycle = V[V.size() - 1].stopp;		//最后一个变量的终止周期

		for (cycle = 1; cycle <= total_cycle; cycle++) {
			//对每个选择器进行遍历，记录其选择的寄存器输入数据到table中
			for (int i = 0; i < Muxs.size(); i++) {
				Register r;
				std::string var;
				if (Muxs[i].chooseReg(cycle, DFG, Regs, _REG, CSP, Compute, r, var)) {
					cycletable t;
					t.mux = i;
					t.reg = r.reg_index;
					t.var = var;
					t.com = Muxs[i].getCompute();
					CycleTables.push_back(std::make_pair(t, cycle));
				}
			}
		}
	}

	void generateCycles(std::vector<std::pair<std::string, int>> _REG) {
		std::vector<varPeriod> V = graph2VarPeriods(DFG);
		sort(V.begin(), V.end(), varPeridCmp_stop);
		int total_cycle = V[V.size() - 1].stopp;		//最后一个变量的终止周期
		std::vector<Cycle> cycle(total_cycle + 1);

		std::vector<node> List = DFG.get_opList();		
		int num_node = 0;								//标识当前的节点语句

		for (int i = 1; i < List.size() - 1; i++) {
			Statement statement;
			statement.compute_resource_index = -1;
			//对于ASSIGN、BR、RET、LOAD、STORE、PHI指令，需要另外处理
			if (List[i].element.getOPtype() == OP_BR) {
				if (List[i].element.getInputvars().size() == 3) {
					statement.optype = OP_BR;
					statement.vars = List[i].InputVar;
					statement.outreg = findregister(_REG, statement.vars[0]);

					for (int j = List[i].getTstart(); j <= List[i].getTend(); j++)
						cycle[j].Statements.push_back(statement);
				}
			}
			else if (List[i].element.getOPtype() == OP_RET) {
				statement.optype = OP_RET;
				statement.vars = List[i].InputVar;
				statement.outreg = findregister(_REG, statement.vars[0]);

				for (int j = List[i].getTstart(); j <= List[i].getTend(); j++)
					cycle[j].Statements.push_back(statement);
			}
			else if (List[i].element.getOPtype() == OP_ASSIGN) {
				statement.optype = OP_ASSIGN;
				statement.vars = List[i].InputVar;
				statement.outreg = findregister(_REG, List[i].element.getOutputvar());

				for (int j = List[i].getTstart(); j <= List[i].getTend(); j++)
					cycle[j].Statements.push_back(statement);
			}
			else if (List[i].element.getOPtype() == OP_LOAD) {
				statement.optype = OP_LOAD;
				statement.vars = List[i].InputVar;
				statement.outreg = findregister(_REG, List[i].element.getOutputvar());

				for (int j = 0; j < statement.vars.size(); j++) {
					int r = findregister(_REG, statement.vars[j]);
					statement.regs.push_back(r);
				}
				for (int j = List[i].getTstart(); j <= List[i].getTend(); j++)
					cycle[j].Statements.push_back(statement);
			}
			//store: 将数据存储到数组：如store(a, 10, c)，将c存储到a[10]
			//因此statement设置的输入为a、10，输出的寄存器为c所存的寄存器
			else if (List[i].element.getOPtype() == OP_STORE) {
				statement.optype = OP_STORE;
				statement.outreg = findregister(_REG, List[i].InputVar[2]);

				for (int j = 0; j < List[i].InputVar.size() - 1; j++) {
					statement.vars.push_back(List[i].InputVar[j]);
					int r = findregister(_REG, List[i].InputVar[j]);
					statement.regs.push_back(r);
				}
				for (int j = List[i].getTstart(); j <= List[i].getTend(); j++)
					cycle[j].Statements.push_back(statement);
			}
			//phi操作
			else if (List[i].element.getOPtype() == OP_PHI) {
				statement.optype = OP_PHI;
				statement.outreg = findregister(_REG, List[i].element.getOutputvar());
				
				for (int j = 0; j < List[i].InputVar.size(); j++) {
					statement.vars.push_back(List[i].InputVar[j]);
					int r = findregister(_REG, List[i].InputVar[j]);
					statement.regs.push_back(r);
					std::string str = List[i].element.getInputvars()[2 * j + 1];
					statement.label.push_back(str);
				}
				for (int j = List[i].getTstart(); j <= List[i].getTend(); j++)
					cycle[j].Statements.push_back(statement);
			}
			//其他指令，均使用到计算资源
			else {
				//对每个OP进行遍历
				int opType = List[i].element.getOPtype();	//每句话的optype
				int compute_index;							//每句话绑定的计算资源
				int reg_return;
				compute_index = CSP[num_node].second;		//每句话绑定的计算资源
				reg_return = Compute[compute_index].outputregister;
			
				std::vector<std::string> inputvars = List[i].InputVar;
				std::vector<int> regs;

				for (int j = 0; j < inputvars.size(); j++) {
					int r = findregister(_REG, inputvars[j]);
					regs.push_back(r);
				}

				statement.compute_resource_index = compute_index;
				statement.optype = opType;
				statement.outreg = reg_return;
				statement.regs = regs;
				statement.vars = inputvars;

				for (int l = List[i].getTstart(); l <= List[i].getTend(); l++) {
					cycle[l].Statements.push_back(statement);
				}
				num_node++;
			}
		}
		C = cycle;
	}

	std::vector<Cycle> getCycle() {
		return C;
	}

	std::vector<std::pair<cycletable, int>> getCycleTables() {
		return CycleTables;
	}

	std::vector<Mux> getMuxs() {
		return Muxs;
	}

	std::vector<Register> getRegs() {
		return Regs;
	}
};
#endif
