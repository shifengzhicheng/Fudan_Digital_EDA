#ifndef HUNGARIAN_H
#define HUNGARIAN_H
#include "leftAlgorithm.h"
#include "computeresource.h"
#include "schedule.h"

// 寻找特定DFG下的变量对应的寄存器
// REGi是对应特定DFG的寄存器与变量绑定结果
int findregister(std::vector<std::pair<std::string, int>> REGi, std::string val)
{
	for (std::vector<std::pair<std::string, int>>::iterator it = REGi.begin(); it != REGi.end(); it++)
	{
		if (val == it->first)
		{
			return it->second;
		}
		else
		{
			continue;
		}
	}
	return -1;
}
// 通过oplist角标查找其对应计算资源种类
int getresourcetype(DataFlowGraph &DFG, int num)
{
	// std::cout << "allll" << num << std::endl;
	if (DFG.get_opList()[num].element.getOPtype() == 1 || DFG.get_opList()[num].element.getOPtype() == 2 || DFG.get_opList()[num].element.getOPtype() == 5 || DFG.get_opList()[num].element.getOPtype() == 6 || DFG.get_opList()[num].element.getOPtype() == 8 || DFG.get_opList()[num].element.getOPtype() == 9 || DFG.get_opList()[num].element.getOPtype() == 10 || DFG.get_opList()[num].element.getOPtype() == 1 || DFG.get_opList()[num].element.getOPtype() == 11 || DFG.get_opList()[num].element.getOPtype() == 12 || DFG.get_opList()[num].element.getOPtype() == 13)
		return 1;
	else if (DFG.get_opList()[num].element.getOPtype() == 3)
		return 2;
	else if (DFG.get_opList()[num].element.getOPtype() == 4)
		return 3;
	else
	{
		return 0;
	}
	return -1;
}
// 生成某一时刻的矩阵
std::vector<std::vector<int>> creatematrix(DataFlowGraph &DFG, std::vector<int> list, std::vector<std::pair<std::string, int>> REGi, std::vector<computeresource> &CORE, int flag, Hardware &hardware)
{				 // flag表征生成的矩阵计算资源类别
	int col = 0; // sum为矩阵的列数（即）
	for (std::vector<int>::iterator it = list.begin(); it != list.end(); it++)
	{
		if (flag == 1)
		{ // 该计算资源为加法器
			if (DFG.get_opList()[(*it)].element.getOPtype() == 1 || DFG.get_opList()[(*it)].element.getOPtype() == 2 || DFG.get_opList()[(*it)].element.getOPtype() == 5 || DFG.get_opList()[(*it)].element.getOPtype() == 6 || DFG.get_opList()[(*it)].element.getOPtype() == 8 || DFG.get_opList()[(*it)].element.getOPtype() == 9 || DFG.get_opList()[(*it)].element.getOPtype() == 10 || DFG.get_opList()[(*it)].element.getOPtype() == 11 || DFG.get_opList()[(*it)].element.getOPtype() == 12 || DFG.get_opList()[(*it)].element.getOPtype() == 13)
				col++;
		}
		else if (flag == 2)
		{
			if (DFG.get_opList()[(*it)].element.getOPtype() == 3)
				col++;
		}
		else if (flag == 3)
		{
			if (DFG.get_opList()[(*it)].element.getOPtype() == 4)
				col++;
		}
	}
	std::vector<std::vector<int>> ans;
	if (col == 0)
	{
		return ans; // 如果col为0，表明该块内无该类型的计算资源调度
	}
	int row = 0; // 矩阵的列数，表征
	if (flag == 1)
	{
		row = hardware.get_adder_num();
	}
	else if (flag == 2)
	{
		row = hardware.get_mul_num();
	}
	else if (flag == 3)
	{
		row = hardware.get_div_num();
	}
	for (std::vector<int>::iterator it = list.begin(); it != list.end(); it++)
	{ // 对应矩阵行
		std::vector<int> ans1;
		if (flag == 1)
		{
			for (int i = 0; i < row; i++)
			{
				int val = 0;
				if (!CORE[i].findareg(findregister(REGi, (DFG.get_opList()[*it].InputVar[0]))))
					val++;
				if (DFG.get_opList()[*it].InputVar.size() > 1 && !CORE[i].findareg(findregister(REGi, (DFG.get_opList()[*it].InputVar[1]))))
					val++;
				// if (!CORE[i].findoutreg(findregister(REGi, (DFG.get_opList()[*it].InputVar[0]))))
				//	val++;
				ans1.push_back(val);
			}
		}
		else if (flag == 2)
		{
			for (int i = hardware.get_adder_num(); i < hardware.get_adder_num() + row; i++)
			{
				int val = 0;
				if (!CORE[i].findareg(findregister(REGi, (DFG.get_opList()[*it].InputVar[0]))))
					val++;
				if (DFG.get_opList()[*it].InputVar.size() > 1 && !CORE[i].findareg(findregister(REGi, (DFG.get_opList()[*it].InputVar[1]))))
					val++;
				if (!CORE[i].findoutreg(findregister(REGi, (DFG.get_opList()[*it].InputVar[0]))))
					val++;
				ans1.push_back(val);
			}
		}
		else if (flag == 3)
		{
			for (int i = hardware.get_adder_num() + hardware.get_mul_num(); i < hardware.get_adder_num() + hardware.get_mul_num() + row; i++)
			{
				int val = 0;
				if (!CORE[i].findareg(findregister(REGi, (DFG.get_opList()[*it].InputVar[0]))))
					val++;
				if (DFG.get_opList()[*it].InputVar.size() > 1 && !CORE[i].findareg(findregister(REGi, (DFG.get_opList()[*it].InputVar[1]))))
					val++;
				if (!CORE[i].findoutreg(findregister(REGi, (DFG.get_opList()[*it].InputVar[0]))))
					val++;
				ans1.push_back(val);
			}
		}
		ans.push_back(ans1);
	}
	return ans;
}
struct cost_matrix_node
{
	int cost;	  // 代价
	int flag = 0; // 表征是否被选中
};
class position
{
public:
	int col;
	int row;
	position(int i, int j)
	{
		col = i;
		row = j;
	}
	position();
};
class circle
{ // 用以存储最大匹配的圈
public:
	int flag; // flag = 1表明为col，flag = 2表明为row
	int num;  // 第num行or列
	circle(int i, int j)
	{
		flag = i;
		num = j;
	}
	circle();
};
// 得到最大匹配值，并对matrix2进行覆盖
int maxcompair(std::vector<std::vector<cost_matrix_node>> &matrix2)
{
	std::vector<position> zeros; // 存储matrix2中所有的零的位置
	for (int i = 0; i < matrix2.size(); i++)
	{
		for (int j = 0; j < matrix2[0].size(); j++)
		{
			if (matrix2[i][j].cost == 0)
			{
				zeros.push_back(position(i, j));
			}
			matrix2[i][j].flag = 0; // 初始化flag，便于后续覆盖
		}
	}
	// 生成zeros
	std::vector<circle> cir;
	for (int i = 0; i < zeros.size(); i++)
	{
		if (matrix2[zeros[i].col][zeros[i].row].flag == 0)
		{ // 说明尚未被覆盖
			int ll = 0;
			for (int j = i + 1; j < zeros.size(); j++)
			{
				if (zeros[i].col == zeros[j].col && matrix2[zeros[j].col][zeros[j].row].flag == 0)
				{
					cir.push_back(circle(1, zeros[i].col));
					for (int k = 0; k < matrix2[0].size(); k++)
					{
						matrix2[zeros[i].col][k].flag = matrix2[zeros[i].col][k].flag + 1; // 对该圈进行覆盖
					}
					ll = 1;
					break;
				}
				if (zeros[i].row == zeros[j].row && matrix2[zeros[j].col][zeros[j].row].flag == 0)
				{
					cir.push_back(circle(2, zeros[i].row));
					for (int k = 0; k < matrix2.size(); k++)
					{
						matrix2[k][zeros[i].row].flag = matrix2[k][zeros[i].row].flag + 1; // 对该圈进行覆盖
					}
					ll = 1;
					break;
				}
			}
			if (ll == 0)
			{											// 说明该0并没有被某一个圈包括，需要自己生成一个圈
				cir.push_back(circle(1, zeros[i].col)); // 按行画圈
				for (int k = 0; k < matrix2[0].size(); k++)
				{
					matrix2[zeros[i].col][k].flag = matrix2[zeros[i].col][k].flag + 1; // 对该圈进行覆盖
				}
			}
		}
	}
	return cir.size();
}
// 匈牙利算法化简
std::vector<std::pair<int, int>> Hungarian(DataFlowGraph &DFG, std::vector<int> &list, std::vector<std::pair<std::string, int>> REGi, std::vector<computeresource> &CORE, std::vector<std::vector<int>> matrix, int flag, Hardware &hardware, int &k)
{ // k代表已经绑定的op个数
	std::vector<std::pair<int, int>> ans;
	int num = -1;
	if (list.size() == 0)
	{ // 如果list为空，则不匹配
		return ans;
	}
	// 匈牙利算法第一步，完成
	std::vector<std::vector<int>> matrix1;
	for (std::vector<std::vector<int>>::iterator itcol = matrix.begin(); itcol != matrix.end(); itcol++)
	{
		num++;
		// 如果计算资源的个数为1，那么这种类型的计算操作只能绑定该计算资源
		if (itcol->size() == 1)
		{
			int i;
			if (flag == 1)
				i = 0;
			else if (flag == 2)
				i = hardware.get_adder_num();
			else if (flag == 3)
				i = hardware.get_adder_num() + hardware.get_mul_num();
			ans.push_back(std::make_pair(list[num], i));
			k++;
		}
		else
		{
			int min = 3;
			// 找出每一行行中最小值
			for (std::vector<int>::iterator itrow = itcol->begin(); itrow != itcol->end(); itrow++)
			{
				if (*itrow < min)
				{
					min = *itrow;
				}
			}
			std::vector<int> matrixcol;
			for (std::vector<int>::iterator itrow = itcol->begin(); itrow != itcol->end(); itrow++)
			{
				int key = *itrow - min;
				matrixcol.push_back(key);
			}
			matrix1.push_back(matrixcol);
		}
	}
	if (matrix[0].size() == 1)
	{
		return ans;
	}
	matrix.clear();

	std::vector<int> min;
	std::vector<std::vector<cost_matrix_node>> matrix2;
	for (int i = 0; i < matrix1[0].size(); i++)
	{
		min.push_back(3);
		for (std::vector<std::vector<int>>::iterator itcol = matrix1.begin(); itcol != matrix1.end(); itcol++)
		{
			if ((*itcol)[i] < min[i])
				min[i] = (*itcol)[i];
		}
	}
	for (std::vector<std::vector<int>>::iterator itcol = matrix1.begin(); itcol != matrix1.end(); itcol++)
	{
		std::vector<cost_matrix_node> matrix1col;
		for (std::vector<int>::iterator itrow = itcol->begin(); itrow != itcol->end(); itrow++)
		{
			cost_matrix_node n1;
			if (min[*itrow] != 0)
			{
				int key = *itrow - min[*itrow];
				n1.cost = key;
			}
			else
			{
				int key = *itrow;
				n1.cost = key;
			}
			matrix1col.push_back(n1);
		}
		matrix2.push_back(matrix1col);
	}
	matrix1.clear();

	// 生成了matrix2
	int pairsum;
	if (matrix2.size() >= matrix2[0].size())
	{ // node结点大于计算资源数
		pairsum = matrix2[0].size();
	}
	else // node结点小于计算资源数
		pairsum = matrix2.size();
	int pair = maxcompair(matrix2);
	while (pair < pairsum)
	{
		int sum = 3;
		for (int i = 0; i < matrix2.size(); i++)
		{
			for (int j = 0; j < matrix2[0].size(); j++)
			{
				if (matrix2[i][j].flag == 0)
				{ // 从未覆盖的元素中找最小值
					if (matrix2[i][j].cost < sum)
					{
						sum = matrix2[i][j].cost;
					}
				}
			}
		}
		for (int i = 0; i < matrix2.size(); i++)
		{
			for (int j = 0; j < matrix2[0].size(); j++)
			{
				if (matrix2[i][j].flag == 0)
				{
					matrix2[i][j].cost = matrix2[i][j].cost - sum;
				}
				else if (matrix2[i][j].flag == 2)
				{
					matrix2[i][j].cost = matrix2[i][j].cost + sum;
				}
			}
		}							// 进一步对矩阵操作
		pair = maxcompair(matrix2); // 再次运行最大匹配算法
	}
	// 匹配操作结束，已找到最大匹配
	std::vector<position> zeros; // 存储matrix2中所有的零的位置
	for (int i = 0; i < matrix2.size(); i++)
	{
		for (int j = 0; j < matrix2[0].size(); j++)
		{
			if (matrix2[i][j].cost == 0)
			{
				zeros.push_back(position(i, j));
			}
			matrix2[i][j].flag = 0; // 初始化flag，便于后续覆盖
		}
	}
	// 生成zeros
	std::vector<circle> cir;
	for (int i = 0; i < zeros.size(); i++)
	{
		if (matrix2[zeros[i].col][zeros[i].row].flag == 0)
		{ // 说明尚未被覆盖
			int ll = 0;
			for (int j = i + 1; j < zeros.size(); j++)
			{
				if (zeros[i].col == zeros[j].col && matrix2[zeros[j].col][zeros[j].row].flag == 0)
				{
					cir.push_back(circle(1, zeros[i].col));
					for (int k = 0; k < matrix2[0].size(); k++)
					{
						matrix2[zeros[i].col][k].flag = 1; // 对该圈进行覆盖
					}
					ll = 1;
					break;
				}
				if (zeros[i].row == zeros[j].row && matrix2[zeros[j].col][zeros[j].row].flag == 0)
				{
					cir.push_back(circle(2, zeros[i].row));
					for (int k = 0; k < matrix2.size(); k++)
					{
						matrix2[k][zeros[i].row].flag = 1; // 对该圈进行覆盖
					}
					ll = 1;
					break;
				}
			}
			if (ll == 0)
			{ // 说明该0并没有被某一个圈包括，需要自己生成一个圈
				// cir.push_back(circle(1, zeros[i].col));//按行画圈
				if (flag == 1 && ans.size() <= hardware.get_adder_num())
				{
					ans.push_back(std::make_pair(list[zeros[i].col], zeros[i].row));
					// list[zeros[i].col] = -1;//绑定后，该值变为-1
				}
				else if (flag == 2 && ans.size() <= hardware.get_mul_num())
				{
					ans.push_back(std::make_pair(list[zeros[i].col], zeros[i].row + hardware.get_adder_num()));
					// list[zeros[i].col] = -1;
				}
				else if (flag == 3 && ans.size() <= hardware.get_div_num())
				{
					ans.push_back(std::make_pair(list[zeros[i].col], zeros[i].row + hardware.get_adder_num() + hardware.get_available_mul()));
					// list[zeros[i].col] = -1;
				}
				k++;
				// 完成绑定计算资源
				for (int k = 0; k < matrix2[0].size(); k++)
				{
					matrix2[zeros[i].col][k].flag = 2; // 2表示选定
				}
				for (int k = 0; k < matrix2.size(); k++)
				{
					matrix2[k][zeros[i].row].flag = 2;
				}
				// std::cout << ans.size() << std::endl;
			}
		}
	}
	for (std::vector<circle>::iterator it = cir.begin(); it != cir.end(); it++)
	{ // 每一个圈都要进行一个计算资源匹配

		if (it->flag == 1)
		{
			for (int i = 0; i < matrix2[0].size(); i++)
			{
				if (matrix2[it->num][i].flag == 1 && matrix2[it->num][i].cost == 0)
				{
					if (flag == 1 && ans.size() <= hardware.get_adder_num())
					{
						ans.push_back(std::make_pair(list[it->num], i));
						// list[it->num] = -1;
					}
					else if (flag == 2 && ans.size() <= hardware.get_mul_num())
					{
						ans.push_back(std::make_pair(list[it->num], i + hardware.get_adder_num()));
						// list[it->num] = -1;
					}
					else if (flag == 3 && ans.size() <= hardware.get_div_num())
					{
						ans.push_back(std::make_pair(list[it->num], +hardware.get_adder_num() + hardware.get_available_mul()));
						// list[it->num] = -1;
					}
					k++;
					// 完成绑定计算资源
					for (int k = 0; k < matrix2[0].size(); k++)
					{
						matrix2[it->num][k].flag = 2; // 2表示选定
					}
					for (int k = 0; k < matrix2.size(); k++)
					{
						matrix2[k][i].flag = 2;
					}
					// std::cout << ans.size() << std::endl;
					continue;
				}
			}
		}
		else if (it->flag == 2)
		{
			for (int i = 0; i < matrix2.size(); i++)
			{
				if (matrix2[i][it->num].flag == 1 && matrix2[i][it->num].cost == 0)
				{
					if (flag == 1 && ans.size() <= hardware.get_adder_num())
					{
						ans.push_back(std::make_pair(list[i], it->num));
						// list[i] = -1;
					}
					else if (flag == 2 && ans.size() <= hardware.get_mul_num())
					{
						ans.push_back(std::make_pair(list[i], it->num + hardware.get_adder_num()));
						// list[i] = -1;
					}
					else if (flag == 3 && ans.size() <= hardware.get_div_num())
					{
						ans.push_back(std::make_pair(list[i], it->num + hardware.get_adder_num() + hardware.get_available_mul()));
						// list[i] = -1;
					}
					k++;
					// 完成绑定计算资源
					for (int k = 0; k < matrix2[0].size(); k++)
					{
						matrix2[i][k].flag = 2; // 2表示选定
					}
					for (int k = 0; k < matrix2.size(); k++)
					{
						matrix2[k][it->num].flag = 2;
					}
					// std::cout << ans.size() << std::endl;
					continue;
				}
			}
		}
	}
	return ans;
}
std::vector<std::pair<int, int>> bindcomputeresource(DataFlowGraph &DFG, std::vector<std::pair<std::string, int>> REGi, std::vector<computeresource> &CORE, Hardware &hardware)
{
	std::vector<std::pair<int, int>> ans;
	// 标号
	int num = -1;
	// 初始时按照hardware实例化计算资源
	if (CORE.size() == 0)
	{
		int numofadder = hardware.get_adder_num();
		int numofmul = hardware.get_mul_num();
		int numofdiv = hardware.get_div_num();
		for (int i = 0; i < numofadder; i++)
		{
			CORE.push_back(1);
		}
		for (int i = numofadder; i < numofadder + numofmul; i++)
		{
			CORE.push_back(2);
		}
		for (int i = numofadder + numofmul; i < numofadder + numofmul + numofdiv; i++)
		{
			CORE.push_back(3);
		}
	}
	DFG.Initialize();
	// 压入所有入度为0的节点
	int CurrentNode;
	std::queue<int> tq1, tq2;
	// std::vector<int> listre = DFG.get_opList()[0].next;//list为同一周期内并行的所有op操作
	for (int i = 0; i < DFG.get_opList().size(); i++)
	{
		if (DFG.InVertex[i] == 0)
		{
			tq1.push(i);
		}
	}
	int k = 0;
	// std::cout <<"该块中的总node数为：" << tq1.size()  << std::endl;
	while (!tq1.empty() || !tq2.empty())
	{
		std::vector<int> listre;
		while (!tq1.empty())
		{
			CurrentNode = tq1.front();
			tq1.pop();
			DFG.Mark[CurrentNode] = VISITED;
			listre.push_back(CurrentNode);
			for (int i = 0; i < DFG.ToVertex(CurrentNode).size(); i++)
			{
				// 得到第i个下一节点，如果这个节点这某个输入变量依赖于当前节点
				int nextNodeIndex = DFG.ToVertex(CurrentNode)[i];
				node &nextNode = DFG.get_opList()[nextNodeIndex];
				// 根据第i个节点的输入变量情况，减少入度
				DFG.InVertex[nextNodeIndex]--;
				// 入度小于等于0则进入队列
				if (DFG.InVertex[DFG.ToVertex(CurrentNode)[i]] <= 0 && DFG.Mark[DFG.ToVertex(CurrentNode)[i]] == UNVISITED)
				{
					tq2.push(DFG.ToVertex(CurrentNode)[i]);
				}
			}
		}
		std::vector<int> listadd;
		std::vector<int> listmul;
		std::vector<int> listdiv;
		// 按计算资源种类分类
		for (std::vector<int>::iterator it1 = listre.begin(); it1 != listre.end(); it1++)
		{
			int flag = getresourcetype(DFG, *it1);
			// std::cout << "flag:" << flag << std::endl;
			int tmp = *it1;
			if (flag == 1)
				listadd.push_back(tmp);
			else if (flag == 2)
				listmul.push_back(tmp);
			else if (flag == 3)
				listdiv.push_back(tmp);
		}

		while (listadd.size() > 0)
		{
			std::vector<std::vector<int>> matrixadder = creatematrix(DFG, listadd, REGi, CORE, 1, hardware);
			std::vector<std::pair<int, int>> ansadder = Hungarian(DFG, listadd, REGi, CORE, matrixadder, 1, hardware, k);
			for (std::vector<std::pair<int, int>>::iterator ita = ansadder.begin(); ita != ansadder.end(); ita++)
			{
				ans.push_back(*ita);
				// 更改输入寄存器（默认node的InputerVar只有一个或两个）
				for (std::vector<std::string>::iterator it3 = DFG.get_opList()[ita->first].InputVar.begin(); it3 != DFG.get_opList()[ita->first].InputVar.end(); it3++)
				{
					if (1 == DFG.get_opList()[ita->first].InputVar.size())
					{
						int reg1 = findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]);
						// cout << "reg1:" << reg1 << endl;
						if (CORE[ita->second].findareg(reg1) || CORE[ita->second].findbreg(reg1))
							continue;
						else
						{
							CORE[ita->second].setinputAregisters(reg1);
							continue;
						}
					}
					if (2 == DFG.get_opList()[ita->first].InputVar.size())
					{
						if (CORE[ita->second].flag == 1 || CORE[ita->second].flag == 2)
						{
							if (CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])) && CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])) || CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])) && CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								continue;
							else if (CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
							else if (CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
							else if (CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])))
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
							else if (CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])))
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
							else
							{
								// std::cout << "左边输入寄存器和右边输入寄存器分别绑定为"<< DFG.get_opList()[ita->first].InputVar[0] <<findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]) << findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]) << std::endl;
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
							}
						}
						else if (CORE[ita->second].flag == 3)
						{
							if (!CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
							if (!CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])))
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
						}
					}
				}
			}
			std::vector<int> listtmp;
			for (std::vector<int>::iterator itla = listadd.begin(); itla != listadd.end(); itla++)
			{
				int tik = 0;
				for (std::vector<std::pair<int, int>>::iterator itans = ansadder.begin(); itans != ansadder.end(); itans++)
				{
					if (*itla == itans->first)
					{
						tik = 1;
					}
				}
				if (tik == 0)
				{
					listtmp.push_back(*itla);
				}
			}
			listadd.clear();
			listadd = listtmp;
		}
		while (listmul.size() > 0)
		{
			std::vector<std::vector<int>> matrixmul = creatematrix(DFG, listmul, REGi, CORE, 2, hardware);
			std::vector<std::pair<int, int>> ansmul = Hungarian(DFG, listmul, REGi, CORE, matrixmul, 2, hardware, k);
			for (std::vector<std::pair<int, int>>::iterator ita = ansmul.begin(); ita != ansmul.end(); ita++)
			{
				ans.push_back(*ita);
				// 更改输入寄存器（默认node的InputerVar只有一个或两个）
				for (std::vector<std::string>::iterator it3 = DFG.get_opList()[ita->first].InputVar.begin(); it3 != DFG.get_opList()[ita->first].InputVar.end(); it3++)
				{
					if (1 == DFG.get_opList()[ita->first].InputVar.size())
					{
						int reg1 = findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]);
						// cout << "reg1:" << reg1 << endl;
						if (CORE[ita->second].findareg(reg1) || CORE[ita->second].findbreg(reg1))
							continue;
						else
						{
							CORE[ita->second].setinputAregisters(reg1);
							continue;
						}
					}
					if (2 == DFG.get_opList()[ita->first].InputVar.size())
					{
						if (CORE[ita->second].flag == 1 || CORE[ita->second].flag == 2)
						{
							if (CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])) && CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])) || CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])) && CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								continue;
							else if (CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
							else if (CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
							else if (CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])))
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
							else if (CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])))
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
							else
							{
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
							}
						}
						else if (CORE[ita->second].flag == 3)
						{
							if (!CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
							if (!CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])))
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
						}
					}
				}
			}
			std::vector<int> listtmp;
			for (std::vector<int>::iterator itla = listmul.begin(); itla != listmul.end(); itla++)
			{
				int tik = 0;
				for (std::vector<std::pair<int, int>>::iterator itans = ansmul.begin(); itans != ansmul.end(); itans++)
				{
					if (*itla == itans->first)
					{
						tik = 1;
					}
				}
				if (tik == 0)
				{
					listtmp.push_back(*itla);
				}
			}
			listmul.clear();
			listmul = listtmp;
		}
		while (listdiv.size() > 0)
		{
			std::vector<std::vector<int>> matrixdiv = creatematrix(DFG, listdiv, REGi, CORE, 3, hardware);
			std::vector<std::pair<int, int>> ansdiv = Hungarian(DFG, listdiv, REGi, CORE, matrixdiv, 3, hardware, k);
			for (std::vector<std::pair<int, int>>::iterator ita = ansdiv.begin(); ita != ansdiv.end(); ita++)
			{
				ans.push_back(*ita);
				// 更改输入寄存器（默认node的InputerVar只有一个或两个）
				for (std::vector<std::string>::iterator it3 = DFG.get_opList()[ita->first].InputVar.begin(); it3 != DFG.get_opList()[ita->first].InputVar.end(); it3++)
				{
					if (1 == DFG.get_opList()[ita->first].InputVar.size())
					{
						int reg1 = findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]);
						// cout << "reg1:" << reg1 << endl;
						if (CORE[ita->second].findareg(reg1) || CORE[ita->second].findbreg(reg1))
							continue;
						else
						{
							CORE[ita->second].setinputAregisters(reg1);
							continue;
						}
					}
					if (2 == DFG.get_opList()[ita->first].InputVar.size())
					{
						if (CORE[ita->second].flag == 1 || CORE[ita->second].flag == 2)
						{
							if (CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])) && CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])) || CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])) && CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								continue;
							else if (CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
							else if (CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
							else if (CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])))
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
							else if (CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])))
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
							else
							{
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
							}
						}
						else if (CORE[ita->second].flag == 3)
						{
							if (!CORE[ita->second].findareg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0])))
								CORE[ita->second].setinputAregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[0]));
							if (!CORE[ita->second].findbreg(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1])))
								CORE[ita->second].setinputBregisters(findregister(REGi, DFG.get_opList()[ita->first].InputVar[1]));
						}
					}
				}
			}
			std::vector<int> listtmp;
			for (std::vector<int>::iterator itla = listdiv.begin(); itla != listdiv.end(); itla++)
			{
				int tik = 0;
				for (std::vector<std::pair<int, int>>::iterator itans = ansdiv.begin(); itans != ansdiv.end(); itans++)
				{
					if (*itla == itans->first)
					{
						tik = 1;
					}
				}
				if (tik == 0)
				{
					listtmp.push_back(*itla);
				}
			}
			listdiv.clear();
			listdiv = listtmp;
		}

		// std::cout << "1" << std::endl;

		while (!tq2.empty())
		{
			CurrentNode = tq2.front();
			tq2.pop();
			tq1.push(CurrentNode);
		}
	}
	for (int k = 0; k < ans.size(); k++)
	{
		std::string name;
		if (CORE[ans[k].second].flag == 1)
		{
			name = "adder     ";
		}
		else if (CORE[ans[k].second].flag == 2)
		{
			name = "multiplier";
		}
		else if (CORE[ans[k].second].flag == 3)
		{
			name = "divider   ";
		}
		std::cout << "Node Number: " << ans[k].first << "\tComputation Resource Type:" << name << "\tComputation Resource Number:  " << ans[k].second << std::endl;
	}
	return ans;
}

void bindoutputregister(DataFlowGraph &DFG, std::vector<std::pair<std::string, int>> REGi, std::vector<computeresource> &CORE, std::vector<std::pair<int, int>> CSPi)
{
	for (int k = 0; k < CSPi.size(); k++)
	{

		int outreg = findregister(REGi, DFG.get_opList()[CSPi[k].first].element.getOutputvar());
		if (CORE[CSPi[k].second].findoutreg(outreg))
		{
			continue;
		}
		else
		{
			CORE[CSPi[k].second].setoutputregister(outreg);
		}
	}
}
#endif
