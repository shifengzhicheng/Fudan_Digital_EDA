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
    int sram = 1;
};

bool meet_resources_constraint(std::map<int,struct Hardware>& rec,int i,DataFlowGraph& DFG);
void table_schedule(ControlFlowGraph& CFG);
void ASAP(DataFlowGraph& DFG);
void ALAP(DataFlowGraph& DFG);
bool cmp(const std::pair<int,int>& a,const std::pair<int,int>& b);
void improved_table_schedule_forDFG(DataFlowGraph& DFG);
void improved_schedule_forCFG(ControlFlowGraph& CFG);

#endif