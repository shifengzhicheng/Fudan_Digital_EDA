#ifndef SCHEDULE_H
#define SCHEDULE_H
#include <algorithm>
#include "dataflowgraph.h"
#include "controlflowgraph.h"
#include <queue>
#include <map>

class Hardware
{
private:
    int adder;
    int mul;
    int div;
    int sram;
    int adder_available;
    int mul_available;
    int div_available;
    int sram_available;

public:
    Hardware()
    {
        adder = 2;
        mul = 1;
        div = 1;
        sram = 1;
        adder_available = adder;
        mul_available = mul;
        div_available = div;
        sram_available = sram;
    }
    Hardware(int adder_, int mul_, int div_, int sram_)
    {
        adder = adder_;
        mul = mul_;
        div = div_;
        sram = sram_;
        adder_available = adder;
        mul_available = mul;
        div_available = div;
        sram_available = sram;
    }
    int get_adder_num()
    {
        return adder;
    }
    int get_mul_num()
    {
        return mul;
    }
    int get_div_num()
    {
        return div;
    }
    int get_sram_num()
    {
        return sram;
    }
    int get_available_adder()
    {
        return adder_available;
    }
    int get_available_mul()
    {
        return mul_available;
    }
    int get_available_div()
    {
        return div_available;
    }
    int get_available_sram()
    {
        return sram_available;
    }
    void set_available_adder(int adder_curr)
    {
        adder_available = adder_curr;
    }
    void set_available_mul(int mul_curr)
    {
        mul_available = mul_curr;
    }
    void set_available_div(int div_curr)
    {
        div_available = div_curr;
    }
    void set_available_sram(int sram_curr)
    {
        sram_available = sram_curr;
    }
};

class Period_Rec
{
private:
    std::vector<std::pair<int, int>> ASAP_RES;
    std::vector<std::pair<int, int>> ALAP_RES;

public:
    Period_Rec(DataFlowGraph &DFG)
    {
        for (int k = 0; k < DFG.get_opList().size(); k++)
        {
            ASAP_RES.push_back(std::make_pair(0, 0));
            ALAP_RES.push_back(std::make_pair(0, 0));
        }
    }
    std::vector<std::pair<int, int>> &getASAP()
    {
        return ASAP_RES;
    }
    std::vector<std::pair<int, int>> &getALAP()
    {
        return ALAP_RES;
    }
};

bool meet_resources_constraint(std::map<int, struct Hardware> &rec, int i, DataFlowGraph &DFG);
void reset(Hardware &hardware, int i, DataFlowGraph &DFG);
int max(int a, int b);
int min(int a, int b);
void ASAP(DataFlowGraph &DFG, Period_Rec &REC);
void ALAP(DataFlowGraph &DFG, Period_Rec &REC);
bool cmp(const std::pair<int, int> &a, const std::pair<int, int> &b);
void improved_table_schedule_forDFG(DataFlowGraph &DFG);
void improved_schedule_forCFG(ControlFlowGraph &CFG);

#endif