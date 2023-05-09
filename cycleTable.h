#pragma once
#ifndef CYCLETABLE_H
#define CYCLETABLE_H
struct Statement {
	std::vector<std::string> vars;
	std::vector<int> regs;
	int optype;
	int com;
	int outreg;
};

struct Cycle {
	std::vector<Statement> Statements;
};
#endif
