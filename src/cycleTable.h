#ifndef CYCLETABLE_H
#define CYCLETABLE_H
#include <iostream>
#include <vector>
#include <string>
struct cycletable {
	std::string var;	//变量
	int com;			//变量var活跃期间使用的计算资源
	int reg;			//使用的寄存器
	int mux;			//使用的选择器
};

struct Statement {
	std::vector<std::string> vars;		//输入变量名
	std::vector<int> regs;				//变量对应的寄存器编号
	int optype;							//操作类型
	int compute_resource_index;			//绑定的计算资源标号
	int outreg;							//传出的寄存器编号
	std::vector<std::string> label;		//为phi操作而设，记录数据来自哪一个块
};

struct Cycle {
	std::vector<Statement> Statements;
};
#endif
