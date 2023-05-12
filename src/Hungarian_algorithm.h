#ifndef HUNGARIAN_H
#define HUNGARIAN_H
#include"leftAlgorithm.h"
#include"computeresource.h"


//寻找特定DFG下的变量对应的寄存器
//REGi是对应特定DFG的寄存器与变量绑定结果
int findregister(std::vector<std::pair<std::string, int>> REGi, std::string val) {
	for (std::vector<std::pair<std::string, int>>::iterator it = REGi.begin(); it != REGi.end(); it++) {
		if (it->first == val) {
			return it->second;
		}
	}
	return -1;
}
//寻找特定DFG下寄存器对应的变量名
//string findval(vector<pair<string, int>>REGi, int reg) {
//	for (vector<pair<string, int>>::iterator it = REGi.begin(); it != REGi.end(); it++) {
//		if (it->second == reg) {
//			return it->first;
//		}
//	}
//	cout << "cannot find the val" << endl;
//	return NULL;
//}

//匹配方法：通过计算资源的输出寄存器进行匹配
std::vector<std::pair<int, int>> bindcomputeresource(DataFlowGraph& DFG, std::vector<std::pair<std::string, int>>REGi, std::vector<computeresource>& CORE) {
	std::vector<std::pair<int, int>> ans;
	//标号
	int num = -1;
	//从头到尾遍历oplist，除去头部的空op和尾部的空op不用绑定计算资源
	for (std::vector<node>::iterator it = DFG.get_opList().begin() + 1; it != DFG.get_opList().end() - 1; it++) {
		num++;
		int optype = it->element.getOPtype();
		int reg = findregister(REGi, it->element.getOutputvar());
		//cout << endl;
		//cout << "变量" << it->element.getOutputvar()<< "寄存器" << reg << endl;
		//cout << "node optype:" << optype << endl;
		if (reg <= 0 || optype == 0) {
			//cout << "该node并不需要匹配计算资源" << endl;
		}
		int resourcetype = 0;
		switch (optype) {
			//需要使用乘法器
		case 3:
			resourcetype = 2;
			break;
			//需要使用除法器
		case 4:
			resourcetype = 3;
			break;
			//需要使用加法器
		case 1:
		case 2:
		case 5:
		case 6:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			resourcetype = 1;
			break;
			//其他指令
		default:
			resourcetype = 0;
		}
		int flag1 = 0;
		//首先寻找已经实例化的计算硬件，若有则优先实现匹配
		for (int i = 0; i < CORE.size(); i++) {
			if (CORE[i].outputregister == reg && resourcetype == CORE[i].flag) {//找到了输出寄存器匹配的对应计算资源
				//进行时序判断
				int flagtimejudgement = 0;
				for (std::vector<std::pair<int, int>>::iterator it2 = ans.begin(); it2 != ans.end(); it2++) {
					if (it2->second == i) {//说明该DFG之前有匹配过该计算资源
						if (DFG.get_opList()[it2->first].getTstart() > it->getTend() || DFG.get_opList()[it2->first].getTend() < it->getTstart()) {//满足时序
							continue;
						}
						else {//满足，即该时间段已经被其他op绑定
							flagtimejudgement = 1;
						}
					}
				}
				if (flagtimejudgement == 0) {//匹配到之前已经实例化的计算资源
					//更改输入寄存器（默认node的InputerVar只有一个或两个）
					for (std::vector<std::string>::iterator it3 = it->InputVar.begin(); it3 != it->InputVar.end(); it3++) {
						if (1 == it->InputVar.size()) {
							int reg1 = findregister(REGi, it->InputVar[0]);
							//cout << "reg1:" << reg1 << endl;
							if (CORE[i].findareg(reg1) || CORE[i].findbreg(reg1))
								continue;
							else {
								CORE[i].setinputAregisters(reg1);
								continue;
							}
						}
						if (2 == it->InputVar.size()) {
							if (CORE[i].flag == 1 || CORE[i].flag == 2) {
								if (CORE[i].findareg(findregister(REGi, it->InputVar[0])) && CORE[i].findbreg(findregister(REGi, it->InputVar[1])) || CORE[i].findareg(findregister(REGi, it->InputVar[1])) && CORE[i].findbreg(findregister(REGi, it->InputVar[0])))
									continue;
								else if (CORE[i].findareg(findregister(REGi, it->InputVar[0])))
									CORE[i].setinputBregisters(findregister(REGi, it->InputVar[1]));
								else if (CORE[i].findbreg(findregister(REGi, it->InputVar[0])))
									CORE[i].setinputAregisters(findregister(REGi, it->InputVar[1]));
								else if (CORE[i].findareg(findregister(REGi, it->InputVar[1])))
									CORE[i].setinputBregisters(findregister(REGi, it->InputVar[0]));
								else if (CORE[i].findbreg(findregister(REGi, it->InputVar[1])))
									CORE[i].setinputAregisters(findregister(REGi, it->InputVar[0]));
								else {
									CORE[i].setinputAregisters(findregister(REGi, it->InputVar[0]));
									CORE[i].setinputBregisters(findregister(REGi, it->InputVar[1]));
								}
							}
							else if (CORE[i].flag == 3) {
								if (!CORE[i].findareg(findregister(REGi, it->InputVar[0])))
									CORE[i].setinputAregisters(findregister(REGi, it->InputVar[0]));
								if (!CORE[i].findbreg(findregister(REGi, it->InputVar[1])))
									CORE[i].setinputBregisters(findregister(REGi, it->InputVar[1]));
							}

						}
					}
					ans.push_back(std::make_pair(num, i));
					flag1 = 1;
					//cout << "已存在计算资源，并完成分配计算资源" << endl;
				}
			}
			else {
				continue;
			}
		}
		if (flag1 == 0 && resourcetype != 0) {//未匹配到实例化的计算资源,则实例化一个新的计算资源，并将该资源匹配给该node
			computeresource com(resourcetype, reg);
			if (it->InputVar.size() == 1) {
				//cout << "reg1:" << findregister(REGi, it->InputVar[0]) << endl;
				com.setinputAregisters(findregister(REGi, it->InputVar[0]));
			}
			else if (it->InputVar.size() == 2) {
				com.setinputAregisters(findregister(REGi, it->InputVar[0]));
				//cout << "reg1:" << findregister(REGi, it->InputVar[0]) << endl;
				com.setinputBregisters(findregister(REGi, it->InputVar[1]));
				//cout << "reg2:" << findregister(REGi, it->InputVar[1]) << endl;
			}
			ans.push_back(std::make_pair(num, CORE.size()));
			CORE.push_back(com);
			//cout << "未匹配到旧有计算资源，新创建一个计算资源" << endl;
		}
	}
	return ans;
}

#endif
