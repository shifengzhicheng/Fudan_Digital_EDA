#ifndef LEFT_H
#define LEFT_H
#include <algorithm>
#include "dataflowgraph.h"

//using namespace std;

struct varPeriod {
	std::string var;
	int startp;
	int stopp;
};

bool varPeridCmp(varPeriod a, varPeriod b)
{
	return a.startp < b.startp;
}

std::vector<std::pair<std::string, int>> leftAlgorithm(std::vector<varPeriod> V)
{
	std::vector<std::pair<std::string, int>> ans;
	int reg = 1;
	sort(V.begin(), V.end(), varPeridCmp);
	while (!V.empty())
	{
		varPeriod v = V[0];
		ans.push_back(make_pair(v.var, reg));
		int endpos = v.stopp;
		V.erase(V.begin());
		for (std::vector<varPeriod>::iterator iter = V.begin(); iter != V.end();)
		{
			if ((*iter).startp > endpos)
			{
				ans.push_back(make_pair((*iter).var, reg));
				endpos = (*iter).stopp;
				iter = V.erase(iter);
			}
			else
				iter++;
		}
		reg++;
	}
	return ans;
}

bool isPureNumber(std::string str)
{
	bool isPureNumber = true;
	for (char c : str) {
		if (!std::isdigit(c)) {
			isPureNumber = false;
			break;
		}
	}
	return isPureNumber;
}


std::vector<varPeriod> graph2VarPeriods(DataFlowGraph& DFG)
{
	std::vector<node> m_ops = DFG.get_opList();
	std::unordered_map<std::string, int> m_map = DFG.myOutvartable();
	std::unordered_map<std::string, int> varMap; // int为该var存活的最后周期
	std::vector<varPeriod> varPeriods;
	for (std::vector<node>::iterator iter = m_ops.begin(); iter != m_ops.end(); iter++)
	{
		std::vector<std::string> m_inputVars = (*iter).InputVar;
		for (std::vector<std::string>::iterator varIter = m_inputVars.begin(); varIter != m_inputVars.end(); varIter++)
		{
			if (varMap.find(*varIter) != varMap.end())
			{
				if (varMap[*varIter] < (*iter).getTend())
					varMap[*varIter] = (*iter).getTend();
			}
			else if(!isPureNumber(*varIter))
				varMap[*varIter] = (*iter).getTend();
		}
	}
	for (std::unordered_map<std::string, int>::iterator miter = m_map.begin(); miter != m_map.end(); miter++)
	{
		std::string var = (*miter).first;
		int startp = m_ops[(*miter).second].getTend() + 1;
		int endp = varMap[var];
		varPeriods.push_back(varPeriod{ var, startp, endp });
	}
	return varPeriods;
}

std::vector<std::pair<std::string, int>> binding(DataFlowGraph& DFG)
{
	return leftAlgorithm(graph2VarPeriods(DFG));
}

#endif