#ifndef LC3CIRCUIT_H
#define LC3CIRCUIT_H
#include "../GlobalDefinitions/typedef.h"

#include <unordered_map>
#include <string_view>
#include <map>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <variant>
#include <string_view>
#pragma once

/* Binding Between Modules And Lines*/
Data RunPC();
Data RunInstMemory();
Data RunDataMemory();
Data RunMux1();
Data RunMux2();
Data RunMux3();
Data RunMux4();
Data RunMux5();
Data RunAdd();
Data RunBranchController();
Data RunALU();
Data RunDecoder();
Data RunRegFile();
Data RunSignextend();
Data RunStatusController();
Data RunTrapDetector();


/* Spetial Functions Which Directly Change Circuit Status*/
void LC3SetClockPositiveEdge();
void LC3FinishClockPositiveEdge();
bool LC3IsTrap();
void LC3SetMemory(bitwd*);
void LC3SetPC(bitwd &newpc);

std::string LC3GetCircuitStatus();

/* Functions That Give Structure Information Of The Circuit*/
int LC3CLKLoopCycle();
std::vector<std::string_view> LC3CLKSyncedModule();
std::unordered_map<std::string_view, BitsetVariant> LC3DataMap();
std::unordered_map<std::string_view, Data (*)()> LC3ModuleMap();
std::unordered_map<std::string_view, int> LC3ModuleTime();
std::unordered_map<std::string_view, std::vector<std::string_view>> LC3NextModule();
#endif
