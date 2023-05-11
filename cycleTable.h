#pragma once
#ifndef CYCLETABLE_H
#define CYCLETABLE_H
struct Statement {
	std::vector<std::string> vars;		//输入变量名
	std::vector<int> regs;				//变量对应的寄存器编号
	int optype;							//操作类型
	int compute_resource_index;			//绑定的计算资源标号
	int outreg;							//传出的寄存器编号
	std::vector<std::string> label;		//为phi操作而设，记录数据来自哪一个块
	//bool S_or_E;
};

struct Cycle {
	std::vector<Statement> Statements;
};
#endif
