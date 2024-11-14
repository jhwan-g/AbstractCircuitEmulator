#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "./LC3/lc3binarycodegen.h"
#include "circuitwrapper.h"
#include "./GlobalDefinitions/circuittype.h"

#include <queue>
#include <iostream>
#include <algorithm>
#pragma once

// 회로의 상태 전이 높은 수준에서 관리 -> circuitwrapper를 이용해 조작
class sImulator{
    std::unordered_map<std::string_view, int> module_time;
    std::unordered_map<std::string_view, std::vector<std::string_view>> next_module;
    std::vector<std::string_view> clock_sync_modules;
    std::priority_queue<std::pair<int, std::pair<std::string_view, Data>>> data_buffer;
    int CLKLoopLength;
    int cycle;
    std::string_view circuit_type;

    // circuit which this simulator will run
    circuitWrapper circuit;

    bool IsClockSyncModule(std::string_view module_name);

public:
    /* function InitializeSimulator
        input: std::string-name of file, std::string_view-
        output: 0(if finished successfully)
    */
    int InitializeSimulator(std::string, std::string_view);

    /* function RunSimulator
        input: none
        output: 1(if simulator ended noramlly) 0(if current cycle ended)
    */
    int RunSimulator();

    /* function PrintCircuitStatus
        input: none
        output: none (print current circuit status to io module)
    */
    void PrintCircuitStat();
};
#endif
