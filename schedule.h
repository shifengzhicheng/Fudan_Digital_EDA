#ifndef SCHEDULE_H
#define SCHEDULE_H
#include <algorithm>
#include "dataflowgraph.h"
#include "controlflowgraph.h"
#include<queue>
#include<map>


struct Hardware{
    int adder = 2;
    int mul = 1;
    int div = 1;
    int jmp = 1;
    int sram = 1;
};

bool meet_resources_constraint(std::map<int,struct Hardware>& rec,int i,DataFlowGraph& DFG);

#endif